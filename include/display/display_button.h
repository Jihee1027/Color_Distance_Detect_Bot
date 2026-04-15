#ifndef DISPLAY_BUTTON_H
#define DISPLAY_BUTTON_H

#include <stdbool.h>

void display_button_init(void);

bool display_button_is_pressed(void);
bool display_button_was_pressed(void);

bool display_next_button_is_pressed(void);
bool display_next_button_was_pressed(void);

#endif