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

#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// #include "TMC7300.h"
// #include "TMC7300_Registers.h"
// #include "TMC7300_Fields.h"


const uint LED_PIN = PICO_DEFAULT_LED_PIN;

TaskHandle_t taskhndl_Task_blink_LED;
TaskHandle_t taskhndl_Task_read_gamepad;
TaskHandle_t taskhndl_Task_display_LCD;
TaskHandle_t taskhndl_Task_drive_motors;

SemaphoreHandle_t semaphore_gamepad_data ;

//==============================================================================
// System initiation
//==============================================================================

int main() {

    stdio_init_all();
 
    semaphore_gamepad_data = xSemaphoreCreateMutex();

    xTaskCreate(Task_blink_LED,
                "blink_task",
                256,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_blink_LED
    );

    xTaskCreate(Task_read_gamepad,
                "Task_read_gamepad",
                256,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_gamepad
    );

       xTaskCreate(Task_display_LCD,
                "Task_display_LCD",
                256,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_display_LCD
    );

    xTaskCreate(Task_drive_motors,
                "Task_drive_motors",
                256,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_drive_motors
    );

    vTaskStartScheduler();

    HANG;
}

//    while(1){
//        sleep_ms(100);
//        tuh_task();
//    }
//}

