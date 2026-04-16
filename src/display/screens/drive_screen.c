#include "display/screens/drive_screen.h"

#include "display/display_gfx.h"
#include "display/display_theme.h"
#include "display/widgets/button_widget.h"
#include "display/widgets/car_widget.h"

#include <stdbool.h>
#include <stdint.h>

#define DRIVE_STRIP_H 18

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

static bool strip_intersects_y(int y, int h) {
    return !(y + h <= g_strip_y0 || y >= g_strip_y0 + g_strip_h);
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

static void drive_draw_smoke_buf(uint8_t phase, int car_x, int car_y, int scale) {
    int sx = car_x - 8 * scale;
    int sy = car_y + 7 * scale;

    if (!strip_intersects_y(sy - 8, 28)) {
        return;
    }

    switch (phase) {
        case 0:
            drive_strip_fill_circle(sx,     sy + 10, 3, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 8, sy + 6,  4, COLOR_CLOUD);
            break;
        case 1:
            drive_strip_fill_circle(sx - 2,  sy + 8,  3, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 11, sy + 4,  5, COLOR_CLOUD);
            break;
        case 2:
            drive_strip_fill_circle(sx - 4,  sy + 6,  4, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 14, sy + 2,  5, COLOR_CLOUD);
            break;
        case 3:
            drive_strip_fill_circle(sx - 6,  sy + 4,  4, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 17, sy + 0,  6, COLOR_CLOUD);
            break;
        case 4:
            drive_strip_fill_circle(sx - 8,  sy + 3,  4, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 20, sy - 2,  5, COLOR_CLOUD);
            break;
        default:
            drive_strip_fill_circle(sx - 10, sy + 2,  3, COLOR_CLOUD_SHADE);
            drive_strip_fill_circle(sx - 23, sy - 3,  4, COLOR_CLOUD);
            break;
    }
}

static void drive_draw_wheel_motion_buf(uint8_t phase, int car_x, int car_y, int scale) {
    int wx1 = car_x + 7 * scale;
    int wx2 = car_x + 17 * scale;
    int wy  = car_y + 12 * scale;

    if (!strip_intersects_y(wy - 4, 8)) {
        return;
    }

    switch (phase) {
        case 0:
            drive_strip_fill_rect(wx1 - 1, wy - 4, 2, 8, COLOR_WHITE);
            drive_strip_fill_rect(wx2 - 1, wy - 4, 2, 8, COLOR_WHITE);
            break;

        case 1:
            gfx_stripbuffer_draw_pixel(wx1 - 3, (wy - 3) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 - 2, (wy - 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 + 2, (wy + 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 + 3, (wy + 3) - g_strip_y0, COLOR_WHITE);

            gfx_stripbuffer_draw_pixel(wx2 - 3, (wy - 3) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 - 2, (wy - 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 + 2, (wy + 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 + 3, (wy + 3) - g_strip_y0, COLOR_WHITE);
            break;

        case 2:
            drive_strip_fill_rect(wx1 - 4, wy - 1, 8, 2, COLOR_WHITE);
            drive_strip_fill_rect(wx2 - 4, wy - 1, 8, 2, COLOR_WHITE);
            break;

        default:
            gfx_stripbuffer_draw_pixel(wx1 - 3, (wy + 3) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 - 2, (wy + 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 + 2, (wy - 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx1 + 3, (wy - 3) - g_strip_y0, COLOR_WHITE);

            gfx_stripbuffer_draw_pixel(wx2 - 3, (wy + 3) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 - 2, (wy + 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 + 2, (wy - 2) - g_strip_y0, COLOR_WHITE);
            gfx_stripbuffer_draw_pixel(wx2 + 3, (wy - 3) - g_strip_y0, COLOR_WHITE);
            break;
    }
}

void drive_screen_render(display_data_t *data) {
    const int car_x = 120;
    const int car_y = 135;
    const int car_scale = 3;
    const int car_top = car_y + 3 * car_scale;
    const int car_h = 13 * car_scale;

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

    wheel_phase = (uint8_t)((wheel_phase + 1) % 4);
    smoke_phase = (uint8_t)((smoke_phase + 1) % 6);

    road_offset += 2;
    hill_offset += 3;
    cloud_offset += 2;

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
            drive_draw_smoke_buf(smoke_phase, car_x, car_y, car_scale);

            car_widget_draw_to_buffer((uint16_t)car_x,
                                      (uint16_t)car_y,
                                      (uint8_t)car_scale,
                                      COLOR_RED,
                                      drive_strip_fill_rect,
                                      drive_strip_fill_circle);

            drive_draw_wheel_motion_buf(wheel_phase, car_x, car_y, car_scale);
        }

        gfx_stripbuffer_present(y0, h);
    }
}