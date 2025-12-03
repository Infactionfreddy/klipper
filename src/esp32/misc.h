#ifndef __ESP32_BOARD_MISC_H
#define __ESP32_BOARD_MISC_H

#include <stdint.h>
#include <stdarg.h>
#include "autoconf.h" // CONFIG_CLOCK_FREQ

// Timer functions - definiert in generic/timer_irq.c oder timer.c
uint32_t timer_from_us(uint32_t us);
uint8_t timer_is_before(uint32_t time1, uint32_t time2);
uint32_t timer_read_time(void);
void timer_kick(void);

// Memory management wird in main.c definiert
void *dynmem_start(void);
void *dynmem_end(void);

// CRC function
uint16_t crc16_ccitt(uint8_t *buf, uint_fast8_t len);

// Console functions - definiert in console.c oder serial_irq.c
struct command_encoder;
void console_sendf(const struct command_encoder *ce, va_list args);
void *console_receive_buffer(void);

// Bootloader (not used on ESP32)
static inline void bootloader_request(void) {}

// PROGMEM-like macros für ESP32 (kein PROGMEM)
#define memcpy_P memcpy

#endif // __ESP32_BOARD_MISC_H
