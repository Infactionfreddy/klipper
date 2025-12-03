// Chip ID reading for ESP32 family
// Copyright (C) 2024 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_*
#include "generic/irq.h" // irq_disable
#include "internal.h" // chipid_read
#include "esp_chip_info.h"
#include "esp_mac.h"

void
chipid_read(uint8_t *id)
{
    // Read unique MAC address (6 bytes) - works across all ESP32 variants
    // This uses the base MAC address stored in eFuse
    esp_efuse_mac_get_default(id);
}
