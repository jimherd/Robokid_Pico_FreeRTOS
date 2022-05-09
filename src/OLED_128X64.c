/* 
 * File:   ssd1306.c
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

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "OLED_128X64.h"

#include "system.h"

#include "font.h"
#include "bitmap.h"


uint8_t  Oled_RAM[1024];  
int32_t _cursor_x, _cursor_y;

//==============================================================================
// Display configuration information
//==============================================================================

//uint32_t display_height = 64;
//uint32_t display_width  = 128;

//==============================================================================
// Declaration of global variables from OLED_128X64.h
//==============================================================================
unsigned char *_console_font_data;  
unsigned int _console_font_width;
unsigned int _double_height_text;
    
const unsigned char *font, *font2;
uint8_t width, height, min, max;

uint8_t cnt_width;
uint8_t cnt_character;

//==============================================================================
// Declaration of global variables.
//==============================================================================
const unsigned char *font, *font2;
uint8_t width, height, min, max;
uint8_t Oled_RAM[1024];
uint8_t cnt_width = 0;
uint8_t cnt_character = 0;

//==============================================================================
// Sequence of commands to initialise SSD1306 display
//==============================================================================
const uint8_t init_sequence [] = { 
    SET_DISPLAY_OFF ,               // Display OFF (sleep mode)
    SET_MEMORY_ADDRESSING_MODE , 
        HORIZONTAL_ADDRESSING_MODE, // Set Memory Addressing Mode
          // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
          // 10=Page Addressing Mode (RESET); 11=Invalid
    SET_PAGE_START_ADDRESS_PAGE_MODE,   // Set Page Start Address for Page Addressing Mode, 0-7
    SET_COM_OUT_DIRECTION,              // Set COM Output Scan Direction
    (SET_LOWER_COLUMN_START_ADDRESS_PAGE_MODE | 0x00),     // --set low column address
    (SET_HIGHER_COLUMN_START_ADDRESS_PAGE_MODE | 0x00),    // --set high column address
    (SET_DISPLAY_START_LINE | PAGE0),                      // --set start line address
    SET_CONTRAST, 
        0x3F,                       // Set contrast control register
    SET_SEGMENT_REMAP_127_to_SEG0,  // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    SET_NORMAL_DISPLAY,             // Set display mode. A6=Normal; A7=Inverse
    SET_MULTIPLEX_RATIO, 
        DISPLAY_HEIGHT-1,           // Set multiplex ratio(1 to 64)
    SET_ALL_PIXELS_NORMAL ,         // Output RAM to Display
					         // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    SET_DISPLAY_OFFSET, 
        0x00,                       // Set display offset. 00 = no offset
    SET_DISPLAY_CLK_DIV,            // --set display clock divide ratio/oscillator frequency
        0xF0,                       // --set divide ratio
    SET_PRE_CHARGE_PERIOD, 
        0x22,                       // Set pre-charge period 
    SET_COM_PINS_CONFIG,            // Set com pins hardware configuration
        0x12,      
    SET_VCOM_DESELECT_LEVEL,        // --set vcomh
        0x20,                       // 0x20,0.77xVcc
    SET_CHARGE_PUMP, 
        ENABLE_CHARGE_PUMP,         // Set DC-DC enable
};

//==============================================================================
// Initializes the OLED module.
//==============================================================================
void  SSD1306_init(void){

    _cursor_x = 0;
    _cursor_y = 0;

#ifdef SSD1306_INTERFACE_SPI
    gpio_init(OLED_CS);
    gpio_init(OLED_DC);
    gpio_set_dir(OLED_CS, GPIO_OUT);
    gpio_set_dir(OLED_DC, GPIO_OUT);

// Initialize SPI pins
    gpio_set_function(OLED_SCK, GPIO_FUNC_SPI);
    gpio_set_function(OLED_CS, GPIO_FUNC_SPI);
    gpio_set_function(OLED_MOSI, GPIO_FUNC_SPI);

    spi_init(SPI_PORT, SSD1306_SPI_SPEED);   // SPI_SPEED_5MHz
#endif

#ifdef SSD1306_INTERFACE_I2C
    i2c_init(I2C_PORT, SSD1306_I2C_SPEED);
    gpio_set_function(SSD1306_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_I2C_SDA_PIN);
    gpio_pull_up(SSD1306_I2C_SCL_PIN);
#endif

    SSD1306_command_seq(init_sequence, sizeof(init_sequence));

    Oled_Delay();


/*    Oled_Reset();
    SSD1306_command(SET_DISPLAY_OFF);                  // Set Display OFF
    SSD1306_command(SET_CONTRAST); SSD1306_command(0xCF); // Set Contrast Control
    SSD1306_command(SET_ALL_PIXELS_NORMAL);            // Entire Display ON
    SSD1306_command(SET_NORMAL_DISPLAY); 

    SSD1306_command(SET_MEMORY_ADDRESSING_MODE); SSD1306_command(PAGE_ADDRESSING_MODE); 
    SSD1306_command(SET_LOWER_COLUMN_START_ADDRESS_PAGE_MODE + 0);
    SSD1306_command(SET_HIGHER_COLUMN_START_ADDRESS_PAGE_MODE + 0);
    SSD1306_command(SET_PAGE_START_ADDRESS_PAGE_MODE | PAGE0);

    SSD1306_command(SET_DISPLAY_START_LINE | 0);
    SSD1306_command(SET_SEGMENT_REMAP_127_to_SEG0);
    SSD1306_command(SET_MULTIPLEX_RATIO); SSD1306_command(0x3F); // Set Multiplex Ratio
    SSD1306_command(0xC8);                     // Set COM Output
    SSD1306_command(SET_DISPLAY_OFFSET); SSD1306_command(0); // Set Display Offset
    SSD1306_command(0xDA); SSD1306_command(0x12); // Set COM Pins Hardware Configuration

    SSD1306_command(0xD5); SSD1306_command(0x80); // Set Display Clock
    SSD1306_command(0xD9); SSD1306_command(0xF1); // Set Pre-charge Period
    SSD1306_command(0xDB); SSD1306_command(0x40); // Set VCOMH Deselect Level
    SSD1306_command(0x8D); SSD1306_command(0x14); // Charge Pump Setting

    SSD1306_command(SET_DISPLAY_ON);           // Set Display ON
    Oled_FillScreen(0x00);                  // Clear screen */

    Oled_FillScreen(0x00);                  // Clear screen */
    SSD1306_command(SET_DISPLAY_ON);           // Set Display ON
}

//==============================================================================
// Delay of 500 ms.
//==============================================================================
void  Oled_Delay(void){
    sleep_us(500000);
}

//==============================================================================
// Resets the OLED module. Not available on SPI SSD1306 unit being used
//==============================================================================
void  Oled_Reset(void)
{
    //OLED_RES = 0;
    //Oled_Delay(); Oled_Delay();
    //OLED_RES = 1;
    Oled_Delay();
}

//==============================================================================
// Write a single command.
//==============================================================================
/**
 * @brief Write a command to the OLED SSD1306 display
 * 
 * @note
 *      Code can access SPI or I2C interface
 * 
 * @param cmd 
 */
void  SSD1306_command(const uint8_t cmd)
{

#ifdef SSD1306_INTERFACE_SPI
    gpio_put(OLED_CS, 1);
    gpio_put(OLED_DC, 0);
    gpio_put(OLED_CS, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(OLED_CS, 1);
#endif

#ifdef SSD1306_INTERFACE_I2C
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    int reply;
    reply = i2c_write_blocking(I2C_PORT, SSD1306_ADDRESS, buf, 2, false);
    if (reply == PICO_ERROR_GENERIC ) {
        __breakpoint;
    }
#endif

}
//==============================================================================
// Write a single command.
//==============================================================================
void  SSD1306_command_seq(const uint8_t *cmd_seq, uint32_t len)
{
#ifdef SSD1306_INTERFACE_SPI
    gpio_put(OLED_CS, 1);
    gpio_put(OLED_DC, 0);
    gpio_put(OLED_CS, 0);
    spi_write_blocking(SPI_PORT, cmd_seq, len);
    gpio_put(OLED_CS, 1);
#endif

#ifdef SSD1306_INTERFACE_I2C
    for (uint32_t i = 0; i < len; i++) {
        SSD1306_command(cmd_seq[i]);
    }
#endif
}

//==============================================================================
// Write a byte to RAM memory.
//==============================================================================
void  Oled_WriteRam(uint8_t dat)
{
#ifdef SSD1306_INTERFACE_SPI
    gpio_put(OLED_CS, 1);
    gpio_put(OLED_DC, 1);
    gpio_put(OLED_CS, 0);
    spi_write_blocking(SPI_PORT, &dat, 1);
    gpio_put(OLED_CS, 1);   
#endif

#ifdef SSD1306_INTERFACE_I2C
    // Co = 0, D/C = 1 => the driver expects data to be written to RAM
    uint8_t buf[2] = {0x40, dat};
    i2c_write_blocking(I2C_PORT, SSD1306_ADDRESS, buf, 2, false);
#endif
}

//==============================================================================
// Sets the pointer in frame memory where MCU can access.
// seg: sets the segment.
// pag: sets the  page.
//==============================================================================
void  Oled_SetPointer(uint8_t seg, uint8_t pag)
{
uint8_t low_column, hig_column;

    low_column = (seg & 0b00001111);
    hig_column = (seg & 0b11110000)>>4;
    hig_column = hig_column | 0b00010000;
    pag = (pag & 0b00000111);
    pag = (pag | 0b10110000);
    SSD1306_command(low_column); // Set Lower Column
    SSD1306_command(hig_column); // Set Higher Column
    SSD1306_command(pag);        // Set Page Start
}

//==============================================================================
// Sets font that will be used.
// _font: desired font.
// _width: sets the width in units of pixels.
// _height: sets the height in units of pixels. 
// _min: sets the range minimum.
// _max: sets the range maximum.
//
// Note: this library supports only  fonts with multiples of 8 pixels in height.
//==============================================================================
void  Oled_SetFont(const unsigned char *_font, uint8_t _width, uint8_t _height, uint8_t _min, uint8_t _max)
{
    font2  = _font;
    width  = _width;
    height = _height / 8;
    min    = _min;
    max    = _max;
}

//==============================================================================
// Writes a char on the OLED.
// c: char to be written.
// seg: sets the segment. Valid values: 0..127
// pag: sets the page. Valid values: 0..7
//==============================================================================
void  Oled_WriteChar(uint8_t c, uint8_t seg, uint8_t pag)
{
uint i, j;
uint x_seg, y_pag;

    x_seg = seg;
    y_pag = pag;
    font = font2;
    j = c - min;
    j = j * (width * height);
    for(i = 0; i < j; i++) {
        font++;
    }
    for(i = 0; i < width; i++) {
        y_pag = pag;
        for(j = 0; j < height; j++) {
            if(x_seg < 128) {
                Oled_SetPointer(x_seg, y_pag);
                Oled_WriteRam(*font);
                Oled_RAM[x_seg + (y_pag * 128)] = *font;  // keep copy
            }
            y_pag++;
            font++;
        }
        x_seg++;
    }
}

//==============================================================================
// Prints text constant on OLED.
// buffer: text to be written.
// seg: sets the segment. Valid values: 0..127
// pag: sets the page. Valid values: 0..7
//==============================================================================
void  Oled_ConstText(char *buffer, uint8_t seg, uint8_t pag)
{
    uint8_t x_seg = seg;
    while(*buffer) {
        Oled_WriteChar(*buffer, x_seg, pag);
        x_seg = x_seg + width;
        buffer++;
    }
}

//==============================================================================
// Prints text variable on OLED.
// buffer: text to be written.
// seg: sets the segment. Valid values: 0..127
// pag: sets the page. Valid values: 0..7
//==============================================================================
void  Oled_Text(char *buffer, uint8_t seg, uint8_t pag)
{
    
    uint8_t x_seg = seg;
    while(*buffer) {
        Oled_WriteChar(*buffer, x_seg, pag);
        x_seg = x_seg + width;
        buffer++;
    }
}

//==============================================================================
// Prints text constant on OLED with scroll.
// buffer: text to be written.
//==============================================================================
void  Oled_ConstTextScroll(char *buffer)
{
uint i;

    cnt_width++;
    if(cnt_character == strlen(buffer)) {
        cnt_character = 0;
    }
    if(cnt_width == width) {
        for(i = 0; i < cnt_character; i++) {
            buffer++;
        }  
        Oled_WriteChar(*buffer, 127 - width, 0);   
        cnt_character++;
        cnt_width = 0; 
    }
    Oled_SetPointer(0, 0);
    for(i = 0; i < 127; i++) {
        Oled_RAM[i] = Oled_RAM[i+1];
        Oled_WriteRam(Oled_RAM[i]);
    } 
    Oled_SetPointer(0, 1);
    for(i = 128; i < 255; i++) {
        Oled_RAM[i] = Oled_RAM[i+1];
        Oled_WriteRam(Oled_RAM[i]);
    }
}

//==============================================================================
// Prints text variable on OLED with scroll.
// buffer: text to be written.
//==============================================================================
void  Oled_TextScroll(char *buffer)
{
uint i;

    cnt_width++;
    if(cnt_character == strlen(buffer)) {
        cnt_character = 0;
    }
    if(cnt_width == width) {
        for(i = 0; i < cnt_character; i++) {
            buffer++;
        }  
        Oled_WriteChar(*buffer, 127 - width, 0);   
        cnt_character++;
        cnt_width = 0; 
    }
    Oled_SetPointer(0, 0);
    for(i = 0; i < 127; i++) {
        Oled_RAM[i] = Oled_RAM[i+1];
        Oled_WriteRam(Oled_RAM[i]);
    } 
    Oled_SetPointer(0, 1);
    for(i = 128; i < 255; i++) {
        Oled_RAM[i] = Oled_RAM[i+1];
        Oled_WriteRam(Oled_RAM[i]);
    }
}

//==============================================================================
// Resets the scroll.
// This function must be called before of a new scroll function.
//==============================================================================
void  Oled_ResetScroll(void)
{
    cnt_width = 0;
    cnt_character = 0;
}

//==============================================================================
// Fills OLED memory.
// pattern: byte to fill OLED memory.
//==============================================================================
void  Oled_FillScreen(uint8_t pattern)
{
uint8_t i, j;
uint k = 0;

    for(i = 0; i < 8; i++) {
        Oled_SetPointer(0, i);
        for(j = 0; j < 128; j++) {
            Oled_WriteRam(pattern);
            Oled_RAM[k] = pattern;
            k++;
        }
    }
}

//==============================================================================
// Displays bitmap on OLED.
// buffer: image to be displayed.
//==============================================================================
void  Oled_Image(const unsigned char *buffer)
{
uint8_t i, j;
uint k = 0;

    for(i = 0; i < 8; i++) {
        Oled_SetPointer(0, i);
        for(j = 0; j < 128; j++) {
            Oled_WriteRam(*buffer);
            Oled_RAM[k] = *buffer;
            buffer++;
            k++;
        }
    }
}

//==============================================================================
// Displays an image on a desired location.
// buffer: image to be displayed.
// seg: sets the segment. Valid values: 0..127
// pag: sets the page. Valid values: 0..7
// _width sets the width of the image.
// _height sets the height of the image.
//==============================================================================
void  Oled_Icon(const unsigned char *buffer, uint8_t seg, uint8_t pag, uint8_t _width, uint8_t _height)
{
uint8_t i, j;
uint x_seg, y_pag;

    y_pag = pag;
    for(i = 0; i < _height / 8; i++) {
        x_seg = seg;
        for(j = 0; j < _width; j++) {
            if(x_seg < 128) {
                Oled_SetPointer(x_seg, y_pag);
                Oled_WriteRam(*buffer);
                Oled_RAM[x_seg + (y_pag * 128)] = *buffer;
            }
            buffer++;
            x_seg++;
        }
        y_pag++;
    }          
}

//==============================================================================
// Inverts the RAM memory.
// seg1 sets the start segment. Valid values: 0..127
// pag1 sets the start page. Valid values: 0..7
// seg2 sets the end segment. Valid values: 0..127
// pag2 sets the end page. Valid values: 0..7
//==============================================================================
void  Oled_InvertRam(uint8_t seg1, uint8_t pag1, uint8_t seg2, uint8_t pag2)
{
uint i, j, k;

    for(i = pag1; i <= pag2; i++) {
        for(j = seg1; j <= seg2; j++) {
            k = j + (i * 128);
            Oled_RAM[k] = ~Oled_RAM[k];
            Oled_SetPointer(j, i);
            Oled_WriteRam(Oled_RAM[k]);
        }
    }
}

//==============================================================================
// Draws a Pixel on OLED.
// x: x position. Valid values: 0..127 
// y: y position. Valid values: 0..63 
// color: color parameter. Valid values: 1,2,3 
//==============================================================================
void  Oled_Pixel(uint8_t x, uint8_t y, uint8_t color)
{
    
    if((x < 0) || (x > 127) || (y < 0) || (y > 63)){
        return;
    }  
    Oled_SetPointer(x, y/8);    
    switch(color) {
       case 1: Oled_WriteRam(Oled_RAM[x+((y/8)*128)] |=  (1<<(y%8))); break;
       case 2: Oled_WriteRam(Oled_RAM[x+((y/8)*128)] &= ~(1<<(y%8))); break;
       case 3: Oled_WriteRam(Oled_RAM[x+((y/8)*128)] ^=  (1<<(y%8))); break;
    }
}

//==============================================================================
// Draws a Line on OLED.
//
// Bresenham algorithm.
// 
// x1: x coordinate of the line start. Valid values: 0..127
// y1: y coordinate of the line start. Valid values: 0..63 
// x2: x coordinate of the line end. Valid values: 0..127 
// y2: y coordinate of the line end. Valid values: 0..63 
// color: color parameter. Valid values: 1,2,3 
//==============================================================================
void  Oled_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
 uint8_t i;
 uint8_t longest, shortest; 
 uint8_t numerator;

     uint8_t w = x2 - x1;
     uint8_t h = y2 - y1;
     uint8_t dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

    if(w < 0) dx1 = -1; else if(w > 0) dx1 = 1;
    if(h < 0) dy1 = -1; else if(h > 0) dy1 = 1;
    if(w < 0) dx2 = -1; else if(w > 0) dx2 = 1;
    if(w < 0) w *= -1;
    if(h < 0) h *= -1;
    longest = w; 
    shortest = h;
    if(!(longest > shortest)) {//if(w < 0) w *= -1; //if(h < 0) h *= -1; 
        longest = h; 
        shortest = w;
        if(h < 0) dy2 = -1; else if(h > 0) dy2 = 1;
        dx2 = 0;         
    }
    numerator = longest >> 1;
    for (i = 0; i <= longest; i++) {
        Oled_Pixel(x1, y1, color);
        numerator += shortest;
        if(!(numerator < longest)) {
            numerator -= longest;
            x1 += dx1;
            y1 += dy1;
        } else {
            x1 += dx2;
            y1 += dy2;
        }
    }
}

//==============================================================================
// Draws a rectangle on Display.
// x1: x coordinate of the upper left rectangle corner. Valid values: 0..127  
// y1: y coordinate of the upper left rectangle corner. Valid values: 0..63 
// x2: x coordinate of the lower right rectangle corner. Valid values: 0..127 
// y2: y coordinate of the lower right rectangle corner. Valid values: 0..63 
// color: color parameter. Valid values: 1,2,3 
//==============================================================================
void  Oled_Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    Oled_Line(x1, y1, x2, y1, color);
    Oled_Line(x1, y2, x2, y2, color);
    Oled_Line(x1, y1, x1, y2, color);
    Oled_Line(x2, y1, x2, y2, color);
}

//==============================================================================
// Draws a circle on Display.
//
// Midpoint circle algorithm.
//
// x1: x coordinate of the circle center. Valid values: 0..127
// y1: y coordinate of the circle center. Valid values: 0..63
// radius: radius size
// color: color parameter. Valid values: 1,2,3 
//==============================================================================
void  Oled_Circle(uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color)
{
    
     uint8_t x = radius, y = 0;
     uint8_t radiusError = 1 - x;

    

    while(x 
    >= y) {
        Oled_Pixel(x + x1, y + y1, color);
        Oled_Pixel(y + x1, x + y1, color);
        Oled_Pixel(-x + x1, y + y1, color);
        Oled_Pixel(-y + x1, x + y1, color);
        Oled_Pixel(-x + x1, -y + y1, color);
        Oled_Pixel(-y + x1, -x + y1, color);
        Oled_Pixel(x + x1, -y + y1, color);
        Oled_Pixel(y + x1, -x + y1, color);
        y++;
        if(radiusError<0) {
            radiusError += 2 * y + 1;
        } else   {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

//==============================================================================
// Sets the scroll. 
// start_page: Sets the start page. Valid values: 0..7
// end_page: Sets the end page. Valid values: 0..7
// set_time: sets time. Valid values: 0..7
//==============================================================================
void  Right_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time)
{
    Deactivate_Scroll();
    SSD1306_command(0x26);
    SSD1306_command(0x00);
    SSD1306_command(start_page);
    SSD1306_command(set_time);
    SSD1306_command(end_page);
    SSD1306_command(0x00);
    SSD1306_command(0xFF);
    Activate_Scroll();
}

void  Left_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time)
{
    Deactivate_Scroll();
    SSD1306_command(0x27);
    SSD1306_command(0x00);
    SSD1306_command(start_page);
    SSD1306_command(set_time);
    SSD1306_command(end_page);
    SSD1306_command(0x00);
    SSD1306_command(0xFF);
    Activate_Scroll();
}

void  VerticalRight_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time)
{
    Deactivate_Scroll();
    SSD1306_command(0x29);
    SSD1306_command(0x00);
    SSD1306_command(start_page);
    SSD1306_command(set_time);
    SSD1306_command(end_page);
    SSD1306_command(0x01); //scrolling_offset
    Activate_Scroll();
}

void  VerticalLeft_HorizontalScroll(uint8_t start_page, uint8_t end_page, uint8_t set_time)
{
    Deactivate_Scroll();
    SSD1306_command(0x2A);
    SSD1306_command(0x00);
    SSD1306_command(start_page);
    SSD1306_command(set_time);
    SSD1306_command(end_page);
    SSD1306_command(0x01); //scrolling_offset
    Activate_Scroll();
}

void  Deactivate_Scroll(void)
{
    SSD1306_command(0x2E);
}

void  Activate_Scroll(void)
{
    SSD1306_command(0x2F);
}

//==============================================================================
// Prints text variable on OLED with scroll.
// buffer: text to be written.
//==============================================================================
void  Oled_Demo(void)
{
uint32_t  i, j, k;
char buffer2[20];

    Oled_FillScreen(0x00);
    Oled_ConstText("lines", 30, 0);
    for(i=16; i<64; i=i+4) {
        Oled_Line(0, 16, 127, i, BLACK);
        sleep_ms(500);
    }
    sleep_ms(3000);

    Oled_FillScreen(0x00);
    Oled_ConstText("Circles", 10, 0);
    for(i=1; i<16; i=i+3) {
        Oled_Circle(63, 31, i, BLACK);
        sleep_ms(500);
    }
    sleep_ms(3000);

    Oled_FillScreen(0x00);
    Oled_ConstText("Rectangles", 0, 0);
    for(i = 0; i < 15; i = i + 4) {
        Oled_Rectangle(16+i, 16+i, 112-i, 46-i, BLACK);
        sleep_ms(500);
    }
    sleep_ms(3000);

    Oled_FillScreen(0x00);
    Oled_ConstText("  Inverts  ", 0, 0);
    for(i=0; i<8; i++) {
        Oled_InvertRam(0, 0, 127, 7);
        sleep_ms(1500);
    } 

    sleep_ms(1000);
    Oled_FillScreen(0x00);
    Oled_ConstText("Main Menu", 8, 0);
    Oled_Icon(icon1,20,3,32,32); 
    Oled_Icon(icon2,72,3,32,32); sleep_ms(3000);
    Oled_Icon(icon3,20,3,32,32); 
    Oled_Icon(icon4,72,3,32,32); sleep_ms(3000);

    sleep_ms(1000);
    Oled_FillScreen(0x00);
    Oled_ConstText("Fonts", 35, 0);
    for(i=0; i<113; i++) {
        sprintf(buffer2,"%03d",i);
        Oled_SetFont(Segment_25x40, 25, 40, 46, 58);
        Oled_Text(buffer2, 0, 3);
        Oled_SetFont(Terminal_12x16, 12, 16, 32 ,127);
        Oled_Text(buffer2, 90, 3);
        Oled_SetFont(Font_6x8, 6, 8, 32, 127);
        Oled_Text(buffer2, 99, 6);
        sleep_ms(200);
    }

    sleep_ms(3500);
    Oled_Image(truck_bmp);

    sleep_ms(5000);
    Oled_FillScreen(0x00);
    Oled_SetFont(Terminal_12x16, 12, 16, 32 ,127);
    Oled_ResetScroll();
    for(k=0; k<500; k++) {
        Oled_ConstTextScroll("This is a new scroll function  ");
        sleep_ms(50);
    }

    Oled_FillScreen(0x00);
    i=10; j=0;
    Oled_ResetScroll();
    while(1) {
        Oled_SetFont(Terminal_12x16, 12, 16, 32,127); 
        Oled_ConstTextScroll("The Serial Peripheral Interface (SPI) bus is a synchronous serial data communication bus that operates in Full-Duplex mode.   ");
        if(i==10) { 
            sprintf(buffer2,"%03d",j);
            Oled_SetFont(Segment_25x40, 25, 40, 46, 58);
            Oled_Text(buffer2, 26, 3); 
            if(j==120) {
                j = 0;
            } else {
                j++;
            }
            i=0;
        }
        sleep_ms(50);
        i++;
    }
}
