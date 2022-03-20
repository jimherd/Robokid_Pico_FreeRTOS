/**
 * @file    Task_sounder.c
 * @author  Jim Herd
 * @brief   Drive a piezo sounder with varying frequences
 */
#include <stdio.h>

#include "system.h"
#include "common.h"
#include "tunes.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"

#include "FreeRTOS.h"

//==============================================================================
// Function prototypes
//==============================================================================

void sound_init(void);
void set_tone(uint8_t note);
void tone_on(uint8_t note);
void tone_off(void);

//==============================================================================
// PWM setup values for each frequency
//==============================================================================

const struct  {
    uint16_t    note_frequency;
    uint16_t    PWM_period;
} tone_info[NOS_OCTAVE_NOTES] = {
    FREQ_C,         (SOUNDER_COUNT_FREQ / FREQ_C),
    FREQ_CS,        (SOUNDER_COUNT_FREQ / FREQ_CS),
    FREQ_D,         (SOUNDER_COUNT_FREQ / FREQ_D),
    FREQ_DS,        (SOUNDER_COUNT_FREQ / FREQ_DS),
    FREQ_E,         (SOUNDER_COUNT_FREQ / FREQ_E),
    FREQ_F,         (SOUNDER_COUNT_FREQ / FREQ_F),
    FREQ_FS,        (SOUNDER_COUNT_FREQ / FREQ_FS),
    FREQ_G,         (SOUNDER_COUNT_FREQ / FREQ_G),
    FREQ_GS,        (SOUNDER_COUNT_FREQ / FREQ_GS),
    FREQ_A,         (SOUNDER_COUNT_FREQ / FREQ_A),
    FREQ_AS,        (SOUNDER_COUNT_FREQ / FREQ_AS),
    FREQ_B,         (SOUNDER_COUNT_FREQ / FREQ_B),
    SILENT_NOTE,    (SOUNDER_COUNT_FREQ / FREQ_C),
};

//==============================================================================
// Globals
//==============================================================================

uint8_t     sounder_slice_num;

struct tune_data_t test_tune = {
    true,                   // new
    &test_notes[0], 3,      // tune
    true,                   // enable
    1,                      // repeat count
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
uint8_t     index, repeat_count, duration_count;

    sound_init();
    
    set_tune_data(test_notes, NOS_NOTES(test_notes), true, 1);
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_SOUNDER_FREQUENCY_TICK_COUNT );

        xSemaphoreTake(semaphore_tune_data, portMAX_DELAY);

            if (tune_data.enable == false){
                xSemaphoreGive(semaphore_tune_data);
                continue;
            }

            if (tune_data.new == true) {  // must be updated tune
                tune_data.new = false;
                index = 0; 
                repeat_count = tune_data.repeat_count;
                note_pt = tune_data.note_pointer;
                duration_count = note_pt[index].duration_100mS;
                set_tone(note_pt[index].tone);
                index++; repeat_count--;
                xSemaphoreGive(semaphore_tune_data);
                continue;
            }

            if (duration_count > 0) {
                duration_count--;
                xSemaphoreGive(semaphore_tune_data);
                continue;
            } else {
                if (index >= tune_data.nos_notes) {     // tune complete
                    if (repeat_count == 0) {
                        tune_data.enable = false;
                        set_tone(SILENT_NOTE); 
                        xSemaphoreGive(semaphore_tune_data);
                        continue;
                    } else {  // restart tune
                        index = 0;  
                        note_pt = tune_data.note_pointer;
                        duration_count = note_pt[index].duration_100mS;
                        set_tone(note_pt[index].tone);
                        index++; repeat_count--;
                        xSemaphoreGive(semaphore_tune_data);
                        continue;
                    }
                } else {  // set next note
                    if (tune_data.enable == true) {
                        duration_count = note_pt[index].duration_100mS;
                        set_tone(note_pt[index].tone);
                        index++;
                    }
                }
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
    if (note == SILENT_NOTE) {
        pwm_set_wrap(sounder_slice_num, tone_info[NOTE_C].PWM_period);   // set to "C" 
        pwm_set_chan_level(sounder_slice_num, SOUNDER_SLICE_CHANNEL, 0); // no pulse
    } else {
        pwm_set_wrap(sounder_slice_num, tone_info[note].PWM_period);
        pwm_set_chan_level(sounder_slice_num, SOUNDER_SLICE_CHANNEL, (tone_info[note].PWM_period / 2));
    }
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
