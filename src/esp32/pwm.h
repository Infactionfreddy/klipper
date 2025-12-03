#ifndef __ESP32_PWM_H
#define __ESP32_PWM_H

#include <stdint.h>

struct gpio_pwm {
    uint8_t channel;
};

struct gpio_pwm gpio_pwm_setup(uint8_t pin, uint32_t cycle_time, uint8_t val);
void gpio_pwm_write(struct gpio_pwm g, uint32_t val);

#endif // __ESP32_PWM_H
