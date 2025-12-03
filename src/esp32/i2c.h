#ifndef __ESP32_I2C_H
#define __ESP32_I2C_H

#include <stdint.h>

struct i2c_config {
    uint8_t i2c_num;
    uint8_t addr;
};

struct i2c_config i2c_setup(uint32_t bus, uint32_t rate, uint8_t addr);
int i2c_write(struct i2c_config config, uint8_t write_len, uint8_t *write);
int i2c_read(struct i2c_config config, uint8_t reg_len, uint8_t *reg, uint8_t read_len, uint8_t *read);

#endif // __ESP32_I2C_H
