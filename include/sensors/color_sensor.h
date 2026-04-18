#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <stdlib.h>

void init_i2c();
uint8_t read_i2c_register(uint8_t address);
void write_i2c_register(uint8_t address, uint8_t value);
void init_color_sensor();
void calibrate_colors();
void get_rgb(int* red, int* green, int* blue);
int get_color();
int color_check(int target_color);

#endif