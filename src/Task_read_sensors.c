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
#include "hardware/divider.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//==============================================================================
// look-up table to convert 8-bit a/d values to %
//==============================================================================

const uint8_t byte_to_percent[] = {
    0, 0, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 6,
    6, 7, 7, 7, 8, 8, 9, 9, 9,10,10,11,11,11,12,12,
    13,13,13,14,14,15,15,15,16,16,16,17,17,18,18,18,
    19,19,20,20,20,21,21,22,22,22,23,23,24,24,24,25,
    25,25,26,26,27,27,27,28,28,29,29,29,30,30,31,31,
    31,32,32,33,33,33,34,34,35,35,35,36,36,36,37,37,
    38,38,38,39,39,40,40,40,41,41,42,42,42,43,43,44,
    44,44,45,45,45,46,46,47,47,47,48,48,49,49,49,50,
    50,51,51,51,52,52,53,53,53,54,54,55,55,55,56,56,
    56,57,57,58,58,58,59,59,60,60,60,61,61,62,62,62,
    63,63,64,64,64,65,65,65,66,66,67,67,67,68,68,69,
    69,69,70,70,71,71,71,72,72,73,73,73,74,74,75,75,
    75,76,76,76,77,77,78,78,78,79,79,80,80,80,81,81,
    82,82,82,83,83,84,84,84,85,85,85,86,86,87,87,87,
    88,88,89,89,89,90,90,91,91,91,92,92,93,93,93,94,
    94,95,95,95,96,96,96,97,97,98,98,98,99,99,100,100
};

//==============================================================================
// function prototypes for local routines
//==============================================================================

static void set_CD4051_address(uint8_t index);

//==============================================================================
// Local globals
//==============================================================================
static struct push_button_data_s  temp_push_button_data[NOS_ROBOKID_PUSH_BUTTONS];
static struct LED_data_s          temp_LED_data[NOS_ROBOKID_LEDS];
static struct analogue_data_s     temp_analogue_data;

uint32_t    switch_samples[NOS_SWITCH_SAMPLES];     // circular buffer
uint8_t     switch_sample_index;
uint32_t    sample_count;

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
uint32_t    sample_count;
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
    adc_select_input(0);  
    
    sample_count = 0;                              // Select ADC input 0 (GPIO26)

//
// Task code
//
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_READ_SENSORS_FREQUENCY_TICK_COUNT );
START_PULSE;
        start_time = time_us_32();
        sample_count++;
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
            uint16_t tmp_data = adc_read();
            temp_analogue_data.CD4051[index].raw_data = tmp_data;
            temp_analogue_data.CD4051[index].percent  = byte_to_percent[(tmp_data >> 4)];
            
        }
        set_CD4051_address(0);    // reset CD4051 address to 0

    // Threshold IR line sensors

        system_IO_data.line_sensor_data[0].percent_value = system_IO_data.analogue_data.CD4051[LINE_SENSOR_LEFT_CHANNEL].percent;
        system_IO_data.line_sensor_data[1].percent_value = system_IO_data.analogue_data.CD4051[LINE_SENSOR_MID_CHANNEL].percent;
        system_IO_data.line_sensor_data[2].percent_value = system_IO_data.analogue_data.CD4051[LINE_SENSOR_RIGHT_CHANNEL].percent;
        for(index=0;index <NOS_ROBOKID_LINE_SENSORS ; index++) {
            if (system_IO_data.line_sensor_data[index].percent_value > (system_IO_data.line_sensor_data[index].threshhold)) {
                system_IO_data.line_sensor_data[index].binary_value = 1;
            } else {
                system_IO_data.line_sensor_data[index].binary_value = 0;
            }
        }

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
 *      This routine relies on CD4051 address ABC to be consecutive digital
 *      output lines.
 * 
 *      The datasheet gives a MAXIMUM Propogation Delay Time (at 5V) for
 *      Address-to-signal OUT of 750nS (typ. 450nS).  We are using 3.3v so it 
 *      may be a little greater.  Initial design to insert a 3000nS delay to
 *      allow analogue value to settle. Checked on oscilloscope.
 */
static void set_CD4051_address(uint8_t index)
{
    gpio_put_masked(CD4051_ADDRESS_MASK, (index << CD4051_ADDRESS_A_PIN));
    busy_wait_us_32(CD4051_SETTLING_TIME_US);
}

struct analogue_raw_data_s analogue_raw_data[NOS_CD4051_CHANNELS];
struct analogue_processed_data_s analogue_processed_data[NOS_CD4051_CHANNELS];

static void filter_analogue(void){

struct analogue_raw_data_s *raw_data_pt;
struct analogue_processed_data_s *processed_data_pt;

uint32_t delta;

    for(uint8_t index ; index < NOS_CD4051_CHANNELS; index++){
        raw_data_pt       = &analogue_raw_data[index];
        processed_data_pt = &analogue_processed_data[index];
        set_CD4051_address(index);
        uint16_t tmp_data = adc_read();
        raw_data_pt->sample_count++;
        raw_data_pt->current_value = tmp_data;
    //
    // Set all relevant variables to read value until averaging buffer is full
    //
        if (sample_count < BUFF_SIZE) {  // circular buffer not full
            raw_data_pt->last_value = tmp_data;
            raw_data_pt->cir_buffer.buffer[raw_data_pt->cir_buffer.buff_ptr++] = tmp_data;
            processed_data_pt->value = tmp_data;
            if (raw_data_pt->cir_buffer.buff_ptr >= BUFF_SIZE) {
                raw_data_pt->cir_buffer.buff_ptr = 0;
            }
            return;
        }
    //
    // run glitch filter by testing between this and last value
    //
        if(tmp_data > raw_data_pt->last_value) {
            delta = tmp_data - raw_data_pt->last_value;
        } else {
            delta = raw_data_pt->last_value - tmp_data;
        }
        if (delta > processed_data_pt->glitch_threshold) {
            raw_data_pt->current_value = raw_data_pt->last_value;
            raw_data_pt->glitch_count++;
            // log glitch detected
            if (raw_data_pt->glitch_count > processed_data_pt->glitch_error_threshold) {  // run of glitches
                raw_data_pt->sample_count = 0;
                raw_data_pt->glitch_count =0;
            }
        } else {
            raw_data_pt->glitch_count = 0;
        }
    //
    // Add value to circular buffer and adjust pointer as necessary
    //
        raw_data_pt->cir_buffer.buffer[raw_data_pt->cir_buffer.buff_ptr++] = tmp_data;
        if (raw_data_pt->cir_buffer.buff_ptr >= BUFF_SIZE) {
            raw_data_pt->cir_buffer.buff_ptr = 0;
        }
    //
    // calculate average
    //
        uint32_t sum = 0;
        for(uint8_t i = 0; i <BUFF_SIZE; i++) {
            sum += raw_data_pt->cir_buffer.buffer[i];
        }
        processed_data_pt->value = hw_divider_u32_quotient_inlined(sum, BUFF_SIZE);
    }
    
}

