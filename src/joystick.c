/**
 * @file joystick.c
 * @author Jim Herd (you@domain.com)
 * @brief Implement joystick activities
 */

// Notes
//
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
    LCD_write_row(0, MESSAGE_ROW, main_modes[PRIMARY_NULL_MODE]);
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
 */
error_codes_te run_joystick_mode_1(void)
{
struct SNES_gamepad_report_s    temp_gamepad_data;
struct motor_cmd_packet_s       motor_cmd_packet;
uint32_t                        DPAD_code;

    if (gamepad_data.state == DISABLED) {
        return USB_CONTROLLER_NOT_CONNECTED;
    }

// get gamepad data

    FOREVER {
        xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
            memcpy(&temp_gamepad_data, &gamepad_data, sizeof(struct SNES_gamepad_report_s));
        xSemaphoreGive(semaphore_gamepad_data);
        if (temp_gamepad_data.button_SELECT == true) {
            break;
        }

// calculate motor commands

        DPAD_code = ((temp_gamepad_data.dpad_y << 8) + temp_gamepad_data.dpad_x);

        if (DPAD_code == DPAD_STOP) {

        } else if (DPAD_code == DPAD_FORWARD) {

        } else if (DPAD_code == DPAD_BACKWARD) {

        } else if (DPAD_code == DPAD_SPIN_RIGHT) {

        } else if (DPAD_code == DPAD_SPIN_LEFT) {

        } else if (DPAD_code == ARC_FORWARD_RIGHT) {

        } else if (DPAD_code == ARC_FORWARD_LEFT) {

        } else if (DPAD_code == ARC_BACKWARD_RIGHT) {

        } else if (DPAD_code == ARC_BACKWARD_RIGHT) {

        }

        vTaskDelay(100/portTICK_PERIOD_MS);   // approx 10Hz reading og gamepad
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