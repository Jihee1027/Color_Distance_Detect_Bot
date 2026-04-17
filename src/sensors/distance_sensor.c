#include "uart.h"
#include "io_bank0.h"

#define UART_TX_GPIO 12
#define UART_RX_GPIO 13

void init_uart() {

    gpio_set_function(UART_TX_GPIO, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_GPIO, GPIO_FUNC_UART);

    uart_init(uart0, 9600); //specified in datasheet

}

/*-----------------------------------------------------------------------------------
Distance sensor code 
-----------------------------------------------------------------------------------*/

void init_distance_sensor () {

    //Datasheet doesn't describe anything for this?

}

double get_distance_inches() {

    int src = 0x55;

    uart_write_blocking(uart0, &src, 1);

    uint8_t distance_millimeters[2];
    uart_read_blocking(uart0, distance_millimeters, 2);

    //Convert to inches
    return (double)(((int)distance_millimeters[0] << 8) | (int)distance_millimeters[1]) * 0.039;

}

/*
Consider having a mode where if it's close enough it switches to the I2C thing,
just for the sake of meeting the proposal objective
*/
