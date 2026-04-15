#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

#include <stdint.h>

void text_draw_char_scaled(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale);
void text_draw_text_scaled(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);
void text_draw_char_scaled_transparent(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t scale);
void text_draw_text_scaled_transparent(uint16_t x, uint16_t y, const char *text, uint16_t color, uint8_t scale);
uint16_t text_get_width(const char *text, uint8_t scale);

void text_draw_char_scaled_transparent_to_buf(
    int x, int y, char c, uint16_t color, uint8_t scale,
    void (*fill_rect)(int x, int y, int w, int h, uint16_t color));

void text_draw_text_scaled_transparent_to_buf(
    int x, int y, const char *text, uint16_t color, uint8_t scale,
    void (*fill_rect)(int x, int y, int w, int h, uint16_t color));

#endif