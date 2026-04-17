#ifndef DISPLAY_GFX_H
#define DISPLAY_GFX_H

#include <stdint.h>
#include "display/display_hw.h"

void gfx_fill_screen(uint16_t color);
void gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void gfx_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void gfx_fill_circle(int x0, int y0, int r, uint16_t color);

void gfx_stripbuffer_clear(uint16_t color, int h);
void gfx_stripbuffer_draw_pixel(int x, int y_in_strip, uint16_t color);
void gfx_stripbuffer_fill_rect(int x, int y_in_strip, int w, int h, uint16_t color);
void gfx_stripbuffer_fill_circle(int x0, int y0_in_strip, int r, uint16_t color);
void gfx_stripbuffer_present(int y_start, int h);
void gfx_stripbuffer_draw_hline_pattern(int y_in_strip,
                                        const uint16_t *pattern,
                                        int pattern_w,
                                        int x_offset);

#endif