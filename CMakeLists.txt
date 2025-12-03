# Klipper firmware for ESP32
# Minimal ESP-IDF project wrapper

cmake_minimum_required(VERSION 3.16)

# Set component directories
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/esp32_component")

# Include ESP-IDF CMake project
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(klipper-esp32)
