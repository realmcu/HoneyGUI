/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __SENSOR_UART_TEST_H__
#define __SENSOR_UART_TEST_H__

#include "board.h"
#include "rtl_i2c.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"


#define SENSOR_UART_TX      P3_0
#define SENSOR_UART_RX      P3_1


void sensor_mode_uart_write(void);

void sensor_mode_uart_rtc_int_init(void);


#endif  /* __SENSOR_UART_TEST_H__ */
