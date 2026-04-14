#include "display_spi.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

// Pin configuration
#define DISP_SPI_PORT   spi1
#define DISP_PIN_SCK    14
#define DISP_PIN_TX     15
#define DISP_PIN_CSN    13
#define DISP_PIN_DC     12
#define DISP_PIN_RST    11

// Character display assumptions
#define DISPLAY_COLS    16

static spi_inst_t *display_port = DISP_SPI_PORT;

// Internal helpers
static void cs_select(void);
static void cs_deselect(void);
static void dc_command(void);
static void dc_data(void);
static void display_reset(void);
static void display_set_cursor(uint8_t row, uint8_t col);
static void display_write_char(char c);
static void display_write_string(const char *str);
static void display_line1(const char *str);
static void display_line2(const char *str);
static void format_line(char *dst, size_t dst_size, const char *src);
static const char *state_to_string(display_state_t state);

// Low level pin helpers
static void cs_select(void) {
    gpio_put(DISP_PIN_CSN, 0);
}
static void cs_deselect(void) {
    gpio_put(DISP_PIN_CSN, 1);
}
static void dc_command(void) {
    gpio_put(DISP_PIN_DC, 0);
}
static void dc_data(void) {
    gpio_put(DISP_PIN_DC, 1);
}
static void display_reset(void) {
    gpio_put(DISP_PIN_RST, 0);
    sleep_ms(20);
    gpio_put(DISP_PIN_RST, 1);
    sleep_ms(20);
}

// SPI command/data write
void send_spi_cmd(spi_inst_t *spi, uint8_t value) {
    cs_select();
    dc_command();
    spi_write_blocking(spi, &value, 1);
    cs_deselect();
    sleep_us(50);
}

void send_spi_data(spi_inst_t *spi, uint8_t value) {
    cs_select();
    dc_data();
    spi_write_blocking(spi, &value, 1);
    cs_deselect();
    sleep_us(50);
}

// Public functions
void display_init(void) {
    // SPI setup
    spi_init(display_port, 1000000); // 1 MHz safe starting point
    spi_set_format(display_port, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(DISP_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DISP_PIN_TX, GPIO_FUNC_SPI);

    gpio_init(DISP_PIN_CSN);
    gpio_set_dir(DISP_PIN_CSN, GPIO_OUT);
    gpio_put(DISP_PIN_CSN, 1);

    gpio_init(DISP_PIN_DC);
    gpio_set_dir(DISP_PIN_DC, GPIO_OUT);
    gpio_put(DISP_PIN_DC, 1);

    gpio_init(DISP_PIN_RST);
    gpio_set_dir(DISP_PIN_RST, GPIO_OUT);
    gpio_put(DISP_PIN_RST, 1);

    display_reset();

    send_spi_cmd(display_port, 0x38); // function set
    send_spi_cmd(display_port, 0x08); // display off
    send_spi_cmd(display_port, 0x01); // clear display
    sleep_ms(2);
    send_spi_cmd(display_port, 0x06); // entry mode set
    send_spi_cmd(display_port, 0x0C); // display on, cursor off
    sleep_ms(2);

    display_clear();
    display_line1("Robot Display");
    display_line2("Initializing...");
    sleep_ms(500);
    display_clear();
}

void display_clear(void) {
    send_spi_cmd(display_port, 0x01);
    sleep_ms(2);
}

void display_update(const display_data_t *data) {
    char line1[DISPLAY_COLS + 1];
    char line2[DISPLAY_COLS + 1];

    if (data == NULL) {
        return;
    }

    // line1 example: RED 12.3IN
    // line2 example: B3.91 SEARCH
    snprintf(line1, sizeof(line1), "%s %.1fIN", data->color_name ? data->color_name : "NONE", data->distance_in);
    snprintf(line2, sizeof(line2), "B%.2f %s", data->battery_v, state_to_string(data->state));
    display_line1(line1);
    display_line2(line2);
}

// Internal display helpers
static void display_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr;
    if (row == 0) {
        addr = 0x80 + col;
    } else {
        addr = 0xC0 + col;
    }
    send_spi_cmd(display_port, addr);
    sleep_us(50);
}

static void display_write_char(char c) {
    send_spi_data(display_port, (uint8_t)c);
}

static void display_write_string(const char *str) {
    if (str == NULL) {
        return;
    }
    while (*str != '\0') {
        display_write_char(*str);
        str++;
    }
}

static void format_line(char *dst, size_t dst_size, const char *src) {
    size_t i = 0;
    if (dst == NULL || src == NULL || dst_size < (DISPLAY_COLS + 1)) {
        return;
    }
    for (i = 0; i < DISPLAY_COLS && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    for (; i < DISPLAY_COLS; i++) {
        dst[i] = ' ';
    }
    dst[DISPLAY_COLS] = '\0';
}

static void display_line1(const char *str) {
    char line[DISPLAY_COLS + 1];
    format_line(line, sizeof(line), str);
    display_set_cursor(0, 0);
    display_write_string(line);
}

static void display_line2(const char *str) {
    char line[DISPLAY_COLS + 1];
    format_line(line, sizeof(line), str);
    display_set_cursor(1, 0);
    display_write_string(line);
}

static const char *state_to_string(display_state_t state) {
    switch (state) {
        case DISPLAY_STATE_SEARCHING: return "SEARCH";
        case DISPLAY_STATE_MOVING:    return "MOVING";
        case DISPLAY_STATE_STOPPED:   return "STOPPED";
        default:                      return "UNKNOWN";
    }
}