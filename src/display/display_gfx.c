#include "display/display_gfx.h"
#include "display/display_hw.h"
#include "display/display_theme.h"
#include "hardware/spi.h"

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