#include "display/screens/drive_screen.h"

#include "display/display_gfx.h"
#include "display/display_theme.h"
#include "display/widgets/button_widget.h"
#include "display/widgets/car_widget.h"
#include "display/display_text.h"
#include <stdio.h>
#include "pico/time.h"

#include <stdbool.h>
#include <stdint.h>

#define DRIVE_STRIP_H 22

static bool initialized = false;

static uint8_t wheel_phase = 0;
static uint8_t smoke_phase = 0;
static uint16_t road_offset = 0;
static uint16_t hill_offset = 0;
static uint16_t cloud_offset = 0;

static int g_strip_y0 = 0;
static int g_strip_h = DRIVE_STRIP_H;

static uint16_t g_road_pattern[24];
static bool g_road_pattern_ready = false;


static bool strip_intersects_y(int y, int h) {
    return !(y + h <= g_strip_y0 || y >= g_strip_y0 + g_strip_h);
}

static void drive_text_fill_rect(int x, int y, int w, int h, uint16_t color) {
    int local_y = y - g_strip_y0;

    if (w <= 0 || h <= 0) {
        return;
    }

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (local_y < 0) {
        h += local_y;
        local_y = 0;
    }
    if (x + w > TFT_WIDTH) {
        w = TFT_WIDTH - x;
    }
    if (local_y + h > g_strip_h) {
        h = g_strip_h - local_y;
    }
    if (w <= 0 || h <= 0) {
        return;
    }

    gfx_stripbuffer_fill_rect(x, local_y, w, h, color);
}

static void drive_draw_shadowed_text(int x, int y, const char *text, uint8_t scale) {
    text_draw_text_scaled_transparent_to_buf(
        (uint16_t)(x + 1),
        (uint16_t)(y + 1),
        text,
        COLOR_BLACK,
        scale,
        drive_text_fill_rect
    );

    text_draw_text_scaled_transparent_to_buf(
        (uint16_t)x,
        (uint16_t)y,
        text,
        COLOR_WHITE,
        scale,
        drive_text_fill_rect
    );
}

static void drive_draw_status_text(const display_data_t *data) {
    const uint8_t scale = 1;
    char battery_line[24];
    char total_line[28];
    char current_line[28];

    snprintf(battery_line, sizeof(battery_line), "BATTERY: %.1fV", data->battery_v);
    snprintf(total_line, sizeof(total_line), "TOTAL: %.1f in", data->total_distance_in);
    snprintf(current_line, sizeof(current_line), "CURRENT: %.1f in", data->distance_in);

    const int battery_w = (int)text_get_width(battery_line, scale);

    const int left_x = 10;
    const int total_y = 200;
    const int current_y = 215;
    const int battery_x = TFT_WIDTH - battery_w - 10;
    const int battery_y = 215;

    if (strip_intersects_y(total_y, 8)) {
        drive_draw_shadowed_text(left_x, total_y, total_line, scale);
    }

    if (strip_intersects_y(current_y, 8)) {
        drive_draw_shadowed_text(left_x, current_y, current_line, scale);
        drive_draw_shadowed_text(battery_x, battery_y, battery_line, scale);
    }
}

static void drive_init_road_pattern(void) {
    if (g_road_pattern_ready) {
        return;
    }

    for (int i = 0; i < 24; i++) {
        if (i < 8) {
            g_road_pattern[i] = COLOR_DIRT_DARK;
        } else if (i < 14) {
            g_road_pattern[i] = COLOR_DIRT_LIGHT;
        } else {
            g_road_pattern[i] = COLOR_DIRT;
        }
    }

    g_road_pattern_ready = true;
}

static void drive_strip_fill_rect(int x, int y, int w, int h, uint16_t color) {
    int local_y = y - g_strip_y0;
    gfx_stripbuffer_fill_rect(x, local_y, w, h, color);
}

static void drive_strip_fill_circle(int x, int y, int r, uint16_t color) {
    int local_y = y - g_strip_y0;
    gfx_stripbuffer_fill_circle(x, local_y, r, color);
}

static void drive_draw_sky_strip(void) {
    for (int ly = 0; ly < g_strip_h; ly++) {
        int y = g_strip_y0 + ly;
        uint16_t color;

        if (y < 25) {
            color = COLOR_SKY_TOP;
        } else if (y < 55) {
            color = COLOR_SKY_MID1;
        } else if (y < 95) {
            color = COLOR_SKY_MID2;
        } else if (y < 130) {
            color = COLOR_SKY_LOW;
        } else {
            color = COLOR_SKY_MID2;
        }

        gfx_stripbuffer_fill_rect(0, ly, TFT_WIDTH, 1, color);
    }
}

static void drive_draw_clouds_strip(void) {
    int c = cloud_offset % 320;

    if (!strip_intersects_y(0, 125)) {
        return;
    }

    draw_cloud_full_generic(8   - c,       20,  9, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(58  - c,       42,  6, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(112 - c,       16,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(165 - c,       36,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(220 - c,       18,  6, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(260 - c,       48,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(86  - c,       92,  5, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(150 - c,      108,  5, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(228 - c,       96,  5, 0, 0, drive_strip_fill_rect);

    draw_cloud_full_generic(8   - c + 320, 20,  9, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(58  - c + 320, 42,  6, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(112 - c + 320, 16,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(165 - c + 320, 36,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(220 - c + 320, 18,  6, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(260 - c + 320, 48,  7, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(86  - c + 320, 92,  5, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(150 - c + 320,108,  5, 0, 0, drive_strip_fill_rect);
    draw_cloud_full_generic(228 - c + 320, 96,  5, 0, 0, drive_strip_fill_rect);
}

static void drive_draw_hills_ground_strip(void) {
    int hx = hill_offset % 320;

    if (strip_intersects_y(142, 60)) {
        fill_circle_generic(28  - hx,       178, 28, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(78  - hx,       172, 38, COLOR_HILL,      drive_strip_fill_rect);
        fill_circle_generic(132 - hx,       180, 26, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(188 - hx,       170, 40, COLOR_HILL,      drive_strip_fill_rect);
        fill_circle_generic(246 - hx,       182, 28, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(302 - hx,       174, 34, COLOR_HILL,      drive_strip_fill_rect);

        fill_circle_generic(28  - hx + 320, 178, 28, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(78  - hx + 320, 172, 38, COLOR_HILL,      drive_strip_fill_rect);
        fill_circle_generic(132 - hx + 320, 180, 26, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(188 - hx + 320, 170, 40, COLOR_HILL,      drive_strip_fill_rect);
        fill_circle_generic(246 - hx + 320, 182, 28, COLOR_HILL_DARK, drive_strip_fill_rect);
        fill_circle_generic(302 - hx + 320, 174, 34, COLOR_HILL,      drive_strip_fill_rect);
    }

    if (strip_intersects_y(162, 28)) {
        drive_strip_fill_rect(0, 162, TFT_WIDTH, 28, COLOR_GRASS);
        drive_strip_fill_rect(0, 162, TFT_WIDTH, 3, COLOR_GRASS_DARK);
    }

    if (strip_intersects_y(190, 50)) {
        drive_strip_fill_rect(0, 190, TFT_WIDTH, 50, COLOR_DIRT);
        drive_strip_fill_rect(0, 190, TFT_WIDTH, 4, COLOR_DIRT_DARK);
    }
}

static void drive_draw_road_motion_strip(void) {
    if (!strip_intersects_y(194, 46)) {
        return;
    }

    drive_init_road_pattern();

    int x_offset = road_offset % 24;

    int y_start = g_strip_y0;
    int y_end = g_strip_y0 + g_strip_h;

    if (y_start < 194) {
        y_start = 194;
    }
    if (y_end > 240) {
        y_end = 240;
    }

    for (int y = y_start; y < y_end; y++) {
        int local_y = y - g_strip_y0;

        if (y < 198) {
            gfx_stripbuffer_fill_rect(0, local_y, TFT_WIDTH, 1, COLOR_DIRT_DARK);
        } else {
            gfx_stripbuffer_draw_hline_pattern(local_y, g_road_pattern, 24, -x_offset);
        }
    }
}

static void drive_draw_arrived_text(void) {
    const uint8_t scale = 2;
    const char *text = "ARRIVED";

    int text_w = (int)text_get_width(text, scale);
    int text_h = 7 * scale;

    int tx = (TFT_WIDTH - text_w) / 2;
    int ty = 108;

    if (!strip_intersects_y(ty, text_h + 2)) {
        return;
    }

    drive_draw_shadowed_text(tx, ty, text, scale);
}

static void drive_draw_interrupted_text(void) {
    const uint8_t scale = 2;
    static uint8_t dots = 0;
    static absolute_time_t tick;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        tick = get_absolute_time();
        dots = 0;
    }

    if (absolute_time_diff_us(tick, get_absolute_time()) >= 400000) {
        tick = get_absolute_time();
        dots = (uint8_t)((dots + 1) % 4);
    }

    char text[24];
    const char *base = "INTERRUPTED";

    int len = 0;
    while (base[len] != '\0') {
        text[len] = base[len];
        len++;
    }

    for (uint8_t i = 0; i < dots; i++) {
        text[len++] = '.';
    }

    text[len] = '\0';

    int text_w = (int)text_get_width(text, scale);
    int text_h = 7 * scale;

    int tx = (TFT_WIDTH - text_w) / 2;
    int ty = 108;

    if (!strip_intersects_y(ty, text_h + 2)) {
        return;
    }

    drive_draw_shadowed_text(tx, ty, text, scale);
}


void drive_screen_render(display_data_t *data) {
    const int car_x = 120;
    const int car_y = 135;
    const int car_scale = 3;
    const int car_top = car_y + 3 * car_scale;
    const int car_h = 13 * car_scale;

    bool is_stopped = (data->state == DISPLAY_STATE_STOPPED);
    bool is_interrupted = (data->state == DISPLAY_STATE_INTERRUPTED);

    if (data->screen != DISPLAY_SCREEN_DRIVE) {
        initialized = false;
        return;
    }

    if (!initialized) {
        initialized = true;
        wheel_phase = 0;
        smoke_phase = 0;
        road_offset = 0;
        hill_offset = 0;
        cloud_offset = 0;
    }

    if (!is_stopped && !is_interrupted) {
        wheel_phase = (wheel_phase + 3) % 4;
        smoke_phase = (smoke_phase + 4) % 6;

        road_offset += 1;
        hill_offset += 3;
        cloud_offset += 1;
    }

    for (int y0 = 0; y0 < TFT_HEIGHT; y0 += DRIVE_STRIP_H) {
        int h = DRIVE_STRIP_H;
        if (y0 + h > TFT_HEIGHT) {
            h = TFT_HEIGHT - y0;
        }

        g_strip_y0 = y0;
        g_strip_h = h;

        drive_draw_sky_strip();
        drive_draw_clouds_strip();
        drive_draw_hills_ground_strip();
        drive_draw_road_motion_strip();

        if (strip_intersects_y(car_top, car_h)) {

            if (is_stopped || is_interrupted) {
                car_widget_draw_stopped_to_buffer(
                    (uint16_t)car_x,
                    (uint16_t)car_y,
                    (uint8_t)car_scale,
                    data->selected_color_hex,
                    drive_strip_fill_rect,
                    drive_strip_fill_circle
                );
            } else {
                car_widget_draw_moving_to_buffer(
                    (uint16_t)car_x,
                    (uint16_t)car_y,
                    (uint8_t)car_scale,
                    data->selected_color_hex,
                    drive_strip_fill_rect,
                    drive_strip_fill_circle,
                    wheel_phase,
                    smoke_phase
                );
            }
        }

        drive_draw_status_text(data);

        if (is_stopped) {
            drive_draw_arrived_text();
        }

        if (is_interrupted) {
            drive_draw_interrupted_text();
        }

        gfx_stripbuffer_present(y0, h);
    }
}