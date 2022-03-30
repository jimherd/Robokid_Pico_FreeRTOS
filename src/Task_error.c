/**
 * @file Task_error.c
 * @author Jim Herd
 * @brief log received errors and update system error
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

struct error_message_s     error_message_log[LOG_SIZE];
uint8_t error_log_ptr;

//==============================================================================
void Task_error(void *p) 
{
    error_log_ptr = 0;
    
    FOREVER {
        xQueueReceive(queue_error_messages, &error_message_log[error_log_ptr],  portMAX_DELAY);
    
    // added time stamp to logged error message

        error_message_log[error_log_ptr].log_time = time_us_32();

    // update global error flag. Blink task will flash this error code

        xSemaphoreTake(semaphore_system_status, portMAX_DELAY);
            system_status.error_state = error_message_log[error_log_ptr].error_code;
        xSemaphoreGive(semaphore_system_status);

    // update circular buffer

        if (error_log_ptr >= LOG_SIZE) { 
            error_log_ptr = 0;
        }
    }
}

