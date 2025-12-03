// Serial port support for ESP32
// Copyright (C) 2024 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_SERIAL_PORT
#include "io.h" // readb
#include "irq.h" // irq_save
#include "command.h" // DECL_CONSTANT_STR
#include "internal.h" // uart_*
#include "sched.h" // DECL_INIT
#include <stdarg.h>

// IRAM attribute for interrupt handlers
#define IRAM_ATTR __attribute__((section(".iram1")))

// Forward declarations for console functions
struct command_encoder;
void console_sendf(const struct command_encoder *ce, va_list args);
void *console_receive_buffer(void);

// UART register offsets
#define UART_FIFO_REG(i)      (0x3FF40000 + (i) * 0x10000 + 0x00)
#define UART_INT_RAW_REG(i)   (0x3FF40000 + (i) * 0x10000 + 0x04)
#define UART_INT_ST_REG(i)    (0x3FF40000 + (i) * 0x10000 + 0x08)
#define UART_INT_ENA_REG(i)   (0x3FF40000 + (i) * 0x10000 + 0x0C)
#define UART_INT_CLR_REG(i)   (0x3FF40000 + (i) * 0x10000 + 0x10)
#define UART_CLKDIV_REG(i)    (0x3FF40000 + (i) * 0x10000 + 0x14)
#define UART_STATUS_REG(i)    (0x3FF40000 + (i) * 0x10000 + 0x1C)
#define UART_CONF0_REG(i)     (0x3FF40000 + (i) * 0x10000 + 0x20)
#define UART_CONF1_REG(i)     (0x3FF40000 + (i) * 0x10000 + 0x24)

// Interrupt bits
#define UART_RXFIFO_FULL_INT_ENA (1 << 0)
#define UART_TXFIFO_EMPTY_INT_ENA (1 << 1)
#define UART_RXFIFO_FULL_INT_CLR (1 << 0)
#define UART_TXFIFO_EMPTY_INT_CLR (1 << 1)

// Status register bits
#define UART_RXFIFO_CNT_M 0xFF
#define UART_TXFIFO_CNT_M (0xFF << 16)

#ifndef CONFIG_SERIAL_PORT
#define CONFIG_SERIAL_PORT 0
#endif

#define UART_NUM CONFIG_SERIAL_PORT

DECL_CONSTANT_STR("RESERVE_PINS_serial", "GPIO1,GPIO3");

// Forward declarations
void console_sendf(const struct command_encoder *ce, va_list args);
void *console_receive_buffer(void);

// Initialize UART
void
uart_init(uint32_t baud)
{
    // Calculate clock divisor
    // ESP32 UART clock is typically APB_CLK (80MHz)
    uint32_t apb_clk = 80000000;
    uint32_t clkdiv = (apb_clk << 4) / baud;
    
    // Set baud rate
    volatile uint32_t *clkdiv_reg = (volatile uint32_t*)UART_CLKDIV_REG(UART_NUM);
    *clkdiv_reg = clkdiv;
    
    // Configure: 8N1
    volatile uint32_t *conf0_reg = (volatile uint32_t*)UART_CONF0_REG(UART_NUM);
    *conf0_reg = (0x3 << 0);  // 8 data bits
    
    // Configure FIFO thresholds
    volatile uint32_t *conf1_reg = (volatile uint32_t*)UART_CONF1_REG(UART_NUM);
    *conf1_reg = (1 << 0) | (120 << 8);  // RX threshold=1, TX threshold=120
    
    // Enable RX interrupt
    volatile uint32_t *int_ena_reg = (volatile uint32_t*)UART_INT_ENA_REG(UART_NUM);
    *int_ena_reg = UART_RXFIFO_FULL_INT_ENA;
}
DECL_INIT(uart_init);

// Enable TX interrupt
void
uart_enable_tx_irq(void)
{
    volatile uint32_t *int_ena_reg = (volatile uint32_t*)UART_INT_ENA_REG(UART_NUM);
    *int_ena_reg |= UART_TXFIFO_EMPTY_INT_ENA;
}

// Read a byte from UART (blocking)
uint8_t
uart_read_byte(void)
{
    // Wait for data
    volatile uint32_t *status_reg = (volatile uint32_t*)UART_STATUS_REG(UART_NUM);
    while ((*status_reg & UART_RXFIFO_CNT_M) == 0)
        ;
    
    // Read byte
    volatile uint32_t *fifo_reg = (volatile uint32_t*)UART_FIFO_REG(UART_NUM);
    return (uint8_t)*fifo_reg;
}

// Send a byte over UART
void
uart_send_byte(uint8_t data)
{
    // Wait for space in TX FIFO
    volatile uint32_t *status_reg = (volatile uint32_t*)UART_STATUS_REG(UART_NUM);
    while (((*status_reg & UART_TXFIFO_CNT_M) >> 16) >= 127)
        ;
    
    // Write byte
    volatile uint32_t *fifo_reg = (volatile uint32_t*)UART_FIFO_REG(UART_NUM);
    *fifo_reg = data;
}

// Flush TX buffer
void
uart_tx_flush(void)
{
    volatile uint32_t *status_reg = (volatile uint32_t*)UART_STATUS_REG(UART_NUM);
    while (((*status_reg & UART_TXFIFO_CNT_M) >> 16) != 0)
        ;
}

// UART interrupt handler
void IRAM_ATTR
uart_isr_handler(void *arg)
{
    volatile uint32_t *int_st_reg = (volatile uint32_t*)UART_INT_ST_REG(UART_NUM);
    volatile uint32_t *int_clr_reg = (volatile uint32_t*)UART_INT_CLR_REG(UART_NUM);
    uint32_t status = *int_st_reg;
    
    if (status & UART_RXFIFO_FULL_INT_ENA) {
        // Handle RX
        *int_clr_reg = UART_RXFIFO_FULL_INT_CLR;
    }
    
    if (status & UART_TXFIFO_EMPTY_INT_ENA) {
        // Handle TX
        *int_clr_reg = UART_TXFIFO_EMPTY_INT_CLR;
    }
}

// Stub implementations for console functions
void
console_sendf(const struct command_encoder *ce, va_list args)
{
    // Simple implementation: encode and send directly
    uint8_t buf[96];
    uint_fast8_t msglen = command_encode_and_frame(buf, ce, args);
    
    for (uint_fast8_t i = 0; i < msglen; i++) {
        uart_send_byte(buf[i]);
    }
}

void *
console_receive_buffer(void)
{
    static uint8_t receive_buf[192];
    return receive_buf;
}
