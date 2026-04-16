#ifndef DISPLAY_TYPES_H
#define DISPLAY_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DISPLAY_STATE_SEARCHING = 0,
    DISPLAY_STATE_MOVING,
    DISPLAY_STATE_STOPPED
} display_state_t;

typedef enum {
    DISPLAY_SCREEN_START = 0,
    DISPLAY_SCREEN_COLOR,
    DISPLAY_SCREEN_LOADING,
    DISPLAY_SCREEN_DRIVE,
    DISPLAY_SCREEN_MAIN
} display_screen_t;

typedef enum {
    DISPLAY_COLOR_NONE = -1,
    DISPLAY_COLOR_RED = 0,
    DISPLAY_COLOR_GREEN,
    DISPLAY_COLOR_BLUE
} display_color_t;

typedef enum {
    DISPLAY_LOADING_STAGE_SEARCH_COLOR = 0,
    DISPLAY_LOADING_STAGE_CALC_DISTANCE,
    DISPLAY_LOADING_STAGE_START_TEXT,
    DISPLAY_LOADING_STAGE_CLEAR_TEXT
} display_loading_stage_t;

typedef struct {
    display_screen_t screen;
    float distance_in;
    float battery_v;
    const char *color_name;
    display_state_t state;

    display_color_t selected_color;
    uint16_t selected_color_hex;   // RGB565 hex
    bool start_requested;
    bool color_locked;
    display_loading_stage_t loading_stage;
    bool color_found_pulse;
    bool distance_done_pulse;
    uint16_t selected_color_rgb565;
} display_data_t;

#endif