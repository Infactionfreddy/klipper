#ifndef __ESP32_SPI_H
#define __ESP32_SPI_H

#include <stdint.h>

struct spi_config {
    uint8_t spi_num;
    uint32_t rate;
    uint8_t mode;
};

struct spi_config spi_setup(uint32_t bus, uint8_t mode, uint32_t rate);
void spi_prepare(struct spi_config spi);
void spi_transfer(struct spi_config spi, uint8_t receive_data, uint8_t len, uint8_t *data);

#endif // __ESP32_SPI_H
