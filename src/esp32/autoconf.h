// autoconf.h für ESP32 - ESP-IDF integration
// Copyright (C) 2025  Your Name <your.email@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#ifndef __ESP32_AUTOCONF_H
#define __ESP32_AUTOCONF_H

// Include ESP-IDF sdkconfig
#include "sdkconfig.h"

// Klipper MCU configuration
#define CONFIG_MCU "esp32"
#ifndef CONFIG_CLOCK_FREQ
#define CONFIG_CLOCK_FREQ 240000000UL
#endif
#ifndef CONFIG_SERIAL_BAUD
#define CONFIG_SERIAL_BAUD 250000
#endif

// Board configuration
#define CONFIG_BOARD_DIRECTORY "esp32"

// Enable features
#define CONFIG_HAVE_GPIO 1
#define CONFIG_HAVE_GPIO_ADC 1
#define CONFIG_HAVE_GPIO_SPI 1
#define CONFIG_HAVE_GPIO_I2C 1
#define CONFIG_HAVE_GPIO_HARD_PWM 1
#define CONFIG_HAVE_STEPPER_BOTH_EDGE 1
#define CONFIG_INLINE_STEPPER_HACK 0
#define CONFIG_HAVE_BOOTLOADER_REQUEST 0

// Platform flags
#define CONFIG_MACH_AVR 0

// Memory
#define CONFIG_RAM_START 0x3FFB0000
#define CONFIG_RAM_SIZE 0x50000

// Initial pin configuration (empty string = no initial pins)
#define CONFIG_INITIAL_PINS ""

// Stack
#define CONFIG_STACK_SIZE 4096

#endif // __ESP32_AUTOCONF_H
