/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __SENSOR_MODE_I2C_H__
#define __SENSOR_MODE_I2C_H__

#include "board.h"
#include "rtl_i2c.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"





uint32_t sensor_mode_i2c_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t sensor_mode_i2c_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

void sensor_mode_i2c_set_scl_sda(uint8_t scl_pin, uint8_t sda_pin);


#endif  /* __SENSOR_MODE_I2C_H__ */
