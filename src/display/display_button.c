#include "display/display_button.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdbool.h>

#define DISPLAY_BUTTON_PIN 21
#define DISPLAY_NEXT_BUTTON_PIN 26

void display_button_init(void) {
    gpio_init(DISPLAY_BUTTON_PIN);
    gpio_set_dir(DISPLAY_BUTTON_PIN, GPIO_IN);
    gpio_pull_down(DISPLAY_BUTTON_PIN);

    gpio_init(DISPLAY_NEXT_BUTTON_PIN);
    gpio_set_dir(DISPLAY_NEXT_BUTTON_PIN, GPIO_IN);
    gpio_pull_down(DISPLAY_NEXT_BUTTON_PIN);
}

bool display_button_is_pressed(void) {
    return gpio_get(DISPLAY_BUTTON_PIN) == 1;
}

bool display_button_was_pressed(void) {
    static bool prev = false;

    bool now = display_button_is_pressed();
    bool edge = (now && !prev);
    prev = now;
    return edge;
}

bool display_next_button_is_pressed(void) {
    return gpio_get(DISPLAY_NEXT_BUTTON_PIN) == 1;
}

bool display_next_button_was_pressed(void) {
    static bool prev = false;

    bool now = display_next_button_is_pressed();
    bool edge = (now && !prev);

    prev = now;
    return edge;
}