/**
 * @file    common.h
 * @author  Jim Herd (you@domain.com)
 * @brief   Prototypes for common.c
 * @date    2022-02-26
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include "system.h"

void update_task_execution_time(task_t task, uint32_t start_time, uint32_t end_time);
void log_error(error_codes_te error_code, task_t task);
void reset_push_button_timers(void);
uint32_t wait_for_button_press(uint8_t push_button, uint32_t time_out);
EventBits_t wait_for_any_button_press(uint32_t time_out_us);
void LCD_write_row(uint8_t font, uint8_t row, const char *row_string, bool invert);
void set_tune_data(struct note_data_s *notes, uint16_t nos_notes, bool enable, uint32_t repeat_count);
void tune_off(void);
void tune_on(void);
void set_leds(LED_state_te LED_A_state,  
              LED_state_te LED_B_state, 
              LED_state_te LED_C_state,
              LED_state_te LED_D_state);
void set_n_LEDS (uint8_t nos_LEDS);
void reset_USB(void);
error_codes_te null_function(uint32_t parameter);


#endif