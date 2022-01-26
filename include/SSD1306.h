#ifndef __SSD1306_h__
#define __SSD1306_h__

uint8_t SSD1306_write_string(uint8_t font_code, uint8_t pag_window, uint8_t *buffer);
void SSD1306_set_window(uint8_t window, uint8_t byte_value);

#endif