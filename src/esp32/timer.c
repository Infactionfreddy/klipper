// ESP32 timer support mit ESP-IDF gptimer driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h"
#include "internal.h"
#include "command.h"
#include "sched.h"

#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_attr.h"

// FreeRTOS spinlock für timer dispatch
static portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;

// GPTimer handle
static gptimer_handle_t gptimer = NULL;

// Timer Konfiguration
#define TIMER_RESOLUTION_HZ 1000000  // 1MHz resolution

// IRAM_ATTR: Interrupt handler muss in IRAM liegen für schnellen Zugriff
static bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, 
                                         const gptimer_alarm_event_data_t *edata,
                                         void *user_ctx)
{
    portENTER_CRITICAL_ISR(&timer_mux);
    uint32_t next = sched_timer_dispatch();
    portEXIT_CRITICAL_ISR(&timer_mux);
    
    // Set next alarm
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + next,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    
    return false;  // Don't yield from ISR
}

void timer_init_board(void)
{
    // GPTimer Konfiguration
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
    };
    
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    
    // Event callbacks registrieren
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_isr_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    
    // Alarm konfigurieren
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000,  // 1ms initial
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    
    // Timer starten
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

// Timer lesen (in Mikrosekunden)
uint32_t timer_read_time(void)
{
    uint64_t count;
    gptimer_get_raw_count(gptimer, &count);
    return (uint32_t)count;
}

// Timer aus Frequenz berechnen
uint32_t timer_from_us(uint32_t us)
{
    return us;  // Bereits 1MHz, keine Umrechnung nötig
}

uint8_t timer_is_before(uint32_t time1, uint32_t time2)
{
    return (int32_t)(time1 - time2) < 0;
}

// Timer "kick" - stellt sicher, dass der Timer läuft
void timer_kick(void)
{
    // ESP32 gptimer läuft kontinuierlich, kein "kick" nötig
    // Diese Funktion existiert nur für API-Kompatibilität
}

void timer_periodic(void)
{
    // Wird von generic/timer_irq.c aufgerufen
}

uint32_t timer_dispatch_many(void)
{
    // Diese Funktion wird vom generic/timer_irq.c aufgerufen
    // Sie soll sched_timer_dispatch() aufrufen und den Wert zurückgeben
    return sched_timer_dispatch();
}
