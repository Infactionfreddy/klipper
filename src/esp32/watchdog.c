// Watchdog support for ESP32
// Copyright (C) 2024 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_*
#include "irq.h" // irq_disable
#include "internal.h" // watchdog_*
#include "sched.h" // DECL_TASK

// Task WDT (Task Watchdog Timer) registers
#define TIMG_WDTCONFIG0_REG(i)    (0x3FF5F000 + (i)*0x1000 + 0x48)
#define TIMG_WDTCONFIG1_REG(i)    (0x3FF5F000 + (i)*0x1000 + 0x4C)
#define TIMG_WDTCONFIG2_REG(i)    (0x3FF5F000 + (i)*0x1000 + 0x50)
#define TIMG_WDTCONFIG3_REG(i)    (0x3FF5F000 + (i)*0x1000 + 0x54)
#define TIMG_WDTCONFIG4_REG(i)    (0x3FF5F000 + (i)*0x1000 + 0x58)
#define TIMG_WDTFEED_REG(i)       (0x3FF5F000 + (i)*0x1000 + 0x60)
#define TIMG_WDTWPROTECT_REG(i)   (0x3FF5F000 + (i)*0x1000 + 0x64)

#define TIMG_WDT_WKEY_VALUE       0x50D83AA1
#define TIMG_WDT_STG_SEL_OFF      0
#define TIMG_WDT_STG_SEL_INT      1
#define TIMG_WDT_STG_SEL_RESET_CPU 2
#define TIMG_WDT_STG_SEL_RESET_SYSTEM 3

#define WDT_TIMER_GROUP 0

// Watchdog feed (reset the watchdog timer)
void
watchdog_reset(void)
{
    volatile uint32_t *wdt_wprotect = (volatile uint32_t*)TIMG_WDTWPROTECT_REG(WDT_TIMER_GROUP);
    volatile uint32_t *wdt_feed = (volatile uint32_t*)TIMG_WDTFEED_REG(WDT_TIMER_GROUP);
    
    // Unlock watchdog
    *wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    
    // Feed the watchdog
    *wdt_feed = 1;
    
    // Lock watchdog
    *wdt_wprotect = 0;
}

// Task to periodically reset watchdog
void
watchdog_task(void)
{
    watchdog_reset();
}
DECL_TASK(watchdog_task);

// Initialize watchdog timer
void
watchdog_init(void)
{
    volatile uint32_t *wdt_wprotect = (volatile uint32_t*)TIMG_WDTWPROTECT_REG(WDT_TIMER_GROUP);
    volatile uint32_t *wdt_config0 = (volatile uint32_t*)TIMG_WDTCONFIG0_REG(WDT_TIMER_GROUP);
    volatile uint32_t *wdt_config1 = (volatile uint32_t*)TIMG_WDTCONFIG1_REG(WDT_TIMER_GROUP);
    volatile uint32_t *wdt_config2 = (volatile uint32_t*)TIMG_WDTCONFIG2_REG(WDT_TIMER_GROUP);
    
    // Unlock watchdog
    *wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    
    // Configure watchdog
    // Stage 0: Interrupt after ~2 seconds
    // Stage 1: System reset after ~4 seconds total
    *wdt_config0 = (TIMG_WDT_STG_SEL_INT << 0) |           // Stage 0: Interrupt
                   (TIMG_WDT_STG_SEL_RESET_SYSTEM << 2) |  // Stage 1: Reset system
                   (1 << 12) |                              // Enable CPU0 reset
                   (1 << 13) |                              // Enable CPU1 reset (if applicable)
                   (1 << 14) |                              // Enable flash boot protection
                   (1 << 18);                               // Enable watchdog
    
    // Set timeout: ~2 seconds (prescaler * timeout / 80MHz)
    // Using prescaler of 40000 -> 2kHz base clock
    // 2 seconds = 4000 ticks
    *wdt_config1 = 40000;  // Prescaler
    *wdt_config2 = 4000;   // Stage 0 timeout (interrupt)
    
    // Stage 1 timeout (another 2 seconds after stage 0)
    volatile uint32_t *wdt_config3 = (volatile uint32_t*)TIMG_WDTCONFIG3_REG(WDT_TIMER_GROUP);
    *wdt_config3 = 4000;
    
    // Feed watchdog initially
    watchdog_reset();
    
    // Lock watchdog
    *wdt_wprotect = 0;
}
DECL_INIT(watchdog_init);
