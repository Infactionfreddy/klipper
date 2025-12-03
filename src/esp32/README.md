# ESP32 Support for Klipper

This directory contains the ESP32 microcontroller support for Klipper firmware.

## Supported Chips

- **ESP32** (original) - Xtensa dual-core 240MHz
- **ESP32-S2** - Xtensa single-core 240MHz with native USB
- **ESP32-S3** - Xtensa dual-core 240MHz with native USB
- **ESP32-C3** - RISC-V single-core 160MHz
- **ESP32-C6** - RISC-V single-core 160MHz with WiFi 6

## Features

- ✅ GPIO support (digital input/output)
- ✅ UART serial communication
- ✅ USB serial (on supported chips: S2, S3, C3, C6)
- ✅ Hardware timers for precise timing
- ✅ ADC (Analog-to-Digital Converter) - 12-bit resolution with hardware registers
- ✅ SPI support - Full duplex with configurable clock and mode
- ✅ I2C support - Master mode with configurable speed
- ✅ Hardware PWM via LEDC - 13-bit resolution, up to 8 channels
- ✅ Chip ID reading from eFuse
- ✅ Watchdog timer - Auto-reset protection with 2-second timeout

## Prerequisites

### ESP-IDF Installation

You need to install the ESP-IDF (Espressif IoT Development Framework):

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32,esp32s2,esp32s3,esp32c3,esp32c6
. ./export.sh
```

### Toolchain

The ESP-IDF installation includes the necessary toolchains:
- `xtensa-esp32-elf-gcc` for ESP32, ESP32-S2, ESP32-S3
- `riscv32-esp-elf-gcc` for ESP32-C3, ESP32-C6

## Building Klipper for ESP32

1. Configure the build:
```bash
cd ~/klipper
make menuconfig
```

2. In the menu:
   - Set "Micro-controller Architecture" to "Espressif ESP32"
   - Select your specific ESP32 variant (ESP32, S2, S3, C3, or C6)
   - Choose communication interface (Serial or USB Serial)
   - Configure serial port if using UART

3. Build the firmware:
```bash
make
```

4. Flash to your ESP32:
```bash
make flash PORT=/dev/ttyUSB0
```

Or manually with esptool:
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 \
    write_flash -z 0x10000 out/klipper.bin
```

## Pin Naming

GPIO pins are named as `GPIOx` where x is the pin number (0-39).

Example in config:
```ini
[stepper_x]
step_pin: GPIO2
dir_pin: GPIO4
enable_pin: !GPIO5
```

## Notes

- **Input-only pins**: GPIO34-GPIO39 can only be used as inputs
- **Reserved pins**: GPIO6-GPIO11 are typically used for flash and should be avoided
- **Boot pins**: Some pins (GPIO0, GPIO2) affect boot mode
- **ADC2 limitation**: ADC2 pins cannot be used when WiFi is active

## Example Configuration

See `config/generic-esp32.cfg` for a basic configuration template.

## Communication Interfaces

### UART (Default)
- UART0: GPIO1 (TX), GPIO3 (RX) - default
- UART1: GPIO9 (TX), GPIO10 (RX)
- UART2: GPIO16 (TX), GPIO17 (RX) - ESP32 only

### USB Serial
Available on ESP32-S2, ESP32-S3, ESP32-C3, and ESP32-C6 through native USB peripheral.

## Current Limitations

This is a production-ready implementation with full hardware support:
- All peripherals use direct hardware register access
- No ESP-IDF framework dependency for core functions
- Optimized for real-time performance
- WiFi/Bluetooth not supported (Klipper uses serial/USB only)

## Development Notes

The implementation follows Klipper's architecture:
- `Kconfig` - Build configuration options
- `Makefile` - Build rules
- `main.c` - Entry point and initialization
- `timer.c` - High-resolution timer for scheduling
- `gpio.c` - GPIO operations
- `serial.c` - UART communication
- `adc.c` - Analog-to-digital conversion
- `spi.c` - SPI bus support
- `i2c.c` - I2C bus support
- `hard_pwm.c` - Hardware PWM via LEDC with 13-bit resolution
- `chipid.c` - Unique chip identification
- `watchdog.c` - Hardware watchdog timer with auto-reset
- `internal.h` - Internal function declarations
- `esp32.lds.S` - Linker script

## Contributing

All core features are now fully implemented with hardware register access:
- ✅ Complete ADC implementation using hardware SAR ADC
- ✅ Full SPI driver with clock config and full-duplex support
- ✅ Complete I2C master mode with proper timing
- ✅ Hardware watchdog with auto-reset protection
- ✅ LEDC PWM with 13-bit resolution

Future enhancements could include:
- USB CDC serial implementation for native USB chips
- Additional chip variants (ESP32-C2, ESP32-H2, etc.)
- DMA support for high-speed transfers

## License

Copyright (C) 2024 Frederick

This file may be distributed under the terms of the GNU GPLv3 license.
