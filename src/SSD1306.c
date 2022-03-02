#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "OLED_128X64.h"

#include "system.h"

//#include "font.h"
//#include "bitmap.h"

uint8_t  page_buffer[128];

LCD_scroll_data_t   LCD_scroll_data;

//
// Table of virtual windows in SSD1306 display
//
struct {
    uint8_t     X1_seg_value;
    uint8_t     Y1_pag_value;
    uint8_t     X2_seg_value;
    uint8_t     Y2_pag_value;
} page_window_table[] = {
        {0, 0, 127, 7}, // window 0 is full display
        {0, 0, 127, 1}, // window 1 : rows 0 and 1 - ICON area
        {0, 2, 127, 3}, // window 2 : rows 2 and 3 - important message area
        {0, 4, 127, 5}, // window 3 : rows 4 and 5 - upper scroll page
        {0, 6, 127, 7}, // window 4 : rows 6 and 7 - lower scroll page
        {0, 4, 127, 7}, // window 5 : rows 4 to 7 - info scroll area
};

/**
 * @brief Write a set of bytes to SSD1306 using page addressing
 * 
 * @param buffer    Set of bytes
 * @param len       Length of buffer
 * @param seg       start point in display page (0->127)
 * @param pag       selected page (0->7) (0 is top page)
 * @return uint8_t  ** to be decided
 */
uint8_t SSD1306_write_pag_buf(uint8_t font_index, uint8_t *buffer, uint8_t len){

    //Oled_SetPointer(seg, pag);      // set start point
    for (uint8_t i=0; i < len ; i++) {
        Oled_WriteRam(*buffer);     // write sequentially
    }
    return 0;
}

/**
 * @brief output string to SSD1306 using horizontal indexing
 * 
 * @param font_code Index into table of available fonts
 * @param window    Index into table of defined windows
 * @param buffer    null terminated string to be output
 * @return uint8_t 
 */
uint8_t SSD1306_write_string(uint8_t font_code, uint8_t window,  uint8_t *buffer) {

uint8_t     pixel_width, pixel_height, nos_pages;
uint8_t     segment, page;
uint8_t     first_char, last_char;
uint8_t     pixel_count;
uint8_t     char_cnt, character;
uint8_t     *ch_pt, *font_index, *font_pt;
uint8_t const *font_base;

    font_base    =  font_table[font_code];
    pixel_width  = *font_base;
    pixel_height = *(font_base + 1);
    first_char   = *(font_base + 2);
    last_char    = *(font_base + 3);
    font_base    =  font_base + 4;       // skip  font header

    segment = page_window_table[window].X1_seg_value;
    page    = page_window_table[window].Y1_pag_value;

    nos_pages = pixel_height/8;

    char_cnt = strlen(buffer);
    pixel_count = 0;
    
// outer page loop
    for (uint8_t i=0 ; i < nos_pages ; i++) {
        ch_pt = buffer;
// string loop
        Oled_SetPointer(segment, page);
        for (uint8_t j=0 ; j<char_cnt ; j++) {
            character = *ch_pt++;
            font_index = (uint8_t*)(font_base + ((character - first_char) * (pixel_width * nos_pages)));
// write character
            for (uint8_t k=i; k < (pixel_width * nos_pages); k=k+nos_pages) {
                Oled_WriteRam(*(font_index + k));
            }
        }
        page++;
    }
    return  0;
}

/**
 * @brief Set each byte (1024) of the display
 * 
 * @param window        Index into table of defined windows
 * @param byte_value     
 */
void SSD1306_set_window(uint8_t window, uint8_t byte_value) {

    uint8_t nos_pages  = ((page_window_table[window].Y2_pag_value - page_window_table[window].Y1_pag_value) + 1);
    uint8_t nos_pixels = ((page_window_table[window].X2_seg_value - page_window_table[window].X1_seg_value) + 1);

    for (uint8_t i = page_window_table[window].Y1_pag_value ; i <= page_window_table[window].Y2_pag_value ; i++) {
        Oled_SetPointer(page_window_table[window].X1_seg_value, i);
        for (uint8_t j = page_window_table[window].X1_seg_value; j <= page_window_table[window].X2_seg_value ; j++){
            Oled_WriteRam(byte_value);
        }
    }
}

/**
 * @brief Load scroller data structure
 * 
 * @param LCD_scroll_data 
 */
void SSD1306_set_text_area_scroller(uint8_t window, uint8_t nos_strings, char **scroll_strings)
{
uint8_t index, string_length;

    LCD_scroll_data.enable = false;

    LCD_scroll_data.nos_strings = nos_strings;
    for (index = 0; index < nos_strings; index++) {
        string_length = strlen(scroll_strings[index]);
        if (string_length >= MAX_SSD1306_STRING_LENGTH) {
            string_length = MAX_SSD1306_STRING_LENGTH;
            scroll_strings[index][MAX_SSD1306_STRING_LENGTH] = '\0';
        }
        strcpy(LCD_scroll_data.string_data[index], scroll_strings[index]);
    }
    LCD_scroll_data.string_count = 0;
    LCD_scroll_data.nos_LCD_rows = page_window_table[window].Y2_pag_value - page_window_table[window].Y1_pag_value - 1;
    LCD_scroll_data.first_LCD_row = page_window_table[window].Y1_pag_value;

    LCD_scroll_data.scroll_delay_count = 0;
    LCD_scroll_data.scroll_delay = SCROLL_DELAY_TICK_COUNT;

    return;
}