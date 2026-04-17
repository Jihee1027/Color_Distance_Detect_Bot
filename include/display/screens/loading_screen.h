#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "display/display_types.h"

void loading_screen_render(display_data_t *data);
uint16_t *loading_screen_get_buf(void);
void loading_screen_blit_buf(void);

#endif