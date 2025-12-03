// ESP32 ADC support mit ESP-IDF adc_oneshot driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h"
#include "command.h"
#include "sched.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

struct gpio_adc {
    adc_oneshot_unit_handle_t handle;
    adc_channel_t channel;
    adc_unit_t unit;
};

// ADC Units (max 2 auf ESP32)
static adc_oneshot_unit_handle_t adc_units[2] = {NULL, NULL};

// ADC Pin zu Unit/Channel Mapping (vereinfacht, siehe ESP32 datasheet)
static const struct {
    uint8_t gpio;
    adc_unit_t unit;
    adc_channel_t channel;
} adc_mapping[] = {
    {36, ADC_UNIT_1, ADC_CHANNEL_0}, // GPIO36
    {37, ADC_UNIT_1, ADC_CHANNEL_1}, // GPIO37
    {38, ADC_UNIT_1, ADC_CHANNEL_2}, // GPIO38
    {39, ADC_UNIT_1, ADC_CHANNEL_3}, // GPIO39
    {32, ADC_UNIT_1, ADC_CHANNEL_4}, // GPIO32
    {33, ADC_UNIT_1, ADC_CHANNEL_5}, // GPIO33
    {34, ADC_UNIT_1, ADC_CHANNEL_6}, // GPIO34
    {35, ADC_UNIT_1, ADC_CHANNEL_7}, // GPIO35
    {4,  ADC_UNIT_2, ADC_CHANNEL_0}, // GPIO4
    {0,  ADC_UNIT_2, ADC_CHANNEL_1}, // GPIO0
    {2,  ADC_UNIT_2, ADC_CHANNEL_2}, // GPIO2
    {15, ADC_UNIT_2, ADC_CHANNEL_3}, // GPIO15
    {13, ADC_UNIT_2, ADC_CHANNEL_4}, // GPIO13
    {12, ADC_UNIT_2, ADC_CHANNEL_5}, // GPIO12
    {14, ADC_UNIT_2, ADC_CHANNEL_6}, // GPIO14
    {27, ADC_UNIT_2, ADC_CHANNEL_7}, // GPIO27
    {25, ADC_UNIT_2, ADC_CHANNEL_8}, // GPIO25
    {26, ADC_UNIT_2, ADC_CHANNEL_9}, // GPIO26
};

static int find_adc_mapping(uint8_t pin, adc_unit_t *unit, adc_channel_t *channel)
{
    for (int i = 0; i < sizeof(adc_mapping)/sizeof(adc_mapping[0]); i++) {
        if (adc_mapping[i].gpio == pin) {
            *unit = adc_mapping[i].unit;
            *channel = adc_mapping[i].channel;
            return 0;
        }
    }
    return -1;
}

struct gpio_adc gpio_adc_setup(uint32_t pin)
{
    adc_unit_t unit;
    adc_channel_t channel;
    
    if (find_adc_mapping(pin, &unit, &channel) < 0)
        shutdown("Invalid ADC pin");
    
    // ADC Unit initialisieren (falls noch nicht)
    uint8_t unit_idx = (unit == ADC_UNIT_1) ? 0 : 1;
    if (adc_units[unit_idx] == NULL) {
        adc_oneshot_unit_init_cfg_t init_config = {
            .unit_id = unit,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_units[unit_idx]));
    }
    
    // Channel konfigurieren
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,      // 0-3.3V range
        .bitwidth = ADC_BITWIDTH_12,   // 12-bit resolution
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_units[unit_idx], channel, &config));
    
    struct gpio_adc adc = {
        .handle = adc_units[unit_idx],
        .channel = channel,
        .unit = unit,
    };
    return adc;
}

uint32_t gpio_adc_sample(struct gpio_adc adc)
{
    int raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc.handle, adc.channel, &raw));
    return (uint32_t)raw;
}

uint16_t gpio_adc_read(struct gpio_adc adc)
{
    return (uint16_t)gpio_adc_sample(adc);
}

void gpio_adc_cancel_sample(struct gpio_adc adc)
{
    // Oneshot ADC hat kein cancel
}
