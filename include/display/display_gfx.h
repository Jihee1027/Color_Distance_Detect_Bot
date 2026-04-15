#ifndef DISPLAY_GFX_H
#define DISPLAY_GFX_H

#include <stdint.h>

void gfx_fill_screen(uint16_t color);
void gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void gfx_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void gfx_fill_circle(int x0, int y0, int r, uint16_t color);

#endif