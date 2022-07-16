/**
 * @file run_test_modes.c
 * @author Jim Herd (you@domain.com)
 * @brief Implement hidden test features
 */

// Notes
//      Active switches are 
//          switch A = go/stop button
//          switch C = exit to main slection level
//          switch D = step through selections

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "run_test_modes.h"

#include "FreeRTOS.h"

error_codes_te run_gamepad_modes(void) 
{
    return OK;
}