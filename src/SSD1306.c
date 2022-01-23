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
} page_window_table[6] = {
        {0, 0, 128}
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

uint8_t SSD1306_write_string(/* uint8_t font_code, uint8_t pag_window, */ uint8_t *buffer) {

uint8_t     nos_pages = 2;      // tmp : pixel_height / 2;
uint8_t     nos_x_pixels = 9;   // tmp : pixel_width
uint8_t     seg = 0;            // tmp : get from window data
uint8_t     pag = 0;            // tmp : get from window data
uint8_t     font_chr_1 = ' ';   // tmp : get from font file
uint8_t     pixel_count = 0;
uint8_t     char_cnt, character;
uint8_t     *ch_pt, *font_index, *font_pt, *font_base;


    char_cnt = strlen(buffer);
    pixel_count = 0;
    font_base = (uint8_t*)&Terminal_9x16[0];

// outer page loop
    for (uint8_t i=0 ; i < nos_pages ; i++) {
        ch_pt = buffer;
// string loop
        Oled_SetPointer(seg, pag);
        for (uint8_t j=0 ; j<char_cnt ; j++) {
            character = *ch_pt++;
            font_index = (uint8_t*)(font_base + ((character - font_chr_1) * (nos_x_pixels * nos_pages)));
// write character
            for (uint8_t k=i; k < (nos_x_pixels * nos_pages); k=k+nos_pages) {
                Oled_WriteRam(*(font_index + k));
            }
        }
        pag++;
    }
    return  0;
}