#include <math.h>
#include "hardware/timer.h"
#include "i2c.c"
#include "display/display_spi.c"

void initialize_search_timer();
void search_handler();
void initialize_distance_check_timer();
void distance_check_timer_handler();
void initialize_display_update_timer();
void display_update_timer_handler();

//The number of seconds the robot takes to turn 180 degrees. Determined experimentally
#define seconds_rotate_180 5.0

//The current angle that the servo with the color sensor is oriented at.
int current_servo_angle = 0;
//The current angle the robot is facing from -90 to 90 degrees from its starting orientation.
int current_robot_angle = 0;

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
int target_color = RED;

//Directions for rotation (of servo or robot)
#define LEFT 0
#define RIGHT 1

//Current servo rotation direction
int servo_rotation_direction = LEFT;
//Current robot rotation direction
int robot_rotation_direction = LEFT;

//The number of degrees the robot turns the servo with each search increment
#define SEARCH_INCREMENT 1.0
//The number of seconds the robot spends on each degree of search
#define SEARCH_INTERVAL 0.02
//The interval between checking distance while moving forward
#define FORWARD_INTERVAL 0.02
//The interval between updates of the display
#define DISPLAY_REFRESH_INTERVAL 0.01

//The maximum motor speed the robot will use
#define MAX_MOTOR_SPEED 1.0

/*-----------------------------------------------------------------------------------
Main function
-----------------------------------------------------------------------------------*/

void main() {

    //Initialize components
    pwm_init();
    display_init();
    init_i2c();
    init_color_sensor();

    //Start program logic
    initialize_search_timer();

}

/*-----------------------------------------------------------------------------------
Functions for CONFIGURE state
-----------------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------------
Functions for SEARCH state
-----------------------------------------------------------------------------------*/

void initialize_search_timer() {

    //Enable interrupt for timer0 alarm1
    timer0_hw->inte |= 2;  

    //Set handler for interrupt to search_handler
    irq_set_exclusive_handler(TIMER0_IRQ_1, search_handler);

    //Enable IRQ TIMER0_IRQ_1
    irq_set_enabled(TIMER0_IRQ_1, 1);

    //Set TIMER0 to fire alarm 1 after SEARCH_INTERVAL seconds
    timer0_hw->alarm[1] = timer0_hw->timerawl + (SEARCH_INTERVAL * 1000000);

}

void search_handler() {

    //Acknowledge the interrupt
    timer0_hw->intr |= 2ul;

    //check color
    if (color_check(target_color)) {

        //go to ROTATE state
        current_state = ROTATE;
        intialize_rotation(current_servo_angle);

    } else {

        //Move the servo
        if (current_servo_angle <= -90) {

            servo_rotation_direction = RIGHT;

        } else if (current_servo_angle >= 90) {

            servo_rotation_direction = LEFT;

        }
        else {

            int degrees_to_turn;

            if (servo_rotation_direction == RIGHT) {

                degrees_to_turn = SEARCH_INCREMENT;

            } else {

                degrees_to_turn = -1 * SEARCH_INCREMENT;

            }

            move_servo(degrees_to_turn); //IMPLEMENTED BY PWM?
            current_servo_angle += degrees_to_turn;

        }

        //Arm timer again
        timer0_hw->alarm[1] = timer0_hw->timerawl + (SEARCH_INTERVAL * 1000000);

    }

}

/*-----------------------------------------------------------------------------------
Functions for ROTATE state
-----------------------------------------------------------------------------------*/

void initialize_rotation(int degrees /*from -90 to 90*/) {

    double seconds_rotate = abs(degrees) * seconds_rotate_180 / 180.0;

    if (degrees < 0) {

        set_left_motor_speed(); //to be implemented by pwm?
        set_right_motor_speed();

    }

    else if (degrees > 0) {

        set_left_motor_speed(); //to be implemented by pwm?
        set_right_motor_speed();

    }

    //Enable interrupt for timer1 alarm0
    timer1_hw->inte |= 1;  

    //Set handler for interrupt to stop_rotation_handler
    irq_set_exclusive_handler(TIMER1_IRQ_0, stop_rotation_handler);

    //Enable IRQ TIMER1_IRQ_0
    irq_set_enabled(TIMER1_IRQ_0, 1);

    //Set TIMER1 to fire alarm 0 after (seconds_rotate) seconds
    timer1_hw->alarm[0] = timer1_hw->timerawl + (seconds_rotate * 1000000);

}

void stop_rotation_handler() {

    //Acknowledge the interrupt
    timer1_hw->intr |= 1ul;

    //Stop rotation
    set_left_motor_speed(0); //to be implemented by pwm?
    set_right_motor_speed(0);

    //Move to the next state
    current_state = FORWARD;
    initialize_distance_check_timer();

}

/*-----------------------------------------------------------------------------------
Functions for FORWARD state
-----------------------------------------------------------------------------------*/

void initialize_distance_check_timer() {

    //Enable interrupt for timer0 alarm2
    timer0_hw->inte |= 4;

    //Set handler for interrupt to distance_check_timer_handler
    irq_set_exclusive_handler(TIMER0_IRQ_2, distance_check_timer_handler);

    //Enable IRQ TIMER0_IRQ_2
    irq_set_enabled(TIMER0_IRQ_2, 1);

    //Set TIMER0 to fire alarm 2 after FORWARD_INTERVAL seconds
    timer0_hw->alarm[2] = timer0_hw->timerawl + (FORWARD_INTERVAL * 1000000);

}

void distance_check_timer_handler() {

    //Acknowledge the interrupt
    timer0_hw->intr |= 4ul;

    int distance_inches = get_distance_inches();

    if (distance_inches < 2.1) {

        //go to STOPPED state
        set_left_motor_speed(0); //to be implemented by pwm?
        set_right_motor_speed(0);
        current_state = STOPPED;
        initialize_display_update_timer();

    } else {

        int motor_speed = MAX_MOTOR_SPEED * sqrt(distance_inches - 2);
        set_left_motor_speed(motor_speed); //to be implemented by pwm?
        set_right_motor_speed(motor_speed);

        //Arm timer again
        timer0_hw->alarm[2] = timer0_hw->timerawl + (FORWARD_INTERVAL * 1000000);

    }

}

/*-----------------------------------------------------------------------------------
Functions for STOPPED state
-----------------------------------------------------------------------------------*/

void initialize_display_update_timer() {

    //Enable interrupt for timer0 alarm3
    timer0_hw->inte |= 8;

    //Set handler for interrupt to display_update_timer_handler
    irq_set_exclusive_handler(TIMER0_IRQ_3, display_update_timer_handler);

    //Enable IRQ TIMER0_IRQ_3
    irq_set_enabled(TIMER0_IRQ_3, 1);

    //Set TIMER0 to fire alarm 3 after DISPLAY_REFRESH_INTERVAL seconds
    timer0_hw->alarm[3] = timer0_hw->timerawl + (DISPLAY_REFRESH_INTERVAL * 1000000);

}

void display_update_timer_handler() {

    //Acknowledge the interrupt
    timer0_hw->intr |= 8ul;

    //Update display
    //display_update(); //NEED PARAMETER FOR THIS?

}

/*-----------------------------------------------------------------------------------
Other functions
-----------------------------------------------------------------------------------*/

int get_state() {

    return current_state;

}
