/**
 * @file joystick.c
 * @author Jim Herd (you@domain.com)
 * @brief Implement joystick activities
 */

// Notes
//      Active switches are 
//          switch A = go/stop button
//          switch C = exit to main slection level
//          switch D = step through selections

#include <string.h>

#include "system.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "joystick.h"

#include "FreeRTOS.h"

error_codes_te run_joystick_mode(void)
{
EventBits_t             event_bits;
secondary_sys_modes_te  secondary_mode;
error_codes_te          error;

    secondary_mode = JOYSTICK_MODE_1;
    LCD_write_row(0, MESSAGE_ROW, mode_J[secondary_mode - (10 * JOYSTICK_MODE)]);
    SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
    set_leds(LED_FLASH, LED_FLASH, LED_OFF, LED_OFF);
    event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

    if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
        switch (secondary_mode) {
            case JOYSTICK_MODE_1 : {
                return (run_joystick_mode_1());
            }
            case JOYSTICK_MODE_2 : {
                  //  run_joystick_mode_2();
                break;
            }
            case JOYSTICK_MODE_3 : {
                  //  run_joystick_mode_3();
                break;
            }
            case SECONDARY_NULL_MODE :
            default : {
                break;
            }
        } 
    }
    if (event_bits == PUSH_BUTTON_B_EVENT_MASK) {
        if (secondary_mode == LAST_JOYSTICK_MODE) {
            secondary_mode = FIRST_JOYSTICK_MODE;
        } else {
            secondary_mode++;
        }
    }
    if (event_bits == PUSH_BUTTON_C_EVENT_MASK){
        NULL;   // ignore press of button C
    }
    if (event_bits == PUSH_BUTTON_D_EVENT_MASK){
        NULL;   // ignore press of button D
    }
    return OK;
}

/**
 * @brief Run Robokid vehicle with DPAD control
 * 
 * DPAD : forward, backward, left, right
 * SELECT : exit this mode
 * 
 * @note
 *  The same motor packet command structure is used to send both right
 * and left commands into the FreeRTOS queue.  This present no problems
 * as the queue entries are based on data copying and are not passed by
 * reference.
 * 
 */
error_codes_te run_joystick_mode_1(void)
{
struct gamepad_data_s    temp_gamepad_data;
struct motor_cmd_packet_s       motor_cmd_packet;
volatile uint32_t                        DPAD_code;
uint8_t left_cmd, right_cmd, left_PWM, right_PWM;

    if (gamepad_data.state == DISABLED) {
        return USB_CONTROLLER_NOT_CONNECTED;
    }

// get gamepad data

    FOREVER {
        xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
            memcpy(&temp_gamepad_data, &gamepad_data, sizeof(struct gamepad_data_s));
        xSemaphoreGive(semaphore_gamepad_data);
        if (temp_gamepad_data.button_SELECT == true) {
            break;
        }

// calculate motor commands

        DPAD_code = ((temp_gamepad_data.dpad_x << 8) + temp_gamepad_data.dpad_y);

        if (DPAD_code == DPAD_STOP) {
            right_cmd = MOTOR_BRAKE; left_cmd = MOTOR_BRAKE; 
            right_PWM = 0; left_PWM = 0;
        } else if (DPAD_code == DPAD_FORWARD) {
            right_cmd = MOVE; left_cmd = MOVE;
            right_PWM = +100; left_PWM = +100;
        } else if (DPAD_code == DPAD_BACKWARD) {
            right_cmd = MOVE; left_cmd = MOVE;
            right_PWM = -100; left_PWM = -100;
        } else if (DPAD_code == DPAD_SPIN_RIGHT) {
            right_cmd = MOVE; left_cmd = MOVE;
            right_PWM = -100; left_PWM = +100;
        } else if (DPAD_code == DPAD_SPIN_LEFT) {
            right_cmd = MOVE; left_cmd = MOVE;
            right_PWM = +100; left_PWM = -100;
        } else if (DPAD_code == ARC_FORWARD_RIGHT) {
            right_cmd = MOTOR_BRAKE; left_cmd = MOVE;
            right_PWM = 0; left_PWM = +100;
        } else if (DPAD_code == ARC_FORWARD_LEFT) {
            right_cmd = MOVE; left_cmd = MOTOR_BRAKE;
            right_PWM = +100; left_PWM = 0;
        } else if (DPAD_code == ARC_BACKWARD_RIGHT) {
            right_cmd = MOTOR_BRAKE; left_cmd = MOVE;
            right_PWM = 0; left_PWM = -100;
        } else if (DPAD_code == ARC_BACKWARD_RIGHT) {
            right_cmd = MOVE; left_cmd = MOTOR_BRAKE;
            right_PWM = -100; left_PWM = 0;
        }

// If Y-switch pressed then set half speed
        if (temp_gamepad_data.button_Y == true) {
            left_PWM = left_PWM >> 1;
            right_PWM = right_PWM >> 1;
        }

// send right motor command
        motor_cmd_packet.cmd = right_cmd;
        motor_cmd_packet.param1  = RIGHT_MOTOR;
        motor_cmd_packet.param2  = right_PWM;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);

// send left motor command ( See not at function header)
        motor_cmd_packet.cmd = left_cmd;
        motor_cmd_packet.param1  = LEFT_MOTOR;
        motor_cmd_packet.param2  = left_PWM;
        xQueueSend(queue_motor_cmds, &motor_cmd_packet, portMAX_DELAY);

        vTaskDelay(100/portTICK_PERIOD_MS);   // approx 10Hz reading of gamepad
    }
    return OK;
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