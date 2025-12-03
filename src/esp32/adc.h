#ifndef __ESP32_ADC_H
#define __ESP32_ADC_H

#include <stdint.h>

struct gpio_adc {
    uint8_t chan;
};

struct gpio_adc gpio_adc_setup(uint8_t pin);
uint32_t gpio_adc_sample(struct gpio_adc g);
uint16_t gpio_adc_read(struct gpio_adc g);

#endif // __ESP32_ADC_H
