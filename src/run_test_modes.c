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
#include "common.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "run_test_modes.h"

#include "FreeRTOS.h"

//==============================================================================
//  Table for test mode selection
//==============================================================================

#define     MAX_NOS_MODES      16

struct menu {
    uint8_t         nos_modes;
    const char      *mode_strings[MAX_NOS_MODES];
    error_codes_te  (* const mode_function[MAX_NOS_MODES])(uint32_t);
};

struct menu test_mode_menu = {
    4,
    {   "   T mode 0   ",
        "   T mode 1   ",
        "   T mode 2   ",
        "   T mode 3   ",  },
    {   run_test_mode_0, 
        run_test_mode_1,
        run_test_mode_2,
        run_test_mode_3
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
        }
        if (event_bits == PUSH_BUTTON_C_EVENT_MASK) {  // previous mode
            mode_index = (mode_index == 0) ? (menu_pt->nos_modes - 1) : (mode_index- - 1);
        }
        if (event_bits == PUSH_BUTTON_D_EVENT_MASK) {    // exit to parent menu
            return OK;
        }
    }
}

//==============================================================================
// Main routine
//==============================================================================

error_codes_te run_test_modes(void) 
{
    run_menu(&test_mode_menu);
    return OK;
}



//==============================================================================
// Test routines
//==============================================================================

/**
 * @brief Print version and scroll LEDs
 * 
 * @return * error_codes_te 
 */
error_codes_te run_test_mode_0(uint32_t parameter)
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
 error_codes_te run_test_mode_1(uint32_t parameter)
{
    printf("Test mode 1\n");
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_mode_2(uint32_t parameter)
{
    printf("Test mode 2\n");
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_mode_3(uint32_t parameter)
{
    printf("Test mode 3\n");
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
