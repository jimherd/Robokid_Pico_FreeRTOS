/**
 * @file run_test_modes.c
 * @author Jim Herd
 * @brief Implement hidden test features
 */

// Notes
//      These tests are used to check various aspects of the robot operation.
//      Mostly, they exercise some aspect of the system and dump data to the
//      rp2040 uart channel for collection by a PC.  Where possible, this
//      data will be in a comma delimited ASCI format to allow it to be
//      imported into excel for display.
//
//      Tests
//          0. Print version and cycle LEDs
//          1. Read raw data from CD4051 8-channel analogue inputs
//          2. Log data from a single CD4051 channel
//          3. Print relevant task data
//          4. ........

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
// Local data
//==============================================================================

char    temp_string[128];
static struct  analogue_global_data_s      temp_analogue_global_data[NOS_CD4051_CHANNELS];

//==============================================================================
// Main routine
//==============================================================================

error_codes_te run_test_modes(uint8_t mode_index, uint32_t parameter) 
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
error_codes_te run_test_0(uint8_t mode_index, uint32_t parameter)
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
 * @param parameter 
 * @return error_codes_te 
 */
 error_codes_te run_test_1(uint8_t mode_index, uint32_t parameter)
{
    print_string("POT A,POT B,Spare,Vm,IR Left,IR Mid,IR Right, POT C\n");
    for (uint32_t index = 0; index < 100; index++) {
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
           memcpy(&temp_analogue_global_data, &system_IO_data.analogue_global_data, (NOS_CD4051_CHANNELS * sizeof(struct analogue_global_data_s)));
        xSemaphoreGive(semaphore_system_IO_data);
        sprintf(temp_string, "%u,%u,%u,%u,%u,%u,%u,%u\n", 
            temp_analogue_global_data[0].processed.value,
            temp_analogue_global_data[1].processed.value,
            temp_analogue_global_data[2].processed.value,
            temp_analogue_global_data[3].processed.value,
            temp_analogue_global_data[4].processed.value,
            temp_analogue_global_data[5].processed.value,
            temp_analogue_global_data[6].processed.value,
            temp_analogue_global_data[7].processed.value
        );
        print_string(temp_string);
        vTaskDelay(TWO_SECONDS);
    }
    return OK;
}

/**
 * @brief Select a CD4051 channel to read and log
 * 
 * @param parameter 
 * @return error_codes_te 
 */
 error_codes_te run_test_2_menu(uint8_t mode_index, uint32_t parameter)
{
    run_menu(&test_mode_2_menu);
    return OK;
}

/**
 * @brief output data from selected CD4051 channel
 * 
 * @param parameter 
 * @return error_codes_te 
 */
 error_codes_te run_test_2(uint8_t mode_index, uint32_t parameter)
{
    sprintf(temp_string, "Channel %u\n", mode_index);
    print_string(temp_string);
    sprintf(temp_string,"Raw,Filter,G_thresh,G_count,Max_delta\n");
    print_string(temp_string);
    for (uint32_t index = 0; index < 100; index++) {
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&temp_analogue_global_data, &system_IO_data.analogue_global_data, (NOS_CD4051_CHANNELS * sizeof(struct analogue_global_data_s)));
        xSemaphoreGive(semaphore_system_IO_data);
        sprintf(temp_string, "%u,%u,%u,%u,%u\n", 
                temp_analogue_global_data[mode_index].raw.current_value,
                temp_analogue_global_data[mode_index].processed.value,
                temp_analogue_global_data[mode_index].raw.glitch_threshold,
                temp_analogue_global_data[mode_index].raw.glitch_count,
                temp_analogue_global_data[mode_index].raw.max_delta
        );
        print_string(temp_string);
        vTaskDelay(ONE_SECOND);
    }
    return OK;
}

/**
 * @brief Output system data for test/debug
 * 
 * @param parameter 
 * @return error_codes_te 
 * 
 * Data includes, task stack info, task execution times info, error counts
 */
error_codes_te run_test_3(uint8_t mode_index, uint32_t parameter)
{
     for (uint8_t index; index < NOS_TASKS; index++) {
        sprintf(temp_string, "%u,%u,%u,%u,%u\n",
            system_IO_data.task_data[index].pxStackBase,
            system_IO_data.task_data[index].StackHighWaterMark,
            system_IO_data.task_data[index].last_exec_time,
            system_IO_data.task_data[index].lowest_exec_time,
            system_IO_data.task_data[index].highest_exec_time
        );
        print_string(temp_string);
     }
    return OK;
}

error_codes_te run_test_4(uint8_t mode_index, uint32_t parameter)
{
    return OK;
}

error_codes_te run_test_5(uint8_t mode_index, uint32_t parameter)
{
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
