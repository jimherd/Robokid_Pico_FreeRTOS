/**
 * @file Task_display_LCD
 * @author Jim Herd
 * @brief Manage display of information on SSD1306 LCD display
 * @version 0.1
 * @date 2022-01-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>

#include "system.h"
#include "common.h"
#include "OLED_128X64.h"
#include "SSD1306.h"
#include "images.h"
#include "Robokid_strings.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"

//==============================================================================
// function prototypes
//==============================================================================
void process_icons(void);       // prototype
void process_scroller(void);
void LCD_dump_row_data(void);

//==============================================================================
// Task code
//==============================================================================
/**
 * @brief Task to control display of SSD1306 display
 * 
 * @param p 
 */
void Task_display_LCD (void *p) 
{
uint32_t    start_time, end_time;
TickType_t  xLastWakeTime;
BaseType_t  xWasDelayed;

    Oled_Init();

// print hello message 

    LCD_write_row(0, MESSAGE_ROW, "Robokid 2");
    SSD1306_set_text_area_scroller(SCROLL_ROW_UPPER, 2, 4, test_scroll_string_data);
    ENABLE_SCROLLER;
    
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_DISPLAY_LCD_FREQUENCY_TICK_COUNT );

        process_icons();
    //  process_message
        process_scroller();
    // update display
        LCD_dump_row_data();

     //   vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

//==============================================================================
/**
 * @brief Generate set of diplay icons.
 * 
 */
void process_icons(void) {

char    buffer[16], buffer_pt;
int8_t  error;

    buffer_pt = 0;

// Error state

    xSemaphoreTake(semaphore_system_status, portMAX_DELAY);
        error = system_status.error_state;
    xSemaphoreGive(semaphore_system_status);
    if (error <= OK) {
        buffer[buffer_pt++] = ERROR_ICON;
    }

// Battery icon

    uint8_t battery_volts = 73;   // % value
 
    if (battery_volts > 80) {
        buffer[buffer_pt] = BATTERY_FULL;
    } else if (battery_volts > 60) {
        buffer[buffer_pt] = BATTERY_75;
    } else if (battery_volts > 40) {
        buffer[buffer_pt] = BATTERY_HALF;
    } else if (battery_volts > 20) {
        buffer[buffer_pt] = BATTERY_25;
    } else
        buffer[buffer_pt] = BATTERY_EMPTY;

    buffer_pt++;

// Gamepad icons : access data via MUTEX

    xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
        if (gamepad_data.state == ENABLED) {
            buffer[buffer_pt++] = GAMEPAD_CONNECTED;
            if (gamepad_data.dpad_x ==  0) {
                buffer[buffer_pt++] = DPAD_LEFT;
            } else if (gamepad_data.dpad_x == 0xFF) {
                buffer[buffer_pt++] = DPAD_RIGHT;
            } 

            if (gamepad_data.dpad_y ==  0) {
                buffer[buffer_pt++] = DPAD_UP;
            } else if (gamepad_data.dpad_y == 0xFF) {
                buffer[buffer_pt++] = DPAD_DOWN;
            }

            if ((gamepad_data.button_SELECT == 1) && (gamepad_data.button_START == 1)) {
                buffer[buffer_pt++] = SELECT_START;
            } else if (gamepad_data.button_SELECT == 1) {
                buffer[buffer_pt++] = SELECT;
            }else if (gamepad_data.button_START == 1) {
                buffer[buffer_pt++] = START;
            }

            if ((gamepad_data.button_A == 1) && (gamepad_data.button_B == 1)) {
                buffer[buffer_pt++] = BUTTON_A_B;
            } else if (gamepad_data.button_A == 1) {
                buffer[buffer_pt++] = BUTTON_A;
            }else if (gamepad_data.button_B == 1) {
                buffer[buffer_pt++] = BUTTON_B;
            }

            if ((gamepad_data.button_X == 1) && (gamepad_data.button_Y == 1)) {
                buffer[buffer_pt++] = BUTTON_X_Y;
            } else if (gamepad_data.button_X == 1) {
                buffer[buffer_pt++] = BUTTON_X;
            }else if (gamepad_data.button_Y == 1) {
                buffer[buffer_pt++] = BUTTON_Y;
            }

            if ((gamepad_data.button_L == 1) && (gamepad_data.button_R == 1)) {
                buffer[buffer_pt++] = BUTTON_L_R;
            } else if (gamepad_data.button_L == 1) {
                buffer[buffer_pt++] = BUTTON_L;
            }else if (gamepad_data.button_R == 1) {
                buffer[buffer_pt++] = BUTTON_R;
            }
        } 
    xSemaphoreGive(semaphore_gamepad_data);

    buffer[buffer_pt] = '\0';
    LCD_write_row(1, ICON_ROW, buffer);
    return;
}

//==============================================================================
/**
 * @brief Manage scroll display
 * 
 */
void process_scroller(void)
{
uint8_t window, index;

    if (LCD_scroll_data.enable == false) {
        return;
    }
    if (LCD_scroll_data.scroll_delay_count > 0) {
        LCD_scroll_data.scroll_delay_count--;
        return;
    } else {
        LCD_scroll_data.scroll_delay_count = LCD_scroll_data.scroll_delay;  // reset delay count
    }
    window = LCD_scroll_data.first_LCD_row;
    for (index = 0; index < LCD_scroll_data.nos_LCD_rows; index++) {
        LCD_write_row(0, LCD_scroll_data.first_LCD_row+index, LCD_scroll_data.string_data[LCD_scroll_data.string_count]);
        LCD_scroll_data.string_count += 1;
        if (LCD_scroll_data.string_count >= LCD_scroll_data.nos_strings) {
            LCD_scroll_data.string_count = 0;
        }
        window++;
    }
    if (LCD_scroll_data.string_count == 0) {
        LCD_scroll_data.string_count = LCD_scroll_data.nos_strings - 1;
    } else {
        LCD_scroll_data.string_count--;
    }
    return;
}

/**
 * @brief dump SSD1306 row data to display.
 * 
 * SSD1306 is used as FOUR rows of 14 characters.  If possible,
 * all SSD1306 output should go through this routine.
 */
void LCD_dump_row_data(void)
{
    for (uint8_t index = 0; index < SS1306_NOS_LCD_ROWS; index++) {
        if (LCD_row_data[index].dirty_bit == true) {
            xSemaphoreTake(semaphore_SSD1306_display, portMAX_DELAY);
              SSD1306_write_string(LCD_row_data[index].font, index+1, &LCD_row_data[index].row_string[0]);
            xSemaphoreGive(semaphore_SSD1306_display);
            LCD_row_data[index].dirty_bit = false;
        }
    }
    return;
}

