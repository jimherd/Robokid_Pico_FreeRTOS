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
void process_icons(void);


/**
 * @brief Task to control display of SSD1306 display
 * 
 * @param p 
 */
void Task_display_LCD (void *p) {

    Oled_Init();

//
// print hello message 
  START_PULSE;
    SSD1306_write_string(0, 0, "Robokid 2");
    // Oled_SetFont(Terminal9x16, 9, 16, 32,127);
    // Oled_ConstText("Test code",   0, 0);
    // Oled_ConstText("Robikid 2d",   0, 2);

  STOP_PULSE;
//  SSD1306_write_string(1, 1,"#&\%2<>?");
//     Oled_ConstText("Oled 128x64", 0, 4);
//     Oled_ConstText("SSD1306  ",   0, 6);

//     Oled_SetFont(robokid_LCD_icons_font15x16, 15, 16, 32, 127);
//     Oled_ConstText("#&", 0,0);

// //    sleep_ms(3000);
//     printf("Loaded\r\n");

    // Oled_Demo();

    
    FOREVER {
        process_icons();
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

#define BATTERY_FULL    33  // '!'
#define BATTERY_75      34  // '"'
#define BATTERY_HALF    35  // '#'
#define BATTERY_25      36  // '$'
#define BATTERY_EMPTY   37  // '%'

#define DPAD_NORTH      38  // '&'
#define DPAD_SOUTH      39  // '\''
#define DPAD_WEST       40  // '('
#define DPAD_EAST       41  // ')'

#define GAMEPAD_CONNECTED   61  // '='


/**
 * @brief Generate set of diplay icons.
 * 
 */
void process_icons(void) {

char    buffer[10], buffer_pt;

    buffer_pt = 0;

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

// Gamepad icons

    xSemaphoreTake(semaphore_gamepad_data, portMAX_DELAY);
        if (gamepad_data.state == ENABLED) {
            buffer[buffer_pt++] = GAMEPAD_CONNECTED;
        } else {
            buffer[buffer_pt++] = ' ';
        }
    xSemaphoreGive(semaphore_gamepad_data);

    buffer[buffer_pt] = '\0';
    SSD1306_write_string(1, 1, buffer);

    return;
}