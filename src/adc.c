#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "io_bank0.h"
#include "hardware/gpio.h"

/*

Phone flashlight: 50 ohms
Normal room lighting: 1120 ohms
Lightly cover with hand: 6300 ohms

*/

#define ADC_GPIO 45

void init_adc() {

    //Configure ADC_GPIO as an input
    gpio_init(ADC_GPIO);
    gpio_set_dir(ADC_GPIO, 0);
    gpio_set_function(ADC_GPIO, GPIO_FUNC_NULL);

    //Disable pulls
    hw_write_masked(
            &pads_bank0_hw->io[ADC_GPIO],
            (bool_to_bit(false) << PADS_BANK0_GPIO0_PUE_LSB) | (bool_to_bit(false) << PADS_BANK0_GPIO0_PDE_LSB),
            PADS_BANK0_GPIO0_PUE_BITS | PADS_BANK0_GPIO0_PDE_BITS
    );

    //Set input enabled to false
    hw_clear_bits(&pads_bank0_hw->io[ADC_GPIO], PADS_BANK0_GPIO0_IE_BITS);

    //Enable ADC. Bit 0 of adc_hw->cs is EN
    adc_hw->cs |= 1ul;

    //Set the ADC to use channel 5. Bits 12-15 of adc_hw->cs are AINSEL
    adc_hw->cs |= 5ul << 12; 

}

uint16_t read_adc() 
{
    //sets the ADC_CS_START_ONCE bit to begin a single conversation
    return adc_read(); //waits for conversion to complete and returns the result
    
}

void init_adc_freerun() {
    
    init_adc();

    //Start a free-running conversion. Bit 3 of adc_hw->cs is START_MANY
    adc_hw->cs &= ~(1ul << 2); //disable one-shot
    adc_hw->cs |= 1ul << 3;
    //Producing a new sample takes 96 cycles so at 48 MHz it can produce 500,000 samples/second

}

uint16_t get_adc_value() {

    return adc_hw-> result;

}
