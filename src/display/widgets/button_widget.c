#include "display/widgets/button_widget.h"
#include "display/display_theme.h"
#include "display/display_text.h"
#include "display/display_hw.h"
#include "hardware/spi.h"
#include "display/display_gfx.h"
#include <stdint.h>
#include <stdbool.h>

/* Shared widget buffer: sized by the largest widget area instead of the widest widget. */
#define WIDGET_SHARED_BUF_PIXELS (START_BUF_W * START_BUF_H)
static uint16_t g_widget_buf[WIDGET_SHARED_BUF_PIXELS];
static int g_widget_stride = START_BUF_W;
static int g_widget_height = START_BUF_H;

typedef void (*buf_fill_rect_fn)(int x, int y, int w, int h, uint16_t color);

static void widget_buf_set_geometry(int stride, int height) {
    g_widget_stride = stride;
    g_widget_height = height;
}

static void widget_buf_fill_rect_common(int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) return;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > g_widget_stride) w = g_widget_stride - x;
    if (y + h > g_widget_height) h = g_widget_height - y;
    if (w <= 0 || h <= 0) return;

    for (int yy = y; yy < y + h; yy++) {
        uint16_t *row = &g_widget_buf[yy * g_widget_stride + x];
        for (int xx = 0; xx < w; xx++) {
            row[xx] = color;
        }
    }
}

static void button_widget_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    widget_buf_fill_rect_common(x, y, w, h, color);
}

static void start_box_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    widget_buf_fill_rect_common(x, y, w, h, color);
}

static void color_title_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    widget_buf_fill_rect_common(x, y, w, h, color);
}

static void color_option_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    widget_buf_fill_rect_common(x, y, w, h, color);
}

static void color_hint_buf_fill_rect(int x, int y, int w, int h, uint16_t color) {
    widget_buf_fill_rect_common(x, y, w, h, color);
}

void fill_circle_generic(int cx, int cy, int r,
                                uint16_t color,
                                buf_fill_rect_fn fill_rect) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                fill_rect(cx + x, cy + y, 1, 1, color);
            }
        }
    }
}

void draw_cloud_full_generic(int x, int y, int s,
                                    int origin_x, int origin_y,
                                    buf_fill_rect_fn fill_rect) {
    int r1 = 8 * s / 10;
    int r2 = 11 * s / 10;
    int r3 = 8 * s / 10;

    fill_circle_generic(x + 12 * s / 10 + 2 - origin_x,
                        y + 14 * s / 10 + 2 - origin_y,
                        r1, COLOR_CLOUD_DARK, fill_rect);
    fill_circle_generic(x + 28 * s / 10 + 2 - origin_x,
                        y + 8 * s / 10 + 2 - origin_y,
                        r2, COLOR_CLOUD_DARK, fill_rect);
    fill_circle_generic(x + 46 * s / 10 + 2 - origin_x,
                        y + 14 * s / 10 + 2 - origin_y,
                        r3, COLOR_CLOUD_DARK, fill_rect);
    fill_rect(x + 12 * s / 10 + 2 - origin_x,
              y + 14 * s / 10 + 2 - origin_y,
              34 * s / 10, 12 * s / 10, COLOR_CLOUD_DARK);

    fill_circle_generic(x + 11 * s / 10 + 1 - origin_x,
                        y + 13 * s / 10 + 1 - origin_y,
                        r1, COLOR_CLOUD_SHADE, fill_rect);
    fill_circle_generic(x + 27 * s / 10 + 1 - origin_x,
                        y + 7 * s / 10 + 1 - origin_y,
                        r2, COLOR_CLOUD_SHADE, fill_rect);
    fill_circle_generic(x + 45 * s / 10 + 1 - origin_x,
                        y + 13 * s / 10 + 1 - origin_y,
                        r3, COLOR_CLOUD_SHADE, fill_rect);
    fill_rect(x + 11 * s / 10 + 1 - origin_x,
              y + 13 * s / 10 + 1 - origin_y,
              34 * s / 10, 12 * s / 10, COLOR_CLOUD_SHADE);

    fill_circle_generic(x + 10 * s / 10 - origin_x,
                        y + 12 * s / 10 - origin_y,
                        r1, COLOR_CLOUD, fill_rect);
    fill_circle_generic(x + 26 * s / 10 - origin_x,
                        y + 6 * s / 10 - origin_y,
                        r2, COLOR_CLOUD, fill_rect);
    fill_circle_generic(x + 44 * s / 10 - origin_x,
                        y + 12 * s / 10 - origin_y,
                        r3, COLOR_CLOUD, fill_rect);
    fill_rect(x + 10 * s / 10 - origin_x,
              y + 12 * s / 10 - origin_y,
              34 * s / 10, 12 * s / 10, COLOR_CLOUD);

    fill_rect(x + 16 * s / 10 - origin_x,
              y + 8 * s / 10 - origin_y,
              10 * s / 10, 2, COLOR_WHITE);
    fill_rect(x + 28 * s / 10 - origin_x,
              y + 11 * s / 10 - origin_y,
              8 * s / 10, 2, COLOR_WHITE);
}

void draw_start_box_scene_overlay_generic(int origin_x, int origin_y,
                                                 buf_fill_rect_fn fill_rect) {
    fill_circle_generic(28  - origin_x, 178 - origin_y, 28, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(78  - origin_x, 172 - origin_y, 38, COLOR_HILL,      fill_rect);
    fill_circle_generic(132 - origin_x, 180 - origin_y, 26, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(188 - origin_x, 170 - origin_y, 40, COLOR_HILL,      fill_rect);
    fill_circle_generic(246 - origin_x, 182 - origin_y, 28, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(302 - origin_x, 174 - origin_y, 34, COLOR_HILL,      fill_rect);

    draw_cloud_full_generic(8,   20, 9, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(58,  42, 6, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(112, 16, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(165, 36, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(220, 18, 6, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(260, 48, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(86,  92, 5, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(150, 108, 5, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(228, 96, 5, origin_x, origin_y, fill_rect);
}

void draw_loading_scene_overlay_generic(int origin_x, int origin_y,
                                          buf_fill_rect_fn fill_rect) {
    // sky gradient
    for (int y = 0; y < TFT_HEIGHT; y++) {
        int screen_y = origin_y + y;
        uint16_t color;

        if (screen_y < 25) {
            color = COLOR_SKY_TOP;
        } else if (screen_y < 55) {
            color = COLOR_SKY_MID1;
        } else if (screen_y < 95) {
            color = COLOR_SKY_MID2;
        } else if (screen_y < 130) {
            color = COLOR_SKY_LOW;
        } else {
            color = COLOR_SKY_MID2;
        }

        fill_rect(0, y, TFT_WIDTH, 1, color);
    }

    // hills first
    fill_circle_generic(28  - origin_x, 178 - origin_y, 28, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(78  - origin_x, 172 - origin_y, 38, COLOR_HILL,      fill_rect);
    fill_circle_generic(132 - origin_x, 180 - origin_y, 26, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(188 - origin_x, 170 - origin_y, 40, COLOR_HILL,      fill_rect);
    fill_circle_generic(246 - origin_x, 182 - origin_y, 28, COLOR_HILL_DARK, fill_rect);
    fill_circle_generic(302 - origin_x, 174 - origin_y, 34, COLOR_HILL,      fill_rect);

    // grass band like start screen
    fill_rect(0, 162 - origin_y, TFT_WIDTH, 28, COLOR_GRASS);

    // dirt
    fill_rect(0, 190 - origin_y, TFT_WIDTH, 50, COLOR_DIRT);

    // edges
    fill_rect(0, 162 - origin_y, TFT_WIDTH, 3, COLOR_GRASS_DARK);
    fill_rect(0, 190 - origin_y, TFT_WIDTH, 4, COLOR_DIRT_DARK);

    // dirt stripes
    for (int x = 0; x < TFT_WIDTH; x += 24) {
        fill_rect(x - origin_x, 194 - origin_y, 8, 46, COLOR_DIRT_DARK);
        fill_rect(x + 8 - origin_x, 194 - origin_y, 6, 46, COLOR_DIRT_LIGHT);
    }

    // 9 clouds
    draw_cloud_full_generic(8,   20, 9, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(58,  42, 6, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(112, 16, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(165, 36, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(220, 18, 6, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(260, 48, 7, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(86,  92, 5, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(150, 108, 5, origin_x, origin_y, fill_rect);
    draw_cloud_full_generic(228, 96, 5, origin_x, origin_y, fill_rect);
}

static void color_option_buf_draw_panel(int x, int y, int w, int h,
                                        uint16_t face, uint16_t light, uint16_t dark) {
    color_option_buf_fill_rect(x, y, w, h, face);
    color_option_buf_fill_rect(x, y, w, 2, light);
    color_option_buf_fill_rect(x, y, 2, h, light);
    color_option_buf_fill_rect(x, y + h - 2, w, 2, dark);
    color_option_buf_fill_rect(x + w - 2, y, 2, h, dark);
}

static void color_option_buf_draw_panel_double(int x, int y, int w, int h,
                                               uint16_t outer_face, uint16_t outer_light, uint16_t outer_dark,
                                               uint16_t inner_face, uint16_t inner_light, uint16_t inner_dark) {
    color_option_buf_draw_panel(x, y, w, h, outer_face, outer_light, outer_dark);
    color_option_buf_draw_panel(x + 4, y + 4, w - 8, h - 8, inner_face, inner_light, inner_dark);
}

static void button_widget_buf_draw_panel(int x, int y, int w, int h,
                                         uint16_t face, uint16_t light, uint16_t dark) {
    button_widget_buf_fill_rect(x, y, w, h, face);
    button_widget_buf_fill_rect(x, y, w, 2, light);
    button_widget_buf_fill_rect(x, y, 2, h, light);
    button_widget_buf_fill_rect(x, y + h - 2, w, 2, dark);
    button_widget_buf_fill_rect(x + w - 2, y, 2, h, dark);
}

static void button_widget_buf_draw_panel_double(int x, int y, int w, int h,
                                                uint16_t outer_face, uint16_t outer_light, uint16_t outer_dark,
                                                uint16_t inner_face, uint16_t inner_light, uint16_t inner_dark) {
    button_widget_buf_draw_panel(x, y, w, h, outer_face, outer_light, outer_dark);
    button_widget_buf_draw_panel(x + 4, y + 4, w - 8, h - 8, inner_face, inner_light, inner_dark);
}

static void start_box_buf_draw_panel(int x, int y, int w, int h,
                                     uint16_t face, uint16_t light, uint16_t dark) {
    start_box_buf_fill_rect(x, y, w, h, face);
    start_box_buf_fill_rect(x, y, w, 2, light);
    start_box_buf_fill_rect(x, y, 2, h, light);
    start_box_buf_fill_rect(x, y + h - 2, w, 2, dark);
    start_box_buf_fill_rect(x + w - 2, y, 2, h, dark);
}

static void start_box_buf_draw_panel_double(int x, int y, int w, int h,
                                            uint16_t outer_face, uint16_t outer_light, uint16_t outer_dark,
                                            uint16_t inner_face, uint16_t inner_light, uint16_t inner_dark) {
    start_box_buf_draw_panel(x, y, w, h, outer_face, outer_light, outer_dark);
    start_box_buf_draw_panel(x + 4, y + 4, w - 8, h - 8, inner_face, inner_light, inner_dark);
}

static void color_title_buf_draw_panel(int x, int y, int w, int h,
                                       uint16_t face, uint16_t light, uint16_t dark) {
    color_title_buf_fill_rect(x, y, w, h, face);
    color_title_buf_fill_rect(x, y, w, 2, light);
    color_title_buf_fill_rect(x, y, 2, h, light);
    color_title_buf_fill_rect(x, y + h - 2, w, 2, dark);
    color_title_buf_fill_rect(x + w - 2, y, 2, h, dark);
}

static void color_title_buf_draw_panel_double(int x, int y, int w, int h,
                                              uint16_t outer_face, uint16_t outer_light, uint16_t outer_dark,
                                              uint16_t inner_face, uint16_t inner_light, uint16_t inner_dark) {
    color_title_buf_draw_panel(x, y, w, h, outer_face, outer_light, outer_dark);
    color_title_buf_draw_panel(x + 4, y + 4, w - 8, h - 8, inner_face, inner_light, inner_dark);
}

static void button_widget_buf_draw_char_transparent(int x, int y, char c, uint16_t color, uint8_t scale) {
    if ((unsigned char)c >= 128) return;

    static const uint8_t font5x7[128][5] = {
        [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
        ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36},
        ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
        ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
        ['N'] = {0x7F, 0x02, 0x04, 0x08, 0x7F},
        ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
        ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06},
        ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
        ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
        ['U'] = {0x3F, 0x40, 0x40, 0x40, 0x3F},
    };

    for (int col = 0; col < 5; col++) {
        uint8_t bits = font5x7[(int)c][col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                button_widget_buf_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

static void button_widget_buf_draw_text_transparent(int x, int y, const char *text, uint16_t color, uint8_t scale) {
    while (*text) {
        button_widget_buf_draw_char_transparent(x, y, *text, color, scale);
        x += 6 * scale;
        text++;
    }
}

static uint16_t button_widget_buf_text_width(const char *text, uint8_t scale) {
    uint16_t width = 0;
    while (*text) {
        width += 6 * scale;
        text++;
    }
    return width;
}

static void color_title_buf_draw_char_transparent(int x, int y, char c, uint16_t color, uint8_t scale) {
    if ((unsigned char)c >= 128) return;

    static const uint8_t font5x7[128][5] = {
        [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
        ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22},
        ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
        ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
        ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40},
        ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
        ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46},
        ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
        ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
        ['X'] = {0x63, 0x14, 0x08, 0x14, 0x63},
        ['Y'] = {0x03, 0x04, 0x78, 0x04, 0x03},
    };

    for (int col = 0; col < 5; col++) {
        uint8_t bits = font5x7[(int)c][col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                color_title_buf_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

static void color_title_buf_draw_text_transparent(int x, int y, const char *text, uint16_t color, uint8_t scale) {
    while (*text) {
        color_title_buf_draw_char_transparent(x, y, *text, color, scale);
        x += 6 * scale;
        text++;
    }
}

static uint16_t color_title_buf_text_width(const char *text, uint8_t scale) {
    uint16_t width = 0;
    while (*text) {
        width += 6 * scale;
        text++;
    }
    return width;
}

static void color_option_buf_draw_char_transparent(int x, int y, char c, uint16_t color, uint8_t scale) {
    if ((unsigned char)c >= 128) return;

    static const uint8_t font5x7[128][5] = {
        [' '] = {0x00,0x00,0x00,0x00,0x00},
        ['B'] = {0x7F,0x49,0x49,0x49,0x36},
        ['D'] = {0x7F,0x41,0x41,0x22,0x1C},
        ['E'] = {0x7F,0x49,0x49,0x49,0x41},
        ['G'] = {0x3E,0x41,0x49,0x49,0x7A},
        ['L'] = {0x7F,0x40,0x40,0x40,0x40},
        ['N'] = {0x7F,0x02,0x04,0x08,0x7F},
        ['R'] = {0x7F,0x09,0x19,0x29,0x46},
        ['U'] = {0x3F,0x40,0x40,0x40,0x3F},
    };

    for (int col = 0; col < 5; col++) {
        uint8_t bits = font5x7[(int)c][col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                color_option_buf_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

static void color_option_buf_draw_text_transparent(int x, int y, const char *text, uint16_t color, uint8_t scale) {
    while (*text) {
        color_option_buf_draw_char_transparent(x, y, *text, color, scale);
        x += 6 * scale;
        text++;
    }
}

static uint16_t color_option_buf_text_width(const char *text, uint8_t scale) {
    uint16_t width = 0;
    while (*text) {
        width += 6 * scale;
        text++;
    }
    return width;
}

static void start_box_buf_draw_char_transparent(int x, int y, char c, uint16_t color, uint8_t scale) {
    if ((unsigned char)c >= 128) return;

    static const uint8_t font5x7[128][5] = {
        ['A'] = {0x7E, 0x09, 0x09, 0x09, 0x7E},
        ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46},
        ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
        ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
    };

    for (int col = 0; col < 5; col++) {
        uint8_t bits = font5x7[(int)c][col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                start_box_buf_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

static void start_box_buf_draw_text_transparent(int x, int y, const char *text, uint16_t color, uint8_t scale) {
    while (*text) {
        start_box_buf_draw_char_transparent(x, y, *text, color, scale);
        x += 6 * scale;
        text++;
    }
}

static uint16_t start_box_buf_text_width(const char *text, uint8_t scale) {
    uint16_t width = 0;
    while (*text) {
        width += 6 * scale;
        text++;
    }
    return width;
}

static void button_widget_start_draw_bg(void) {
    widget_buf_set_geometry(BTN_BUF_W, BTN_BUF_H);
    for (int y = 0; y < BTN_BUF_H; y++) {
        int screen_y = BTN_SCREEN_Y + y;

        if (screen_y < 160) {
            button_widget_buf_fill_rect(0, y, BTN_BUF_W, 1, COLOR_SKY_MID2);
        } else if (screen_y < 162) {
            button_widget_buf_fill_rect(0, y, BTN_BUF_W, 1, COLOR_SKY_MID2);
        } else if (screen_y < 165) {
            button_widget_buf_fill_rect(0, y, BTN_BUF_W, 1, COLOR_GRASS_DARK);
        } else if (screen_y < 190) {
            button_widget_buf_fill_rect(0, y, BTN_BUF_W, 1, COLOR_GRASS);
        } else if (screen_y < 194) {
            button_widget_buf_fill_rect(0, y, BTN_BUF_W, 1, COLOR_DIRT_DARK);
        } else {
            for (int x = 0; x < BTN_BUF_W; x++) {
                int screen_x = BTN_SCREEN_X + x;
                int mod = screen_x % 24;

                if (mod < 8) g_widget_buf[y * g_widget_stride + x] = COLOR_DIRT_DARK;
                else if (mod < 14) g_widget_buf[y * g_widget_stride + x] = COLOR_DIRT_LIGHT;
                else g_widget_buf[y * g_widget_stride + x] = COLOR_DIRT;
            }
        }
    }
}

static void color_title_draw_bg(void) {
    widget_buf_set_geometry(COLOR_TITLE_BUF_W, COLOR_TITLE_BUF_H);
    draw_loading_scene_overlay_generic(COLOR_TITLE_SCREEN_X, COLOR_TITLE_SCREEN_Y,
                                       color_title_buf_fill_rect);
}

static void color_option_draw_bg(void) {
    widget_buf_set_geometry(COLOR_OPTION_BUF_W, COLOR_OPTION_BUF_H);
    draw_loading_scene_overlay_generic(COLOR_OPTION_SCREEN_X, COLOR_OPTION_SCREEN_Y,
                                       color_option_buf_fill_rect);
}

static void color_hint_draw_bg(void) {
    widget_buf_set_geometry(COLOR_HINT_BUF_W, COLOR_HINT_BUF_H);
    draw_loading_scene_overlay_generic(COLOR_HINT_SCREEN_X, COLOR_HINT_SCREEN_Y,
                                       color_hint_buf_fill_rect);
}

static void start_box_draw_bg(void) {
    widget_buf_set_geometry(START_BUF_W, START_BUF_H);
    draw_loading_scene_overlay_generic(START_SCREEN_X, START_SCREEN_Y,
                                       start_box_buf_fill_rect);
}

static void button_widget_start_draw_label(uint8_t press_depth) {
    const char *text = "PUSH THE BUTTON";
    uint16_t tw = button_widget_buf_text_width(text, BTN_TEXT_SCALE);
    uint16_t th = 7 * BTN_TEXT_SCALE;

    int tx = (BTN_BUF_W - tw) / 2;
    int ty = (BTN_BUF_H - th) / 2 + (press_depth ? 1 : 0);

    button_widget_buf_draw_text_transparent(tx + 2, ty + 2, text, 0x0000, BTN_TEXT_SCALE);
    button_widget_buf_draw_text_transparent(tx + 1, ty + 1, text, 0x0004, BTN_TEXT_SCALE);
    button_widget_buf_draw_text_transparent(tx, ty, text, 0x000E, BTN_TEXT_SCALE);
    button_widget_buf_draw_text_transparent(tx + 1, ty, text, 0x000E, BTN_TEXT_SCALE);
    button_widget_buf_draw_text_transparent(tx, ty + 1, text, 0x000E, BTN_TEXT_SCALE);

    if (press_depth == 0) {
        button_widget_buf_draw_text_transparent(tx, ty - 1, text, 0x545F, BTN_TEXT_SCALE);
    }
}

static void start_box_draw_label(uint8_t press_depth) {
    const char *text = "START";
    uint8_t scale = 5;

    uint16_t tw = start_box_buf_text_width(text, scale);
    uint16_t th = 7 * scale;

    int tx = (START_BUF_W - tw) / 2;
    int ty = (START_BUF_H - th) / 2 + (press_depth ? 1 : 0);

    start_box_buf_draw_text_transparent(tx + 3, ty + 4, text, COLOR_START_SHADOW, scale);
    start_box_buf_draw_text_transparent(tx + 2, ty + 1, text, COLOR_START_OUTLINE, scale);
    start_box_buf_draw_text_transparent(tx, ty, text, COLOR_START_MAIN, scale);

    if (press_depth == 0) {
        start_box_buf_draw_text_transparent(tx, ty - 2, text, COLOR_START_LIGHT, scale);
    }
}

static void color_title_draw_label(void) {
    const char *text = "SELECT THE COLOR";
    uint16_t tw = color_title_buf_text_width(text, COLOR_TITLE_TEXT_SCALE);
    uint16_t th = 7 * COLOR_TITLE_TEXT_SCALE;

    int tx = (COLOR_TITLE_BUF_W - tw) / 2;
    int ty = (COLOR_TITLE_BUF_H - th) / 2;

    color_title_buf_draw_text_transparent(tx + 2, ty + 2, text, 0x0000, COLOR_TITLE_TEXT_SCALE);
    color_title_buf_draw_text_transparent(tx + 1, ty + 1, text, 0x0004, COLOR_TITLE_TEXT_SCALE);
    color_title_buf_draw_text_transparent(tx, ty, text, 0x000E, COLOR_TITLE_TEXT_SCALE);
    color_title_buf_draw_text_transparent(tx + 1, ty, text, 0x000E, COLOR_TITLE_TEXT_SCALE);
    color_title_buf_draw_text_transparent(tx, ty + 1, text, 0x000E, COLOR_TITLE_TEXT_SCALE);
    color_title_buf_draw_text_transparent(tx, ty - 1, text, 0x545F, COLOR_TITLE_TEXT_SCALE);
}

void button_widget_start_blit(uint16_t screen_x, uint16_t screen_y) {
    widget_buf_set_geometry(BTN_BUF_W, BTN_BUF_H);
    display_hw_set_window(screen_x, screen_y, screen_x + BTN_BUF_W - 1, screen_y + BTN_BUF_H - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < BTN_BUF_W * BTN_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_widget_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_widget_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void button_widget_start_draw_frame(uint8_t scale_step, uint8_t press_depth) {
    uint16_t grow_w = scale_step;
    uint16_t grow_h = scale_step / 4;

    uint16_t btn_w = BTN_BASE_W + grow_w;
    uint16_t btn_h = BTN_BASE_H + grow_h;

    uint16_t btn_x = (BTN_BUF_W - btn_w) / 2;
    uint16_t btn_y = (BTN_BUF_H - btn_h) / 2;

    button_widget_start_draw_bg();

    button_widget_buf_fill_rect(btn_x + 4, btn_y + 4, btn_w, btn_h, COLOR_SHADOW_STRONG);

    if (press_depth == 0) {
        button_widget_buf_draw_panel_double(
            btn_x, btn_y, btn_w, btn_h,
            COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_BOX_BLUE_DARK,
            COLOR_TEXT_BOX, COLOR_BOX_BLUE_LIGHT, COLOR_BOX_BLUE_DARK
        );
    } else {
        button_widget_buf_draw_panel_double(
            btn_x, btn_y, btn_w, btn_h,
            COLOR_WHITE, COLOR_BOX_BLUE_DARK, COLOR_WHITE_SOFT,
            COLOR_TEXT_BOX, COLOR_BOX_BLUE_DARK, COLOR_BOX_BLUE_LIGHT
        );

        button_widget_buf_fill_rect(btn_x + 3, btn_y + 3, btn_w - 6, 2, COLOR_BOX_BLUE_DARK);
        button_widget_buf_fill_rect(btn_x + 3, btn_y + 3, 2, btn_h - 6, COLOR_BOX_BLUE_DARK);
    }

    button_widget_start_draw_label(press_depth);
}

void button_widget_start_box_draw_frame(uint8_t press_depth) {
    uint16_t box_w = START_BASE_W;
    uint16_t box_h = START_BASE_H;

    uint16_t box_x = 0;
    uint16_t box_y = 0;

    start_box_draw_bg();

    uint8_t shadow_offset = (press_depth > 0) ? 2 : 4;
    start_box_buf_fill_rect(box_x + shadow_offset, box_y + shadow_offset, box_w, box_h, COLOR_SHADOW_STRONG);

    if (press_depth == 0) {
        start_box_buf_draw_panel_double(
            box_x, box_y, box_w, box_h,
            COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_PINK_DARK,
            COLOR_PANEL_FILL, COLOR_PINK_LIGHT, COLOR_START_OUTLINE
        );
    } else {
        start_box_buf_draw_panel_double(
            box_x, box_y, box_w, box_h,
            COLOR_WHITE, COLOR_PINK_DARK, COLOR_WHITE_SOFT,
            COLOR_PANEL_FILL, COLOR_PINK_DARK, COLOR_PINK_LIGHT
        );
    }

    start_box_draw_label(press_depth);
}

void button_widget_start_box_blit(uint16_t screen_x, uint16_t screen_y) {
    widget_buf_set_geometry(START_BUF_W, START_BUF_H);
    display_hw_set_window(screen_x, screen_y, screen_x + START_BUF_W - 1, screen_y + START_BUF_H - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < START_BUF_W * START_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_widget_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_widget_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void button_widget_start_draw_disappear_frame(uint8_t t) {
    button_widget_start_draw_bg();

    if (t >= 13) return;

    int shrink_w = t * 18;
    int shrink_h = t * 8;

    int btn_w = BTN_BASE_W - shrink_w;
    int btn_h = BTN_BASE_H - shrink_h;

    if (btn_w <= 0 || btn_h <= 0) return;
    int center_x = BTN_BUF_W / 2;
    int center_y = BTN_BUF_H / 2;

    int btn_x = center_x - btn_w / 2;
    int btn_y = center_y - btn_h / 2;

    button_widget_buf_draw_panel_double(
        btn_x, btn_y, btn_w, btn_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_BOX_BLUE_DARK,
        COLOR_TEXT_BOX, COLOR_BOX_BLUE_LIGHT, COLOR_BOX_BLUE_DARK
    );

    if (t < 3) {
        const char *text = "PUSH THE BUTTON";
        int tw = (int)button_widget_buf_text_width(text, BTN_TEXT_SCALE);
        int th = 7 * BTN_TEXT_SCALE;

        int tx = center_x - tw / 2;
        int ty = center_y - th / 2 - 1;

        button_widget_buf_draw_text_transparent(tx + 2, ty + 2, text, 0x0000, BTN_TEXT_SCALE);
        button_widget_buf_draw_text_transparent(tx + 1, ty + 1, text, 0x0004, BTN_TEXT_SCALE);
        button_widget_buf_draw_text_transparent(tx, ty, text, 0x000E, BTN_TEXT_SCALE);
        button_widget_buf_draw_text_transparent(tx + 1, ty, text, 0x000E, BTN_TEXT_SCALE);
        button_widget_buf_draw_text_transparent(tx, ty + 1, text, 0x000E, BTN_TEXT_SCALE);
        button_widget_buf_draw_text_transparent(tx, ty - 1, text, 0x545F, BTN_TEXT_SCALE);
    }
}

void button_widget_start_box_draw_disappear_frame(uint8_t t) {
    start_box_draw_bg();

    if (t >= 13) return;

    int shrink_w = t * 18;
    int shrink_h = t * 8;

    int box_w = START_BASE_W - shrink_w;
    int box_h = START_BASE_H - shrink_h;

    if (box_w <= 0 || box_h <= 0) return;

    int box_x = (START_BUF_W - box_w) / 2;
    int box_y = (START_BUF_H - box_h) / 2;

    int shadow_offset = (t < 4) ? (4 - t) : 0;
    if (shadow_offset > 0) {
        start_box_buf_fill_rect(box_x + shadow_offset, box_y + shadow_offset,
                                box_w, box_h, COLOR_SHADOW_STRONG);
    }

    start_box_buf_draw_panel_double(
        box_x, box_y, box_w, box_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_PINK_DARK,
        COLOR_PANEL_FILL, COLOR_PINK_LIGHT, COLOR_START_OUTLINE
    );

    if (t < 3) {
        const char *text = "START";
        uint8_t scale = 5;

        uint16_t tw = start_box_buf_text_width(text, scale);
        uint16_t th = 7 * scale;

        int tx = (START_BUF_W - tw) / 2;
        int ty = (START_BUF_H - th) / 2;

        start_box_buf_draw_text_transparent(tx + 3, ty + 4, text, COLOR_START_SHADOW, scale);
        start_box_buf_draw_text_transparent(tx + 2, ty + 1, text, COLOR_START_OUTLINE, scale);
        start_box_buf_draw_text_transparent(tx, ty, text, COLOR_START_MAIN, scale);

        if (t < 2) {
            start_box_buf_draw_text_transparent(tx, ty - 2, text, COLOR_START_LIGHT, scale);
        }
    }
}

void button_widget_color_title_draw_frame(uint8_t scale_step) {
    uint16_t grow_w = scale_step;
    uint16_t grow_h = scale_step / 4;

    uint16_t btn_w = COLOR_TITLE_BASE_W + grow_w;
    uint16_t btn_h = COLOR_TITLE_BASE_H + grow_h;

    uint16_t btn_x = 0;
    uint16_t btn_y = 0;

    color_title_draw_bg();

    color_title_buf_fill_rect(btn_x + 3, btn_y + 3, btn_w, btn_h, COLOR_SHADOW_STRONG);

    color_title_buf_draw_panel_double(
        btn_x, btn_y, btn_w, btn_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_BOX_BLUE_DARK,
        COLOR_TEXT_BOX, COLOR_BOX_BLUE_LIGHT, COLOR_BOX_BLUE_DARK
    );

    color_title_draw_label();
}

void button_widget_color_title_blit(uint16_t screen_x, uint16_t screen_y) {
    widget_buf_set_geometry(COLOR_TITLE_BUF_W, COLOR_TITLE_BUF_H);
    display_hw_set_window(screen_x, screen_y,
                          screen_x + COLOR_TITLE_BUF_W - 1,
                          screen_y + COLOR_TITLE_BUF_H - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < COLOR_TITLE_BUF_W * COLOR_TITLE_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_widget_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_widget_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void button_widget_color_option_draw_frame(const char *label, uint16_t main_color, uint16_t light_color, uint16_t dark_color, uint8_t scale_step) {
    uint16_t grow_w = scale_step * 2;
    uint16_t grow_h = scale_step;

    uint16_t btn_w = COLOR_OPTION_BASE_W + grow_w;
    uint16_t btn_h = COLOR_OPTION_BASE_H + grow_h;

    uint16_t btn_x = 0;
    uint16_t btn_y = 0;

    color_option_draw_bg();

    color_option_buf_fill_rect(btn_x + 3, btn_y + 3, btn_w, btn_h, COLOR_SHADOW_STRONG);

    color_option_buf_draw_panel_double(
        btn_x, btn_y, btn_w, btn_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, dark_color,
        main_color, light_color, dark_color
    );

    uint16_t tw = color_option_buf_text_width(label, COLOR_OPTION_TEXT_SCALE);
    uint16_t th = 7 * COLOR_OPTION_TEXT_SCALE;
    int tx = (COLOR_OPTION_BUF_W - tw) / 2;
    int ty = (COLOR_OPTION_BUF_H - th) / 2;

    color_option_buf_draw_text_transparent(tx + 2, ty + 2, label, 0x0000, COLOR_OPTION_TEXT_SCALE);
    color_option_buf_draw_text_transparent(tx + 1, ty + 1, label, 0x0004, COLOR_OPTION_TEXT_SCALE);
    color_option_buf_draw_text_transparent(tx, ty, label, 0xFFFF, COLOR_OPTION_TEXT_SCALE);
}

void button_widget_color_option_blit(uint16_t screen_x, uint16_t screen_y) {
    widget_buf_set_geometry(COLOR_OPTION_BUF_W, COLOR_OPTION_BUF_H);
    display_hw_set_window(screen_x, screen_y,
                          screen_x + COLOR_OPTION_BUF_W - 1,
                          screen_y + COLOR_OPTION_BUF_H - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < COLOR_OPTION_BUF_W * COLOR_OPTION_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_widget_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_widget_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void button_widget_color_hint_blit(uint16_t screen_x, uint16_t screen_y) {
    widget_buf_set_geometry(COLOR_HINT_BUF_W, COLOR_HINT_BUF_H);
    display_hw_set_window(screen_x, screen_y,
                          screen_x + COLOR_HINT_BUF_W - 1,
                          screen_y + COLOR_HINT_BUF_H - 1);
    display_hw_begin_pixels();

    for (int i = 0; i < COLOR_HINT_BUF_W * COLOR_HINT_BUF_H; i++) {
        uint8_t pixel[2];
        pixel[0] = (uint8_t)(g_widget_buf[i] >> 8);
        pixel[1] = (uint8_t)(g_widget_buf[i] & 0xFF);
        spi_write_blocking(display_hw_get_port(), pixel, 2);
    }

    display_hw_end_pixels();
}

void button_widget_color_title_draw_disappear_frame(uint8_t t) {
    color_title_draw_bg();

    if (t >= 13) return;

    int shrink_w = t * 18;
    int shrink_h = t * 8;

    int box_w = COLOR_TITLE_BASE_W - shrink_w;
    int box_h = COLOR_TITLE_BASE_H - shrink_h;

    if (box_w <= 0 || box_h <= 0) return;

    int box_x = (COLOR_TITLE_BUF_W - box_w) / 2;
    int box_y = (COLOR_TITLE_BUF_H - box_h) / 2;

    int shadow_offset = (t < 4) ? (4 - t) : 0;
    if (shadow_offset > 0) {
        color_title_buf_fill_rect(box_x + shadow_offset,
                                  box_y + shadow_offset,
                                  box_w, box_h,
                                  COLOR_SHADOW_STRONG);
    }

    color_title_buf_draw_panel_double(
        box_x, box_y, box_w, box_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, COLOR_BOX_BLUE_DARK,
        COLOR_TEXT_BOX, COLOR_BOX_BLUE_LIGHT, COLOR_BOX_BLUE_DARK
    );

    if (t < 3) {
        const char *text = "SELECT THE COLOR";
        uint16_t tw = color_title_buf_text_width(text, COLOR_TITLE_TEXT_SCALE);
        uint16_t th = 7 * COLOR_TITLE_TEXT_SCALE;

        int tx = (COLOR_TITLE_BUF_W - tw) / 2;
        int ty = (COLOR_TITLE_BUF_H - th) / 2;

        color_title_buf_draw_text_transparent(tx + 2, ty + 2, text, 0x0000, COLOR_TITLE_TEXT_SCALE);
        color_title_buf_draw_text_transparent(tx + 1, ty + 1, text, 0x0004, COLOR_TITLE_TEXT_SCALE);
        color_title_buf_draw_text_transparent(tx, ty, text, 0x000E, COLOR_TITLE_TEXT_SCALE);
        color_title_buf_draw_text_transparent(tx + 1, ty, text, 0x000E, COLOR_TITLE_TEXT_SCALE);
        color_title_buf_draw_text_transparent(tx, ty + 1, text, 0x000E, COLOR_TITLE_TEXT_SCALE);

        if (t < 2) {
            color_title_buf_draw_text_transparent(tx, ty - 1, text, 0x545F, COLOR_TITLE_TEXT_SCALE);
        }
    }
}

void button_widget_color_option_draw_disappear_frame(const char *label,
                                                     uint16_t main_color,
                                                     uint16_t light_color,
                                                     uint16_t dark_color,
                                                     uint8_t t) {
    color_option_draw_bg();

    if (t >= 13) return;

    int shrink_w = t * 18;
    int shrink_h = t * 8;

    int box_w = COLOR_OPTION_BASE_W - shrink_w;
    int box_h = COLOR_OPTION_BASE_H - shrink_h;

    if (box_w <= 0 || box_h <= 0) return;

    int box_x = (COLOR_OPTION_BUF_W - box_w) / 2;
    int box_y = (COLOR_OPTION_BUF_H - box_h) / 2;

    int shadow_offset = (t < 4) ? (4 - t) : 0;
    if (shadow_offset > 0) {
        color_option_buf_fill_rect(box_x + shadow_offset,
                                   box_y + shadow_offset,
                                   box_w, box_h,
                                   COLOR_SHADOW_STRONG);
    }

    color_option_buf_draw_panel_double(
        box_x, box_y, box_w, box_h,
        COLOR_WHITE, COLOR_WHITE_SOFT, dark_color,
        main_color, light_color, dark_color
    );

    if (t < 3) {
        uint16_t tw = color_option_buf_text_width(label, COLOR_OPTION_TEXT_SCALE);
        uint16_t th = 7 * COLOR_OPTION_TEXT_SCALE;

        int tx = (COLOR_OPTION_BUF_W - tw) / 2;
        int ty = (COLOR_OPTION_BUF_H - th) / 2;

        color_option_buf_draw_text_transparent(tx + 2, ty + 2, label, 0x0000, COLOR_OPTION_TEXT_SCALE);
        color_option_buf_draw_text_transparent(tx + 1, ty + 1, label, 0x0004, COLOR_OPTION_TEXT_SCALE);
        color_option_buf_draw_text_transparent(tx, ty, label, 0xFFFF, COLOR_OPTION_TEXT_SCALE);
    }
}

void button_widget_color_hint_draw_disappear_frame(void) {
    color_hint_draw_bg();
}