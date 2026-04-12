#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "queue.h"
#include "support.h"
#include "pwm.h"

#define LEFT_MOTOR_PIN 37
#define RIGHT_MOTOR_PIN 38
#define SERVO_PIN 39

//function for the initialization of the PWM pins needed
void pwm_init(void)
{
    gpio_set_function(LEFT_MOTOR_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR_PIN, GPIO_FUNC_PWM);
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    //slice determination
    int right_slice;
    int left_slice;
    int servo_slice;

    right_slice = pwm_gpio_to_slice_num(RIGHT_MOTOR_PIN);
    left_slice = pwm_gpio_to_slice_num(LEFT_MOTOR_PIN);
    servo_slice = pwm_gpio_to_slice_num(SERVO_PIN);

    //channel determination
    int left_channel;
    int right_channel;
    int servo_channel;

    left_channel = pwm_gpio_to_channel(LEFT_MOTOR_PIN);
    right_channel = pwm_gpio_to_channel(RIGHT_MOTOR_PIN);
    servo_channel = pwm_gpio_to_channel(SERVO_PIN);

    //enabling + clkdiv calculations for motor
    pmw_set_clkdiv(left_slice, /*motor clkdiv*/);
    pwm_set_clkdiv(right_slice, /*motor clkdiv*/);

    pwm_set_wrap(left_slice, /*motor wrap*/);
    pwm_set_wrap(right_slice, /*motor wrap*/);

    pwm_set_enabled(left_slice, true);
    pwm_set_enabled(right_slice, true);

    //servo enabling and clkdiv
    pwm_set_clkdiv(servo_slice, /*servo clkdiv*/);
    pwm_set_wrap(servo_slice, /*servo wrap*/);
    pwm_set_enabled(servo_slice, true);

    //setting the initial state
    pwm_set_gpio_level(LEFT_MOTOR_PIN, 0);
    pwm_set_gpio_level(RIGHT_MOTOR_PIN, 0);
    
    



}

//function for setting the left motor speed

//function for setting the right motor speed

//function for the servo control

//extra miscellaneous function