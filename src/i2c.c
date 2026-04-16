#include <stdio.h>
#include <stdlib.h>
#include "hardware/i2c.h"

#define I2C_SDA_GPIO 4
#define I2C_SCL_GPIO 5
#define I2C_ADDRESS 0x52 //The address is 7 bits

//Registers in the color sensor
#define MAIN_CTRL 0x0
#define LS_DATA_GREEN 0x0D //the lowest of the three bytes
#define LS_DATA_BLUE 0x10 //the lowest of the three bytes
#define LS_DATA_RED 0x13 //the lowest of the three bytes

//The colors the robot might search for
#define RED 0
#define GREEN 1
#define BLUE 2

void init_i2c () {

    i2c_init(i2c0, 115200); //The datasheet for the color sensor says to use this baudrate

    gpio_set_function(I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_GPIO, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO);

    //More things to configure? (schimitt trigger, limited slew rate)

}

/*-----------------------------------------------------------------------------------
I2C read and write
-----------------------------------------------------------------------------------*/

uint8_t read_i2c_register (uint8_t address) {

    uint8_t read_value;

    i2c_write_blocking(i2c0, I2C_ADDRESS, &address, 1, true);
    i2c_read_blocking(i2c0, I2C_ADDRESS, &read_value, 1, false);

    return read_value;

}

void write_i2c_register (uint8_t address, uint8_t value) {

    uint8_t write_values[2];
    
    write_values[0] = address;
    write_values[1] = value;

    i2c_write_blocking(i2c0, I2C_ADDRESS, write_values, 2, false);

}

/*-----------------------------------------------------------------------------------
Color sensor code
-----------------------------------------------------------------------------------*/

void init_color_sensor () {

    //Enable bit 2 of MAIN_CTRL to turn on RGB mode
    write_i2c_register(MAIN_CTRL, 0b00000100);

}

int color_check(int target_color) {

    int colors[3] = {0, 0, 0}; //red, green, blue

    colors[0] |= read_i2c_register(LS_DATA_RED);
    colors[0] |= read_i2c_register(LS_DATA_RED + 1) << 8;
    colors[0] |= read_i2c_register(LS_DATA_RED + 2) << 16;

    colors[1] |= read_i2c_register(LS_DATA_GREEN);
    colors[1] |= read_i2c_register(LS_DATA_GREEN + 1) << 8;
    colors[1] |= read_i2c_register(LS_DATA_GREEN + 2) << 16;

    colors[2] |= read_i2c_register(LS_DATA_BLUE);
    colors[2] |= read_i2c_register(LS_DATA_BLUE + 1) << 8;
    colors[2] |= read_i2c_register(LS_DATA_BLUE + 2) << 16;

    //Scale the colors from 18 bits to the [0, 255] range
    colors[0] >>= 10;
    colors[1] >>= 10;
    colors[2] >>= 10;

    //The target color is matched if it has at least intensity 80 and is at least 50 greater than the other two colors
    if (colors[target_color] >= 80 && 
        colors[target_color] >= max(colors[(target_color + 1) % 3], colors[(target_color + 2) % 3])) {

        return true;
    }

    return false;

}

/*-----------------------------------------------------------------------------------
Distance sensor code (but actually it might use UART?)
-----------------------------------------------------------------------------------*/

void init_distance_sensor () {

    

}

int get_distance_inches() {


}