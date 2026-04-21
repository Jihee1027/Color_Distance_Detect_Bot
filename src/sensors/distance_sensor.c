#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "io_bank0.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIG_GPIO 12
#define ECHO_GPIO 13

//volatile because modified by ISRs
//unsigned to prevent overflow
volatile uint32_t echo_start_time;
volatile uint32_t echo_end_time;
volatile uint32_t echo_total_time = 25000;

//tracks whether currently receiving an echo
volatile int mid_echo = 0;

void init_distance_gpio();
void send_pulse();
void pulse_end();
void init_echo_gpio_irq();
void echo_gpio_isr();
double get_distance_inches();

/*-----------------------------------------------------------------------------------
Distance sensor code 
-----------------------------------------------------------------------------------*/

void init_distance_gpio() {

    gpio_init(TRIG_GPIO);
    gpio_init(ECHO_GPIO);

    gpio_set_dir(TRIG_GPIO, 1);
    gpio_set_dir(ECHO_GPIO, 0);

    gpio_put(TRIG_GPIO, 0);

}

//Send a 10us pulse 
void send_pulse() { 
    
    //Enable interrupt for timer0 alarm1 
    timer0_hw->inte |= 2; 

    //Set handler for interrupt to pulse_end 
    irq_set_exclusive_handler(TIMER0_IRQ_1, pulse_end); 

    //Enable IRQ TIMER0_IRQ_1 
    irq_set_enabled(TIMER0_IRQ_1, 1); 

    //Set TIMER0 to fire alarm 1 after 10us 
    timer0_hw->alarm[1] = timer0_hw->timerawl + 10; 

    //Start the pulse 
    gpio_put(TRIG_GPIO, 1); 
} 

void pulse_end() { 

    //Acknowledge the interrupt 
    timer0_hw->intr |= 2ul; 

    //disable previous timer 
    timer0_hw->inte &= ~2; 

    //End the pulse 
    gpio_put(TRIG_GPIO, 0); 

}

//Set up an interrupt where if ECHO goes high, note current time
//Set up an interrupt where if ECHO goes low, note current time again
void init_echo_gpio_irq() {

    //direct events from ECHO_GPIO to echo_gpio_isr
    gpio_add_raw_irq_handler_masked(1ul << ECHO_GPIO, echo_gpio_isr);

    //enable GPIO IRQ and BANK0 IRQ interrupt for both rising and falling edge
    gpio_set_irq_enabled(ECHO_GPIO, GPIO_IRQ_EDGE_RISE, 1);
    gpio_set_irq_enabled(ECHO_GPIO, GPIO_IRQ_EDGE_FALL, 1);
    irq_set_enabled(IO_IRQ_BANK0, 1);

}

void echo_gpio_isr() {

    int event_mask = gpio_get_irq_event_mask(ECHO_GPIO);

    if (((event_mask & GPIO_IRQ_EDGE_RISE) == GPIO_IRQ_EDGE_RISE) && !mid_echo) {

        //acknowledge
        gpio_acknowledge_irq(ECHO_GPIO, GPIO_IRQ_EDGE_RISE);

        echo_start_time = time_us_32();

        mid_echo = 1;

    }
    if (((event_mask & GPIO_IRQ_EDGE_FALL) == GPIO_IRQ_EDGE_FALL) && mid_echo) {

        //acknowledge
        gpio_acknowledge_irq(ECHO_GPIO, GPIO_IRQ_EDGE_FALL);

        echo_end_time = time_us_32();

        //(casting to prevent problems if timer wraps around)
        echo_total_time = (uint32_t)(echo_end_time - echo_start_time);

        mid_echo = 0;
    }

}

//will be called every 100ms no matter what. meets 60ms minimum spacing?
double get_distance_inches() {

    //timeout detection
    if (mid_echo && (time_us_32() - echo_start_time) > 25000) {
        mid_echo = 0;
        printf("timeout\n");
        return 170;
    }

    //Divide by 148 to get inches as instructed in datasheet
    double result = echo_total_time / 148.0;
    //printf("distance: %.2lf inches\n", result);
    return result;

}

/*
Consider having a mode where if it's close enough it switches to the I2C thing,
just for the sake of meeting the proposal objective
*/