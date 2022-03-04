#include <stdio.h>
#include <string.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp/board.h"
#include "tusb.h"



//==============================================================================
// USB gamepad function prototypes
//==============================================================================
void Task_read_gamepad(void *p);

static inline bool is_generic_gamepad(uint8_t dev_addr);
bool diff_report(SNES_gamepad_report_t const* report1,   SNES_gamepad_report_t const* report2);
void process_gamepad_report(uint8_t const* report, uint16_t len);
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t len);
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance);
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len );

//==============================================================================
// Main task routine
//==============================================================================
void Task_read_gamepad(void *p) 
{

    board_init();
    tusb_init();
    gamepad_data.state = DISABLED;

    FOREVER {
       vTaskDelay(100/portTICK_PERIOD_MS);
       tuh_task();
    }
}

//==============================================================================
// USB gamepad functions
//==============================================================================
/**
 * @check correct generic gamepad has been mounted on USB port
 * 
 * @param dev_add   device designator 
 * @return true 
 * @return false 
 */
static inline bool is_generic_gamepad(uint8_t dev_addr) 
{
uint16_t  vid, pid;

    tuh_vid_pid_get(dev_addr, &vid, &pid);
    return ((vid == GENERIC_GAMEPAD_VID) && (pid == GENERIC_GAMEPAD_PID));
}
/**
 * @brief Compare current repot with previous report
 * 
 * @param report1 
 * @param report2 
 * @return true     Reports differ
 * @return false    Reports identical
 */
bool diff_report(SNES_gamepad_report_t const* report1,   SNES_gamepad_report_t const* report2) 
{
    return  memcmp(report1, report2, sizeof(SNES_gamepad_report_t));
}

/**
 * @brief Extract data from gamepad USB report and put in
 *        global data structure for access by other tasks.
 *        Access protected by MUTEX "semaphore_gamepad_data".
 * @param report 
 * @param len 
 */
void process_gamepad_report(uint8_t const* report, uint16_t len) 
{
SNES_gamepad_report_t  gamepad_report;
static SNES_gamepad_report_t previous_gamepad_report = { 127,127,0 };

    memcpy(&gamepad_report, report, sizeof(gamepad_report));
    if (diff_report(&previous_gamepad_report, &gamepad_report)) {

        xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
            gamepad_data.dpad_x        = gamepad_report.dpad_x;
            gamepad_data.dpad_y        = gamepad_report.dpad_y;
            gamepad_data.button_X      = gamepad_report.button_X;
            gamepad_data.button_Y      = gamepad_report.button_Y;
            gamepad_data.button_A      = gamepad_report.button_A;
            gamepad_data.button_B      = gamepad_report.button_B;
            gamepad_data.button_L      = gamepad_report.button_L;
            gamepad_data.button_R      = gamepad_report.button_R;
            gamepad_data.button_START  = gamepad_report.button_START;
            gamepad_data.button_SELECT = gamepad_report.button_SELECT;
        xSemaphoreGive(semaphore_gamepad_data);
    }
    previous_gamepad_report = gamepad_report;
}

//==============================================================================
// USB gamepad callbacks
//==============================================================================
/**
 * @brief Callback routine when USB device is inserted
 * 
 * @param dev_addr 
 * @param instance 
 * @param desc_report 
 * @param desc_len 
 */
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t len)
{
uint16_t  vid, pid;

    tuh_vid_pid_get(dev_addr, &vid, &pid);
        xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
            gamepad_data.state = ENABLED;
            gamepad_data.vid = vid;
            gamepad_data.pid = pid;
        xSemaphoreGive(semaphore_gamepad_data);
    if (is_generic_gamepad(dev_addr)) {
        if ( !tuh_hid_receive_report(dev_addr, instance)) {
            
            //printf("Error : cannot request to receive report\r\n");
        }
    }
    gpio_put(LED_PIN, 1);
}

/**
 * @brief Callback routine when USB device is removed
 * 
 * @param dev_addr 
 * @param instance 
 */
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    gpio_put(LED_PIN, 0);
    xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
            gamepad_data.state = DISABLED;
            gamepad_data.vid = 0;
            gamepad_data.pid = 0;
        xSemaphoreGive(semaphore_gamepad_data);
}

/**
 * @brief Callback routine when data packet received from USB device
 * 
 * @param dev_addr 
 * @param instance 
 * @param report 
 * @param len 
 */
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len )
{
    if (is_generic_gamepad(dev_addr)) {
        process_gamepad_report(report, len);
    }
    if ( !tuh_hid_receive_report(dev_addr, instance)) {
            printf("Error : cannot request to receive report\r\n");
        }

    gpio_put(LED_PIN, 1);
}
