#ifndef DISPLAY_H
#define DISPLAY_H

#include "display/display_types.h"

void display_init(void);
void display_clear(void);
void display_render(display_data_t *data);

#endif