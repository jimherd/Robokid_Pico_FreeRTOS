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

#include "run_gamepad_modes.h"
#include "run_test_modes.h"

//==============================================================================
//  System menu tables
//==============================================================================

struct menu primary_menu = {
    true,
    8,      // does not include TEST mode
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


