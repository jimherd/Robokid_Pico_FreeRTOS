/**
 * @file run_test_modes.c
 * @author Jim Herd
 * @brief Implement hidden test features
 */

// Notes
//      These tests are used to check various aspects of the robot operation.
//      Mostly, they exercise some aspect of the system and dump data to the
//      rp2040 uart channels for collection by a PC.  Where possible, this
//      data will be in a comma delimited ASCI format to allow it to be
//      imported into excel for display.
//
//      Tests
//          a. Print version and cycle LEDs
//          b. Read raw battery voltage value and send to com port
//          c. 
//
//      Active switches are 
//          switch A = go/stop button
//          switch C = exit to main slection level
//          switch D = step through selections

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "menus.h"
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "run_test_modes.h"

#include "FreeRTOS.h"

//==============================================================================
// Main routine
//==============================================================================

error_codes_te run_test_modes(uint32_t parameter) 
{
    return run_menu(&test_mode_menu);
}

//==============================================================================
// Test routines
//==============================================================================

/**
 * @brief Print version and scroll LEDs
 * 
 * @return * error_codes_te 
 */
error_codes_te run_test_0(uint32_t parameter)
{
    printf("Major Verion : %d\n", MAJOR_VERSION);
    printf("Minor Verion : %d\n", MINOR_VERSION);
    printf("Patch Verion : %d\n", PATCH_VERSION);

    set_leds(LED_ON, LED_ON, LED_ON, LED_ON);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_OFF, LED_OFF, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_ON, LED_OFF, LED_OFF, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_ON, LED_OFF, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_OFF, LED_ON, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_OFF, LED_OFF, LED_ON);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_OFF, LED_ON, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_ON, LED_OFF, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_ON, LED_OFF, LED_OFF, LED_OFF);
    vTaskDelay(TWO_SECONDS);
    set_leds(LED_OFF, LED_OFF, LED_OFF, LED_OFF);

    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_1(uint32_t parameter)
{
    printf("Test 1\n");
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_2(uint32_t parameter)
{
    printf("Test 2\n");
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_3(uint32_t parameter)
{
    printf("Test 3\n");
    return OK;
}

//==============================================================================
// Select and run appropriate test routine
//==============================================================================

// error_codes_te run_test_modes(void) 
// {
// EventBits_t             event_bits;
// secondary_sys_modes_te  secondary_mode;
// error_codes_te          error;

//     secondary_mode = TEST_MODE_0;
//     LCD_write_row(0, MESSAGE_ROW, mode_T[secondary_mode - (10 * TEST_MODE)], true);
//     SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
//     set_leds(LED_ON, LED_ON, LED_OFF, LED_OFF);
//     event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

//     if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
//         switch (secondary_mode) {
//             case TEST_MODE_0 : {
//                 return (run_test_mode_0(0));
//                 break;
//             }
//             case TEST_MODE_1 : {
//                 return(run_test_mode_1(0));
//                 break;
//             }
//             default : {
//                 break;
//             }
//         } 
//     }
//     return OK;
// }
