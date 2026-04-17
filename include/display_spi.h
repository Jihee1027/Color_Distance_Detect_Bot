#ifndef DISPLAY_SPI_H
#define DISPLAY_SPI_H

#include <stdint.h>
#include "hardware/spi.h"

typedef enum {
    DISPLAY_STATE_SEARCHING = 0,
    DISPLAY_STATE_MOVING,
    DISPLAY_STATE_STOPPED
} display_state_t;

typedef struct {
    float distance_in;
    float battery_v;
    const char *color_name;
    display_state_t state;
} display_data_t;

void display_init(void);
void display_clear(void);
void display_update(const display_data_t *data);
void display_show_start_screen(void);
void send_spi_cmd(spi_inst_t *spi, uint8_t value);
void send_spi_data(spi_inst_t *spi, uint8_t value);


#endif