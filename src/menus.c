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
#include "run_bump_modes.h"
#include "run_test_modes.h"

//==============================================================================
//  System menu tables
//==============================================================================
//  Data
//==============================================================================
//
// List of new menus structures
//
// menu_1     : primary system menu - select main mode
// menu_1_1   : gamepad menu
// menu_1_9   : test modes menu
// menu_1_9_1 : test mode 9 menu
//
#define     NOS_PRIMARY_MENU_MODES  9

const char* menu_1_strings[NOS_PRIMARY_MENU_MODES] = {
    "Mode Gamepad  ",
    "Mode Activity ",
    "Mode Bump     ",
    "Mode Follow   ",
    "Mode Program  ",
    "Mode Sketch   ",
    "Mode Lab      ",
    "Mode Distance ",
    "Mode TEST     ",
};

mode_function menu_1_mode_functions[NOS_PRIMARY_MENU_MODES] = {
    run_gamepad_modes,
    null_function,
    run_bump_modes,
    null_function,
    null_function,
    null_function,
    null_function,
    null_function,
    run_test_modes,
};

struct menu menu_1 = {
    NOS_PRIMARY_MENU_MODES - 1,  // does not include TEST mode on initialisation
    menu_1_strings,
    menu_1_mode_functions,
};

//==============================================================================
// menu_1_1 : gamepad modes
//
#define NOS_GAMEPAD_MODES   2

const char* menu_1_1_strings[NOS_GAMEPAD_MODES] = {
    "   G mode 0   ",
    "   G mode 1   ",
};

mode_function menu_1_1_mode_functions[NOS_GAMEPAD_MODES] = {
    execute_gamepad_activities,
    execute_gamepad_activities,
};

struct menu menu_1_1 = {
    NOS_GAMEPAD_MODES,
    menu_1_1_strings,
    menu_1_1_mode_functions,
};

//==============================================================================
// menu_1_3 : bump modes
//
#define NOS_BUMP_MODES   3

const char* menu_1_3_strings[NOS_BUMP_MODES] = {
    " Tape bump 1  ",       // reverse and spin 
    " Tape bump 2  ",       //
    " Wall bump    ",
};

mode_function menu_1_3_mode_functions[NOS_BUMP_MODES] = {
     run_bump_mode_0,
     run_bump_mode_1,
     run_bump_mode_2,
};

struct menu menu_1_3 = {
    NOS_BUMP_MODES,
    menu_1_3_strings,
    menu_1_3_mode_functions,
};

//==============================================================================
// menu_1_9 : test modes
//
#define     NOS_TEST_MODES      4

const char* menu_1_9_strings[NOS_TEST_MODES] = {
    "   Test 0     ",
    "   Test 1     ",
    "   Test 2     ",
    "   Test 3     ",  
};

mode_function menu_1_9_mode_functions[NOS_TEST_MODES] = {
    run_test_0, 
    run_test_1,
    run_test_2_menu,
    run_test_3,
};

struct menu menu_1_9 = {
    NOS_TEST_MODES,
    menu_1_9_strings,
    menu_1_9_mode_functions,
};

//==============================================================================
// menu_1_9_2 : test mode 9 menu
//
#define     NOS_TEST_MODE_2_OPTIONS      8

const char* menu_1_9_2_strings[NOS_TEST_MODE_2_OPTIONS] = {
    "log POT A     ",
    "log POT B     ",  
    "log spare     ",
    "log Vmotor    ",  
    "log IR right  ",  
    "log IR mid    ",  
    "log IR left   ", 
    "log POT C     " 
};

mode_function menu_1_9_2_mode_functions[NOS_TEST_MODE_2_OPTIONS] = {
    run_test_2, 
    run_test_2,
    run_test_2,
    run_test_2, 
    run_test_2,
    run_test_2,
    run_test_2, 
    run_test_2,
};

struct menu menu_1_9_2 = {
    NOS_TEST_MODES,
    menu_1_9_2_strings,
    menu_1_9_2_mode_functions,
};

//==============================================================================
//  Code
//==============================================================================
//  Execute test menu table
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
            error = menu_pt->func[mode_index](mode_index, 0);
            return error;
        }

        if (event_bits == PUSH_BUTTON_B_EVENT_MASK) {  // next mode
            mode_index = (mode_index < (menu_pt->nos_modes - 1)) ? (mode_index + 1) : 0;
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
};

// struct menu primary_menu = {
//     true,
//     8,      // does not include TEST mode on initialisation
//     {   
//         "Mode Gamepad  ",
//         "Mode Activity ",
//         "Mode Bump     ",
//         "Mode Follow   ",
//         "Mode Program  ",
//         "Mode Sketch   ",
//         "Mode Lab      ",
//         "Mode Distance ",
//         "Mode TEST     ", 
//     },
//     {
//         run_gamepad_modes,
//         null_function,
//         run_bump_modes,
//         null_function,
//         null_function,
//         null_function,
//         null_function,
//         null_function,
//         run_test_modes,
//     }
// };

// struct menu gamepad_mode_menu = {
//     false, 
//     2, 
//     {
//         "   G mode 0   ",
//         "   G mode 1   ",
//     },
//     {
//         execute_gamepad_activities,
//         execute_gamepad_activities,
//     }
// };

// struct menu bump_mode_menu = {
//     false,
//     3,
//     {
//         " Line bump 1  ",
//         " Line bump 2  ",
//         " Line bump 3  ",
//     },
//     {   
//         execute_bump_activities, 
//         execute_bump_activities,
//         execute_bump_activities,
//     }
// };

// struct menu test_mode_menu = {
//     false,
//     4,
//     {
//         "   Test 0     ",
//         "   Test 1     ",
//         "   Test 2     ",
//         "   Test 3     ",  
//     },
//     {   
//         run_test_0, 
//         run_test_1,
//         run_test_2_menu,
//         run_test_3,
//     }
// };

// struct menu test_mode_2_menu = {
//     false,
//     8,
//     {
//         "log POT A     ",
//         "log POT B     ",  
//         "log spare     ",
//         "log Vmotor    ",  
//         "log IR right  ",  
//         "log IR mid    ",  
//         "log IR left   ", 
//         "log POT C     " 
//     },
//     {   
//         run_test_2, 
//         run_test_2,
//         run_test_2,
//         run_test_2, 
//         run_test_2,
//         run_test_2,
//         run_test_2, 
//         run_test_2,
//     }
// };


