/**
 * @file main.c
 * @author Jim Herd
 * @brief Development program for Robokid based on 
 *          Pico/FreeRTOS/TinyUSB/1306 LCD/Trinamic devices
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// #include "bsp/board.h"
// #include "tusb.h"
 
// #include "OLED_128X64.h"
// #include "system.h"

// #include "TMC7300.h"
// #include "TMC7300_Registers.h"
// #include "TMC7300_Fields.h"

// #include "robokid_2.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

#define FOREVER     for(;;)

TaskHandle_t taskhndl_blink_task;

void blink_task(void *p) {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    FOREVER {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

int main() {

//    board_init();

    stdio_init_all();

    xTaskCreate(blink_task,
                "blink_task",
                256,
                0,
                1,
                &taskhndl_blink_task
    );

    vTaskStartScheduler();

    FOREVER;
}

//    Oled_Init();
//    TMC7300_Init();

//    tusb_init();
//
// print hello message 

    // Oled_SetFont(Terminal12x16, 12, 16, 32,127);
    // Oled_ConstText("Test code",   0, 0);
    // Oled_ConstText("Robikid 2",   0, 2);
    // Oled_ConstText("Oled 128x64", 0, 4);
    // Oled_ConstText("SSD1306  ",   0, 6);

//    sleep_ms(3000);
//    printf("Loaded\r\n");

    // Oled_Demo();
    
//    while(1){
//        sleep_ms(100);
//        tuh_task();
//    }
//}

// #define GENERIC_GAMEPAD_VID     0x081F
// #define GENERIC_GAMEPAD_PID     0xE401

// /**
 // * @brief data structure for report returned from gamepad
 // * 
 // */
// typedef struct TU_ATTR_PACKED       // packed attribute
// {
    // uint8_t     dpad_x, dpad_y;         // dpad returned at 8-bit values
    // uint8_t     dummy2, dummy3,dummy4;

    // struct {
        // uint8_t dummy5      :4;
        // uint8_t BUTTON_X    :1;
        // uint8_t BUTTON_A    :1;
        // uint8_t BUTTON_B    :1;
        // uint8_t BUTTON_Y    :1;
    // };

    // struct {
        // uint8_t BUTTON_L      :1;
        // uint8_t BUTTON_R      :1;
        // uint8_t dummy6        :2;
        // uint8_t BUTTON_START  :1;
        // uint8_t BUTTON_SELECT :1;
        // uint8_t dummy7        :2;
    // };
    // uint8_t     dummy8;
// } SNES_gamepad_report_t;

// /**
 // * @check correct generic gamepad has been mounted on USB port
 // * 
 // * @param dev_add   device designator 
 // * @return true 
 // * @return false 
 // */
// static inline bool is_generic_gamepad(uint8_t dev_addr) {
// uint16_t  vid, pid;

    // tuh_vid_pid_get(dev_addr, &vid, &pid);
    // return ((vid == GENERIC_GAMEPAD_VID) && (pid == GENERIC_GAMEPAD_PID));
// }
// /**
 // * @brief Compare current repot with previous report
 // * 
 // * @param report1 
 // * @param report2 
 // * @return true     Reports differ
 // * @return false    Reports identical
 // */
// bool diff_report(SNES_gamepad_report_t const* report1,   SNES_gamepad_report_t const* report2) 
// {
    // return  memcmp(report1, report2, sizeof(SNES_gamepad_report_t));
// }


// /**
 // * @brief 
 // * 
 // * @param report 
 // * @param len 
 // */
// void process_gamepad_report(uint8_t const* report, uint16_t len) 
// {
// SNES_gamepad_report_t  gamepad_report;
// static SNES_gamepad_report_t previous_gamepad_report = { 0 };

    // memcpy(&gamepad_report, report, sizeof(gamepad_report));
    // if (diff_report(&previous_gamepad_report, &gamepad_report)) {
        // printf("Report length = %d\r\n", len);
        // printf("DPAD:X = %04x :: DPAD:Y = %04x\r\n", gamepad_report.dpad_x, gamepad_report.dpad_y);
    // }
    // previous_gamepad_report = gamepad_report;
// }

//==============================================================================
// USB gamepad callbacks
//==============================================================================
// /**
 // * @brief Callback routine when USB device is inserted
 // * 
 // * @param dev_addr 
 // * @param instance 
 // * @param desc_report 
 // * @param desc_len 
 // */
// void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t len)
// {
// uint16_t  vid, pid;

    // tuh_vid_pid_get(dev_addr, &vid, &pid);
    // printf("VID = %04x, PID = %04x\r\n", vid, pid);
    // if (is_generic_gamepad(dev_addr)) {
        // if ( !tuh_hid_receive_report(dev_addr, instance)) {
            // printf("Error : cannot request to receive report\r\n");
        // }
    // }
    // gpio_put(LED_PIN, 1);
// }

// /**
 // * @brief Callback routine when USB device is removed
 // * 
 // * @param dev_addr 
 // * @param instance 
 // */
// void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
// {
    // gpio_put(LED_PIN, 0);
// }

// /**
 // * @brief Callback routine when data packet received fro USB device
 // * 
 // * @param dev_addr 
 // * @param instance 
 // * @param report 
 // * @param len 
 // */
// void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len )
// {
    // if (is_generic_gamepad(dev_addr)) {
        // process_gamepad_report(report, len);
    // }
    // if ( !tuh_hid_receive_report(dev_addr, instance)) {
            // printf("Error : cannot request to receive report\r\n");
        // }

    // gpio_put(LED_PIN, 1);
// }
