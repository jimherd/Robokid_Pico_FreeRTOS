/**
 * @file    Task_serial_output .c
 * @author  Jim Herd
 * @brief   output ASCII strings 
 * 
 * @note
 * Manages the printing of text strings to the com port.
 * 
 * Task uses a fixed set of statically allocated 2D array of buffers.
 * Indices to free line buffers are held in a queue.  A task that wants 
 * to print a line of text reads an idex from the queue, looafds the 
 * indexed line buffer with text, then passes the index to the print
 * task for printing.  Subsequently, the print task returns the index
 * to the free buffer queue to be reused.  
 * 
 * Method ensures that buffers are used in a safe manner.
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

struct string_buffer_s  buffer_index;
TickType_t              start_time, end_time;

//TickType_t            xLastWakeTime;
//error_codes_te        error;


    //xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xQueueReceive(queue_print_string_buffers, &buffer_index,  portMAX_DELAY);
        start_time = time_us_32();

        printf("%s", print_string_buffers[buffer_index.buffer_index]);
    
        xQueueSend(queue_free_buffers, &buffer_index, portMAX_DELAY);
        end_time = time_us_32();
        update_task_execution_time(TASK_SERIAL_OUTPUT, start_time, end_time);
    }
}

