#ifndef CAR_WIDGET_H
#define CAR_WIDGET_H

#include <stdint.h>

typedef void (*car_fill_rect_fn)(int x, int y, int w, int h, uint16_t color);
typedef void (*car_fill_circle_fn)(int x, int y, int r, uint16_t color);

void car_widget_draw(uint16_t x, uint16_t y, uint8_t scale, uint16_t color);

void car_widget_draw_to_buffer(uint16_t x, uint16_t y, uint8_t scale, uint16_t color,
                               car_fill_rect_fn fill_rect,
                               car_fill_circle_fn fill_circle);
typedef void (*buf_fill_rect_fn)(int x, int y, int w, int h, uint16_t color);
typedef void (*buf_fill_circle_fn)(int cx, int cy, int r, uint16_t color);

void car_widget_draw_moving_to_buffer(uint16_t x, uint16_t y, uint8_t scale,
                                      uint16_t color,
                                      buf_fill_rect_fn fill_rect,
                                      buf_fill_circle_fn fill_circle,
                                      uint8_t wheel_phase,
                                      uint8_t smoke_phase);

void car_widget_draw_stopped_to_buffer(uint16_t x, uint16_t y, uint8_t scale,
                                       uint16_t color,
                                       buf_fill_rect_fn fill_rect,
                                       buf_fill_circle_fn fill_circle);

#endif