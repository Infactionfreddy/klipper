#ifndef __ESP32_TIMER_H
#define __ESP32_TIMER_H

#include <stdint.h>

// Timer hardware functions
uint32_t timer_read_time(void);
void timer_periodic(void);
void timer_kick(void);
void timer_init(void);

#endif // __ESP32_TIMER_H
