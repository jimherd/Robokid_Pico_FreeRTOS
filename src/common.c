/**
 * @file    common.c
 * @author  Jim Herd (you@domain.com)
 * @brief   Some general functions
 * @date    2022-02-26
 */

#include    "common.h"

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