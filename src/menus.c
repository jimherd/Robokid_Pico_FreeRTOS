/**
 * @brief 
 * 
 * @author Jim Herd 
 */

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "common.h"
#include "menus.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "tunes.h"

#include "run_gamepad_modes.h"
#include "run_test_modes.h"

//==============================================================================
//  System menu tables
//==============================================================================

struct menu primary_menu = {
    true,
    8,      // does not include TEST mode on initialisation
    {   
        "Mode Gamepad  ",
        "Mode Activity ",
        "Mode Bump     ",
        "Mode Follow   ",
        "Mode Program  ",
        "Mode Sketch   ",
        "Mode Lab      ",
        "Mode Distance ",
        "Mode TEST     ", 
    },
    {
        run_gamepad_modes,
        null_function,
        null_function,
        null_function,
        null_function,
        null_function,
        null_function,
        null_function,
        run_test_modes,
    }
};

struct menu gamepad_mode_menu = {
    false,
    2,
    {
        "   G mode 0   ",
        "   G mode 1   ",
    },
    {   
        execute_gamepad_activities, 
        execute_gamepad_activities,
    }
};

struct menu test_mode_menu = {
    false,
    4,
    {
        "   Test 0     ",
        "   Test 1     ",
        "   Test 2     ",
        "   Test 3     ",  
    },
    {   
        run_test_0, 
        run_test_1,
        run_test_2,
        run_test_3,
    }
};

//==============================================================================
// Execute test menu table
//==============================================================================
/**
 * @brief 
 * 
 * @param menu_pt 
 * @return error_codes_te 
 */
error_codes_te  run_menu(struct menu *menu_pt)
{
uint8_t             mode_index;
EventBits_t         event_bits;
error_codes_te      error;

    mode_index = 0;

    FOREVER {
        LCD_write_row(0, MESSAGE_ROW, menu_pt->mode_strings[mode_index], true);
        SSD1306_set_text_area_scroller(STRING_COUNT(menu_button_data), menu_button_data);
        set_leds(LED_ON, LED_ON, LED_ON, LED_ON);
        event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

        if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {  // run current mode
            error = menu_pt->mode_function[mode_index](0);
            return error;
        }

        if (event_bits == PUSH_BUTTON_B_EVENT_MASK) {  // next mode
            mode_index = (mode_index < (menu_pt->nos_modes - 1)) ? (mode_index+ + 1) : 0;
            set_tune_data(next_mode, NOS_NOTES(next_mode), true, 1);
        }
        if (event_bits == PUSH_BUTTON_C_EVENT_MASK) {  // previous mode
            mode_index = (mode_index == 0) ? (menu_pt->nos_modes - 1) : (mode_index - 1);
            set_tune_data(last_mode, NOS_NOTES(last_mode), true, 1);
        }
        if (event_bits == PUSH_BUTTON_D_EVENT_MASK) {  // exit to parent menu
            return OK;
        }
    }
}



