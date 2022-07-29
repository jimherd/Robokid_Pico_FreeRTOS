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
//          0. Print version and cycle LEDs
//          1. Read raw data from CD4051 8-channel analogue inputs
//          2. 

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
// Global data
//==============================================================================

char    temp_string[128];
struct  analogue_data_s  temp_analogue_data;

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
    sprintf(temp_string, "Major Verion : %d\n", MAJOR_VERSION);
    print_string(temp_string);
    sprintf(temp_string, "Minor Verion : %d\n", MINOR_VERSION);
    print_string(temp_string);
    sprintf(temp_string, "Patch Verion : %d\n", PATCH_VERSION);
    print_string(temp_string);

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
 * @brief Read raw data from CD4051 8-channel analogue inputs
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_1(uint32_t parameter)
{
    print_string("Test 1\n");
    for (uint32_t index = 0; index < 10; index++) {
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
           memcpy(&temp_analogue_data, &system_IO_data.analogue_data, sizeof(struct analogue_data_s));
        xSemaphoreGive(semaphore_system_IO_data);
        sprintf(temp_string, "%u,%u,%u,%u,%u,%u,%u,%u\n", 
            temp_analogue_data.CD4051[0].raw_data,
            temp_analogue_data.CD4051[1].raw_data,
            temp_analogue_data.CD4051[2].raw_data,
            temp_analogue_data.CD4051[3].raw_data,
            temp_analogue_data.CD4051[4].raw_data,
            temp_analogue_data.CD4051[5].raw_data,
            temp_analogue_data.CD4051[6].raw_data,
            temp_analogue_data.CD4051[7].raw_data
        );
        print_string(temp_string);
        vTaskDelay(TWO_SECONDS);
    }
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_2(uint32_t parameter)
{
    print_string("Test 2\n");
    return OK;
}

/**
 * @brief Read battery data and print results 
 * 
 * @return error_codes_te 
 */
 error_codes_te run_test_3(uint32_t parameter)
{
    print_string("Test 3\n");
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
