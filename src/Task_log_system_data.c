/**
 * @file Task_log_system_data.c
 * @author Jim Herd
 * @brief log task data
 * 
 * Store task stack data in task structure
 */

//#include <stdio.h>
//#include <stdlib.h>

#include <string.h>

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "FreeRTOS.h"
#include "semphr.h"

static TaskStatus_t task_info;
static struct task_data_s temp_task_data[NOS_TASKS];

void Task_log_system_data(void *p) 
{
TickType_t  xLastWakeTime;
BaseType_t  xWasDelayed;
uint8_t     index;
TickType_t  start_time, end_time;

    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_LOG_FREQUENCY_TICK_COUNT );
        start_time = time_us_32();
    xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&temp_task_data[0], &system_IO_data.task_data[0], (NOS_TASKS *  sizeof(struct task_data_s)));
    xSemaphoreGive(semaphore_system_IO_data);

        for(index=0 ; index < NOS_TASKS ; index++) {
            // vTaskGetInfo(system_IO_data.task_data[index].task_handle,
            //                 &task_info,
            //                 pdTRUE,
            //                 eInvalid);
            // temp_task_data[index].pxStackBase        = task_info.pxStackBase;
            // temp_task_data[index].StackHighWaterMark = task_info.usStackHighWaterMark;
        }
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&system_IO_data.task_data[0], &temp_task_data[0], (NOS_TASKS *  sizeof(struct task_data_s)));
        xSemaphoreGive(semaphore_system_IO_data);
    }
    end_time = time_us_32();
    update_task_execution_time(TASK_LOG, start_time, end_time);
}
