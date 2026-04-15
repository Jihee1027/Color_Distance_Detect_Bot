#include "display/screens/color_screen.h"
#include "display/display_theme.h"
#include "display/widgets/button_widget.h"
#include "display/display_button.h"
#include "display/display_text.h"
#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *label;
    uint16_t face;
    uint16_t light;
    uint16_t dark;
} color_option_t;

static void color_screen_draw_hints(void) {
    const uint8_t scale = 1;

    const char *left = "NEXT: PRESS 26";
    const char *right = "SELECT: PRESS 21";

    uint16_t right_w = text_get_width(right, scale);

    text_draw_text_scaled_transparent(11, 216, left, COLOR_BLACK, scale);
    text_draw_text_scaled_transparent(TFT_WIDTH - right_w - 9, 216, right, COLOR_BLACK, scale);

    text_draw_text_scaled_transparent(10, 215, left, COLOR_WHITE, scale);
    text_draw_text_scaled_transparent(TFT_WIDTH - right_w - 10, 215, right, COLOR_WHITE, scale);
}

void color_screen_render(display_data_t *data) {
    static bool initialized = false;
    static uint8_t title_idx = 0;
    static uint8_t option_idx = 0;
    static bool title_done = false;
    static bool option_done = false;
    static bool hints_done = false;
    static uint8_t current_color = 0;

    static bool disappearing = false;
    static bool hidden = false;
    static uint8_t disappear_step = 0;
    static absolute_time_t disappear_tick;

    static const uint8_t title_intro_seq[] = {6, 4, 3, 2, 1, 0};
    static const uint8_t option_intro_seq[] = {10, 8, 6, 4, 2, 0};

    static const color_option_t options[3] = {
        {"RED",   0xF800, 0xFB2C, 0x9800},
        {"GREEN", 0x07E0, 0x87F0, 0x03A0},
        {"BLUE",  0x051F, 0x86DF, 0x0210}
    };

    if (!initialized) {
        initialized = true;
        title_idx = 0;
        option_idx = 0;
        title_done = false;
        option_done = false;
        hints_done = false;
        current_color = 0;
        disappearing = false;
        hidden = false;
        disappear_step = 0;
    }

    if (hidden) {
        return;
    }

    if (!title_done) {
        if (title_idx < (sizeof(title_intro_seq) / sizeof(title_intro_seq[0]))) {
            button_widget_color_title_draw_frame(title_intro_seq[title_idx]);
            button_widget_color_title_blit(COLOR_TITLE_SCREEN_X, COLOR_TITLE_SCREEN_Y);
            title_idx++;
            return;
        } else {
            title_done = true;
        }
    }

    if (!option_done) {
        if (option_idx < (sizeof(option_intro_seq) / sizeof(option_intro_seq[0]))) {
            button_widget_color_option_draw_frame(
                options[current_color].label,
                options[current_color].face,
                options[current_color].light,
                options[current_color].dark,
                option_intro_seq[option_idx]
            );
            button_widget_color_option_blit(COLOR_OPTION_SCREEN_X, COLOR_OPTION_SCREEN_Y);
            option_idx++;
            return;
        } else {
            option_done = true;
        }
    }

    if (!hints_done) {
        color_screen_draw_hints();
        hints_done = true;
    }

    if (disappearing) {
        if (absolute_time_diff_us(disappear_tick, get_absolute_time()) >= 35000) {
            button_widget_color_title_draw_disappear_frame(disappear_step);
            button_widget_color_title_blit(COLOR_TITLE_SCREEN_X, COLOR_TITLE_SCREEN_Y);

            button_widget_color_option_draw_disappear_frame(
                options[current_color].label,
                options[current_color].face,
                options[current_color].light,
                options[current_color].dark,
                disappear_step
            );
            button_widget_color_option_blit(COLOR_OPTION_SCREEN_X, COLOR_OPTION_SCREEN_Y);

            button_widget_color_hint_draw_disappear_frame();
            button_widget_color_hint_blit(COLOR_HINT_SCREEN_X, COLOR_HINT_SCREEN_Y);

            disappear_step++;
            disappear_tick = get_absolute_time();

            if (disappear_step > 13) {
                disappearing = false;
                hidden = true;
                data->start_requested = true;
                data->color_locked = true;
                data->loading_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;
                data->screen = DISPLAY_SCREEN_LOADING;

                return;
            }
        }

        return;
    }

    if (display_next_button_was_pressed()) {
        current_color = (current_color + 1) % 3;
        option_idx = 0;
        option_done = false;
        return;
    }

    if (display_button_was_pressed()) {
        if (current_color == 0) {
            data->selected_color = DISPLAY_COLOR_RED;
        } else if (current_color == 1) {
            data->selected_color = DISPLAY_COLOR_GREEN;
        } else {
            data->selected_color = DISPLAY_COLOR_BLUE;
        }

        data->color_name = options[current_color].label;
        data->selected_color_hex = options[current_color].face;

        disappearing = true;
        disappear_step = 0;
        disappear_tick = get_absolute_time();
        return;
    }
}