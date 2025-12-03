#ifndef __ESP32_BOARD_TIMER_IRQ_H
#define __ESP32_BOARD_TIMER_IRQ_H

#include <stdint.h>

// Timer IRQ interface für ESP32
// timer_dispatch_many() wird von generic/timer_irq.c aufgerufen
uint32_t timer_dispatch_many(void);

#endif // __ESP32_BOARD_TIMER_IRQ_H
