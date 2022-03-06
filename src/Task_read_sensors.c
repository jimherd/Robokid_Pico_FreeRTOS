/**
 * @file    Task_read_sensors.c
 * @author  Jim Herd
 * @brief   read Robokid sensors
 * @version 0.1
 * @date    2022-02-16
 */

#include <string.h>

#include "system.h"
#include "Pico_IO.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

push_button_data_t  temp_push_button_data[NOS_ROBOKID_PUSH_BUTTONS];
LED_data_t          temp_LED_data[NOS_ROBOKID_LEDS];

uint32_t            switch_samples[NOS_SWITCH_SAMPLES];     // circular buffer
uint8_t             switch_sample_index;

//==============================================================================
// Main task routine
//==============================================================================
//
// 1. read push buttons and debounce
// 2. read line sensors
// 3. update central data store

void Task_read_sensors(void *p) 
{
TickType_t  xLastWakeTime;
BaseType_t  xWasDelayed;
uint8_t     index;
uint32_t    start_time, end_time;
//
// Task init
//
    gpio_init(PUSH_BUTTON_A_PIN); gpio_set_dir(PUSH_BUTTON_A_PIN, GPIO_IN); gpio_pull_down(PUSH_BUTTON_A_PIN); 
    gpio_init(PUSH_BUTTON_B_PIN); gpio_set_dir(PUSH_BUTTON_B_PIN, GPIO_IN); gpio_pull_down(PUSH_BUTTON_B_PIN); 
    gpio_init(PUSH_BUTTON_C_PIN); gpio_set_dir(PUSH_BUTTON_C_PIN, GPIO_IN); gpio_pull_down(PUSH_BUTTON_C_PIN);  
    gpio_init(PUSH_BUTTON_D_PIN); gpio_set_dir(PUSH_BUTTON_D_PIN, GPIO_IN); gpio_pull_down(PUSH_BUTTON_D_PIN);  

    gpio_init(LED_0_PIN); gpio_set_dir(LED_0_PIN, GPIO_OUT);
    gpio_init(LED_1_PIN); gpio_set_dir(LED_1_PIN, GPIO_OUT);
    gpio_init(LED_2_PIN); gpio_set_dir(LED_2_PIN, GPIO_OUT);
    gpio_init(LED_3_PIN); gpio_set_dir(LED_3_PIN, GPIO_OUT);
    
    for (uint8_t i=0; i < NOS_SWITCH_SAMPLES ; i++) {
        switch_samples[i] = 0;
    }
    switch_sample_index = 0;
//
// Task code
//
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_READ_SENSORS_FREQUENCY_TICK_COUNT );
START_PULSE;
        start_time = time_us_32();
    //
    // Get currect switch and LED data
    //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&temp_push_button_data[0], &system_IO_data.push_button_data[0], (NOS_ROBOKID_PUSH_BUTTONS *  sizeof(push_button_data_t)));
            memcpy(&temp_LED_data[0], &system_IO_data.LED_data[0], (NOS_ROBOKID_LEDS * sizeof(LED_data_t)));
        xSemaphoreGive(semaphore_system_IO_data);
    //
    // read push switches and debounce
    //
        switch_samples[switch_sample_index] = gpio_get_all() & SWITCH_MASK;
        uint32_t switch_value = 0xFFFF;
        for (index=0; index < NOS_SWITCH_SAMPLES ; index++) {
            switch_value &= switch_samples[index];
        }
        switch_sample_index++;
        if (switch_sample_index >= NOS_SWITCH_SAMPLES) {
            switch_sample_index = 0;
        }
        temp_push_button_data[0].switch_value = (switch_value & PUSH_BUTTON_A_MASK) >> PUSH_BUTTON_A_PIN;
        temp_push_button_data[1].switch_value = (switch_value & PUSH_BUTTON_B_MASK) >> PUSH_BUTTON_B_PIN;
        temp_push_button_data[2].switch_value = (switch_value & PUSH_BUTTON_C_MASK) >> PUSH_BUTTON_C_PIN;
        temp_push_button_data[3].switch_value = (switch_value & PUSH_BUTTON_D_MASK) >> PUSH_BUTTON_D_PIN;



// maybe form into a loop
        if (temp_push_button_data[0].switch_value == 1) {
            xEventGroupSetBits (eventgroup_push_buttons, PUSH_BUTTON_A_EVENT_BIT);
            xEventGroupClearBits (eventgroup_push_buttons, (PUSH_BUTTON_A_EVENT_BIT<<4));
        } else {
            xEventGroupClearBits (eventgroup_push_buttons, PUSH_BUTTON_A_EVENT_BIT);
            xEventGroupSetBits (eventgroup_push_buttons, (PUSH_BUTTON_A_EVENT_BIT<<4));
        }
    //
    // update on-time counter
    //
        for (index = 0 ; index < NOS_ROBOKID_PUSH_BUTTONS ; index++) {
            if (temp_push_button_data[index].switch_value == true) {
                temp_push_button_data[index].on_time += TASK_READ_SENSORS_FREQUENCY_TICK_COUNT;
            } else {
                temp_push_button_data[index].on_time = 0;
            }
        }
    //
    // Process LED data
    //
        for (index = 0 ; index < NOS_ROBOKID_LEDS ; index++) {
            if (temp_LED_data[index].value == false) {
                gpio_put(temp_LED_data[index].pin_number, false);
            } else {
                if ((temp_LED_data[index].value == true) && (temp_LED_data[index].flash == false)) {
                    gpio_put(temp_LED_data[index].pin_number, true);
                } else {
                    if (temp_LED_data[index].flash_counter == 0) {
                        temp_LED_data[index].flash_counter = temp_LED_data[index].flash_time;
                        temp_LED_data[index].flash_value = !temp_LED_data[index].flash_value;
                        gpio_put(temp_LED_data[index].pin_number, temp_LED_data[index].flash_value);
                    } else {
                        temp_LED_data[index].flash_counter--;
                    }
                }
            }
        }
    //
    // Update global system data 
    //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
           memcpy(&system_IO_data.push_button_data[0], &temp_push_button_data[0], (NOS_ROBOKID_PUSH_BUTTONS *  sizeof(push_button_data_t)));
           memcpy(&system_IO_data.LED_data[0], &temp_LED_data[0], (NOS_ROBOKID_LEDS * sizeof(LED_data_t)));
        xSemaphoreGive(semaphore_system_IO_data);

        end_time = time_us_32();
        update_task_execution_time(TASK_READ_SENSORS, start_time, end_time);
        
STOP_PULSE;
    }
}

