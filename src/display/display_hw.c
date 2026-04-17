#include "display/display_hw.h"
#include "display/display_theme.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

static spi_inst_t *g_display_port = DISP_SPI_PORT;

static void display_hw_cs_select(void) {
    gpio_put(DISP_PIN_CSN, 0);
}

static void display_hw_cs_deselect(void) {
    gpio_put(DISP_PIN_CSN, 1);
}

static void display_hw_dc_command(void) {
    gpio_put(DISP_PIN_DC, 0);
}

static void display_hw_dc_data(void) {
    gpio_put(DISP_PIN_DC, 1);
}

static void display_hw_reset(void) {
    gpio_put(DISP_PIN_RST, 0);
    sleep_ms(20);
    gpio_put(DISP_PIN_RST, 1);
    sleep_ms(120);
}

void display_hw_send_cmd(spi_inst_t *spi, uint8_t value) {
    gpio_put(DISP_PIN_CSN, 0);
    gpio_put(DISP_PIN_DC, 0);
    spi_write_blocking(spi, &value, 1);
    gpio_put(DISP_PIN_CSN, 1);
}

void display_hw_send_data(spi_inst_t *spi, uint8_t value) {
    gpio_put(DISP_PIN_CSN, 0);
    gpio_put(DISP_PIN_DC, 1);
    spi_write_blocking(spi, &value, 1);
    gpio_put(DISP_PIN_CSN, 1);
}

void display_hw_write_cmd(uint8_t cmd) {
    display_hw_cs_select();
    display_hw_dc_command();
    spi_write_blocking(g_display_port, &cmd, 1);
    display_hw_cs_deselect();
}

void display_hw_write_data(uint8_t data) {
    display_hw_cs_select();
    display_hw_dc_data();
    spi_write_blocking(g_display_port, &data, 1);
    display_hw_cs_deselect();
}

void display_hw_write_data_buf(const uint8_t *buf, int len) {
    display_hw_cs_select();
    display_hw_dc_data();
    spi_write_blocking(g_display_port, buf, len);
    display_hw_cs_deselect();
}

void display_hw_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t buf[4];

    display_hw_write_cmd(0x2A);
    buf[0] = x0 >> 8;
    buf[1] = x0 & 0xFF;
    buf[2] = x1 >> 8;
    buf[3] = x1 & 0xFF;
    display_hw_write_data_buf(buf, 4);

    display_hw_write_cmd(0x2B);
    buf[0] = y0 >> 8;
    buf[1] = y0 & 0xFF;
    buf[2] = y1 >> 8;
    buf[3] = y1 & 0xFF;
    display_hw_write_data_buf(buf, 4);

    display_hw_write_cmd(0x2C);
}

void display_hw_begin_pixels(void) {
    display_hw_cs_select();
    display_hw_dc_data();
}

void display_hw_end_pixels(void) {
    display_hw_cs_deselect();
}

spi_inst_t *display_hw_get_port(void) {
    return g_display_port;
}

void display_hw_init(void) {
    spi_init(g_display_port, 40000000);
    spi_set_format(g_display_port, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(DISP_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DISP_PIN_TX, GPIO_FUNC_SPI);

    gpio_init(DISP_PIN_CSN);
    gpio_set_dir(DISP_PIN_CSN, GPIO_OUT);
    gpio_put(DISP_PIN_CSN, 1);

    gpio_init(DISP_PIN_DC);
    gpio_set_dir(DISP_PIN_DC, GPIO_OUT);

    gpio_init(DISP_PIN_RST);
    gpio_set_dir(DISP_PIN_RST, GPIO_OUT);

    display_hw_reset();

    display_hw_write_cmd(0x01);
    sleep_ms(150);

    display_hw_write_cmd(0x11);
    sleep_ms(150);

    display_hw_write_cmd(0x3A);
    display_hw_write_data(0x05);

    display_hw_write_cmd(0x36);
    display_hw_write_data(0x28);

    display_hw_write_cmd(0x29);
    sleep_ms(100);
}