/**
 * @file   Task_Robokid.c
 * @author Jim Herd
 * @brief  Run user level Robokid system
 * @date   2022-02-28
 */
#include "system.h"
#include "common.h"
#include "OLED_128X64.h"
#include "SSD1306.h"
#include "Robokid_strings.h"
#include "tunes.h"
#include "menus.h"
#include "run_gamepad_modes.h"
#include "run_test_modes.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define   TASK_ROBOKID_START_DELAY_SECONDS    5

#define   LONG_PRESS_MS  4000        // 4 seconds

//==============================================================================
// Main task routine
//==============================================================================
void Task_Robokid(void *p) 
{
uint32_t                    start_time, end_time;
uint8_t                     index;
EventBits_t                 event_bits;
primary_sys_modes_te        primary_mode, first_mode, last_mode;
uint32_t                    press_time;
error_codes_te              error;

    for (index = 0; index < (TASK_ROBOKID_START_DELAY_SECONDS * 2); index++) {
        LCD_write_row(0, MESSAGE_ROW, system_busy[index % 4], false);
        vTaskDelay(HALF_SECOND);
    }

    set_n_LEDS(STRING_COUNT(wait_start));
    SSD1306_set_text_area_scroller(STRING_COUNT(wait_start), wait_start);

    first_mode = GAMEPAD_MODE;
    last_mode = TEST_MODE - 1;  // hide experiment mode from normal operation
    LCD_write_row(0, MESSAGE_ROW, system_ready[0], true);
    press_time = wait_for_button_press(PUSH_BUTTON_A, portMAX_DELAY);
    if (press_time > LONG_PRESS_MS) {
        primary_menu.nos_modes++;
        last_mode = TEST_MODE;   // expose experiment mode if press time is > xxxx
    }
    set_tune_data(beep, NOS_NOTES(beep), true, 2);

    // primary_mode = PRIMARY_NULL_MODE;

    FOREVER {
        error = run_menu(&primary_menu);
        if (error != OK) {
            log_error(error, TASK_ROBOKID);
        }
    }
}


    // FOREVER {
    //     LCD_write_row(0, MESSAGE_ROW, main_modes[primary_mode], true);
    //     set_n_LEDS(STRING_COUNT(top_level_button_data));
    //     SSD1306_set_text_area_scroller(STRING_COUNT(top_level_button_data), top_level_button_data);
        
    //     event_bits = (wait_for_any_button_press(portMAX_DELAY) & PUSH_BUTTON_ON_EVENT_MASK);

    //     if (event_bits == PUSH_BUTTON_A_EVENT_MASK) {
    //         switch (primary_mode) {
    //             case PRIMARY_NULL_MODE :  // no mode selected as yet
    //                 break;
    //             case GAMEPAD_MODE : 
    //                 error = run_gamepad_modes(0);
    //                 if (error != OK) {
    //                     log_error(error, TASK_ROBOKID);
    //                 }
    //                 break;
    //             case TEST_MODE : 
    //                 error = run_test_modes(0);
    //                 break;
    //             default : 
    //                 break;
    //         }  // end of switch 
    //     } else if (event_bits == PUSH_BUTTON_B_EVENT_MASK) {
    //         if (primary_mode == last_mode) {
    //             primary_mode = first_mode;
    //         } else {
    //             primary_mode++;
    //         }
    //     } else if (event_bits == PUSH_BUTTON_C_EVENT_MASK) {
    //         NULL;  // ignore press of button C
    //     } else if (event_bits == PUSH_BUTTON_D_EVENT_MASK) {
    //         NULL;  // ignore press of button D
    //     }
    // }

