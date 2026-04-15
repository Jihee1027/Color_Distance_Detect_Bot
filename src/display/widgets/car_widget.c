#include "display/widgets/car_widget.h"
#include "display/display.h"
#include "pico/stdlib.h"
#include "display/display_theme.h" 
#include "display/display_gfx.h"

void car_widget_draw(uint16_t x, uint16_t y, uint8_t scale, uint16_t color) {
    if (scale == 0) {
        return;
    }

    // body shade colors
    uint16_t shadow_color = COLOR_CAR_RED_DARK;
    uint16_t highlight_color = COLOR_CAR_RED_LIGHT;

    // if the selected body color is blue, use blue-toned shading
    if (color == COLOR_BLUE) {
        shadow_color = COLOR_BLUE_DARK;
        highlight_color = COLOR_BLUE_LIGHT;
    }

    // if the selected body color is green, use green-toned shading
    if (color == COLOR_GREEN) {
        shadow_color = COLOR_GREEN_DARK;
        highlight_color = COLOR_GREEN_LIGHT;
    }

    // ===== main body =====
    gfx_fill_rect(x + 3 * scale, y + 6 * scale, 18 * scale, 6 * scale, color);
    gfx_fill_rect(x + 6 * scale, y + 3 * scale, 11 * scale, 4 * scale, color);

    // ===== body shaping =====
    gfx_fill_rect(x + 4 * scale, y + 5 * scale, 1 * scale, 1 * scale, color);
    gfx_fill_rect(x + 5 * scale, y + 4 * scale, 1 * scale, 2 * scale, color);
    gfx_fill_rect(x + 17 * scale, y + 4 * scale, 1 * scale, 2 * scale, color);
    gfx_fill_rect(x + 18 * scale, y + 5 * scale, 1 * scale, 1 * scale, color);

    // ===== lower body shadow =====
    gfx_fill_rect(x + 4 * scale, y + 10 * scale, 16 * scale, 2 * scale, shadow_color);
    gfx_fill_rect(x + 18 * scale, y + 8 * scale, 2 * scale, 2 * scale, shadow_color);
    gfx_fill_rect(x + 5 * scale, y + 8 * scale, 2 * scale, 1 * scale, shadow_color);

    // ===== roof / upper highlight =====
    gfx_fill_rect(x + 7 * scale, y + 3 * scale, 8 * scale, 1 * scale, highlight_color);
    gfx_fill_rect(x + 4 * scale, y + 6 * scale, 12 * scale, 1 * scale, highlight_color);

    // ===== windows =====
    gfx_fill_rect(x + 7 * scale,  y + 4 * scale, 4 * scale, 3 * scale, COLOR_WINDOW_BLUE);
    gfx_fill_rect(x + 12 * scale, y + 4 * scale, 4 * scale, 3 * scale, COLOR_WINDOW_BLUE);

    // windshield / rear window shaping
    gfx_fill_rect(x + 6 * scale,  y + 5 * scale, 1 * scale, 2 * scale, COLOR_WINDOW_BLUE);
    gfx_fill_rect(x + 16 * scale, y + 5 * scale, 1 * scale, 2 * scale, COLOR_WINDOW_BLUE);

    // window highlight
    gfx_fill_rect(x + 8 * scale,  y + 4 * scale, 1 * scale, 3 * scale, COLOR_WINDOW_HIGHLIGHT);
    gfx_fill_rect(x + 13 * scale, y + 4 * scale, 1 * scale, 3 * scale, COLOR_WINDOW_HIGHLIGHT);

    // window pillars
    gfx_fill_rect(x + 11 * scale, y + 3 * scale, 1 * scale, 4 * scale, shadow_color);

    // ===== front hood / trunk small shaping =====
    gfx_fill_rect(x + 19 * scale, y + 7 * scale, 2 * scale, 3 * scale, color);
    gfx_fill_rect(x + 2 * scale,  y + 7 * scale, 2 * scale, 3 * scale, color);

    // ===== headlights and taillights =====
    gfx_fill_rect(x + 20 * scale, y + 7 * scale, 1 * scale, 2 * scale, COLOR_HEADLIGHT);
    gfx_fill_rect(x + 2 * scale,  y + 7 * scale, 1 * scale, 2 * scale, COLOR_TAILLIGHT);

    // tiny light accents
    gfx_fill_rect(x + 20 * scale, y + 8 * scale, 1 * scale, 1 * scale, COLOR_WHITE);
    gfx_fill_rect(x + 2 * scale,  y + 8 * scale, 1 * scale, 1 * scale, COLOR_YELLOW);

    // ===== door handle =====
    gfx_fill_rect(x + 13 * scale, y + 6 * scale, 2 * scale, 1 * scale, COLOR_HANDLE);

    // ===== wheel arches shadow =====
    gfx_fill_rect(x + 4 * scale,  y + 9 * scale, 5 * scale, 1 * scale, shadow_color);
    gfx_fill_rect(x + 15 * scale, y + 9 * scale, 5 * scale, 1 * scale, shadow_color);

    // ===== wheels =====
    gfx_fill_circle(x + 7 * scale,  y + 12 * scale, 3 * scale, COLOR_WHEEL_OUTER);
    gfx_fill_circle(x + 17 * scale, y + 12 * scale, 3 * scale, COLOR_WHEEL_OUTER);

    // wheel inner rim
    gfx_fill_circle(x + 7 * scale,  y + 12 * scale, 2 * scale, COLOR_WHEEL_INNER);
    gfx_fill_circle(x + 17 * scale, y + 12 * scale, 2 * scale, COLOR_WHEEL_INNER);

    // wheel center highlight
    gfx_fill_circle(x + 7 * scale,  y + 12 * scale, 1 * scale, COLOR_WHEEL_HIGHLIGHT);
    gfx_fill_circle(x + 17 * scale, y + 12 * scale, 1 * scale, COLOR_WHEEL_HIGHLIGHT);

    // ===== subtle ground shadow under car =====
    gfx_fill_rect(x + 4 * scale, y + 15 * scale, 16 * scale, 1 * scale, COLOR_GROUND_SHADOW);
}