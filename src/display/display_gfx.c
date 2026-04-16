#include "display/display_gfx.h"
#include "display/display_hw.h"
#include "hardware/spi.h"
#include "display/display_theme.h"

#define GFX_STRIP_H 18

static uint16_t g_stripbuf[TFT_WIDTH * GFX_STRIP_H];

void gfx_stripbuffer_clear(uint16_t color, int h) {
    int count = TFT_WIDTH * h;
    for (int i = 0; i < count; i++) {
        g_stripbuf[i] = color;
    }
}

void gfx_stripbuffer_draw_pixel(int x, int y_in_strip, uint16_t color) {
    if (x < 0 || x >= TFT_WIDTH || y_in_strip < 0 || y_in_strip >= GFX_STRIP_H) {
        return;
    }

    g_stripbuf[y_in_strip * TFT_WIDTH + x] = color;
}

void gfx_stripbuffer_fill_rect(int x, int y_in_strip, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    if (x < 0) {
        w += x;
        x = 0;
    }

    if (y_in_strip < 0) {
        h += y_in_strip;
        y_in_strip = 0;
    }

    if (x + w > TFT_WIDTH) {
        w = TFT_WIDTH - x;
    }

    if (y_in_strip + h > GFX_STRIP_H) {
        h = GFX_STRIP_H - y_in_strip;
    }

    if (w <= 0 || h <= 0) {
        return;
    }

    for (int yy = y_in_strip; yy < y_in_strip + h; yy++) {
        uint16_t *row = &g_stripbuf[yy * TFT_WIDTH + x];
        for (int xx = 0; xx < w; xx++) {
            row[xx] = color;
        }
    }
}

void gfx_stripbuffer_fill_circle(int x0, int y0_in_strip, int r, uint16_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                gfx_stripbuffer_draw_pixel(x0 + x, y0_in_strip + y, color);
            }
        }
    }
}

void gfx_stripbuffer_present(int y_start, int h) {
    display_hw_set_window(0, y_start, TFT_WIDTH - 1, y_start + h - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < TFT_WIDTH * h; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_stripbuf[i] >> 8);
        pixel[1] = (uint8_t)(g_stripbuf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

/* direct draw functions keep for other screens */

void gfx_fill_screen(uint16_t color) {
    uint8_t pixel[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };

    display_hw_set_window(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    uint8_t pixel[2];

    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) {
        return;
    }

    pixel[0] = (uint8_t)(color >> 8);
    pixel[1] = (uint8_t)(color & 0xFF);

    display_hw_set_window(x, y, x, y);
    display_hw_begin_pixels();
    spi_write_blocking(display_hw_get_port(), pixel, 2);
    display_hw_end_pixels();
}

void gfx_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    uint8_t pixel[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };

    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || w == 0 || h == 0) {
        return;
    }

    if (x + w > TFT_WIDTH) {
        w = TFT_WIDTH - x;
    }

    if (y + h > TFT_HEIGHT) {
        h = TFT_HEIGHT - y;
    }

    display_hw_set_window(x, y, x + w - 1, y + h - 1);
    display_hw_begin_pixels();

    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void gfx_fill_circle(int x0, int y0, int r, uint16_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                int px = x0 + x;
                int py = y0 + y;

                if (px >= 0 && px < TFT_WIDTH && py >= 0 && py < TFT_HEIGHT) {
                    gfx_draw_pixel((uint16_t)px, (uint16_t)py, color);
                }
            }
        }
    }
}

void gfx_stripbuffer_draw_hline_pattern(int y_in_strip,
                                        const uint16_t *pattern,
                                        int pattern_w,
                                        int x_offset) {
    if (y_in_strip < 0 || y_in_strip >= GFX_STRIP_H || pattern_w <= 0) {
        return;
    }

    uint16_t *row = &g_stripbuf[y_in_strip * TFT_WIDTH];

    for (int x = 0; x < TFT_WIDTH; x++) {
        int px = (x + x_offset) % pattern_w;
        if (px < 0) {
            px += pattern_w;
        }
        row[x] = pattern[px];
    }
}