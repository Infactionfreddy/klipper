#ifndef __ESP32_BOARD_INTERNAL_H
#define __ESP32_BOARD_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>

// Timer functions
void timer_init_board(void);
uint32_t timer_dispatch_many(void);

// Console functions
void console_init_board(void);
void report_errno(char *where, int32_t rc);
void set_non_blocking(int fd);

// IRQ functions
void irq_disable_board(void);
void irq_enable_board(void);
uint32_t irq_save_board(void);
void irq_restore_board(uint32_t flag);
void irq_wait_board(void);
void irq_poll_board(void);

#endif // __ESP32_BOARD_INTERNAL_H
