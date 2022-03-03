/**
 * @file Task_blink_LED.c
 * @author Jim Herd
 * @brief Flash error code
 * @version 0.1
 * @date 2022-01-10
 */
#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"

void Task_error(void *p) 
{
uint32_t            start_time, end_time;
error_message_t     error_message;

    FOREVER {
        xQueueReceive(queue_error_messages, &error_message,  portMAX_DELAY);
        start_time = time_us_32();
        
        end_time = time_us_32();
        update_task_execution_time(TASK_ERROR, start_time, end_time);
    }
}

