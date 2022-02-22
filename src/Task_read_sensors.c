/**
 * @file    Task_read_sensors.c
 * @author  Jim Herd
 * @brief   read Robokid sensors
 * @version 0.1
 * @date    2022-02-16
 * 
 */
#include "system.h"
#include "Pico_IO.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define     NOS_SWITCHES            4

#define     SWITCH_0_MASK           (1 << SWITCH_0_PIN)
#define     SWITCH_1_MASK           (1 << SWITCH_1_PIN)
#define     SWITCH_2_MASK           (1 << SWITCH_2_PIN)
#define     SWITCH_3_MASK           (1 << SWITCH_3_PIN)
#define     SWITCH_MASK             (SWITCH_0_MASK | SWITCH_1_MASK |SWITCH_2_MASK | SWITCH_3_MASK)
#define     NOS_SWITCH_SAMPLES      3
#define     NOS_LEDS                4

#define     Task_read_sensors_frequency                 50  //Hz
#define     Task_read_sensors_frequency_tick_count      ((1000/Task_read_sensors_frequency) * portTICK_PERIOD_MS)

//==============================================================================
// Main task routine
//==============================================================================
//
// 1. read push buttons and debounce
// 2. read line sensors

void Task_read_sensors(void *p) 
{
TickType_t          xLastWakeTime;
BaseType_t          xWasDelayed;

uint8_t     switch_state[NOS_SWITCHES];
uint32_t    switch_samples[NOS_SWITCH_SAMPLES];  // circular buffer
uint8_t     switch_sample_index;

    gpio_init(SWITCH_0_PIN); gpio_set_dir(SWITCH_0_PIN, GPIO_IN); gpio_pull_down(SWITCH_0_PIN); 
    gpio_init(SWITCH_1_PIN); gpio_set_dir(SWITCH_1_PIN, GPIO_IN); gpio_pull_down(SWITCH_1_PIN); 
    gpio_init(SWITCH_2_PIN); gpio_set_dir(SWITCH_2_PIN, GPIO_IN); gpio_pull_down(SWITCH_2_PIN);  
    gpio_init(SWITCH_3_PIN); gpio_set_dir(SWITCH_3_PIN, GPIO_IN); gpio_pull_down(SWITCH_3_PIN);  

    gpio_init(LED_0_PIN); gpio_set_dir(LED_0_PIN, GPIO_OUT);
    gpio_init(LED_1_PIN); gpio_set_dir(LED_1_PIN, GPIO_OUT);
    gpio_init(LED_2_PIN); gpio_set_dir(LED_2_PIN, GPIO_OUT);
    gpio_init(LED_3_PIN); gpio_set_dir(LED_3_PIN, GPIO_OUT);
    
    for (uint8_t i=0; i < NOS_SWITCH_SAMPLES ; i++) {
        switch_samples[i] = 0;
    }
    for (uint8_t i=0 ; i < NOS_SWITCHES ; i++) {
        switch_state[i] = 0;
    }
    switch_sample_index = 0;

    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, Task_read_sensors_frequency_tick_count );

        // read push switches and debounce
        // 21/2/22 : delta T = 510nS
START_PULSE;
        switch_samples[switch_sample_index] = gpio_get_all() & SWITCH_MASK;
        uint32_t switch_value = 0xFFFF;
        for (uint8_t i=0; i < NOS_SWITCH_SAMPLES ; i++) {
            switch_value &= switch_samples[i];
        }
        if (switch_sample_index >= NOS_SWITCH_SAMPLES) {
            switch_sample_index = 0;
        }
        switch_state[0] = (switch_value & SWITCH_0_MASK) >> SWITCH_0_PIN;
        switch_state[1] = (switch_value & SWITCH_1_MASK) >> SWITCH_1_PIN;
        switch_state[2] = (switch_value & SWITCH_2_MASK) >> SWITCH_2_PIN;
        switch_state[3] = (switch_value & SWITCH_3_MASK) >> SWITCH_3_PIN;
        //
        // Update global system data
        //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);

        xSemaphoreGive(semaphore_system_IO_data);
STOP_PULSE;
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}