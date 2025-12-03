// ESP32 IRQ handling - FreeRTOS basiert
//
// Copyright (C) 2024  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h" // irqstatus_t
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Warten auf Interrupts - FreeRTOS Task Yield
void
irq_wait(void)
{
    // Kurz yielden, damit andere Tasks/ISRs laufen können
    // Im Polling-Mode (console_poll) warten wir nicht lange
    taskYIELD();
}

// Polling für Interrupts - für ESP32 ist das ein No-Op
// da FreeRTOS die ISRs automatisch verarbeitet
void
irq_poll(void)
{
    // Bei FreeRTOS wird alles über ISRs und Tasks gemacht
    // Kein explizites Polling nötig
}
