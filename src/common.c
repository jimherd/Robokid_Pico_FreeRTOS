/**
 * @file    common.c
 * @author  Jim Herd (you@domain.com)
 * @brief   Some general functions
 * @date    2022-02-26
 */

#include "FreeRTOS.h"

#include "system.h"
#include "common.h"

//==============================================================================
/**
 * @brief Calculate task execution time and record
 * @note  Check for new low/high values and record
 * @param task 
 * @param start_time    units of uS
 * @param end_time      units of uS
 */
void update_task_execution_time(task_t task, uint32_t start_time, uint32_t end_time) 
{
uint32_t delta_time;

    if (end_time > start_time) {
        delta_time = end_time - start_time;
    } else {
        delta_time = (UINT32_MAX - start_time) + end_time + 1;
    }

    task_data[task].last_exec_time = delta_time;

    if (delta_time < task_data[task].lowest_exec_time) {
        task_data[task].lowest_exec_time = delta_time;
    }
    
    if (delta_time > task_data[task].highest_exec_time) {
        task_data[task].highest_exec_time = delta_time;
    }
}

//==============================================================================
/**
 * @brief Send error into error queue
 * 
 * @param error_code 
 * @param task 
 */
void log_error(uint8_t error_code, task_t task)
{
struct error_message_s error_message;

    error_message.error_code = error_code;
    error_message.task       = task;
    xQueueSend(queue_motor_cmds, &error_message, portMAX_DELAY);
    return;
}

//==============================================================================
/**
 * @brief   wait for push buuton to be pressed and released
 * 
 * Mechanism is based on FreeRTOS events.  Each push button is
 * connected to an ON and OFF event that cane be used by
 * the "xEventGroupWaitBits" routine.  This saves using a busy
 * wait method as the CPU is released during the wait time.
 * 
 * An infinite timeout is specified by using the value "portMAX_DELAY"
 * 
 * @param   push_button     PUSH_BUTTON_A to PUSH_BUTTON_D
 * @param   time_out        max time to wait press
 * @return  uint32_t        length of buuton press in mS
 */
uint32_t wait_for_button_press(uint8_t push_button, uint32_t time_out_us)
{
EventBits_t event_bits;

// Wait for push switch to be pressed

    event_bits = xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        (1 << push_button),
        pdFALSE,        //  don't clear bit
        pdFALSE,        
        time_out_us);

// Wait for push button to be released

    event_bits = xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        (1 << (push_button + NOS_ROBOKID_PUSH_BUTTONS)),
        pdFALSE,
        pdFALSE,        
        time_out_us);

// return pulse width in microseconds

    return  system_IO_data.push_button_data[push_button].on_time;
}

//==============================================================================
/**
 * @brief Load string into row of LCD buffer area
 * 
 * Access to buffer is protected by a MUTEX semaphore
 * This buffer will be output by the display task.
 * String is padded to maximum size with spaces
 * 
 * @param font          font of string
 * @param row           window in range 1 to 4
 * @param row_string    string to be 
 */
void LCD_write_row(uint8_t font, uint8_t row, const char *row_string) 
{
bool    end_detect;
uint8_t row_data_index;

    row_data_index = row - 1;
    end_detect = false;
    xSemaphoreTake(semaphore_LCD_data, portMAX_DELAY);
        for (uint8_t index = 0; index < font_data[font].chars_per_row; index++) { 
            if (end_detect == true) {
                LCD_row_data[row_data_index].row_string[index] = ' ';
            } else if (row_string[index] == '\0') {
                end_detect = true;
                LCD_row_data[row_data_index].row_string[index] = ' ';
            } else {
                LCD_row_data[row_data_index].row_string[index] = row_string[index];
            }
        }
        LCD_row_data[row_data_index].row_string[font_data[font].chars_per_row] = '\0';
        LCD_row_data[row_data_index].font = font;
        LCD_row_data[row_data_index].dirty_bit = true;
    xSemaphoreGive(semaphore_LCD_data);
    return;
}

//==============================================================================
/**
 * @brief Set the tune data object
 * 
 * @param tune              pointer to a tune
 * @param enable            play tune
 * @param repeat_count      Number of times tune is to be repeated
 */
void set_tune_data(struct note_data_s *notes, uint16_t nos_notes, bool enable, uint32_t repeat_count)
{
    xSemaphoreTake(semaphore_tune_data, portMAX_DELAY);
        tune_data.new                   = true;
        tune_data.note_pointer          = notes;
        tune_data.nos_notes             = nos_notes;
        tune_data.enable                = enable;
        tune_data.repeat_count          = repeat_count;
    xSemaphoreGive(semaphore_tune_data);
    return;
}

