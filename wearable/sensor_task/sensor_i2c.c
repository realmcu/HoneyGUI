/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "sensor_i2c.h"
#include "trace.h"



void sensor_mode_i2c_set_scl_sda(uint8_t scl_pin, uint8_t sda_pin)
{
    Pad_Config(scl_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(sda_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(scl_pin, I2C0_CLK);
    Pinmux_Config(sda_pin, I2C0_DAT);
}

uint32_t sensor_mode_i2c_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    return 0;
}

uint32_t sensor_mode_i2c_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    return 0;
}

/************** end of file ********************/
