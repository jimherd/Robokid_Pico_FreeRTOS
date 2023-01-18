/**
 * @file    Task_RW_sensors.c
 * @author  Jim Herd
 * @brief   read Robokid sensors and set neopixel LEDs
 */

#include <string.h>
#include <assert.h>

#include "system.h"
#include "Pico_IO.h"
#include "common.h"
#include "neopixel.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/divider.h"
#include "hardware/pio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "ws2812.pio.h"

//==============================================================================
// function prototypes for local routines
//==============================================================================

//==============================================================================
// Local globals
//==============================================================================

//==============================================================================
// temp locals
//==============================================================================

//==============================================================================
// Main task routine
//==============================================================================
//
// 1. read push buttons and debounce
// 2. read line sensors
// 3. read analogue sensors
// 3. update central data store

void Task_neopixels(void *p) 
{


    uint offset = pio_add_program(NEOPIXEL_PIO_UNIT, &ws2812_program);
    ws2812_program_init(NEOPIXEL_PIO_UNIT, 
                        NEOPIXEL_STATE_MACHINE, 
                        offset, 
                        NEOPIXEL_DOUT_PIN, 
                        NEOPIXEL_DATA_RATE,
                        NEOPIXEL_BITS_PER_UNIT 
                        );

//
// Task code
//
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_NEOPIXELS_FREQUENCY_TICK_COUNT );

// Process LED data

        put_pixel(urgb_u32(0xff, 0, 0));  // Red
        put_pixel(urgb_u32(0, 0x7f, 0));
        put_pixel(urgb_u32(0, 0, 0x3f));

        for (index = 0 ; index < NOS_ROBOKID_LEDS ; index++) {

        }
        end_time = time_us_32();
        update_task_execution_time(TASK_READ_SENSORS, start_time, end_time);
    }
}

//==============================================================================
// Archive area
//==============================================================================

        // for (index = 0 ; index < NOS_ROBOKID_LEDS ; index++) {
        //     switch (temp_LED_data[index].state) {
        //         case LED_OFF : {
        //             gpio_put(temp_LED_data[index].pin_number, false);
        //             break;
        //         }
        //         case LED_ON : {
        //             gpio_put(temp_LED_data[index].pin_number, true);
        //             break;
        //         }
        //         case LED_NO_CHANGE : {
        //             break;
        //         }
        //         case LED_FLASH : {
        //             if (temp_LED_data[index].flash_counter == 0) {
        //                 temp_LED_data[index].flash_counter = temp_LED_data[index].flash_time;
        //                 temp_LED_data[index].flash_value = !temp_LED_data[index].flash_value;
        //                 gpio_put(temp_LED_data[index].pin_number, temp_LED_data[index].flash_value);
        //             } else {
        //                 temp_LED_data[index].flash_counter--;
        //             }
        //             break;
        //         }
        //     }
        // }