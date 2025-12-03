// Main code entry point for ESP32 mit FreeRTOS
// Copyright (C) 2025 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_*
#include "irq.h" // irq_disable
#include "internal.h"
#include "command.h" // DECL_CONSTANT
#include "sched.h" // sched_main

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

static const char *TAG = "klipper";

// Memory allocation for Klipper (ESP-IDF heap)
void *dynmem_start(void) {
    // Reserviere 64KB für Klipper
    void *mem = heap_caps_malloc(64 * 1024, MALLOC_CAP_8BIT);
    if (!mem) {
        ESP_LOGE(TAG, "Failed to allocate Klipper memory");
        return NULL;
    }
    return mem;
}

void *dynmem_end(void) {
    // 64KB nach start
    return (uint8_t*)dynmem_start() + 64 * 1024;
}

DECL_CONSTANT_STR("MCU", CONFIG_MCU);
DECL_CONSTANT("CLOCK_FREQ", CONFIG_CLOCK_FREQ);

// Klipper Main Task
static void klipper_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting Klipper scheduler");
    
    // Initialize board peripherals
    console_init_board();
    timer_init_board();
    
    // Run the main scheduler (never returns)
    sched_main();
    
    // Should never reach here
    vTaskDelete(NULL);
}

// Application entry point (called by ESP-IDF)
void app_main(void)
{
    ESP_LOGI(TAG, "Klipper ESP32 starting...");
    
    // Create Klipper task on core 1 with high priority
    xTaskCreatePinnedToCore(
        klipper_task,
        "klipper",
        8192,  // Stack size
        NULL,
        10,    // Priority
        NULL,
        1      // Core 1
    );
    
    // Main task can be deleted, Klipper task runs independently
}
