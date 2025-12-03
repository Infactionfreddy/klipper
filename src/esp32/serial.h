#ifndef __ESP32_SERIAL_H
#define __ESP32_SERIAL_H

#include <stdint.h>

// UART hardware functions
void uart_init(uint32_t baud);
void uart_send_byte(uint8_t data);
uint8_t uart_recv_byte(void);
int uart_is_ready(void);

#endif // __ESP32_SERIAL_H
