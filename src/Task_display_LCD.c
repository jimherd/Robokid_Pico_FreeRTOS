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
#include "images.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"


void Task_display_LCD (void *p) {

    Oled_Init();

//
// print hello message 

    Oled_SetFont(Terminal9x16, 9, 16, 32,127);
    // Oled_ConstText("Test code",   0, 0);
    Oled_ConstText("Robikid 2d",   0, 2);
    Oled_ConstText("Oled 128x64", 0, 4);
    Oled_ConstText("SSD1306  ",   0, 6);

    Oled_SetFont(robokid_LCD_icons_font15x16, 15, 16, 32, 127);
    Oled_ConstText("#&", 0,0);

//    sleep_ms(3000);
    printf("Loaded\r\n");

    // Oled_Demo();


    FOREVER {
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}