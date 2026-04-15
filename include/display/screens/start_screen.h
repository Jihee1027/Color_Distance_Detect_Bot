#ifndef START_SCREEN_H
#define START_SCREEN_H

#include "display/display_types.h"
#include <stdbool.h>

void start_screen_render(display_data_t *data);
void start_screen_render_static(void);
void start_screen_render_animation(bool pressed);

#endif