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

    //Necessary as described in datasheet
    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO); //There will also be need of external pull-up resistors?
    gpio_set_slew_rate(I2C_SDA_GPIO, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(I2C_SCL_GPIO, GPIO_SLEW_RATE_SLOW);

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

//Will place the rgb values detected into those pointer values
void get_rgb(int* red, int* green, int* blue) {

    *red = 0;
    *green = 0;
    *blue = 0;

    *red |= read_i2c_register(LS_DATA_RED);
    *red |= read_i2c_register(LS_DATA_RED + 1) << 8;
    *red |= read_i2c_register(LS_DATA_RED + 2) << 16;

    *green |= read_i2c_register(LS_DATA_GREEN);
    *green |= read_i2c_register(LS_DATA_GREEN + 1) << 8;
    *green |= read_i2c_register(LS_DATA_GREEN + 2) << 16;

    *blue |= read_i2c_register(LS_DATA_BLUE);
    *blue |= read_i2c_register(LS_DATA_BLUE + 1) << 8;
    *blue |= read_i2c_register(LS_DATA_BLUE + 2) << 16;

    //Scale the colors from 18 bits to the [0, 255] range
    *red >>= 10;
    *green >>= 10;
    *blue >>= 10;

}

//0 = red, 1 = green, 2 = blue, -1 = none of those detected
int get_color() {

    int colors[3]; //red, green, blue

    get_rgb(&(colors[0]), &(colors[1]), &(colors[2]));

    //The target color is matched if it has at least intensity 80 and is at least 50 greater than the other two colors
    for (int i = 0; i <=2; i++) {

        if (colors[i] >= 80 && 
            colors[i] >= colors[(i + 1) % 3] + 50 &&
            colors[i] >= colors[(i + 2) % 3] + 50) {

            return i;
        }

    }

    return -1;

}

//checks if detected color matches target color
int color_check(int target_color) {

    return get_color() == target_color;

}
