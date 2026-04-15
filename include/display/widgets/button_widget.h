#ifndef BUTTON_WIDGET_H
#define BUTTON_WIDGET_H

#include <stdint.h>
#include <stdbool.h>

void button_widget_start_draw_frame(uint8_t scale_step, uint8_t press_depth);
void button_widget_start_blit(uint16_t screen_x, uint16_t screen_y);
void button_widget_start_box_draw_frame(uint8_t press_depth);
void button_widget_start_box_blit(uint16_t screen_x, uint16_t screen_y);
void button_widget_start_draw_disappear_frame(uint8_t t);
void button_widget_start_box_draw_disappear_frame(uint8_t t);

void button_widget_color_title_draw_frame(uint8_t scale_step);
void button_widget_color_title_blit(uint16_t screen_x, uint16_t screen_y);
void button_widget_color_option_draw_frame(const char *label,
                                           uint16_t main_color,
                                           uint16_t light_color,
                                           uint16_t dark_color,
                                           uint8_t scale_step);
void button_widget_color_option_blit(uint16_t screen_x, uint16_t screen_y);

void button_widget_color_title_draw_disappear_frame(uint8_t t);
void button_widget_color_option_draw_disappear_frame(const char *label,
                                                     uint16_t main_color,
                                                     uint16_t light_color,
                                                     uint16_t dark_color,
                                                     uint8_t t);

void button_widget_color_hint_draw_disappear_frame(void);
void button_widget_color_hint_blit(uint16_t screen_x, uint16_t screen_y);
void draw_start_box_scene_overlay_generic(int origin_x, int origin_y,
                                          void (*fill_rect)(int x, int y, int w, int h, uint16_t color));
void draw_loading_scene_overlay_generic(int origin_x, int origin_y,
                                        void (*fill_rect)(int x, int y, int w, int h, uint16_t color));

#endif