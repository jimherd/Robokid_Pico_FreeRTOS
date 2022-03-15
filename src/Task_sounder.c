/**
 * @file    Task_sounder.c
 * @author  Jim Herd
 * @brief   Drive a piezo sounder with varying frequences
 */
#include <stdio.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"



//==============================================================================
// function prototypes
//==============================================================================


//==============================================================================
// Task code
//==============================================================================
/**
 * @brief Task to control display of SSD1306 display
 * 
 * @param p 
 */
void Task_sounder (void *p) 
{
TickType_t  xLastWakeTime;
BaseType_t  xWasDelayed;

    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_SOUNDER_FREQUENCY_TICK_COUNT );
        xSemaphoreTake(semaphore_tone_data, portMAX_DELAY);
            // tone_data.tone = tone;
            // tone_data.duration_100mS = duration;
            // tone_data.duration_count = 0;
            // tone_data.enable = true;
        xSemaphoreGive(semaphore_tone_data);
    }
}

//==============================================================================
/**
 * @brief Initialse sound system
 * 
 */
void sound_init(void)
{
    return;
}

//==============================================================================
/**
 * @brief Set the tone object
 * 
 * @param note 
 * @param duration 
 */
void set_tone(uint8_t note, uint16_t duration)
{

    return;
}

// void play_tune(const sound_file_t  *sound_file_pt);
// void stop_tune(void);
// void tone_on(uint8_t note);
// void tone_off(void);
// void beep(uint8_t note, uint8_t count);
