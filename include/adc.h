#ifndef ADC_H
#define ADC_H

#include <stdlib.h>

void init_adc();
void init_adc_freerun();
uint16_t get_adc_value();

#endif