// ESP32 PWM support mit ESP-IDF ledc driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h"
#include "command.h"
#include "sched.h"

#include "driver/ledc.h"

#define MAX_PWM_CHANNELS 16

struct gpio_pwm {
    ledc_channel_t channel;
    ledc_timer_t timer;
    uint32_t max_value;
};

// Resource tracking
static uint8_t timer_used[4] = {0, 0, 0, 0};
static uint8_t channel_used[MAX_PWM_CHANNELS] = {0};

struct gpio_pwm gpio_pwm_setup(uint32_t pin, uint32_t cycle_time, uint8_t val)
{
    // Find free timer
    int timer_idx = -1;
    for (int i = 0; i < 4; i++) {
        if (!timer_used[i]) {
            timer_idx = i;
            break;
        }
    }
    if (timer_idx < 0)
        shutdown("No free PWM timer");
    
    // Find free channel
    int channel_idx = -1;
    for (int i = 0; i < MAX_PWM_CHANNELS; i++) {
        if (!channel_used[i]) {
            channel_idx = i;
            break;
        }
    }
    if (channel_idx < 0)
        shutdown("No free PWM channel");
    
    // Frequency aus cycle_time berechnen (cycle_time in ticks @ 1MHz)
    uint32_t freq = 1000000 / cycle_time;
    
    // Timer konfigurieren
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = timer_idx,
        .freq_hz = freq,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));
    timer_used[timer_idx] = 1;
    
    // Channel konfigurieren
    ledc_channel_config_t chan_conf = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel_idx,
        .timer_sel = timer_idx,
        .duty = val,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&chan_conf));
    channel_used[channel_idx] = 1;
    
    struct gpio_pwm pwm = {
        .channel = channel_idx,
        .timer = timer_idx,
        .max_value = (1 << 13) - 1,  // 13-bit resolution
    };
    return pwm;
}

void gpio_pwm_write(struct gpio_pwm pwm, uint32_t val)
{
    irqstatus_t flags = irq_save();
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm.channel, val));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm.channel));
    irq_restore(flags);
}
