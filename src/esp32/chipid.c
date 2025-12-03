// Chip ID reading for ESP32
// Copyright (C) 2024 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_*
#include "generic/irq.h" // irq_disable
#include "internal.h" // chipid_read

#define EFUSE_BLK0_RDATA1_REG 0x3FF5A004
#define EFUSE_BLK0_RDATA2_REG 0x3FF5A008

void
chipid_read(uint8_t *id)
{
    // Read unique chip ID from eFuse
    volatile uint32_t *efuse1 = (volatile uint32_t*)EFUSE_BLK0_RDATA1_REG;
    volatile uint32_t *efuse2 = (volatile uint32_t*)EFUSE_BLK0_RDATA2_REG;
    
    uint32_t val1 = *efuse1;
    uint32_t val2 = *efuse2;
    
    // Copy to output buffer (6 bytes)
    id[0] = (val1 >> 0) & 0xFF;
    id[1] = (val1 >> 8) & 0xFF;
    id[2] = (val1 >> 16) & 0xFF;
    id[3] = (val1 >> 24) & 0xFF;
    id[4] = (val2 >> 0) & 0xFF;
    id[5] = (val2 >> 8) & 0xFF;
}
