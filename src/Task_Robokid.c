/**
 * @file   Task_Robokid.c
 * @author Jim Herd
 * @brief  Run user level Robokid system
 * @date   2022-02-28
 */
#include "system.h"
#include "common.h"
#include "OLED_128X64.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "tunes.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define WAIT_BUTTON_PRESSED(BUTTON)  xEventGroupWaitBits( eventgroup_push_buttons, (1 << BUTTON), false, false, portMAX_DELAY )
#define WAIT_BUTTON_RELEASED(BUTTON)  xEventGroupWaitBits( eventgroup_push_buttons, (1 << ((BUTTON) + 4)), false, false, portMAX_DELAY )

//==============================================================================
// Main task routine
//==============================================================================
void Task_Robokid(void *p) 
{
uint32_t    start_time, end_time;
uint8_t     index;

struct motor_cmd_packet_s  motor_cmd_packet;

// delay to allow system tasks to be established

#define     TASK_ROBOKID_START_DELAY_SECONDS    10
#define     HALF_SECOND      (500/portTICK_PERIOD_MS)

    for (index = 0; index < (TASK_ROBOKID_START_DELAY_SECONDS * 2); index++) {
        LCD_write_row(0, MESSAGE_ROW, system_busy[index % 4]);
        vTaskDelay(HALF_SECOND);
    }
    LCD_write_row(0, MESSAGE_ROW, wait_start[0]);

 //   xEventGroupWaitBits( eventgroup_push_buttons, PUSH_BUTTON_A, false, false, portMAX_DELAY );
    WAIT_BUTTON_PRESSED(PUSH_BUTTON_A);
    WAIT_BUTTON_RELEASED(PUSH_BUTTON_A);
    set_tune_data(beep, NOS_NOTES(beep), true, 1);
    
    FOREVER {


    }
}

        // motor_cmd_packet.cmd = MOVE;
        // motor_cmd_packet.param1  = RIGHT_MOTOR;
        // motor_cmd_packet.param2  = 75;
        // xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        // motor_cmd_packet.cmd = MOVE;
        // motor_cmd_packet.param1  = LEFT_MOTOR;
        // motor_cmd_packet.param2  = 75;
        // xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        // vTaskDelay(3000/portTICK_PERIOD_MS);

        // motor_cmd_packet.cmd = MOTOR_BRAKE;
        // motor_cmd_packet.param1  = RIGHT_MOTOR;
        // motor_cmd_packet.param2  = 0;
        // xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        // motor_cmd_packet.cmd = MOTOR_BRAKE;
        // motor_cmd_packet.param1  = LEFT_MOTOR;
        // motor_cmd_packet.param2  = 0;
        // xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);
        // vTaskDelay(3000/portTICK_PERIOD_MS);