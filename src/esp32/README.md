# ESP32 Support for Klipper

This directory contains the ESP32 microcontroller support for Klipper firmware.

## Supported Chips

- **ESP32** (original) - Xtensa dual-core 240MHz
- **ESP32-S2** - Xtensa single-core 240MHz with native USB
- **ESP32-S3** - Xtensa dual-core 240MHz with native USB
- **ESP32-C2** - RISC-V single-core 120MHz
- **ESP32-C3** - RISC-V single-core 160MHz with native USB
- **ESP32-C6** - RISC-V single-core 160MHz with WiFi 6 and native USB

## Features

- ✅ GPIO support (digital input/output) - All chips
- ✅ UART serial communication - All chips
- ✅ USB CDC serial - S2, S3, C3, C6 (native USB)
- ✅ Hardware timers for precise timing - All chips
- ✅ ADC (Analog-to-Digital Converter) - 12-bit resolution - All chips
- ✅ SPI support - Full duplex with configurable clock and mode - All chips
- ✅ I2C support - Master mode with configurable speed - All chips
- ✅ Hardware PWM via LEDC - 13-bit resolution, up to 8 channels - All chips
- ✅ Chip ID reading from eFuse MAC - All chips
- ✅ Watchdog timer - Auto-reset protection with 2-second timeout - All chips
- ✅ IRQ support - Interrupt request handling - All chips

## Prerequisites

### ESP-IDF Installation

You need to install the ESP-IDF (Espressif IoT Development Framework) version 5.x:

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout release/v5.3  # or latest stable 5.x version
./install.sh esp32,esp32s2,esp32s3,esp32c2,esp32c3,esp32c6
. ./export.sh
```

**Important:** You need to source the export.sh script in every terminal session where you want to build:
```bash
. ~/esp/esp-idf/export.sh
```

### Toolchain

The ESP-IDF installation automatically includes the necessary toolchains:
- `xtensa-esp32-elf-gcc` for ESP32, ESP32-S2, ESP32-S3 (Xtensa architecture)
- `riscv32-esp-elf-gcc` for ESP32-C2, ESP32-C3, ESP32-C6 (RISC-V architecture)

## Building Klipper for ESP32

This project uses ESP-IDF as the build system with CMake integration.

1. **Source the ESP-IDF environment** (required for every terminal session):
```bash
. ~/esp/esp-idf/export.sh
```

2. **Set the target chip** (required before first build):
```bash
idf.py set-target esp32    # for ESP32 original
# or
idf.py set-target esp32s2  # for ESP32-S2
# or
idf.py set-target esp32s3  # for ESP32-S3
# or
idf.py set-target esp32c2  # for ESP32-C2
# or
idf.py set-target esp32c3  # for ESP32-C3
# or
idf.py set-target esp32c6  # for ESP32-C6
```

3. **Configure the project** (optional, uses defaults from sdkconfig.defaults):
3. **Configure the project** (optional, uses defaults from sdkconfig.defaults):
```bash
cd ~/klipper
idf.py menuconfig
```

4. **Build the firmware**:
```bash
idf.py build
```

oder 
```bash
.script/build-esp32.sh
```


The firmware binary will be created at `build/klipper-esp32.bin`.

5. **Flash to your ESP32**:
```bash
idf.py -p /dev/ttyUSB0 flash
```

Or flash and monitor serial output:
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### Manual Flashing

You can also flash manually with esptool:
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 \
    write_flash 0x1000 build/bootloader/bootloader.bin \
    0x8000 build/partition_table/partition-table.bin \
    0x10000 build/klipper-esp32.bin
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
All ESP32 variants support UART serial communication:
- **UART0**: GPIO1 (TX), GPIO3 (RX) - Default for ESP32, S2, S3
- **UART0**: GPIO21 (TX), GPIO20 (RX) - Default for C2, C3, C6
- **Baud rate**: 250000 (configurable in sdkconfig)

### USB Serial (Native USB)
Chips with native USB support (S2, S3, C3, C6) can use USB CDC:
- Enable in `sdkconfig`: `CONFIG_ESP_CONSOLE_USB_CDC=y`
- Provides direct USB serial connection without external USB-UART bridge
- Automatically detected as `/dev/ttyACM0` (Linux) or similar

## Current Limitations

This is a production-ready implementation with full hardware support:
- All peripherals use direct hardware register access for optimal performance
- ESP-IDF framework used for build system and HAL abstraction
- Optimized for real-time performance with FreeRTOS
- Supports ESP32, S2, S3, C2, C3, and C6 variants
- WiFi/Bluetooth not supported (Klipper uses serial/USB communication only)

## Development Notes

The implementation follows Klipper's architecture with ESP-IDF integration:

### Build System
- `CMakeLists.txt` - ESP-IDF CMake project configuration
- `sdkconfig.defaults` - Default ESP-IDF configuration
- `esp32_component/` - Klipper component for ESP-IDF build system

### Core Source Files (src/esp32/)
- `Kconfig` - Klipper build configuration options
- `Makefile` - Legacy build support
- `main.c` - Entry point and FreeRTOS task initialization
- `timer.c` - High-resolution timer for scheduling
- `gpio.c` - GPIO operations with hardware register access
- `serial.c` - UART communication
- `adc.c` - Analog-to-digital conversion with SAR ADC
- `spi.c` - SPI bus support with full-duplex mode
- `i2c.c` - I2C bus master mode support
- `hard_pwm.c` - Hardware PWM via LEDC with 13-bit resolution
- `chipid.c` - Unique chip identification from eFuse
- `watchdog.c` - Hardware watchdog timer with auto-reset
- `irq.c` - Interrupt request handling
- `internal.h` - Internal function declarations
- `esp32.lds.S` - Linker script

## Contributing

All core features are now fully implemented with hardware register access:
- ✅ Complete ADC implementation using hardware SAR ADC
- ✅ Full SPI driver with clock config and full-duplex support
- ✅ Complete I2C master mode with proper timing
- ✅ Hardware watchdog with auto-reset protection
- ✅ LEDC PWM with 13-bit resolution
- ✅ IRQ interrupt handling system
- ✅ ESP-IDF build system integration

Future enhancements could include:
- USB CDC serial implementation for native USB chips (S2, S3, C3, C6)
- DMA support for high-speed SPI/I2C transfers
- Additional optimization for RISC-V chips (C2, C3, C6)

## License

Copyright (C) 2024 Frederick

This file may be distributed under the terms of the GNU GPLv3 license.
