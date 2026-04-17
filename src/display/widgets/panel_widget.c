#include "display/widgets/panel_widget.h"
#include "display/display_gfx.h"

void panel_widget_draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       uint16_t face, uint16_t light, uint16_t dark) {
    gfx_fill_rect(x, y, w, h, face);
    gfx_fill_rect(x, y, w, 2, light);
    gfx_fill_rect(x, y, 2, h, light);
    gfx_fill_rect(x, y + h - 2, w, 2, dark);
    gfx_fill_rect(x + w - 2, y, 2, h, dark);
}

void panel_widget_draw_double(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                              uint16_t outer_face, uint16_t outer_light, uint16_t outer_dark,
                              uint16_t inner_face, uint16_t inner_light, uint16_t inner_dark) {
    panel_widget_draw(x, y, w, h, outer_face, outer_light, outer_dark);
    panel_widget_draw(x + 4, y + 4, w - 8, h - 8, inner_face, inner_light, inner_dark);
}