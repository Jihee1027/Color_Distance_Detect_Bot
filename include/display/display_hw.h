#ifndef DISPLAY_HW_H
#define DISPLAY_HW_H

#include <stdint.h>
#include "hardware/spi.h"

void display_hw_init(void);

void display_hw_send_cmd(spi_inst_t *spi, uint8_t value);
void display_hw_send_data(spi_inst_t *spi, uint8_t value);

void display_hw_write_cmd(uint8_t cmd);
void display_hw_write_data(uint8_t data);
void display_hw_write_data_buf(const uint8_t *buf, int len);
void display_hw_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void display_hw_begin_pixels(void);
void display_hw_end_pixels(void);
spi_inst_t *display_hw_get_port(void);

#endif