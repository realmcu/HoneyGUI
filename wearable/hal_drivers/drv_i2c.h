/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "board.h"
#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"




uint32_t drv_i2c0_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c0_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c1_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c1_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

void drv_i2c0_init(uint8_t scl_pin, uint8_t sda_pin);

void drv_i2c1_init(uint8_t scl_pin, uint8_t sda_pin);


void hw_i2c_init(void);

#endif  /* __DRV_I2C_H__ */
