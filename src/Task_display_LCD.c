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

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"

//==============================================================================
// function prototypes
//==============================================================================
void process_icons(void);       // prototype
void process_scroller(void);

char        *test_scroll_string_data[4] = {
    "SWA: Exit",
    "SWB: Start",
    "SWC: Reset",
    "SWD: Stop"
};

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
//
// print hello message 

    SSD1306_set_window(0, 0x00);
//    SSD1306_write_string(0, MESSAGE_WINDOW, "Robokid 2");
    SSD1306_set_text_area_scroller(SCROLL_WINDOW, 4, test_scroll_string_data);
    ENABLE_SCROLLER;
    
    xLastWakeTime = xTaskGetTickCount ();
    FOREVER {
        xWasDelayed = xTaskDelayUntil( &xLastWakeTime, TASK_DISPLAY_LCD_FREQUENCY_TICK_COUNT );
        process_icons();
    //  process_message
        process_scroller();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#define BLANK           32  // ' '
#define BATTERY_FULL    33  // '!'
#define BATTERY_75      34  // '"'
#define BATTERY_HALF    35  // '#'
#define BATTERY_25      36  // '$'
#define BATTERY_EMPTY   37  // '%'

#define DPAD_UP         38  // '&'
#define DPAD_DOWN       39  // '\''
#define DPAD_LEFT       40  // '('
#define DPAD_RIGHT      41  // ')'

#define SELECT_START    47  // '/'
#define SELECT          48  // '0'
#define START           49  // '1'

#define BUTTON_A_B      50  // '2'
#define BUTTON_A        51  // '3'
#define BUTTON_B        52  // '4'

#define BUTTON_X_Y      53  // '5'
#define BUTTON_X        54  // '6'
#define BUTTON_Y        55  // '7'

#define BUTTON_L_R      56  // '8'
#define BUTTON_L        57  // '9'
#define BUTTON_R        58  // ':'

#define GAMEPAD_CONNECTED   61  // '='
#define ERROR_ICON      63  // '?'

/**
 * @brief Generate set of diplay icons.
 * 
 */
void process_icons(void) {

char    buffer[10], buffer_pt;
int8_t  error;

    buffer_pt = 0;
    SSD1306_set_window(ICON_WINDOW, 0x00);  // clear ICON window

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
    SSD1306_write_string(1, ICON_WINDOW, buffer);

    return;
}

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
    window = 3;  // SCROLL_WINDOW;
    SSD1306_set_window(SCROLL_WINDOW, 0x00);  // clear scroll area
    for (index = 0; index < LCD_scroll_data.nos_LCD_rows; index++) {
        SSD1306_write_string(0, window, LCD_scroll_data.string_data[LCD_scroll_data.string_count++]);
        if (LCD_scroll_data.string_count >= LCD_scroll_data.nos_strings) {
            LCD_scroll_data.string_count = 0;
        }
        window++;
    }
    LCD_scroll_data.string_count--;
    return;
}