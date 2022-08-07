/**
 * @file Task_error.c
 * @author Jim Herd
 * @brief log received errors in circular buffer and update system error
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

// circular buffer to hold last LOG_SIZE errors

//==============================================================================
void Task_error(void *p) 
{
    error_data.error_log_ptr = 0;
    error_data.error_count   = 0;
    
    FOREVER {

    // wait for error message and copy into log queue

        xQueueReceive(queue_error_messages, &error_data.error_message_log[error_data.error_log_ptr],  portMAX_DELAY);

    // update global error flag. Blink task will flash this error code

        xSemaphoreTake(semaphore_system_status, portMAX_DELAY);
            system_status.error_state = error_data.error_message_log[error_data.error_log_ptr].error_code;
        xSemaphoreGive(semaphore_system_status);

    // update circular buffer pointer
    
        if (error_data.error_log_ptr >= LOG_SIZE) { 
            error_data.error_log_ptr = 0;
        }
        error_data.error_count++;
    }
}

