#ifndef __ESP32_BOARD_IRQ_H
#define __ESP32_BOARD_IRQ_H

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

// FreeRTOS-based IRQ wrappers für ESP32
typedef portMUX_TYPE irqstatus_t;

#define irq_disable() portDISABLE_INTERRUPTS()
#define irq_enable() portENABLE_INTERRUPTS()

static inline irqstatus_t irq_save(void) {
    irqstatus_t state = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&state);
    return state;
}

static inline void irq_restore(irqstatus_t flag) {
    portEXIT_CRITICAL(&flag);
}

void irq_wait(void);
void irq_poll(void);

#endif // __ESP32_BOARD_IRQ_H
