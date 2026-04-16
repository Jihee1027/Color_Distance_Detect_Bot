#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define BATTERY_ADC_PIN 26
#define VOLTAGE_DIVIDER_RATIO 2.0f
#define ADC_REF_VOLTAGE 3.3f
#define ADC_SAMPLES 32

static float battery_voltage = 0.0f;

void battery_init(void)
{
    adc_init();
    
    uint adc_channel = BATTERY_ADC_PIN - 26;   //GPIO26→ADC0, 27→ADC1, 28→ADC2
    
    adc_gpio_init(BATTERY_ADC_PIN);
    adc_select_input(adc_channel);

    printf("Battery monitor initialized (Okcell ICR18650 2200mAh)\n");
    
    printf("Using GPIO%d (ADC%d) with divider ratio %.1f\n", 
           BATTERY_ADC_PIN, adc_channel, VOLTAGE_DIVIDER_RATIO);
}

float battery_read_voltage(void)
{
    uint32_t sum = 0;

    // Average multiple readings to reduce noise
    for (int i = 0; i < ADC_SAMPLES; i++)
    {
        sum += adc_read();
        sleep_ms(2);
    }

    float average_raw = (float)sum / ADC_SAMPLES;

    // Voltage at the ADC input pin
    float voltage_at_pin = (average_raw / 4095.0f) * ADC_REF_VOLTAGE;

    // Apply voltage divider to get real battery voltage
    battery_voltage = voltage_at_pin * VOLTAGE_DIVIDER_RATIO;

    return battery_voltage;
}

float battery_get_voltage(void)
{
    return battery_voltage;
}

void battery_print_status(void)
{
    float voltage = battery_read_voltage();

    float percentage = 0.0f;

    if (voltage >= 4.20f)      percentage = 100.0f;
    else if (voltage >= 4.00f) percentage = 90.0f + (voltage - 4.00f) * 50.0f;   // steep near full
    else if (voltage >= 3.70f) percentage = 40.0f + (voltage - 3.70f) * 100.0f;  // around nominal
    else if (voltage >= 3.40f) percentage = (voltage - 3.40f) * 133.0f;          // lower knee
    else if (voltage >= 3.00f) percentage = (voltage - 3.00f) * 30.0f;           // very low
    else                       percentage = 0.0f;

    if(percentage > 100.0f)
    {
        percentage = 100.0f;
    }

    if(percentage < 0.0f)  
    {
        percentage = 0.0f;
    }

    printf("Battery: %.3f V   (~%.0f%%)\n", voltage, percentage);

    //warnings
    if(voltage < 3.20f)
    {
        printf("*** CRITICAL LOW BATTERY ***\n");
    }

    else if(voltage < 3.40f)
    {
        printf("*** LOW BATTERY WARNING ***\n");
    }

    else if(voltage > 4.25f)
    {
        printf("*** WARNING: Voltage too high ***\n");
    }

}

void battery_monitor_task(void)
{
    static absolute_time_t last_time = 0;

    if (absolute_time_diff_us(last_time, get_absolute_time()) > 5000000) // every 5 seconds
    {
        battery_print_status();
        last_time = get_absolute_time();
    }

}