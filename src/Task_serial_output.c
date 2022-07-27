/**
 * @file Task_serial_output.c
 * @author Jim Herd
 * @brief   output ASCII strings 
 * 
 */
#include <string.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


//==============================================================================
// Main task routine
//==============================================================================
void Task_serial_output(void *p) 
{

struct string_buffer_s      buffer_index;
TickType_t                  xLastWakeTime;
TickType_t                  start_time, end_time;
error_codes_te              error;


    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xQueueReceive(queue_string_buffers, &buffer_index,  portMAX_DELAY);
        start_time = time_us_32();

        puts(string_buffers[buffer_index.buffer_index]);
    
        xQueueSend(queue_free_buffers, &buffer_index, portMAX_DELAY);
        end_time = time_us_32();
        update_task_execution_time(TASK_SERIAL_OUTPUT, start_time, end_time);
    }
}

