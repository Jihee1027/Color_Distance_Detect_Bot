#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

void init_distance_gpio();
void send_pulse();
void pulse_end();
void init_echo_gpio_irq();
void echo_gpio_isr();
double get_distance_inches();

#endif