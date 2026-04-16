#ifndef CAR_WIDGET_H
#define CAR_WIDGET_H

#include <stdint.h>

typedef void (*car_fill_rect_fn)(int x, int y, int w, int h, uint16_t color);
typedef void (*car_fill_circle_fn)(int x, int y, int r, uint16_t color);

void car_widget_draw(uint16_t x, uint16_t y, uint8_t scale, uint16_t color);

void car_widget_draw_to_buffer(uint16_t x, uint16_t y, uint8_t scale, uint16_t color,
                               car_fill_rect_fn fill_rect,
                               car_fill_circle_fn fill_circle);

#endif