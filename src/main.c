#include <math.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "timer.h"
#include "pwm/pwm.h"
#include "servo.h"
#include "sensors/color_sensor.h"
#include "sensors/distance_sensor.h"
#include "display/display.h"
#include "display/display_button.h"
#include "battery/battery.h"
#include "adc.h"

/*
TIMER USAGE

Timer0
    Alarm0: Configure, Search (color check), Rotation, Distance check, Display check
    Alarm1: Distance sensor pulse sending
    Alarm2: Distance sensor echo measuring
    Alarm3: 
Timer1
    Alarm0: 
    Alarm1: 
    Alarm2: 
    Alarm3: 

*/

void initialize_configure_timer();
void configure_handler();
// void initialize_search_timer();
// void search_handler();
// void initialize_rotation(int degrees);
// void stop_rotation_handler();
void search_v2();
void rotate_v2(float degrees);
void distance_v2();
void stopped_v2();
// void initialize_distance_check_timer();
// void distance_check_timer_handler();
// void initialize_display_update_timer();
// void display_update_timer_handler();
// void interrupt_handler();
int get_state();

void init_pwm(void);
void set_right_motor_speed(float speed);
void set_left_motor_speed(float speed);
int areMotorsOn(void);

void init_adc();
void init_adc_freerun();
uint16_t get_adc_value();

void servo_init(void);
void servo_set_angle(float);
void servo_move_by(float);
float servo_get_current_angle(void);


//The number of seconds the robot takes to turn 180 degrees. Determined experimentally
#define seconds_rotate_180 5.0

//The current angle that the servo with the color sensor is oriented at.
float current_servo_angle = 0;

//For the states the robot can be in
#define CONFIGURE 0
#define SEARCH 1
#define ROTATE 2
#define FORWARD 3
#define STOPPED 4

//The robot's current state
int current_state = CONFIGURE;

//The colors the robot might search for
#define RED 0
#define GREEN 1
#define BLUE 2

//The color selected for the robot
int target_color = BLUE;

//Directions for rotation (of servo or robot)
#define LEFT 0
#define RIGHT 1

//Current servo rotation direction
int servo_rotation_direction = LEFT;
//Current robot rotation direction
int robot_rotation_direction = LEFT;

//The number of degrees the robot turns the servo with each search increment
#define SEARCH_INCREMENT 1.0

//The interval between checks for color being selected
#define CONFIGURE_INTERVAL 0.02
//The number of seconds the robot spends on each degree of search
#define SEARCH_INTERVAL 0.02
//The interval between checking distance while moving forward
#define FORWARD_INTERVAL 0.1 
//The interval between updates of the display
#define STOPPED_INTERVAL 1

//The maximum motor speed the robot will use
#define MAX_MOTOR_SPEED 1.0

//for handling a certain problem with the distance check timing
int first_distance_check = true;

//global display data variable
display_data_t data = {
    .screen = DISPLAY_SCREEN_START,
    .distance_in = 0.0f,
    // .total_distance_in = 10.0f,
    .battery_v = 7.8f,
    .color_name = "",
    .state = DISPLAY_STATE_MOVING,
    .selected_color = DISPLAY_COLOR_NONE,
    .selected_color_hex = 0x0000,
    .start_requested = false,
    .color_locked = false,
    .loading_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR,
    .color_found_pulse = false,
    .distance_done_pulse = false
};

static void refresh_display(void) {
    data.battery_v = battery_get_voltage();
    display_render(&data);
}

void configure_v2(void) {
    current_state = CONFIGURE;
    data.screen = DISPLAY_SCREEN_START;
    data.start_requested = false;
    data.color_locked = false;

    while (!data.start_requested) {
        refresh_display();
        sleep_ms(10);
    }

    target_color = data.selected_color;

    if (target_color == RED) {
        data.color_name = "RED";
    } else if (target_color == GREEN) {
        data.color_name = "GREEN";
    } else if (target_color == BLUE) {
        data.color_name = "BLUE";
    }

    data.color_locked = true;
}
/*-----------------------------------------------------------------------------------
Main function
-----------------------------------------------------------------------------------*/

int main() {

    stdio_init_all();

    printf("\nMAIN IS RUNNING\n");

    //Initialize components
    init_pwm();
    servo_init();
    init_i2c();
    init_color_sensor();
    init_distance_gpio();
    battery_init();
    init_echo_gpio_irq();
    init_adc();
    init_adc_freerun();
    get_adc_value();
    //Display Data
    display_init();
    display_button_init();

    //setting the initial battery voltage
    data.battery_v = battery_get_voltage();

    //Set interrupt handler
    //irq_set_exclusive_handler(TIMER0_IRQ_0, interrupt_handler);
    
    //Start program logic
    while (1) {
        switch (current_state) {
            case CONFIGURE: {
                // initialize_configure_timer();
                configure_v2();
                current_state = SEARCH;
                break;
            }
            case SEARCH: {
                //initialize_search_timer();
                search_v2();
                // current_state = ROTATE;
                break;
            }
            case ROTATE: {
                //initialize_rotation(0); //can change argument for debugging purposes
                rotate_v2(current_servo_angle);
                // current_state = FORWARD;
                break;
            }
            case FORWARD: {
                //initialize_distance_check_timer();
                distance_v2();
                // current_state = STOPPED;
                break;
            }
            case STOPPED: {
                // initialize_display_update_timer();
                data.screen = DISPLAY_SCREEN_STOPPED;
                data.state = DISPLAY_STATE_STOPPED;
                refresh_display();
                sleep_ms(20);
                break;
            }
        }
        data.battery_v = battery_get_voltage();
        sleep_ms(10);

    }

    //infinite loop
    // while(1) 
    // {

    // servo_set_angle(45.0);
    // set_left_motor_speed(-0.1);
    // set_right_motor_speed(0.2);
    // sleep_ms(5000);

    // servo_set_angle(0.0);
    // set_left_motor_speed(0.0);
    // set_right_motor_speed(0.0);
    // sleep_ms(2000);

    //battery update
    //     data.battery_v = battery_get_voltage();
    // }

    return 0;
}

/*-----------------------------------------------------------------------------------
Functions for CONFIGURE state
-----------------------------------------------------------------------------------*/

void initialize_configure_timer() {

    //Enable interrupt for timer0 alarm0
    timer0_hw->inte |= 1;  

    //Enable IRQ TIMER0_IRQ_0
    irq_set_enabled(TIMER0_IRQ_0, 1);

    //Set TIMER0 to fire alarm 0 after CONFIGURE_INTERVAL seconds
    timer0_hw->alarm[0] = timer0_hw->timerawl + (CONFIGURE_INTERVAL * 1000000);

}

void configure_handler() {


    if (data.start_requested) { 

        target_color = data.selected_color; 
        timer0_hw->inte &= ~1;  //disable previous timer
        //initialize_search_timer();
        search_v2();

    } else {

        //Arm timer again
        timer0_hw->alarm[0] = timer0_hw->timerawl + (CONFIGURE_INTERVAL * 1000000);

    }

}

/*-----------------------------------------------------------------------------------
Functions for SEARCH state
-----------------------------------------------------------------------------------*/

void search_v2() {
    current_state = SEARCH;
    data.screen = DISPLAY_SCREEN_LOADING;
    data.loading_stage = DISPLAY_LOADING_STAGE_SEARCH_COLOR;
    calibrate_colors();

    while (current_state == SEARCH) {

        //check color
        if (color_check(target_color)) {
            data.color_found_pulse = true;
            refresh_display();
            sleep_ms(150);
            data.color_found_pulse = false;

            //go to ROTATE state
            // rotate_v2(current_servo_angle);
            current_state = ROTATE;
            return;

        } else {

            //Move the servo
            if (current_servo_angle <= -90) {

                servo_rotation_direction = RIGHT;

            } else if (current_servo_angle >= 90) {

                servo_rotation_direction = LEFT;

            }        

            float degrees_to_turn;

            if (servo_rotation_direction == RIGHT) {

                degrees_to_turn = SEARCH_INCREMENT;

            } else {

                degrees_to_turn = -1 * SEARCH_INCREMENT;

            }

            //printf("degrees to turn = %f\n", degrees_to_turn);
            servo_move_by(degrees_to_turn); 
            current_servo_angle += degrees_to_turn;

            sleep_ms(20);
        }

    }

}

// void initialize_search_timer() {

//     current_state = SEARCH;

//     calibrate_colors();

//     //Enable interrupt for timer0 alarm0
//     timer0_hw->inte |= 1;  

//     //Enable IRQ TIMER0_IRQ_0
//     irq_set_enabled(TIMER0_IRQ_0, 1);

//     //Set TIMER0 to fire alarm 1 after SEARCH_INTERVAL seconds
//     timer0_hw->alarm[0] = timer0_hw->timerawl + (SEARCH_INTERVAL * 1000000);

// }

// void search_handler() {

//     //check color
//     if (color_check(target_color)) {

//         //go to ROTATE state
//         //printf("going to ROTATE");
//         timer0_hw->inte &= ~1;  //disable previous timer
//         //initialize_rotation(current_servo_angle);
//         rotate_v2(current_servo_angle);

//     } else {

//         //Move the servo
//         if (current_servo_angle <= -90) {

//             servo_rotation_direction = RIGHT;

//         } else if (current_servo_angle >= 90) {

//             servo_rotation_direction = LEFT;

//         }        

//         float degrees_to_turn;

//         if (servo_rotation_direction == RIGHT) {

//             degrees_to_turn = SEARCH_INCREMENT;

//         } else {

//             degrees_to_turn = -1 * SEARCH_INCREMENT;

//         }

//         //printf("degrees to turn = %f\n", degrees_to_turn);
//         servo_move_by(degrees_to_turn); 
//         current_servo_angle += degrees_to_turn;
        

//         //Arm timer again
//         timer0_hw->alarm[0] = timer0_hw->timerawl + (SEARCH_INTERVAL * 1000000);

//     }

// }

/*-----------------------------------------------------------------------------------
Functions for ROTATE state
-----------------------------------------------------------------------------------*/

void rotate_v2(float degrees) {

    printf("rotate_v2 called\n");

    current_state = ROTATE;
    data.screen = DISPLAY_SCREEN_LOADING;
    data.loading_stage = DISPLAY_LOADING_STAGE_CALC_DISTANCE;
    refresh_display();

    double seconds_rotate = fabs(degrees) * seconds_rotate_180 / 180.0;

    if (degrees < 0) {

        set_left_motor_speed(0.5 * MAX_MOTOR_SPEED); 
        set_right_motor_speed(0.5 * MAX_MOTOR_SPEED);

    }

    else if (degrees > 0) {

        set_left_motor_speed(-0.5 * MAX_MOTOR_SPEED); 
        set_right_motor_speed(-0.5 * MAX_MOTOR_SPEED);

    }

    else
    {
        set_left_motor_speed(0); 
        set_right_motor_speed(0);
    }

    printf("sleeping for %lf seconds\n", seconds_rotate);

    absolute_time_t start = get_absolute_time();
    while (absolute_time_diff_us(start, get_absolute_time()) < (int64_t)(seconds_rotate * 1000000.0)) {
        refresh_display();
        sleep_ms(10);
    }

    // sleep_ms(1000 * seconds_rotate);

    printf("ending sleep statement\n");

    //Stop rotation
    set_left_motor_speed(0); 
    set_right_motor_speed(0);

    data.distance_done_pulse = true;
    refresh_display();
    sleep_ms(150);
    data.distance_done_pulse = false;

    current_state = FORWARD;

    //go to distance state
    // distance_v2();

}

// void initialize_rotation(int degrees /*from -90 to 90*/) {

//     current_state = ROTATE;

//     //printf("entered initialize rotation\n");

//     double seconds_rotate = fabs(degrees) * seconds_rotate_180 / 180.0;

//     if (degrees < 0) {

//         set_left_motor_speed(0.5 * MAX_MOTOR_SPEED); 
//         set_right_motor_speed(0.5 * MAX_MOTOR_SPEED);

//     }

//     else if (degrees > 0) {

//         set_left_motor_speed(-0.5 * MAX_MOTOR_SPEED); 
//         set_right_motor_speed(-0.5 * MAX_MOTOR_SPEED);

//     }

//     //printf("set the motor speeds\n");

//     //Enable interrupt for timer0 alarm0
//     timer0_hw->inte |= 1;  

//     //Enable IRQ TIMER0_IRQ_0
//     irq_set_enabled(TIMER0_IRQ_0, 1);

//     //Set TIMER0 to fire alarm 0 after (seconds_rotate) seconds
//     timer0_hw->alarm[0] = timer0_hw->timerawl + (seconds_rotate * 1000000);

// }

// void stop_rotation_handler() {

//     //printf("\ngoing to stop rotation handler\n");

//     //Stop rotation
//     set_left_motor_speed(0); 
//     set_right_motor_speed(0);

//     //Move to the next state
//     //printf("going to FORWARD");
//     timer0_hw->inte &= ~1;  //disable previous timer
//     //initialize_distance_check_timer();
//     distance_v2();

// }

/*-----------------------------------------------------------------------------------
Functions for FORWARD state
-----------------------------------------------------------------------------------*/

void distance_v2() {

    printf("distance start\n");
    current_state = FORWARD;
    data.screen = DISPLAY_SCREEN_DRIVE;
    data.state = DISPLAY_STATE_MOVING;

    while (current_state == FORWARD) {
        
        printf("distance loop\n");

        send_pulse();
        sleep_ms(10);
        
        double distance_inches = get_distance_inches();

        data.distance_in = distance_inches;

        //To prevent complications from how on the first check distance_inches will probably be 0
        if (first_distance_check) {
            first_distance_check = false;
            sleep_ms(1000 * FORWARD_INTERVAL);
            continue;
        }
        if (distance_inches < 2.1) {

            //go to STOPPED state
            set_left_motor_speed(0); 
            set_right_motor_speed(0);
            data.state = DISPLAY_STATE_STOPPED;
            current_state = STOPPED;
            stopped_v2();

        } else {

            float motor_speed = (float)(MAX_MOTOR_SPEED * 0.25f * sqrt(distance_inches - 2.0f));
            if (motor_speed > MAX_MOTOR_SPEED) {
                motor_speed = MAX_MOTOR_SPEED;
            }
            set_left_motor_speed(motor_speed); 
            set_right_motor_speed(-motor_speed);
            data.state = DISPLAY_STATE_MOVING;

        }
        refresh_display();
        sleep_ms(1000 * FORWARD_INTERVAL);

    }

}

// void initialize_distance_check_timer() {

//     current_state = FORWARD;

//     init_echo_gpio_irq();

//     //Enable interrupt for timer0 alarm0
//     timer0_hw->inte |= 1;

//     //Enable IRQ TIMER0_IRQ_0
//     irq_set_enabled(TIMER0_IRQ_0, 1);

//     //Set TIMER0 to fire alarm 0 after FORWARD_INTERVAL seconds
//     timer0_hw->alarm[0] = timer0_hw->timerawl + (FORWARD_INTERVAL * 1000000);

// }

// void distance_check_timer_handler() {

//     send_pulse();
//     double distance_inches = get_distance_inches();

//     data.distance_in = distance_inches;

//     //To prevent complications from how on the first check distance_inches will probably be 0
//     if (first_distance_check) {
//         first_distance_check = false;
//         //Arm timer again
//         timer0_hw->alarm[0] = timer0_hw->timerawl + (FORWARD_INTERVAL * 1000000);
//         return;
//     }

//     if (distance_inches < 2.1) {

//         //go to STOPPED state
//         set_left_motor_speed(0); 
//         set_right_motor_speed(0);
//         timer0_hw->inte &= ~1;  //disable previous timer
//         initialize_display_update_timer();

//     } else {

//         float motor_speed = (float)(MAX_MOTOR_SPEED * 0.25f * sqrt(distance_inches - 2.0f));
//         if (motor_speed > MAX_MOTOR_SPEED) {
//             motor_speed = MAX_MOTOR_SPEED;
//         }
//         set_left_motor_speed(motor_speed); 
//         set_right_motor_speed(-motor_speed);

//         //Arm timer again
//         timer0_hw->alarm[0] = timer0_hw->timerawl + (FORWARD_INTERVAL * 1000000);

//     }

// }

/*-----------------------------------------------------------------------------------
Functions for STOPPED state
-----------------------------------------------------------------------------------*/

void stopped_v2() {

    current_state = STOPPED;

    while (current_state == STOPPED) {

        //Get ADC value
        uint16_t adc_value = get_adc_value();
        
        printf("ADC value: %d\n", adc_value);

        double estimated_voltage = 3.3 * (double)adc_value / (double)0b111111111111; 

        printf("Voltage: : %lf\n", estimated_voltage);

        if (estimated_voltage < 0.75) {

            printf("Light level: DARK\n");

        } else if (estimated_voltage < 2.2) {

            printf("Light level: MEDIUM\n");

        } else {

            printf("Light level: BRIGHT\n");

        }

        sleep_ms(1000 * STOPPED_INTERVAL);

    }

}

// void initialize_display_update_timer() {

//     current_state = STOPPED;

//     //Enable interrupt for timer0 alarm0
//     timer0_hw->inte |= 1;

//     //Enable IRQ TIMER0_IRQ_0
//     irq_set_enabled(TIMER0_IRQ_0, 1);

//     //Set TIMER0 to fire alarm 3 after STOPPED_INTERVAL onds
//     timer0_hw->alarm[0] = timer0_hw->timerawl + (STOPPED_INTERVAL 000000);

// }

// void display_update_timer_handler() {

//     //Update display
//     //display_update(); //NEED PARAMETER FOR THIS?

// }

/*-----------------------------------------------------------------------------------
Other functions
-----------------------------------------------------------------------------------*/

// void interrupt_handler() {

//     //printf("interrupt handler, state %d\n", current_state);

//     //acknoweldge interrupt
//     timer0_hw->intr |= 1u;

//     switch (current_state) {
//         case CONFIGURE: configure_handler(); break;
//         case SEARCH: search_handler(); break;
//         case ROTATE: stop_rotation_handler(); break;
//         case FORWARD: distance_check_timer_handler(); break;
//         case STOPPED: display_update_timer_handler(); break;
//     }

// }

int get_state() {

    return current_state;

}
