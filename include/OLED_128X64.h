/* 
 * File:   OLED_128X64.h
 * Author: |H_e_c_t_o_r|
 *
 * Graphical display OLED 128x64.
 * Driver SSD1306.
 * Interface: SPI.
 * This library can only be used for educational and hobbyist purposes.
 * Website https://www.youtube.com/user/Hector8389/videos
 * 
 * Version 2.0 
 * 
 */

#ifndef __OLED_128X64_H__
#define __OLED_128X64_H__

#include <stdio.h>

#include "Pico_IO.h"

//==============================================================================
// SPI Connection of module OLED.
//
// VCC         <<<>>> 3.3 V.
// GND         <<<>>> GND.
// OLED_SCK    <<<>>> Connect to SCK MCU.
// OLED_MOSI   <<<>>> Connect to SDO MCU.
// OLED_CS     <<<>>> You can change this pin.
// OLED_D/C    <<<>>> You can change this pin.

// OLED_MISO   <<<>>> This module hasn't got MISO pin. 
// OLED:RESET  <<<>>> This module hasn't got RESET pin. 
//==============================================================================

#define     OLED_MOSI    GP3      // GP3   Pin 5
#define     OLED_MISO    GP4      // GP4   Pin 6
#define     OLED_CS      GP5      // GP5   Pin 7
#define     OLED_SCK     GP6      // GP6   Pin 9
#define     OLED_DC      GP7      // GP7   Pin 10

#define     SPI_PORT        spi0

//==============================================================================
// I2C Connection of module OLED.
//
// VCC         <<<>>> 3.3 V.
// GND         <<<>>> GND.
// OLED_SCL    <<<>>> Connect to I2C SCL line.
// OLED_SDA    <<<>>> Connect to I2C SDA line.
//==============================================================================

#define     OLED_I2C_SCL    GP7      // GP3  Pin 5
#define     OLED_I2C_SDA    GP4      // GP4  Pin 6

#define     I2C_PORT        i2c0

#define     DISPLAY_WIDTH   128
#define     DISPLAY_HEIGHT   64

//==============================================================================
// These are pages and frames that you can use.
//==============================================================================
#define PAGE0         0
#define PAGE1         1
#define PAGE2         2
#define PAGE3         3
#define PAGE4         4
#define PAGE5         5
#define PAGE6         6
#define PAGE7         7
#define FRAME_2       7
#define FRAME_3       4
#define FRAME_4       5
#define FRAME_5       0
#define FRAME_25      6
#define FRAME_64      1
#define FRAME_128     2
#define FRAME_256     3
#define BLACK         1
#define WHITE         2
#define INVERSE       3

//==============================================================================
// SSD1306 commands
//==============================================================================
#define SET_LOWER_COLUMN_START_ADDRESS_PAGE_MODE     0x00
#define SET_HIGHER_COLUMN_START_ADDRESS_PAGE_MODE    0x10

#define SET_MEMORY_ADDRESSING_MODE      0x20
    #define HORIZONTAL_ADDRESSING_MODE      0x00
    #define VERTICAL_ADDRESSING_MODE        0x01
    #define PAGE_ADDRESSING_MODE            0x02

#define SET_DISPLAY_START_LINE      0x40    

#define SET_CONTRAST            0x81

#define SET_SEGMENT_REMAP_0_to_SEG0     0xA0
#define SET_SEGMENT_REMAP_127_to_SEG0   0xA1

#define SET_ALL_PIXELS_NORMAL   0xA4    // pixels defined by RAM contects
#define SET_ALL_PIXELS_ON       0xA5    // RAM ignored, all PIXELS swithed ON

#define SET_NORMAL_DISPLAY      0xA6
#define SET_INVERT_DISPLAY      0xA7

#define SET_MULTIPLEX_RATIO     0xA8

#define SET_DISPLAY_ON          0xAF
#define SET_DISPLAY_OFF         0xAE

#define SET_PAGE_START_ADDRESS_PAGE_MODE   0xB0

#define SET_DISPLAY_OFFSET      0xD3

#define SET_COM_OUT_DIRECTION   0xC8
#define SET_DISPLAY_CLK_DIV     0xD5
#define SET_PRE_CHARGE_PERIOD   0xD9
#define SET_COM_PINS_CONFIG     0xDA
#define SET_VCOM_DESELECT_LEVEL 0xDB
#define SET_CHARGE_PUMP         0x8D

#define DISABLE_CHARGE_PUMP     0x10
#define ENABLE_CHARGE_PUMP      0x14


//==============================================================================
// Function Prototypes.
//==============================================================================
    void Oled_Init(void);
    void Oled_Delay(void);
    void Oled_Reset(void);
    void Oled_Command(uint8_t cmd);
    void  Oled_Command_seq(const uint8_t *cmd_seq, uint32_t len);
    void Oled_WriteRam(uint8_t dat);
    void Oled_SetPointer(uint8_t seg, uint8_t pag);
    void Oled_SetFont(const unsigned char *_font, uint8_t _width, uint8_t _height, uint8_t _min, uint8_t _max);
    void Oled_WriteChar(uint8_t c, uint8_t seg, uint8_t pag);
    void Oled_ConstText(char *buffer, uint8_t seg, uint8_t pag);
    void Oled_Text(char *buffer, uint8_t seg, uint8_t pag);
    void Oled_ConstTextScroll(char *buffer);
    void Oled_TextScroll(char *buffer);
    void Oled_ResetScroll(void);
    void Oled_FillScreen(uint8_t pattern);
    void Oled_Image(const unsigned char *buffer);
    void Oled_InvertRam(uint8_t seg1, uint8_t pag1, uint8_t seg2, uint8_t pag2);
    void Oled_Icon(const unsigned char *buffer, uint8_t seg, uint8_t pag, uint8_t _width, uint8_t _height);
    void Oled_Pixel(uint8_t x, uint8_t y, uint8_t color);
    void Oled_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
    void Oled_Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
    void Oled_Circle(uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color);
    void Right_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time);
    void Left_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time);
    void VerticalRight_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time);
    void VerticalLeft_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time);
    void Deactivate_Scroll(void);
    void Activate_Scroll(void);

    void _send_command(uint8_t code);
    void _send_data(uint8_t value);

    void  Oled_Demo(void);

#endif
