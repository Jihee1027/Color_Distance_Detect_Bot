#include "display/screens/start_screen.h"
#include "display/display_theme.h"
#include "display/display_gfx.h"
#include "display/display_text.h"
#include "display/widgets/panel_widget.h"
#include "display/widgets/button_widget.h"
#include "display/display_button.h"
#include "pico/stdlib.h"

static void start_screen_draw_cloud(int x, int y, int s) {
    int r1 = 8 * s / 10;
    int r2 = 11 * s / 10;
    int r3 = 8 * s / 10;

    gfx_fill_circle(x + 12 * s / 10 + 2, y + 14 * s / 10 + 2, r1, COLOR_CLOUD_DARK);
    gfx_fill_circle(x + 28 * s / 10 + 2, y + 8 * s / 10 + 2,  r2, COLOR_CLOUD_DARK);
    gfx_fill_circle(x + 46 * s / 10 + 2, y + 14 * s / 10 + 2, r3, COLOR_CLOUD_DARK);
    gfx_fill_rect  (x + 12 * s / 10 + 2, y + 14 * s / 10 + 2, 34 * s / 10, 12 * s / 10, COLOR_CLOUD_DARK);

    gfx_fill_circle(x + 11 * s / 10 + 1, y + 13 * s / 10 + 1, r1, COLOR_CLOUD_SHADE);
    gfx_fill_circle(x + 27 * s / 10 + 1, y + 7 * s / 10 + 1, r2, COLOR_CLOUD_SHADE);
    gfx_fill_circle(x + 45 * s / 10 + 1, y + 13 * s / 10 + 1, r3, COLOR_CLOUD_SHADE);
    gfx_fill_rect  (x + 11 * s / 10 + 1, y + 13 * s / 10 + 1, 34 * s / 10, 12 * s / 10, COLOR_CLOUD_SHADE);

    gfx_fill_circle(x + 10 * s / 10, y + 12 * s / 10, r1, COLOR_CLOUD);
    gfx_fill_circle(x + 26 * s / 10, y + 6 * s / 10, r2, COLOR_CLOUD);
    gfx_fill_circle(x + 44 * s / 10, y + 12 * s / 10, r3, COLOR_CLOUD);
    gfx_fill_rect  (x + 10 * s / 10, y + 12 * s / 10, 34 * s / 10, 12 * s / 10, COLOR_CLOUD);

    gfx_fill_rect(x + 16 * s / 10, y + 8 * s / 10, 10 * s / 10, 2, COLOR_WHITE);
    gfx_fill_rect(x + 28 * s / 10, y + 11 * s / 10, 8 * s / 10, 2, COLOR_WHITE);
}

static void start_screen_draw_background(void) {
    for (int y = 0; y < 160; y++) {
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

        gfx_fill_rect(0, y, TFT_WIDTH, 1, color);
    }

    gfx_fill_circle(28, 178, 28, COLOR_HILL_DARK);
    gfx_fill_circle(78, 172, 38, COLOR_HILL);
    gfx_fill_circle(132, 180, 26, COLOR_HILL_DARK);
    gfx_fill_circle(188, 170, 40, COLOR_HILL);
    gfx_fill_circle(246, 182, 28, COLOR_HILL_DARK);
    gfx_fill_circle(302, 174, 34, COLOR_HILL);

    gfx_fill_rect(0, 162, TFT_WIDTH, 28, COLOR_GRASS);
    gfx_fill_rect(0, 190, TFT_WIDTH, 50, COLOR_DIRT);

    gfx_fill_rect(0, 162, TFT_WIDTH, 3, COLOR_GRASS_DARK);
    gfx_fill_rect(0, 190, TFT_WIDTH, 4, COLOR_DIRT_DARK);

    for (int x = 0; x < TFT_WIDTH; x += 24) {
        gfx_fill_rect(x, 194, 8, 46, COLOR_DIRT_DARK);
        gfx_fill_rect(x + 8, 194, 6, 46, COLOR_DIRT_LIGHT);
    }

    start_screen_draw_cloud(8, 20, 9);
    start_screen_draw_cloud(58, 42, 6);
    start_screen_draw_cloud(112, 16, 7);
    start_screen_draw_cloud(165, 36, 7);
    start_screen_draw_cloud(220, 18, 6);
    start_screen_draw_cloud(260, 48, 7);
    start_screen_draw_cloud(86, 92, 5);
    start_screen_draw_cloud(150, 108, 5);
    start_screen_draw_cloud(228, 96, 5);
}

void start_screen_render_static(void) {
    start_screen_draw_background();

    button_widget_start_box_draw_frame(0);
    button_widget_start_box_blit(START_SCREEN_X, START_SCREEN_Y);

    button_widget_start_draw_frame(0, 0);
    button_widget_start_blit(BTN_SCREEN_X, BTN_SCREEN_Y);
}

void start_screen_render(display_data_t *data) {
    (void)data;

    static bool initialized = false;
    static bool prev_pressed = false;
    static bool hidden = false;

    static bool start_pressed_visual = false;
    static absolute_time_t start_press_time;
    static absolute_time_t start_release_time;
    static bool start_timer_started = false;
    static bool start_release_timer_started = false;

    static bool disappearing = false;
    static uint8_t disappear_step = 0;
    static absolute_time_t disappear_tick;

    static uint8_t idx = 0;

    static const uint8_t anim_seq[] = {
        0, 0, 0, 1, 1, 1, 2, 2, 3, 3,
        4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
        14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
        4, 3, 3, 2, 2, 1, 1, 1, 0, 0
    };

    bool pressed = display_button_is_pressed();

    if (!initialized) {
        start_screen_render_static();
        prev_pressed = pressed;
        initialized = true;
        return;
    }

    if (hidden) {
        prev_pressed = pressed;
        return;
    }

    if (disappearing) {
        if (absolute_time_diff_us(disappear_tick, get_absolute_time()) >= 25000) {
            button_widget_start_box_draw_disappear_frame(disappear_step);
            button_widget_start_box_blit(START_SCREEN_X, START_SCREEN_Y);

            button_widget_start_draw_disappear_frame(disappear_step);
            button_widget_start_blit(BTN_SCREEN_X, BTN_SCREEN_Y);

            disappear_step++;
            disappear_tick = get_absolute_time();

            if (disappear_step > 13) {
                disappearing = false;
                hidden = true;
                data->screen = DISPLAY_SCREEN_COLOR;
                return;
            }
        }

        prev_pressed = pressed;
        return;
    }

    button_widget_start_draw_frame(anim_seq[idx], pressed ? 1 : 0);
    button_widget_start_blit(BTN_SCREEN_X, BTN_SCREEN_Y);
    idx = (idx + 1) % (sizeof(anim_seq) / sizeof(anim_seq[0]));

    if (pressed && !prev_pressed) {
        start_press_time = get_absolute_time();
        start_timer_started = true;
        start_release_timer_started = false;
        start_pressed_visual = false;
    }

    if (start_timer_started && !start_pressed_visual) {
        if (absolute_time_diff_us(start_press_time, get_absolute_time()) >= 200000) {
            button_widget_start_box_draw_frame(1);
            button_widget_start_box_blit(START_SCREEN_X, START_SCREEN_Y);

            start_pressed_visual = true;
            start_release_time = get_absolute_time();
            start_release_timer_started = true;
            start_timer_started = false;
        }
    }

    if (start_release_timer_started && start_pressed_visual) {
        if (absolute_time_diff_us(start_release_time, get_absolute_time()) >= 250000) {
            button_widget_start_box_draw_frame(0);
            button_widget_start_box_blit(START_SCREEN_X, START_SCREEN_Y);

            start_pressed_visual = false;
            start_release_timer_started = false;

            disappearing = true;
            disappear_step = 0;
            disappear_tick = get_absolute_time();
        }
    }

    prev_pressed = pressed;
}