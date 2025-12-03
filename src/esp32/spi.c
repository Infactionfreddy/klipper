// ESP32 SPI support mit ESP-IDF spi_master driver
//
// Copyright (C) 2025  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "gpio.h" // struct spi_config
#include "irq.h"
#include "command.h"
#include "sched.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

#define MAX_SPI_DEVICES 8

struct spi_device {
    spi_device_handle_t handle;
    spi_host_device_t host;
};

// SPI Device pool
static struct {
    spi_device_handle_t handle;
    uint8_t in_use;
} spi_device_pool[MAX_SPI_DEVICES];

// SPI Hosts (HSPI=SPI2, VSPI=SPI3)
static uint8_t spi_host_initialized[3] = {0, 0, 0};

struct spi_config spi_setup(uint32_t bus, uint8_t mode, uint32_t rate)
{
    spi_host_device_t host;
    
    // Bus mapping: 2=HSPI, 3=VSPI
    if (bus == 2) {
        host = SPI2_HOST;
    } else if (bus == 3) {
        host = SPI3_HOST;
    } else {
        shutdown("Invalid SPI bus");
    }
    
    // Bus initialisieren (falls noch nicht)
    if (!spi_host_initialized[host]) {
        spi_bus_config_t buscfg = {
            .mosi_io_num = (host == SPI2_HOST) ? 13 : 23,
            .miso_io_num = (host == SPI2_HOST) ? 12 : 19,
            .sclk_io_num = (host == SPI2_HOST) ? 14 : 18,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4096,
        };
        ESP_ERROR_CHECK(spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO));
        spi_host_initialized[host] = 1;
    }
    
    // Find free device slot
    int slot = -1;
    for (int i = 0; i < MAX_SPI_DEVICES; i++) {
        if (!spi_device_pool[i].in_use) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        shutdown("Too many SPI devices");
    
    // Device konfigurieren
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = rate,
        .mode = mode,
        .spics_io_num = -1,  // CS wird manuell verwaltet
        .queue_size = 1,
        .flags = 0,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(host, &devcfg, &spi_device_pool[slot].handle));
    spi_device_pool[slot].in_use = 1;
    
    struct spi_config config = {
        .device_id = slot,
    };
    return config;
}

void spi_prepare(struct spi_config config)
{
    // Device ist bereits vorbereitet
}

void spi_transfer(struct spi_config config, uint8_t receive_data,
                  uint8_t len, uint8_t *data)
{
    if (config.device_id >= MAX_SPI_DEVICES || !spi_device_pool[config.device_id].in_use)
        shutdown("Invalid SPI device");
    
    spi_transaction_t trans = {
        .length = len * 8,  // in bits
        .tx_buffer = data,
        .rx_buffer = receive_data ? data : NULL,
    };
    
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_device_pool[config.device_id].handle, &trans));
}
