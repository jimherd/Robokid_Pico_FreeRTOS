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
#include "joystick.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define     TASK_ROBOKID_START_DELAY_SECONDS    10
#define     HALF_SECOND      (500/portTICK_PERIOD_MS)

#define  LONG_PRESS_MS  4000        // 4 seconds

//==============================================================================
// Main task routine
//==============================================================================
void Task_Robokid(void *p) 
{
uint32_t                    start_time, end_time;
uint8_t                     index;
//struct motor_cmd_packet_s   motor_cmd_packet;
volatile EventBits_t                 event_bits;
primary_sys_modes_te        primary_mode, first_mode, last_mode;
uint32_t                    press_time;
volatile uint32_t temp;

    temp = PUSH_BUTTON_B_EVENT_MASK;

    for (index = 0; index < (TASK_ROBOKID_START_DELAY_SECONDS * 2); index++) {
        LCD_write_row(0, MESSAGE_ROW, system_busy[index % 4]);
        vTaskDelay(HALF_SECOND);
    }

    SSD1306_set_text_area_scroller(STRING_COUNT(wait_start), wait_start);

    first_mode = JOYSTICK_MODE;
    last_mode = EXPERIMENT_MODE - 1;  // hide experiment mode from normal operation
    LCD_write_row(0, MESSAGE_ROW, system_ready[0]);
    press_time = wait_for_button_press(PUSH_BUTTON_A, portMAX_DELAY);
    if (press_time > LONG_PRESS_MS) {
        last_mode = EXPERIMENT_MODE;   // expose experiment mode if press time is > xxxx
    }
    set_tune_data(beep, NOS_NOTES(beep), true, 1);

    primary_mode = PRIMARY_NULL_MODE;

    FOREVER {
        LCD_write_row(0, MESSAGE_ROW, main_modes[primary_mode]);
        SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
        set_leds(LED_FLASH, LED_OFF, LED_FLASH, LED_FLASH);
        event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

        if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
            switch (primary_mode) {
                case PRIMARY_NULL_MODE : {  // no mode selected as yet
                    break;
                }
                case JOYSTICK_MODE : {
                    run_joystick_mode();
                    break;
                }
                default : {
                    break;
                }
            }  // end of switch 
        }
        if (event_bits == PUSH_BUTTON_B_EVENT_MASK) {
            if (primary_mode == last_mode) {
                primary_mode = first_mode;
            } else {
                primary_mode++;
            }
        }
        if (event_bits == PUSH_BUTTON_C_EVENT_MASK){
            NULL;   // ignore press of button C
        }
        if (event_bits == PUSH_BUTTON_D_EVENT_MASK){
            NULL;   // ignore press of button D
        }
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