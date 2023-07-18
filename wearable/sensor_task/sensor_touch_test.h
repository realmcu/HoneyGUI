/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __SENSOR_TOUCH_TEST_H__
#define __SENSOR_TOUCH_TEST_H__

#include "board.h"
#include "rtl_i2c.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"





void sensor_mode_i2c_read(void);

uint32_t sensor_mode_i2c_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);


#endif  /* __SENSOR_TOUCH_TEST_H__ */
