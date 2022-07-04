/**
 * @file    Task_read_sensors.c
 * @author  Jim Herd
 * @brief   read Robokid sensors
 */

#include <string.h>

#include "system.h"
#include "Pico_IO.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

struct push_button_data_s  temp_push_button_data[NOS_ROBOKID_PUSH_BUTTONS];
struct LED_data_s          temp_LED_data[NOS_ROBOKID_LEDS];
struct analogue_data_s     temp_analogue_data;

uint32_t            switch_samples[NOS_SWITCH_SAMPLES];     // circular buffer
uint8_t             switch_sample_index;

//==============================================================================
// function prototypes for local routines
//==============================================================================

static void set_CD4051_address(uint8_t index);

//==============================================================================
// Main task routine
//==============================================================================
//
// 1. read push buttons and debounce
// 2. read line sensors
// 3. read analogue sensors
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
    gpio_init(PUSH_BUTTON_A_PIN); gpio_set_dir(PUSH_BUTTON_A_PIN, GPIO_IN); gpio_pull_up(PUSH_BUTTON_A_PIN); 
    gpio_init(PUSH_BUTTON_B_PIN); gpio_set_dir(PUSH_BUTTON_B_PIN, GPIO_IN); gpio_pull_up(PUSH_BUTTON_B_PIN); 
    gpio_init(PUSH_BUTTON_C_PIN); gpio_set_dir(PUSH_BUTTON_C_PIN, GPIO_IN); gpio_pull_up(PUSH_BUTTON_C_PIN);  
    gpio_init(PUSH_BUTTON_D_PIN); gpio_set_dir(PUSH_BUTTON_D_PIN, GPIO_IN); gpio_pull_up(PUSH_BUTTON_D_PIN);  

    gpio_init(LED_A_PIN); gpio_set_dir(LED_A_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN); gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_init(LED_C_PIN); gpio_set_dir(LED_C_PIN, GPIO_OUT);
    gpio_init(LED_D_PIN); gpio_set_dir(LED_D_PIN, GPIO_OUT);
    
    for (uint8_t i=0; i < NOS_SWITCH_SAMPLES ; i++) {
        switch_samples[i] = 0;
    }
    switch_sample_index = 0;

    gpio_init(CD4051_ADDRESS_A_PIN); gpio_set_dir(CD4051_ADDRESS_A_PIN, GPIO_OUT); gpio_pull_down(CD4051_ADDRESS_A_PIN);
    gpio_init(CD4051_ADDRESS_B_PIN); gpio_set_dir(CD4051_ADDRESS_B_PIN, GPIO_OUT); gpio_pull_down(CD4051_ADDRESS_B_PIN);
    gpio_init(CD4051_ADDRESS_C_PIN); gpio_set_dir(CD4051_ADDRESS_C_PIN, GPIO_OUT); gpio_pull_down(CD4051_ADDRESS_C_PIN);

    adc_gpio_init(ANALOGUE_CD4051_INPUT_CHANNEL);       // Make sure GPIO is high-impedance, no pullups etc
    adc_select_input(0);                                // Select ADC input 0 (GPIO26)

//
// Task code
//
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_READ_SENSORS_FREQUENCY_TICK_COUNT );
START_PULSE;
        start_time = time_us_32();
    //
    // Get current switch and LED data
    //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&temp_push_button_data[0], &system_IO_data.push_button_data[0], (NOS_ROBOKID_PUSH_BUTTONS *  sizeof(struct push_button_data_s)));
            memcpy(&temp_LED_data[0], &system_IO_data.LED_data[0], (NOS_ROBOKID_LEDS * sizeof(struct LED_data_s)));
            memcpy(&temp_analogue_data , &system_IO_data.analogue_data, sizeof(struct analogue_data_s));
        xSemaphoreGive(semaphore_system_IO_data);
    //
    // read push switches and debounce
    // Set read to give results as if default is low
    //
    #ifdef PUSH_SWITCH_DEFAULT_LOW
        switch_samples[switch_sample_index] = (gpio_get_all() & PUSH_BUTTON_HARDWARE_MASK);
    #else
        switch_samples[switch_sample_index] = ~(gpio_get_all() & PUSH_BUTTON_HARDWARE_MASK);
    #endif

        uint32_t switch_value = 0xFFFF;
        for (index=0; index < NOS_SWITCH_SAMPLES ; index++) {
            switch_value &= switch_samples[index];
        }
        switch_sample_index++;
        if (switch_sample_index >= NOS_SWITCH_SAMPLES) {
            switch_sample_index = 0;
        }
        temp_push_button_data[0].switch_value = (switch_value & PUSH_BUTTON_A_HARDWARE_MASK) >> PUSH_BUTTON_A_PIN;
        temp_push_button_data[1].switch_value = (switch_value & PUSH_BUTTON_B_HARDWARE_MASK) >> PUSH_BUTTON_B_PIN;
        temp_push_button_data[2].switch_value = (switch_value & PUSH_BUTTON_C_HARDWARE_MASK) >> PUSH_BUTTON_C_PIN;
        temp_push_button_data[3].switch_value = (switch_value & PUSH_BUTTON_D_HARDWARE_MASK) >> PUSH_BUTTON_D_PIN;

    // set event flags for the four push buttons. Event flags 0 to 3 are set when putton is pressed.
    // Event flags 4 to 7 are set when push button is released
    
    uint32_t  button_on_events, button_off_events;
    
        for (index = PUSH_BUTTON_A_EVENT_BIT; index <= PUSH_BUTTON_D_EVENT_BIT; index++) {
            if (temp_push_button_data[index].switch_value == 1) {
                xEventGroupSetBits(eventgroup_push_buttons, (1 << index));
                xEventGroupClearBits(eventgroup_push_buttons, (1 << (index + 4)));
            } else {
                xEventGroupClearBits(eventgroup_push_buttons, (1 << index));
                xEventGroupSetBits(eventgroup_push_buttons, (1 << (index + 4)));
            }
        }
        xEventGroupClearBits(eventgroup_push_buttons, (1 << index));
        xEventGroupSetBits(eventgroup_push_buttons, (1 << (index + 4)));

    // update on-time counter

        for (index = 0 ; index < NOS_ROBOKID_PUSH_BUTTONS ; index++) {
            if (temp_push_button_data[index].switch_value == true) {
                temp_push_button_data[index].on_time += TASK_READ_SENSORS_FREQUENCY_TICK_COUNT;
            } else {
                // temp_push_button_data[index].on_time = 0;
            }
        }

    // Process LED data

        for (index = 0 ; index < NOS_ROBOKID_LEDS ; index++) {
            switch (temp_LED_data[index].state) {
                case LED_OFF : {
                    gpio_put(temp_LED_data[index].pin_number, false);
                    break;
                }
                case LED_ON : {
                    gpio_put(temp_LED_data[index].pin_number, true);
                    break;
                }
                case LED_NO_CHANGE : {
                    break;
                }
                case LED_FLASH : {
                    if (temp_LED_data[index].flash_counter == 0) {
                        temp_LED_data[index].flash_counter = temp_LED_data[index].flash_time;
                        temp_LED_data[index].flash_value = !temp_LED_data[index].flash_value;
                        gpio_put(temp_LED_data[index].pin_number, temp_LED_data[index].flash_value);
                    } else {
                        temp_LED_data[index].flash_counter--;
                    }
                    break;
                }
            }
        }

    // read 8-channel CD4051 A/D subsystem
        adc_select_input(CD4051_RP2040_channel);
        for (index = 0 ; index < NOS_CD4051_CHANNELS ; index++) {
            set_CD4051_address(index);
            temp_analogue_data.CD4051_raw_data[index] = adc_read();
            busy_wait_us_32(1);
        }
        set_CD4051_address(0);    // reset CD4051 address to 0

    // Update global system data 

        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
           memcpy(&system_IO_data.push_button_data[0], &temp_push_button_data[0], (NOS_ROBOKID_PUSH_BUTTONS *  sizeof(struct push_button_data_s)));
           memcpy(&system_IO_data.LED_data[0], &temp_LED_data[0], (NOS_ROBOKID_LEDS * sizeof(struct LED_data_s)));
           memcpy(&system_IO_data.analogue_data, &temp_analogue_data , sizeof(struct analogue_data_s));
        xSemaphoreGive(semaphore_system_IO_data);

        end_time = time_us_32();
        update_task_execution_time(TASK_READ_SENSORS, start_time, end_time);
        
STOP_PULSE;
    }
}

//==============================================================================
// local functions
//==============================================================================
/**
 * @brief Set the CD4051 ABC address lines
 * 
 * @param index value 0 to 7 to specify 1 of 8 CD4051 analogue channels
 * 
 * @note
 *      This reoutine relies on CD4051 address ABC to be consecutive digital
 *      output lines.
 */
static void set_CD4051_address(uint8_t index)
{
    gpio_put_masked(CD4051_ADDRESS_MASK, (index << CD4051_ADDRESS_A_PIN));
}

