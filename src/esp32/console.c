// ESP32 console support mit ESP-IDF VFS (Virtual File System)
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"
#include "irq.h"
#include "command.h"
#include "sched.h"
#include "generic/serial_irq.h" // serial_rx_byte

#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "driver/usb_serial_jtag.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "sdkconfig.h"

// Forward declare from serial_irq.c
extern uint8_t receive_buf[];

// Serial TX IRQ stub - ESP32 VFS nutzt keinen TX IRQ
// Die Funktion existiert nur für API-Kompatibilität mit serial_irq.c
void serial_enable_tx_irq(void)
{
    // Bei VFS-basiertem UART gibt es keinen TX-Interrupt
    // Das Schreiben blockiert/puffert automatisch
}

// Console mode: UART oder USB Serial JTAG
#define CONSOLE_UART 0
#define CONSOLE_USB_SERIAL_JTAG 1

#ifndef CONFIG_ESP_CONSOLE_MODE
#define CONFIG_ESP_CONSOLE_MODE CONSOLE_UART
#endif

// UART Configuration
#define UART_NUM UART_NUM_0
#define UART_TX_PIN 1
#define UART_RX_PIN 3
#define UART_BAUD_RATE 250000
#define UART_BUF_SIZE 1024

// Error reporting helper
void report_errno(char *where, int32_t rc)
{
    sendf("!! %s rc=%d errno=%d", where, (int)rc, errno);
}

// Set non-blocking mode für file descriptor
void set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        report_errno("fcntl F_GETFL", flags);
        return;
    }
    int rc = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (rc == -1) {
        report_errno("fcntl F_SETFL", rc);
    }
}

void console_init_board(void)
{
#if CONFIG_ESP_CONSOLE_MODE == CONSOLE_UART
    // UART Console über VFS
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, 
                                  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // VFS für UART verwenden
    uart_vfs_dev_use_driver(UART_NUM);
    
#elif CONFIG_ESP_CONSOLE_MODE == CONSOLE_USB_SERIAL_JTAG
    // USB Serial JTAG Console über VFS
    usb_serial_jtag_driver_config_t usb_config = {
        .rx_buffer_size = UART_BUF_SIZE,
        .tx_buffer_size = UART_BUF_SIZE,
    };
    
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_config));
    
    // VFS für USB Serial JTAG verwenden
    esp_vfs_usb_serial_jtag_use_driver();
#endif

    // Set stdin/stdout non-blocking
    set_non_blocking(STDIN_FILENO);
    set_non_blocking(STDOUT_FILENO);
}

// Console Input lesen (non-blocking) - static um Namenskonflikte zu vermeiden
static int console_read(char *buf, int len)
{
    int rc = read(STDIN_FILENO, buf, len);
    if (rc < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        report_errno("console_read", rc);
    }
    return rc > 0 ? rc : 0;
}

// Console Output schreiben - static um Namenskonflikte zu vermeiden
static int console_write(const char *buf, int len)
{
    int rc = write(STDOUT_FILENO, buf, len);
    if (rc < 0) {
        report_errno("console_write", rc);
        return 0;
    }
    return rc;
}

// Poll console für neue Daten
void console_poll(void)
{
    char buf[128];
    int len = console_read(buf, sizeof(buf));
    if (len > 0) {
        // Process incoming data
        for (int i = 0; i < len; i++) {
            serial_rx_byte(buf[i]);
        }
    }
}

// Return pointer to receive buffer for command.c
void *console_receive_buffer(void)
{
    return receive_buf;
}
