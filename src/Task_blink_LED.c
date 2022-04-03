/**
 * @file Task_blink_LED.c
 * @author Jim Herd
 * @brief Flash error code
 * @note    Errors are flashed as decimal values.
 *          errors are negative and flashed as positive numbers.
 *          If all well one long flash every 5 seconds.
 */
#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"

extern void flash(uint8_t nos_flashes);

#define     FLASH_TIME      (300/portTICK_PERIOD_MS)
#define     FLASH_DELAY     (300/portTICK_PERIOD_MS)

void Task_blink_LED(void *p) {

uint8_t     tens, units, count;
int8_t      error;
uint32_t    start_time, end_time;

    gpio_init(BLINK_PIN);
    gpio_set_dir(BLINK_PIN, GPIO_OUT);

    FOREVER {
        start_time = time_us_32();
        xSemaphoreTake(semaphore_system_status, portMAX_DELAY);
            error = system_status.error_state;
        xSemaphoreGive(semaphore_system_status);

        if (error == OK) {                          // no error 
            gpio_put(BLINK_PIN, 1);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            gpio_put(BLINK_PIN, 0);
            vTaskDelay(4000/portTICK_PERIOD_MS);
        } else {                                     // error
            error = abs(error);
            tens = error/10;
            units = error % 10;
            flash(tens);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            flash(units);
            vTaskDelay(3000/portTICK_PERIOD_MS);
        }
    end_time = time_us_32();
    update_task_execution_time(TASK_BLINK, start_time, end_time);
    }
}

/**
 * @brief Generate sequence of flashes
 * 
 * @param nos_flashes 
 */
void flash(uint8_t nos_flashes) {

uint8_t  count;

    for (count = 0 ; count < nos_flashes ; count++) {
        gpio_put(BLINK_PIN, 1);
        vTaskDelay(FLASH_TIME);
        gpio_put(BLINK_PIN, 0);
        vTaskDelay(FLASH_DELAY);
    }
}
