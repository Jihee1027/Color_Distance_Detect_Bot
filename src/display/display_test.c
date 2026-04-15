#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "display/display.h"
#include "display/display_button.h"

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    display_init();
    display_button_init();

    display_data_t data = {
        .screen = DISPLAY_SCREEN_START,
        .distance_in = 0.0f,
        .battery_v = 0.0f,
        .color_name = "",
        .state = DISPLAY_STATE_STOPPED,
        .selected_color = DISPLAY_COLOR_NONE,
        .selected_color_hex = 0x0000,
        .start_requested = false,
        .color_locked = false,
        .loading_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR,
        .color_found_pulse = false,
        .distance_done_pulse = false
    };

    bool last_start_requested = false;
    bool loading_timer_started = false;
    absolute_time_t loading_stage_start;
    display_loading_stage_t last_loading_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;

    while (1) {
        if (data.screen == DISPLAY_SCREEN_LOADING) {
            if (!loading_timer_started) {
                loading_stage_start = get_absolute_time();
                loading_timer_started = true;
                last_loading_stage = data.loading_stage;
            }

            if (data.loading_stage != last_loading_stage) {
                loading_stage_start = get_absolute_time();
                last_loading_stage = data.loading_stage;
            }

            if (data.loading_stage == DISPLAY_LOADING_STAGE_SEARCH_COLOR) {
                if (absolute_time_diff_us(loading_stage_start, get_absolute_time()) >= 2000000) {
                    data.color_found_pulse = true;
                }
            } else if (data.loading_stage == DISPLAY_LOADING_STAGE_CALC_DISTANCE) {
                if (absolute_time_diff_us(loading_stage_start, get_absolute_time()) >= 2000000) {
                    data.distance_done_pulse = true;
                }
            }
        } else {
            loading_timer_started = false;
        }

        display_render(&data);

        if (data.start_requested && !last_start_requested) {
            printf("SELECTED COLOR: %s\r\n", data.color_name);
            printf("SELECTED ENUM: %d\r\n", data.selected_color);
            printf("SELECTED HEX RGB565: 0x%04X\r\n", data.selected_color_hex);
            printf("START: %d\r\n", data.start_requested);
            printf("\r\n");
        }

        if (data.color_found_pulse) {
            printf("COLOR FOUND PULSE\r\n");
            data.color_found_pulse = false;
        }

        if (data.distance_done_pulse) {
            printf("DISTANCE DONE PULSE\r\n");
            data.distance_done_pulse = false;
        }

        last_start_requested = data.start_requested;
        sleep_ms(10);
    }

    return 0;
}