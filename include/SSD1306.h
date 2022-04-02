#ifndef __SSD1306_h__
#define __SSD1306_h__

uint8_t SSD1306_write_string(uint8_t font_code, uint8_t pag_window, const uint8_t *buffer);
void SSD1306_set_window(uint8_t window, uint8_t byte_value);
void SSD1306_set_text_area_scroller(/* uint8_t first_row, uint8_t nos_rows, */ uint8_t nos_strings, const char **scroll_strings);
void SSD1306_set_text_area(uint8_t window, uint8_t nos_strings, char *message_strings[]);

#endif