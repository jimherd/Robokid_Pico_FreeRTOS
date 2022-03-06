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
 * @note  Check for new low/high values
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
/**
 * @brief Send error into error queue
 * 
 * @param error_code 
 * @param task 
 */
void log_error(uint8_t error_code, task_t task)
{
error_message_t error_message;

    error_message.error_code = error_code;
    error_message.task       = task;
    xQueueSend(queue_motor_cmds, &error_message, portMAX_DELAY);
    return;
}

//==============================================================================
/**
 * @brief   wait for push buuton tobe pressed and released
 * 
 * @param   push_button     PUSH_BUTTON_A to PUSH_BUTTON_D
 * @param   time_out        max time to wait press
 * @return  uint32_t        length of buuton press in uS
 */
uint32_t wait_for_button_press(uint8_t push_button, uint32_t time_out)
{

// Wait for push switch to be pressed

    xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        (1 << push_button),
        pdFALSE,        //  don't clear bit
        pdFALSE,        
        time_out);

// Wait for push button to be released

        xEventGroupWaitBits  ( 
        eventgroup_push_buttons,
        (1 << (push_button + NOS_ROBOKID_PUSH_BUTTONS)),
        pdFALSE,
        pdFALSE,        
        time_out);

    return  system_IO_data.push_button_data[push_button].on_time;
}