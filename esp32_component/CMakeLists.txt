# Klipper firmware ESP-IDF component

set(KLIPPER_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/../src")

# ESP32-specific sources
set(ESP32_SRCS
    "${KLIPPER_SRC_DIR}/esp32/main.c"
    "${KLIPPER_SRC_DIR}/esp32/timer.c"
    "${KLIPPER_SRC_DIR}/esp32/gpio.c"
    "${KLIPPER_SRC_DIR}/esp32/console.c"
    "${KLIPPER_SRC_DIR}/esp32/chipid.c"
    "${KLIPPER_SRC_DIR}/esp32/irq.c"
    "${KLIPPER_SRC_DIR}/esp32/ctr_stubs.c"
    "${KLIPPER_SRC_DIR}/esp32/adc.c"
    "${KLIPPER_SRC_DIR}/esp32/spi.c"
    "${KLIPPER_SRC_DIR}/esp32/i2c.c"
    "${KLIPPER_SRC_DIR}/esp32/hard_pwm.c"
)

# Generic Klipper sources
set(GENERIC_SRCS
    "${KLIPPER_SRC_DIR}/generic/alloc.c"
    "${KLIPPER_SRC_DIR}/generic/timer_irq.c"
    "${KLIPPER_SRC_DIR}/generic/serial_irq.c"
    "${KLIPPER_SRC_DIR}/generic/crc16_ccitt.c"
)

# Klipper core sources  
set(CORE_SRCS
    "${KLIPPER_SRC_DIR}/sched.c"
    "${KLIPPER_SRC_DIR}/command.c"
    "${KLIPPER_SRC_DIR}/endstop.c"
    "${KLIPPER_SRC_DIR}/stepper.c"
    "${KLIPPER_SRC_DIR}/basecmd.c"
    "${KLIPPER_SRC_DIR}/trsync.c"
    "${KLIPPER_SRC_DIR}/gpiocmds.c"
    "${KLIPPER_SRC_DIR}/adccmds.c"
    "${KLIPPER_SRC_DIR}/spicmds.c"
    "${KLIPPER_SRC_DIR}/i2ccmds.c"
    "${KLIPPER_SRC_DIR}/pwmcmds.c"
    "${KLIPPER_SRC_DIR}/initial_pins.c"
    # Note: i2c_software.c and spi_software.c are excluded
    # as ESP32 uses hardware peripherals only
)

# Register component
idf_component_register(
    SRCS ${ESP32_SRCS} ${GENERIC_SRCS} ${CORE_SRCS}
    INCLUDE_DIRS 
        "${KLIPPER_SRC_DIR}"
        "${KLIPPER_SRC_DIR}/esp32"
        "${KLIPPER_SRC_DIR}/generic"
    REQUIRES 
        driver
        esp_timer
        nvs_flash
        esp_adc
        vfs
)

# Create board symlink for generic code (board/ -> esp32/)
set(BOARD_LINK_DIR "${CMAKE_CURRENT_BINARY_DIR}/board")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
if(NOT EXISTS "${BOARD_LINK_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_CURRENT_LIST_DIR}/../src/esp32"
            "${BOARD_LINK_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()

# Add board directory to include path
target_include_directories(${COMPONENT_LIB} PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Compiler flags for Klipper
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error=unused-function
    -Wno-error=unused-variable
    -Wno-error=return-type
    -Wno-unused-but-set-variable
    -DCONFIG_CLOCK_FREQ=240000000
    -DCONFIG_SERIAL_BAUD=250000
    -DCONFIG_HAVE_GPIO=1
    -DCONFIG_HAVE_GPIO_ADC=1
    -DCONFIG_HAVE_GPIO_SPI=1
    -DCONFIG_HAVE_GPIO_I2C=1  
    -DCONFIG_HAVE_GPIO_HARD_PWM=1
    -DCONFIG_HAVE_STRICT_TIMING=1
    -DCONFIG_HAVE_CHIPID=1
    -DCONFIG_INITIAL_PINS=\"\"
    -DCONFIG_INLINE_STEPPER_HACK=0
    -DCONFIG_MACH_AVR=0
    -DCONFIG_HAVE_BOOTLOADER_REQUEST=0
    -DCONFIG_WANT_SOFTWARE_SPI=0
    -DCONFIG_WANT_SOFTWARE_I2C=0
    -DCONFIG_MCU=\"esp32\"
)
