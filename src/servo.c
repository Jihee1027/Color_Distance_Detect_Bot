#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "sensors/color_sensor.h"

#define SERVO_PIN 39

//PWM setting at 50 HZ
#define SERVO_WRAP 62499
#define SERVO_CLKDIV 40.0f

//pulse widths for SG_5010
#define PULSE_MIN_US 500 //-90 degrees
#define PULSE_MAX_US 2500 //90 degrees
#define PULSE_MID_US 1500 //0 degrees

//track current servo position
float curr_servo_angle = 0.0f;
//PWM slice and channel for servo
static uint servo_slice;
static uint servo_chan;

void servo_init(void);
void servo_set_angle(float angle);
void servo_move_by(float delta_degrees);
float servo_get_current_angle(void);


void servo_init(void)
{
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    
    servo_slice = pwm_gpio_to_slice_num(SERVO_PIN);
    servo_chan = pwm_gpio_to_channel(SERVO_PIN);

    pwm_set_clkdiv(servo_slice, SERVO_CLKDIV);
    pwm_set_wrap(servo_slice, SERVO_WRAP);
    pwm_set_enabled(servo_slice, true);

    servo_set_angle(0.0f);
    sleep_ms(600);

    printf("Servo returned to foward facing\n");
}

void servo_set_angle(float angle) 
{
    //clamp amgle to valid range
    if(angle < -90.0f) 
    {
        angle = -90.0f;
    }

    if(angle >  90.0f)
    {
        angle =  90.0f;
    }

    //update servo angle
    curr_servo_angle = angle;

    //convert angle to pulse width (us)
    float pulse_us = PULSE_MID_US + (angle * 11.11f); //11.11 us per degree 
    //convert pulse width to PWM level
    uint16_t level = (uint16_t)((pulse_us / 20000.0f) * (SERVO_WRAP + 1));

    //ser PWM duty cycle
    pwm_set_chan_level(servo_slice, servo_chan, level);
}

//move servo by relative amount
void servo_move_by(float delta_degrees) 
{
    servo_set_angle(curr_servo_angle + delta_degrees);
}

//scan for the specific color and return true if found
bool servo_scan_for_color(int target_color)
{
    printf("Starting full scan for color %d...\n", target_color);

    //sweep left to right
    for (float angle = -90.0f; angle <= 90.0f; angle += 3.0f) 
    {     
        servo_set_angle(angle);
        sleep_ms(30);

        if (color_check(target_color)) 
        { 
            printf("\nTARGET COLOR FOUND at %.1f degrees\n", angle);
            return true;
        }
    }

    //sweep right to left
    for (float angle = 90.0f; angle >= -90.0f; angle -= 3.0f) 
    {
        servo_set_angle(angle);
        sleep_ms(30);

        if (color_check(target_color)) 
        {
            printf("\nTARGET COLOR FOUND at %.1f degrees\n", angle);
            return(true);
        }

    }

    printf("Scan completed - Target color not found\n");
    servo_set_angle(0.0f);
    return(false);
}

float servo_get_current_angle(void) 
{
    return(curr_servo_angle);
}
