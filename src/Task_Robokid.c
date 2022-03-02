/**
 * @file   Task_Robokid.c
 * @author Jim Herd
 * @brief  Run user level Robokid system
 * @date   2022-02-28
 */
#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//==============================================================================
// Main task routine
//==============================================================================
void Task_Robokid(void *p) 
{
uint32_t    start_time, end_time;

motor_cmd_packet_t  motor_cmd_packet;

    FOREVER {
        motor_cmd_packet.cmd = MOVE;
        motor_cmd_packet.param1  = RIGHT_MOTOR;
        motor_cmd_packet.param2  = 75;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        motor_cmd_packet.cmd = MOVE;
        motor_cmd_packet.param1  = LEFT_MOTOR;
        motor_cmd_packet.param2  = 75;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        vTaskDelay(3000/portTICK_PERIOD_MS);

        motor_cmd_packet.cmd = MOTOR_BRAKE;
        motor_cmd_packet.param1  = RIGHT_MOTOR;
        motor_cmd_packet.param2  = 0;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        motor_cmd_packet.cmd = MOTOR_BRAKE;
        motor_cmd_packet.param1  = LEFT_MOTOR;
        motor_cmd_packet.param2  = 0;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}