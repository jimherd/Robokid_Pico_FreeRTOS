#include <stdio.h>
#include <string.h>

#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bsp/board.h"
#include "tusb.h"

/**
 * @brief data structure for report returned from gamepad
 * 
 */
typedef struct TU_ATTR_PACKED       // packed attribute
{
    uint8_t     dpad_x, dpad_y;         // dpad returned at 8-bit values
    uint8_t     dummy2, dummy3,dummy4;

    struct {
        uint8_t dummy5      :4;
        uint8_t BUTTON_X    :1;
        uint8_t BUTTON_A    :1;
        uint8_t BUTTON_B    :1;
        uint8_t BUTTON_Y    :1;
    };

    struct {
        uint8_t BUTTON_L      :1;
        uint8_t BUTTON_R      :1;
        uint8_t dummy6        :2;
        uint8_t BUTTON_START  :1;
        uint8_t BUTTON_SELECT :1;
        uint8_t dummy7        :2;
    };
    uint8_t     dummy8;
} SNES_gamepad_report_t;

enum  gamepad_state {DISABLED, ENABLED} ;

struct {
    uint8_t     state;                  // ENABLED or DISAPLED
    uint8_t     dpad_x, dpad_y;
    uint8_t     button_X, button_Y, button_A, button_B;
    uint8_t     button_L, button_R;
    uint8_t     BUTTON_START, BUTTON_SELECT;
} gamepad_data;

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
void Task_read_gamepad(void *p) {

    board_init();
    tusb_init();
    gamepad_data.state = DISABLED;

    FOREVER {
       
    }
}

//    while(1){
//        sleep_ms(100);
//        tuh_task();
//    }
//}



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
static inline bool is_generic_gamepad(uint8_t dev_addr) {
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
 * @brief 
 * 
 * @param report 
 * @param len 
 */
void process_gamepad_report(uint8_t const* report, uint16_t len) 
{
SNES_gamepad_report_t  gamepad_report;
static SNES_gamepad_report_t previous_gamepad_report = { 0 };

    memcpy(&gamepad_report, report, sizeof(gamepad_report));
    if (diff_report(&previous_gamepad_report, &gamepad_report)) {
        printf("Report length = %d\r\n", len);
        printf("DPAD:X = %04x :: DPAD:Y = %04x\r\n", gamepad_report.dpad_x, gamepad_report.dpad_y);
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
    printf("VID = %04x, PID = %04x\r\n", vid, pid);
    if (is_generic_gamepad(dev_addr)) {
        if ( !tuh_hid_receive_report(dev_addr, instance)) {
            printf("Error : cannot request to receive report\r\n");
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
