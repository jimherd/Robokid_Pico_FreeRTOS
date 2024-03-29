/**
 * @file run_bump_modes.c
 * @author Jim Herd
 * @brief Implement robot bump activities
 */

// Notes
//      Active switches are 
//          switch A = go/stop button
//          switch B = read POT settings
//          switch C = exit to main slection level
//          switch D = step through selections

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "run_bump_modes.h"
#include "menus.h"

#include "FreeRTOS.h"

//==============================================================================

enum b_modes {LINE_BUMP_MODE, LINE_BUG_BUMP_MODE, WALL_BUMP_MODE};

struct menu bump_mode_menu = {
    false,
    3,
    {
        " Line bump 1  ",
        " Line bump 2  ",
        " Line bump 3  ",
    },
    {   
        execute_bump_activities, 
        execute_bump_activities,
        execute_bump_activities,
    }
};

// const char* mode_B_button_data[4] = {
//     "A: go/stop    ",
//     "B: Next mode  ",
//     "C: Read POTs  ",
//     "D: Exit mode  "  
// };

const char* config_button_data[4] = {
    "A: Go         ",
    "B: ---------  ",
    "C: Read POTs  ",
    "D: Exit mode  "  
};

//==============================================================================


error_codes_te run_bump_modes(uint8_t mode_index, uint32_t parameter) 
{
    return run_menu(&bump_mode_menu);
}

error_codes_te execute_bump_activities(uint8_t mode_index, uint32_t  parameter)
{



    return OK;
}
