# ESP32 build rules
#
# NOTE: ESP32 requires ESP-IDF framework to build.
# This Makefile provides a simplified wrapper but ESP-IDF must be installed.
# See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

# ESP32 uses ESP-IDF build system instead of traditional Makefile
# This is a compatibility shim only

dirs-$(CONFIG_MACH_ESP32) += src/esp32 src/generic

# Detect if IDF_PATH is set
ifndef IDF_PATH
$(error ESP-IDF not found! Please install ESP-IDF and run: . $$IDF_PATH/export.sh)
endif

# ESP32 source files for reference (actual build uses CMake via ESP-IDF)
src-y += esp32/main.c esp32/timer.c esp32/gpio.c esp32/console.c 
src-y += esp32/chipid.c esp32/irq.c esp32/ctr_stubs.c
src-$(CONFIG_HAVE_GPIO_SPI) += esp32/spi.c
src-$(CONFIG_HAVE_GPIO_I2C) += esp32/i2c.c  
src-$(CONFIG_HAVE_GPIO_ADC) += esp32/adc.c
src-$(CONFIG_HAVE_GPIO_HARD_PWM) += esp32/hard_pwm.c
src-y += generic/crc16_ccitt.c generic/alloc.c generic/timer_irq.c generic/serial_irq.c

# Override default build targets for ESP32
target-y := $(OUT)klipper-esp32.bin

# ESP32 Build via ESP-IDF (not traditional make)
.PHONY: esp32-idf-build

$(OUT)klipper-esp32.bin: 
	@echo ""
	@echo "===================================================================="
	@echo "ESP32 uses ESP-IDF build system (CMake), not traditional Makefile"  
	@echo "===================================================================="
	@echo ""
	@echo "To build for ESP32, you must:"
	@echo "  1. Restore CMakeLists.txt and components/ directory structure"
	@echo "  2. Run: idf.py build"
	@echo ""  
	@echo "Or use the automated ESP-IDF project generator script (if available)"
	@echo ""
	@false

# Alternative: Show error message
all:
	@echo "ERROR: ESP32 cannot use standard Klipper Makefile build"
	@echo "       ESP32 requires ESP-IDF CMake build system"
	@echo ""
	@echo "Please see ESP32-specific documentation for build instructions"
	@false
