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

//Amounts of colors detected with baseline room lighting
int color_baselines[3];

void init_i2c();
uint8_t read_i2c_register(uint8_t address);
void write_i2c_register(uint8_t address, uint8_t value);
void init_color_sensor();
void calibrate_colors();
void get_rgb(int* red, int* green, int* blue);
int get_color();
int color_check(int target_color);

void init_i2c() {

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
    //Enable bit 1 to turn on light sensor
    write_i2c_register(MAIN_CTRL, 0b00000110);

}

void calibrate_colors() {

    int color_read[3];
    int color_totals[3] = {0, 0, 0};

    for (int i = 0; i < 50; i++) {
        get_rgb(&(color_read[0]), &(color_read[1]), &(color_read[2]));
        for (int j = 0; j <= 2; j++) {
            color_totals[j] += color_read[j];
        }
    }

    for (int i = 0; i <= 2; i++) {

        color_baselines[i] = color_totals[i] / 50;

    }

    printf("\ncolors calibrated!\n");

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

    //printf("\n--------\nred: %d\ngreen: %d\nblue: %d\n--------\n", *red, *green, *blue);

}

//0 = red, 1 = green, 2 = blue, -1 = none of those detected
int get_color() {

    int colors[3]; //red, green, blue
    double color_ratios[3];

    get_rgb(&(colors[0]), &(colors[1]), &(colors[2]));

    for (int i = 0; i <= 2; i++) {
        color_ratios[i] = (double)(colors[i]) / (double)(color_baselines[i]);
    }

    printf("\n--------\nred: %f\ngreen: %f\nblue: %f\n--------\n", color_ratios[0], color_ratios[1], color_ratios[2]);

    for (int i = 0; i <=2; i++) {

        if (color_ratios[i] >= 1.15 && 
            color_ratios[i] / color_ratios[(i + 1) % 3] >= 1.05 &&
            color_ratios[i] / color_ratios[(i + 2) % 3] >= 1.05) {

            return i;
        }

    }

    return -1;

}

//checks if detected color matches target color
int color_check(int target_color) {

    int color = get_color();
    //printf("%d\n", color);
    return get_color() == target_color;

}
