// ESP32 GPIO support mit ESP-IDF GPIO driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h"
#include "command.h"
#include "sched.h"

#include "driver/gpio.h"
#include "hal/gpio_hal.h"

struct gpio_out {
    gpio_num_t pin;
    uint8_t inverted;
};

struct gpio_in {
    gpio_num_t pin;
    uint8_t inverted;
};

// GPIO Out initialisieren
struct gpio_out gpio_out_setup(uint32_t pin, uint32_t val)
{
    if (pin >= GPIO_NUM_MAX)
        shutdown("Invalid GPIO pin");
    
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    gpio_set_level(pin, val);
    
    struct gpio_out g = { .pin = pin, .inverted = 0 };
    return g;
}

void gpio_out_reset(struct gpio_out g, uint32_t val)
{
    irq_disable();
    gpio_set_level(g.pin, g.inverted ? !val : val);
    irq_enable();
}

void gpio_out_toggle_noirq(struct gpio_out g)
{
    uint32_t level = gpio_get_level(g.pin);
    gpio_set_level(g.pin, !level);
}

void gpio_out_toggle(struct gpio_out g)
{
    irq_disable();
    gpio_out_toggle_noirq(g);
    irq_enable();
}

void gpio_out_write(struct gpio_out g, uint32_t val)
{
    gpio_set_level(g.pin, g.inverted ? !val : val);
}

// GPIO In initialisieren
struct gpio_in gpio_in_setup(uint32_t pin, int32_t pull_up)
{
    if (pin >= GPIO_NUM_MAX)
        shutdown("Invalid GPIO pin");
    
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = pull_up > 0 ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pull_up < 0 ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    struct gpio_in g = { .pin = pin, .inverted = 0 };
    return g;
}

void gpio_in_reset(struct gpio_in g, int32_t pull_up)
{
    irq_disable();
    gpio_set_pull_mode(g.pin, pull_up > 0 ? GPIO_PULLUP_ONLY : 
                              (pull_up < 0 ? GPIO_PULLDOWN_ONLY : GPIO_FLOATING));
    irq_enable();
}

uint8_t gpio_in_read(struct gpio_in g)
{
    uint32_t level = gpio_get_level(g.pin);
    return g.inverted ? !level : level;
}
