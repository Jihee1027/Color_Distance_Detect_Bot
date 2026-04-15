#include "display/screens/loading_screen.h"
#include "display/display_theme.h"
#include "display/display_text.h"
#include "display/display_hw.h"
#include "display/widgets/button_widget.h"
#include "display/widgets/car_widget.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define LOADING_BUF_W TFT_WIDTH
#define LOADING_BUF_H TFT_HEIGHT
#define LOADING_TEXT_SCALE 2

static uint16_t g_loading_buf[LOADING_BUF_W * LOADING_BUF_H];

static void loading_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > LOADING_BUF_W) {
        w = LOADING_BUF_W - x;
    }
    if (y + h > LOADING_BUF_H) {
        h = LOADING_BUF_H - y;
    }
    if (w <= 0 || h <= 0) {
        return;
    }

    for (int yy = y; yy < y + h; yy++) {
        for (int xx = x; xx < x + w; xx++) {
            g_loading_buf[yy * LOADING_BUF_W + xx] = color;
        }
    }
}

static void loading_draw_background(void) {
    for (int y = 0; y < LOADING_BUF_H; y++) {
        for (int x = 0; x < LOADING_BUF_W; x++) {
            uint16_t color;

            if (y < 25) {
                color = COLOR_SKY_TOP;
            } else if (y < 55) {
                color = COLOR_SKY_MID1;
            } else if (y < 95) {
                color = COLOR_SKY_MID2;
            } else if (y < 130) {
                color = COLOR_SKY_LOW;
            } else if (y < 160) {
                color = COLOR_SKY_MID2;
            } else if (y < 165) {
                color = COLOR_GRASS_DARK;
            } else if (y < 190) {
                color = COLOR_GRASS;
            } else if (y < 194) {
                color = COLOR_DIRT_DARK;
            } else {
                int mod = x % 24;
                if (mod < 8) {
                    color = COLOR_DIRT_DARK;
                } else if (mod < 14) {
                    color = COLOR_DIRT_LIGHT;
                } else {
                    color = COLOR_DIRT;
                }
            }

            g_loading_buf[y * LOADING_BUF_W + x] = color;
        }
    }

    draw_loading_scene_overlay_generic(0, 0, loading_buf_fill_rect);
}

static void loading_blit(void) {
    display_hw_set_window(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < LOADING_BUF_W * LOADING_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_loading_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_loading_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

static void loading_draw_center_text_to_buffer(const char *base_text, uint8_t dots) {
    char text[48];
    int len = 0;

    while (base_text[len] != '\0' && len < 43) {
        text[len] = base_text[len];
        len++;
    }

    for (uint8_t i = 0; i < dots && len < 47; i++) {
        text[len++] = '.';
    }
    text[len] = '\0';

    uint16_t tw = text_get_width(text, LOADING_TEXT_SCALE);
    uint16_t th = 7 * LOADING_TEXT_SCALE;

    int tx = (TFT_WIDTH - tw) / 2;
    int ty = (TFT_HEIGHT - th) / 2 - 6;

    text_draw_text_scaled_transparent_to_buf(
        tx + 1, ty + 1, text, COLOR_BLACK, LOADING_TEXT_SCALE, loading_buf_fill_rect);

    text_draw_text_scaled_transparent_to_buf(
        tx, ty, text, COLOR_WHITE, LOADING_TEXT_SCALE, loading_buf_fill_rect);
}

void loading_screen_render(display_data_t *data) {
    static bool initialized = false;
    static display_loading_stage_t last_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;
    static uint8_t last_dots = 0xFF;
    static absolute_time_t stage_tick;
    static absolute_time_t dot_tick;

    if (!initialized) {
        initialized = true;
        last_stage = data->loading_stage;
        last_dots = 0xFF;
        stage_tick = get_absolute_time();
        dot_tick = get_absolute_time();
    }

    if (data->loading_stage != last_stage) {
        last_stage = data->loading_stage;
        last_dots = 0xFF;
        stage_tick = get_absolute_time();
        dot_tick = get_absolute_time();
    }

    // SEARCH -> CALC
    if (data->color_found_pulse &&
        data->loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR) {
        data->loading_stage = DISPLAY_LOADING_STAGE_CALC_DISTANCE;
        data->color_found_pulse = false;
    }

    // CALC -> START
    if (data->distance_done_pulse &&
        data->loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
        data->loading_stage = DISPLAY_LOADING_STAGE_START_TEXT;
        data->distance_done_pulse = false;
        stage_tick = get_absolute_time();
    }

    // START -> CLEAR 
    if (data->loading_stage == DISPLAY_LOADING_STAGE_START_TEXT) {
        if (absolute_time_diff_us(stage_tick, get_absolute_time()) >= 1500000) {
            data->loading_stage = DISPLAY_LOADING_STAGE_CLEAR_TEXT;
            data->screen = DISPLAY_SCREEN_DRIVE; // Drive stage
        }
    }

    // dots (SEARCH / CALC only)
    uint8_t dots = 0;
    if (data->loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR ||
        data->loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
        uint32_t phase = (uint32_t)(absolute_time_diff_us(dot_tick, get_absolute_time()) / 350000);
        dots = (phase % 3) + 1;
    }

    if (dots != last_dots || data->loading_stage != last_stage) {
        loading_draw_background();

        if (data->loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR) {
            loading_draw_center_text_to_buffer("SEARCHING COLOR", dots);
        } else if (data->loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
            loading_draw_center_text_to_buffer("CALCULATING DISTANCE", dots);
        } else if (data->loading_stage == DISPLAY_LOADING_STAGE_START_TEXT) {
            loading_draw_center_text_to_buffer("START", 0);
        } else {
            // CLEAR_TEXT
        }

        loading_blit();
        last_dots = dots;
        last_stage = data->loading_stage;
    }
}

uint16_t *loading_screen_get_buf(void) {
    return g_loading_buf;
}

void loading_screen_blit_buf(void) {
    loading_blit();
}