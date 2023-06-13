/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     balanceTWK        first version
 */

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <board.h>
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"


#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01


void drv_pin_mode(uint32_t pin, uint32_t mode);
void drv_pin_write(uint32_t pin, uint32_t value);
uint8_t drv_pin_read(uint32_t pin);
uint8_t drv_pin_attach_irq(uint32_t pin, uint32_t mode, void (*hdr)(void *args), void *args);
uint8_t drv_pin_dettach_irq(uint32_t pin);
uint8_t drv_pin_irq_enable(uint32_t pin, uint32_t enabled);

void rt_hw_pin_init(void);

#endif /* __DRV_GPIO_H__ */

