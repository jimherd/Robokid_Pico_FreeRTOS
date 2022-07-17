/**
 * @file run_test_modes.c
 * @author Jim Herd (you@domain.com)
 * @brief Implement hidden test features
 */

// Notes
//      Tests
//          a. Print version and cycle LEDs
//          b. 
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

error_codes_te run_test_modes(void) 
{
EventBits_t             event_bits;
secondary_sys_modes_te  secondary_mode;
error_codes_te          error;

    secondary_mode = GAMEPAD_MODE_1;
    LCD_write_row(0, MESSAGE_ROW, mode_G[secondary_mode - (10 * GAMEPAD_MODE)], true);
    SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
    set_leds(LED_ON, LED_ON, LED_OFF, LED_OFF);
    event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);
    return OK;

    if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
        switch (secondary_mode) {
            case TEST_MODE_0 : {
                return (run_test_mode_1());
            }
            case TEST_MODE_1 : {
                return(run_test_mode_1());
                break;
            }
            default : {
                break;
            }
        } 
    }
}
//==============================================================================
//
error_codes_te run_test_mode_0(void) 
{
    printf("Major Verion : %d\r\n", MAJOR_VERSION);
    printf("Minor Verion : %d\r\n", MINOR_VERSION);
    printf("Patch Verion : %d\r\n", PATCH_VERSION);

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

error_codes_te run_test_mode_1(void)
{
    return OK;
}