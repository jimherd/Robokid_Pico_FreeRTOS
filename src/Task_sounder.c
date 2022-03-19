/**
 * @file    Task_sounder.c
 * @author  Jim Herd
 * @brief   Drive a piezo sounder with varying frequences
 */
#include <stdio.h>

#include "system.h"
#include "common.h"
#include "sound.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"

#include "FreeRTOS.h"

//==============================================================================
// Function rototypes
//==============================================================================

void sound_init(void);
void set_tone(uint8_t note);
void tone_on(uint8_t note);
void tone_off(void);

//==============================================================================
// Globals
//==============================================================================

uint8_t     sounder_slice_num;

struct note_data_t test_notes[8] = {
    {FREQ_A, 50},
    {SILENT_NOTE, 20},
    {FREQ_B, 50}
};

struct tune_data_t test_tune = {
    &test_notes[0],
    true, true,
    0, 0, 0
};

extern struct tune_data_t test_tune;

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
struct note_data_t *note_pt;
uint8_t     index;

    sound_init();
    set_tune_data(test_notes, true, 1);
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_SOUNDER_FREQUENCY_TICK_COUNT );

        xSemaphoreTake(semaphore_tune_data, portMAX_DELAY);
            if (tune_data.enable == false){
                break;
            }
            if (tune_data.new == true) {  // must be updated tune
                tune_data.new = false;
                tune_data.note_index = 0;
                note_pt = tune_data.note_pointer;
                tune_data.note_duration_count = note_pt[0].duration_100mS;
                set_tone(note_pt[0].tone);
            }
            if (tune_data.note_duration_count > 0) {
                tune_data.note_duration_count--;
                break;
            } else {
                index = tune_data.note_index;
                tune_data.note_index++;         // step to next tone
                tune_data.note_pointer;
                // note_pt++ ;
                tune_data.note_duration_count = note_pt[index].duration_100mS;
                set_tone(note_pt[index].tone);
            }
        xSemaphoreGive(semaphore_tune_data);
    }
}

//==============================================================================
/**
 * @brief Initialse sound system
 * 
 */
void sound_init(void)
{

    gpio_set_function(SOUNDER_PIN_A, GPIO_FUNC_PWM);
    sounder_slice_num = pwm_gpio_to_slice_num(SOUNDER_PIN_A);

    pwm_set_clkdiv(sounder_slice_num, SOUNDER_PWM_CLK_DIV);
    pwm_set_wrap(sounder_slice_num, 0);
    pwm_set_chan_level(sounder_slice_num, SOUNDER_SLICE_CHANNEL, 0);
    pwm_set_enabled(sounder_slice_num, true);
 
    return;
}

//==============================================================================
/**
 * @brief Set the tone object
 * 
 * @param note  range 0 to NOS_OCTAVE_NOTES (13)
 */
void set_tone(uint8_t note)
{
    pwm_set_wrap(sounder_slice_num, tone_info[note].PWM_period);
    pwm_set_chan_level(sounder_slice_num, SOUNDER_SLICE_CHANNEL, (tone_info[note].PWM_period / 2));
    return;
}

void tune_off(void)
{
    xSemaphoreTake(semaphore_tune_data, portMAX_DELAY);
        tune_data.enable = false;
    xSemaphoreGive(semaphore_tune_data);

}

void tune_on(void)
{
    xSemaphoreTake(semaphore_tune_data, portMAX_DELAY);
        tune_data.enable = true;
    xSemaphoreGive(semaphore_tune_data);

}
