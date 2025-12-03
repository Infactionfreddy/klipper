// ESP32 I2C support mit ESP-IDF i2c_master driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "irq.h"
#include "command.h"
#include "sched.h"

#include "driver/i2c_master.h"

#define MAX_I2C_DEVICES 8

struct i2c_config {
    i2c_master_dev_handle_t handle;
    uint8_t addr;
};

// I2C Bus handles
static i2c_master_bus_handle_t i2c_buses[2] = {NULL, NULL};

// I2C Device pool
static struct {
    i2c_master_dev_handle_t handle;
    uint8_t in_use;
} i2c_device_pool[MAX_I2C_DEVICES];

struct i2c_config i2c_setup(uint32_t bus, uint32_t rate, uint8_t addr)
{
    if (bus >= 2)
        shutdown("Invalid I2C bus");
    
    // Bus initialisieren (falls noch nicht)
    if (i2c_buses[bus] == NULL) {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = bus,
            .sda_io_num = (bus == 0) ? 21 : 25,
            .scl_io_num = (bus == 0) ? 22 : 26,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_buses[bus]));
    }
    
    // Find free device slot
    int slot = -1;
    for (int i = 0; i < MAX_I2C_DEVICES; i++) {
        if (!i2c_device_pool[i].in_use) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        shutdown("Too many I2C devices");
    
    // Device hinzufügen
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = rate,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_buses[bus], &dev_config, 
                                               &i2c_device_pool[slot].handle));
    i2c_device_pool[slot].in_use = 1;
    
    struct i2c_config config = {
        .handle = i2c_device_pool[slot].handle,
        .addr = addr,
    };
    return config;
}

int i2c_write(struct i2c_config config, uint8_t write_len, uint8_t *write)
{
    esp_err_t err = i2c_master_transmit(config.handle, write, write_len, -1);
    return (err == ESP_OK) ? 0 : -1;
}

int i2c_read(struct i2c_config config, uint8_t reg_len, uint8_t *reg,
              uint8_t read_len, uint8_t *read)
{
    esp_err_t err = i2c_master_transmit_receive(config.handle, reg, reg_len, 
                                                 read, read_len, -1);
    return (err == ESP_OK) ? 0 : -1;
}
