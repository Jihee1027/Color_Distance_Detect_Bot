#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pwm.h"

#define LEFT_MOTOR_PIN 37 //pwmA motor driver
#define RIGHT_MOTOR_PIN 38 //pwmB motor driver

#define AIN1PIN 27 //left forward
#define AIN2PIN 28 //left reverse

#define BIN1PIN 29 //right forward
#define BIN2PIN 30 //right reverse

#define STANDBY 31 //standby for motor driver, active high

#define MOTORWRAP 31249 //combination for pwmF of 4kHz
#define CLKDIV 1.0f


//function for the initialization of the PWM pins needed
void pwm_init(void)
{
    gpio_set_function(LEFT_MOTOR_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR_PIN, GPIO_FUNC_PWM);
    
    //initialize directions
    gpio_init(AIN1PIN);
    gpio_init(AIN2PIN);
    gpio_init(BIN1PIN);
    gpio_init(BIN2PIN);
    gpio_init(STANDBY);

    gpio_set_dir(AIN1PIN, GPIO_OUT);
    gpio_set_dir(AIN2PIN, GPIO_OUT);
    gpio_set_dir(BIN1PIN, GPIO_OUT);
    gpio_set_dir(BIN2PIN, GPIO_OUT);
    gpio_set_dir(STANDBY, GPIO_OUT);

    //standby for motor driver needs to be high
    gpio_put(STANDBY, 1);

    //slice determination
    int right_slice;
    int left_slice;

    right_slice = pwm_gpio_to_slice_num(RIGHT_MOTOR_PIN);
    left_slice = pwm_gpio_to_slice_num(LEFT_MOTOR_PIN);


    //enabling + clkdiv calculations for motor
    pwm_set_clkdiv(left_slice, CLKDIV);
    pwm_set_clkdiv(right_slice, CLKDIV);

    pwm_set_wrap(left_slice, MOTORWRAP);
    pwm_set_wrap(right_slice, MOTORWRAP);

    pwm_set_enabled(left_slice, true);
    pwm_set_enabled(right_slice, true);

    //setting the initial state - stationary
    set_right_motor_speed(0.0);
    set_left_motor_speed(0.0);

    printf("motors initialized\n");

}

//function for setting the right motor speed
void set_right_motor_speed(double speed)
{
    if (speed > 1.0)
    {
        speed = 1.0; //sets it at max
    }

    if (speed < -1.0)
    {
        speed = -1.0; //sets it at max rev

    }

    //for forward movement
    if (speed > 0.0)
    {
        gpio_put(BIN1PIN, 1);
        gpio_put(BIN2PIN, 0);

        pwm_set_gpio_level(RIGHT_MOTOR_PIN, speed * MOTORWRAP);
    }
    else if (speed < 0.0)
    {
        gpio_put(BIN1PIN, 0);
        gpio_put(BIN2PIN, 1); //reverses

        pwm_set_gpio_level(RIGHT_MOTOR_PIN, -(speed) * MOTORWRAP);
    }
    else //stays
    {
        gpio_put(BIN1PIN, 0);
        gpio_put(BIN2PIN, 0);

        pwm_set_gpio_level(RIGHT_MOTOR_PIN, 0);
    }
}

//function for setting the left motor speed
void set_left_motor_speed(double speed)
{
    if (speed > 1.0)
    {
        speed = 1.0; //sets it at max
    }

    if (speed < -1.0)
    {
        speed = -1.0; //sets it at max rev

    }

    //for forward movement
    if (speed > 0.0)
    {
        gpio_put(AIN1PIN, 1);
        gpio_put(AIN2PIN, 0);

        pwm_set_gpio_level(LEFT_MOTOR_PIN, speed * MOTORWRAP);
    }
    else if (speed < 0.0)
    {
        gpio_put(AIN1PIN, 0);
        gpio_put(AIN2PIN, 1); //reverses

        pwm_set_gpio_level(LEFT_MOTOR_PIN, -(speed) * MOTORWRAP);
    }
    else //stays
    {
        gpio_put(AIN1PIN, 0);
        gpio_put(AIN2PIN, 0);

        pwm_set_gpio_level(LEFT_MOTOR_PIN, 0);
    }
}

//function for the servo control

//extra miscellaneous function