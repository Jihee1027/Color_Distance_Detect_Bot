#include "display/screens/loading_screen.h"
#include "display/display_theme.h"
#include "display/display_text.h"
#include "display/display_hw.h"
#include "display/display_gfx.h"
#include "display/widgets/button_widget.h"
#include "display/widgets/car_widget.h"

#include "pico/stdlib.h"
#include "pico/time.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define LOADING_TEXT_SCALE 2
#define LOADING_STRIP_H 18

static bool initialized = false;
static display_loading_stage_t last_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;
static uint8_t last_dots = 0xFF;
static absolute_time_t stage_tick;
static absolute_time_t dot_tick;

static int g_loading_strip_y0;
static int g_loading_strip_h;

static void loading_strip_fill_rect(int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    int local_y = y - g_loading_strip_y0;
    if (local_y < 0) {
        h += local_y;
        local_y = 0;
    }
    if (local_y + h > g_loading_strip_h) {
        h = g_loading_strip_h - local_y;
    }
    if (h <= 0) {
        return;
    }

    gfx_stripbuffer_fill_rect(x, local_y, w, h, color);
}

static void loading_text_fill_rect(int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    int clip_top = g_loading_strip_y0;
    int clip_bottom = g_loading_strip_y0 + g_loading_strip_h;

    if (y < clip_top) {
        h -= (clip_top - y);
        y = clip_top;
    }
    if (y + h > clip_bottom) {
        h = clip_bottom - y;
    }
    if (h <= 0) {
        return;
    }

    loading_strip_fill_rect(x, y, w, h, color);
}

static bool loading_strip_intersects_y(int y, int h) {
    return !(y + h <= g_loading_strip_y0 || y >= g_loading_strip_y0 + g_loading_strip_h);
}

static void loading_draw_sky_strip(void) {
    for (int ly = 0; ly < g_loading_strip_h; ly++) {
        int y = g_loading_strip_y0 + ly;
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
            color = COLOR_DIRT;
        }

        gfx_stripbuffer_fill_rect(0, ly, TFT_WIDTH, 1, color);
    }
}

static void loading_draw_hills_ground_strip(void) {
    if (loading_strip_intersects_y(142, 60)) {
        fill_circle_generic(28,  178, 28, COLOR_HILL_DARK, loading_strip_fill_rect);
        fill_circle_generic(78,  172, 38, COLOR_HILL,      loading_strip_fill_rect);
        fill_circle_generic(132, 180, 26, COLOR_HILL_DARK, loading_strip_fill_rect);
        fill_circle_generic(188, 170, 40, COLOR_HILL,      loading_strip_fill_rect);
        fill_circle_generic(246, 182, 28, COLOR_HILL_DARK, loading_strip_fill_rect);
        fill_circle_generic(302, 174, 34, COLOR_HILL,      loading_strip_fill_rect);
    }

    if (loading_strip_intersects_y(162, 28)) {
        loading_strip_fill_rect(0, 162, TFT_WIDTH, 28, COLOR_GRASS);
        loading_strip_fill_rect(0, 162, TFT_WIDTH, 3, COLOR_GRASS_DARK);
    }

    if (loading_strip_intersects_y(190, 50)) {
        loading_strip_fill_rect(0, 190, TFT_WIDTH, 50, COLOR_DIRT);
        loading_strip_fill_rect(0, 190, TFT_WIDTH, 4, COLOR_DIRT_DARK);
    }
}

static void loading_draw_dirt_pattern_strip(void) {
    if (!loading_strip_intersects_y(194, 46)) {
        return;
    }

    int y_start = g_loading_strip_y0;
    int y_end = g_loading_strip_y0 + g_loading_strip_h;

    if (y_start < 194) {
        y_start = 194;
    }
    if (y_end > TFT_HEIGHT) {
        y_end = TFT_HEIGHT;
    }

    for (int y = y_start; y < y_end; y++) {
        int local_y = y - g_loading_strip_y0;

        for (int x = 0; x < TFT_WIDTH; x += 24) {
            gfx_stripbuffer_fill_rect(x, local_y, 8, 1, COLOR_DIRT_DARK);
            gfx_stripbuffer_fill_rect(x + 8, local_y, 6, 1, COLOR_DIRT_LIGHT);
        }
    }
}

static void loading_draw_clouds_strip(void) {
    if (!loading_strip_intersects_y(0, 125)) {
        return;
    }

    draw_cloud_full_generic(8,   20, 9, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(58,  42, 6, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(112, 16, 7, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(165, 36, 7, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(220, 18, 6, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(260, 48, 7, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(86,  92, 5, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(150, 108, 5, 0, 0, loading_strip_fill_rect);
    draw_cloud_full_generic(228, 96, 5, 0, 0, loading_strip_fill_rect);
}

static void loading_draw_center_text_on_strip(const char *base_text, uint8_t dots) {
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

    int tx = (TFT_WIDTH - (int)tw) / 2;
    int ty = (TFT_HEIGHT - (int)th) / 2 - 6;

    if (!loading_strip_intersects_y(ty, th + 2)) {
        return;
    }

    text_draw_text_scaled_transparent_to_buf(
        (uint16_t)(tx + 1),
        (uint16_t)(ty + 1),
        text,
        COLOR_BLACK,
        LOADING_TEXT_SCALE,
        loading_text_fill_rect
    );

    text_draw_text_scaled_transparent_to_buf(
        (uint16_t)tx,
        (uint16_t)ty,
        text,
        COLOR_WHITE,
        LOADING_TEXT_SCALE,
        loading_text_fill_rect
    );
}

static void loading_render_frame(const char *base_text, uint8_t dots) {
    for (int y0 = 0; y0 < TFT_HEIGHT; y0 += LOADING_STRIP_H) {
        int h = TFT_HEIGHT - y0;
        if (h > LOADING_STRIP_H) {
            h = LOADING_STRIP_H;
        }

        g_loading_strip_y0 = y0;
        g_loading_strip_h = h;

        gfx_stripbuffer_clear(COLOR_BLACK, h);

        loading_draw_sky_strip();
        loading_draw_hills_ground_strip();
        loading_draw_dirt_pattern_strip();
        loading_draw_clouds_strip();

        if (base_text != NULL && base_text[0] != '\0') {
            loading_draw_center_text_on_strip(base_text, dots);
        }

        gfx_stripbuffer_present(y0, h);
    }
}

static const char *loading_stage_text(display_loading_stage_t stage) {
    switch (stage) {
        case DISPLAY_LOADING_STAGE_SEARCH_COLOR:
            return "SEARCHING COLOR";
        case DISPLAY_LOADING_STAGE_CALC_DISTANCE:
            return "CALCULATING DISTANCE";
        case DISPLAY_LOADING_STAGE_START_TEXT:
            return "START";
        case DISPLAY_LOADING_STAGE_CLEAR_TEXT:
            return "";
        default:
            return "LOADING";
    }
}

void loading_screen_render(display_data_t *data) {
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

    if (data->color_found_pulse &&
        data->loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR) {
        data->loading_stage = DISPLAY_LOADING_STAGE_CALC_DISTANCE;
        data->color_found_pulse = false;
        last_stage = data->loading_stage;
        last_dots = 0xFF;
        stage_tick = get_absolute_time();
        dot_tick = get_absolute_time();
    }

    if (data->distance_done_pulse &&
        data->loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
        data->loading_stage = DISPLAY_LOADING_STAGE_START_TEXT;
        data->distance_done_pulse = false;
        last_stage = data->loading_stage;
        last_dots = 0xFF;
        stage_tick = get_absolute_time();
        dot_tick = get_absolute_time();
    }

    if (data->loading_stage == DISPLAY_LOADING_STAGE_START_TEXT) {
        if (absolute_time_diff_us(stage_tick, get_absolute_time()) >= 1500000) {
            data->loading_stage = DISPLAY_LOADING_STAGE_CLEAR_TEXT;
            data->screen = DISPLAY_SCREEN_DRIVE;
            last_stage = data->loading_stage;
            last_dots = 0xFF;
        }
    }

    uint8_t dots = 0;
    if (data->loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR ||
        data->loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
        uint32_t phase = (uint32_t)(absolute_time_diff_us(dot_tick, get_absolute_time()) / 350000);
        dots = (phase % 3) + 1;
    }

    loading_render_frame(loading_stage_text(data->loading_stage), dots);
}

void loading_screen_reset(void) {
    initialized = false;
    last_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;
    last_dots = 0xFF;
}