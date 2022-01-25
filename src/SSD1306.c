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

//
// Table of virtual windows in SSD1306 display
//
struct {
    uint8_t     seg_value;
    uint8_t     pag_value;
    uint8_t     pixel_length;
} page_window_table[] = {
        {0, 0, 128},
        {20, 3, 24},
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

uint8_t SSD1306_write_string(uint8_t font_code, uint8_t window,  uint8_t *buffer) {

uint8_t     pixel_width, pixel_height, nos_pages;
uint8_t     segment, page;
uint8_t     first_char, last_char;
uint8_t     pixel_count;
uint8_t     char_cnt, character;
uint8_t     *ch_pt, *font_index, *font_pt, *font_base;

    font_base    =  font_table[font_code];
    pixel_width  = *font_base;
    pixel_height = *(font_base + 1);
    first_char   = *(font_base + 2);
    last_char    = *(font_base + 3);
    font_base    = font_base + 4;       // skip  font header

    segment = page_window_table[window].seg_value;
    page    = page_window_table[window].pag_value;

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