/**
 * @brief 
 * 
 * @author Jim Herd 
 */

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "menus.h"

#include "run_test_modes.h"

//==============================================================================
//  System menu tables
//==============================================================================

struct menu test_mode_menu = {
    4,
    {   "   Test 0     ",
        "   Test 1     ",
        "   Test 2     ",
        "   Test 3     ",  },
    {   run_test_0, 
        run_test_1,
        run_test_2,
        run_test_3,
    }
};


