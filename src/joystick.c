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



#include "system.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "joystick.h"

#include "FreeRTOS.h"

void run_joystick_mode(void)
{
EventBits_t                 event_bits;
secondary_sys_modes_te secondary_mode;

    secondary_mode = JOYSTICK_MODE_1;
    LCD_write_row(0, MESSAGE_ROW, main_modes[PRIMARY_NULL_MODE]);
    SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
    set_leds(LED_FLASH, LED_OFF, LED_FLASH, LED_FLASH);
    event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

    if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
        switch (secondary_mode) {
            case JOYSTICK_MODE_1 : {
                  //  run_joystick_mode();
                 break;
            }
            case JOYSTICK_MODE_2 : {
                  //  run_joystick_mode();
                break;
            }
            case JOYSTICK_MODE_3 : {
                  //  run_joystick_mode();
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
    return;
}