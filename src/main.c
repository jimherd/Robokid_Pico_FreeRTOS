/**
 * @file    main.c
 * @author  Jim Herd
 * @brief   Development program for Robokid based on 
 *          Pico/FreeRTOS/TinyUSB/1306 LCD/Trinamic devices
 * @version 0.1
 * @date    2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>

#include "system.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//==============================================================================
// Global data
//==============================================================================

// Hardware

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LOG_PIN = 14;
const uint BLINK_PIN = 16;

// FreeRTOS components

TaskHandle_t taskhndl_Task_blink_LED;
TaskHandle_t taskhndl_Task_read_gamepad;
TaskHandle_t taskhndl_Task_display_LCD;
TaskHandle_t taskhndl_Task_drive_motors;

SemaphoreHandle_t semaphore_system_status;
SemaphoreHandle_t semaphore_gamepad_data ;

// System data structures. Protected with MUTEXES

SNES_gamepad_report_t   SNES_gamepad_report;
gamepad_data_t          gamepad_data;
system_status_t         system_status;

//==============================================================================
// System initiation
//==============================================================================

int main() {

    stdio_init_all();

    gpio_init(LOG_PIN);
    gpio_set_dir(LOG_PIN, GPIO_OUT);
    gpio_pull_down(LOG_PIN);         // should default but just make sure
 
    xTaskCreate(Task_blink_LED,
                "blink_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_blink_LED
    );

    xTaskCreate(Task_read_gamepad,
                "Task_read_gamepad",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_gamepad
    );

    xTaskCreate(Task_display_LCD,
                "Task_display_LCD",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_display_LCD
    );

    xTaskCreate(Task_drive_motors,
                "Task_drive_motors",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_drive_motors
    );

    semaphore_system_status  = xSemaphoreCreateMutex();
    semaphore_gamepad_data   = xSemaphoreCreateMutex();

    vTaskStartScheduler();

    HANG;
}

