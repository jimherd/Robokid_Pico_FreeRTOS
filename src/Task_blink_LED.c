/**
 * @file Task_blink_LED.c
 * @author Jim Herd
 * @brief Simple teask to flash Pico on-board LED
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>

#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"

void Task_blink_LED(void *p) {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    FOREVER {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}