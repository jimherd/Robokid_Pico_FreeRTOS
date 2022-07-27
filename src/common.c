/**
 * @file    common.c
 * @author  Jim Herd (you@domain.com)
 * @brief   Some general functions
 * @date    2022-02-26
 */

#include "string.h"

#include "pico/stdlib.h"
#include "hardware/resets.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include "system.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"

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
void log_error(error_codes_te error_code, task_t task)
{
struct error_message_s error_message;

    error_message.error_code = error_code;
    error_message.task       = task;
    error_message.log_time   = time_us_64();
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
 * @return  uint32_t        length of button press in mS
 */
uint32_t wait_for_button_press(uint8_t push_button, uint32_t time_out_us)
{
EventBits_t event_bits;

    reset_push_button_timers();

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

// return pulse width in milliseconds

    return  system_IO_data.push_button_data[push_button].on_time;
}

/**
 * @brief wait for any of the user input push buttons to be pressed
 * 
 * @param time_out_us 
 * @return EventBits_t      test for bit set to identify button pressed
 */
EventBits_t wait_for_any_button_press(uint32_t time_out_us) 
{
EventBits_t    event_bits;

    reset_push_button_timers();

    event_bits = xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        PUSH_BUTTON_ON_EVENT_MASK,
        pdFALSE,
        pdFALSE,        // wait for any button to be pressed
        time_out_us);

    xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        PUSH_BUTTON_OFF_EVENT_MASK,
        pdFALSE,
        pdTRUE,            // wait for all buttons to be released
        time_out_us);

    return (event_bits & PUSH_BUTTON_ON_EVENT_MASK);
}

/**
 * @brief clear push button timers to measure next presses
 * 
 * @param push_button 
 */
void reset_push_button_timers(void)
{
    xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
        for (uint8_t index = 0; index < NOS_ROBOKID_PUSH_BUTTONS; index++) {
            system_IO_data.push_button_data[index].on_time = 0;
        }
    xSemaphoreGive(semaphore_system_IO_data);
    return;
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
void LCD_write_row(uint8_t font, uint8_t row, const char *row_string, bool invert) 
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
        LCD_row_data[row_data_index].invert = invert;
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

/**
 * @brief Set the leds object
 * 
 * @param LED_A_state   LED_OFF, LED_FLASH, LED_ON, or LED_NO_CHANGE
 * @param LED_B_state                   "
 * @param LED_C_state                   "
 * @param LED_D_state                   "
 */
void set_leds(LED_state_te LED_A_state,  
              LED_state_te LED_B_state, 
              LED_state_te LED_C_state,
              LED_state_te LED_D_state)
{
    system_IO_data.LED_data[LED_A].state = LED_A_state;
    system_IO_data.LED_data[LED_B].state = LED_B_state;
    system_IO_data.LED_data[LED_C].state = LED_C_state;
    system_IO_data.LED_data[LED_D].state = LED_D_state;
    return;
}

/**
 * @brief Set the n LED
 * 
 * @param nos_LEDS  1=light A, 2=light AB, 3=light ABC, 4=light ABCD
 * @return * void 
 * 
 * Light LED in sequence. values
 * 
 *  0       all LEDs off
 * 1->4     light sequence of LEDs
 * >4       light all 4 LEDs
 */
void set_n_LEDS (uint8_t nos_LEDS)
{
    switch (nos_LEDS) {
        case 0 : set_leds(LED_OFF, LED_OFF, LED_OFF, LED_OFF); break;
        case 1 : set_leds(LED_ON , LED_OFF, LED_OFF, LED_OFF); break;
        case 2 : set_leds(LED_ON , LED_ON , LED_OFF, LED_OFF); break;
        case 3 : set_leds(LED_ON , LED_ON , LED_ON , LED_OFF); break;
        default: set_leds(LED_ON , LED_ON , LED_ON , LED_ON ); break;
    }
}

/**
 * @brief Reset USB subsystem
 * 
 */
void reset_USB(void)
{
     reset_block(RESETS_RESET_USBCTRL_BITS);
     unreset_block_wait(RESETS_RESET_USBCTRL_BITS);
}

error_codes_te null_function(uint32_t parameter)
{
    return OK;
}

//==============================================================================
/**
 * @brief prime free buffer queue with pointers to all free buffers
 * 
 */
void prime_free_buffer_queue(void)
{
struct string_buffer_s free_buffer_index;

    for (uint8_t i = 0; i < NOS_STRING_BUFFERS; i++) {
        free_buffer_index.buffer_index  = i;
        xQueueSend(queue_free_buffers, &free_buffer_index, portMAX_DELAY);
    }
    return;
}

//==============================================================================
/**
 * @brief send '\0' terminated string to print task
 * 
 * 1. get index of free buffer
 * 2. load buffer
 * 3. send index to print task
 */
void print_string(char *string_pt)
{
struct string_buffer_s free_buffer_index;

    xQueueReceive(queue_free_buffers, &free_buffer_index,  portMAX_DELAY);
    uint32_t index = free_buffer_index.buffer_index;
    strncpy(string_buffers[index], string_pt, (STRING_WIDTH-1));
    xQueueSend(queue_string_buffers, &free_buffer_index, portMAX_DELAY);

    return;
}


