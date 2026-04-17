#include "display/display_text.h"
#include "display/display_gfx.h"
#include <string.h>

static const uint8_t g_font5x7[128][5] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['.'] = {0x00, 0x60, 0x60, 0x00, 0x00},

    [':'] = {0x00, 0x36, 0x36, 0x00, 0x00},

    ['0'] = {0x3E, 0x45, 0x49, 0x51, 0x3E},
    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00},
    ['2'] = {0x42, 0x61, 0x51, 0x49, 0x46},
    ['3'] = {0x21, 0x41, 0x45, 0x4B, 0x31},
    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10},
    ['5'] = {0x27, 0x45, 0x45, 0x45, 0x39},
    ['6'] = {0x3E, 0x49, 0x49, 0x49, 0x30},
    ['7'] = {0x01, 0x71, 0x09, 0x05, 0x03},
    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36},
    ['9'] = {0x06, 0x49, 0x49, 0x29, 0x1E},

    ['A'] = {0x7E, 0x09, 0x09, 0x09, 0x7E},
    ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36},
    ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['G'] = {0x3E, 0x41, 0x49, 0x49, 0x3A},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
    ['I'] = {0x00, 0x41, 0x7F, 0x41, 0x00},
    ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40},
    ['N'] = {0x7F, 0x02, 0x04, 0x08, 0x7F},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
    ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46},
    ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
    ['U'] = {0x3F, 0x40, 0x40, 0x40, 0x3F},
    ['V'] = {0x1F, 0x20, 0x40, 0x20, 0x1F},
    ['V'] = {0x1F, 0x20, 0x40, 0x20, 0x1F},
    ['X'] = {0x63, 0x14, 0x08, 0x14, 0x63},
    ['Y'] = {0x03, 0x04, 0x78, 0x04, 0x03},
};

void text_draw_char_scaled(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale) {
    if ((unsigned char)c >= 128) {
        return;
    }

    for (int col = 0; col < 5; col++) {
        uint8_t bits = g_font5x7[(int)c][col];

        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                gfx_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            } else {
                gfx_fill_rect(x + col * scale, y + row * scale, scale, scale, bg);
            }
        }
    }

    gfx_fill_rect(x + 5 * scale, y, scale, 7 * scale, bg);
}

void text_draw_text_scaled(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale) {
    while (*text) {
        text_draw_char_scaled(x, y, *text, color, bg, scale);
        x += 6 * scale;
        text++;
    }
}

void text_draw_char_scaled_transparent(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t scale) {
    if ((unsigned char)c >= 128) {
        return;
    }

    for (int col = 0; col < 5; col++) {
        uint8_t bits = g_font5x7[(int)c][col];

        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                gfx_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void text_draw_text_scaled_transparent(uint16_t x, uint16_t y, const char *text, uint16_t color, uint8_t scale) {
    while (*text) {
        text_draw_char_scaled_transparent(x, y, *text, color, scale);
        x += 6 * scale;
        text++;
    }
}

uint16_t text_get_width(const char *text, uint8_t scale) {
    return (uint16_t)(strlen(text) * 6 * scale);
}

void text_draw_char_scaled_transparent_to_buf(
    int x, int y, char c, uint16_t color, uint8_t scale,
    void (*fill_rect)(int x, int y, int w, int h, uint16_t color)) {

    if ((unsigned char)c >= 128) {
        return;
    }

    for (int col = 0; col < 5; col++) {
        uint8_t bits = g_font5x7[(int)c][col];

        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void text_draw_text_scaled_transparent_to_buf(
    int x, int y, const char *text, uint16_t color, uint8_t scale,
    void (*fill_rect)(int x, int y, int w, int h, uint16_t color)) {

    while (*text) {
        text_draw_char_scaled_transparent_to_buf(x, y, *text, color, scale, fill_rect);
        x += 6 * scale;
        text++;
    }
}