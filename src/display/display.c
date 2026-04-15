#include "display/display.h"
#include "display/display_hw.h"
#include "display/display_gfx.h"
#include "display/display_manager.h"
#include "display/display_theme.h"

void display_init(void) {
    display_hw_init();
    gfx_fill_screen(COLOR_SKY_MID2);
}

void display_clear(void) {
    gfx_fill_screen(COLOR_SKY_MID2);
}

void display_render(display_data_t *data) {
    display_manager_render(data);
}