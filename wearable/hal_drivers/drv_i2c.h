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
#include "rtl_i2c.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"



struct realtek_i2c_bus
{
    const char *name;
    I2C_TypeDef *Instance;
    IRQn_Type irq_type;
    uint8_t scl_pin;
    uint8_t sda_pin;
    uint8_t scl_pin_func;
    uint8_t sda_pin_func;
    uint32_t periph;
    uint32_t periph_clock;
    bool actived;
};


#define I2C0_CONFIG                                               \
    {                                                              \
        .name = "i2c0",                                           \
                .Instance = I2C0,                                         \
                            .irq_type = I2C0_IRQn,                                    \
                                        .scl_pin = P5_2,                                            \
                                                   .sda_pin = P5_3,                                            \
                                                              .scl_pin_func = I2C0_CLK,                                   \
                                                                              .sda_pin_func = I2C0_DAT,                                   \
                                                                                      .periph = APBPeriph_I2C0,                                 \
                                                                                              .periph_clock = APBPeriph_I2C0_CLOCK,                     \
                                                                                                      .actived = false,                   \
    }
#define I2C1_CONFIG                                               \
    {                                                              \
        .name = "i2c1",                                           \
                .Instance = I2C1,                                         \
                            .irq_type = I2C1_IRQn,                                    \
                                        .scl_pin = P5_2,                                            \
                                                   .sda_pin = P5_3,                                            \
                                                              .scl_pin_func = I2C1_CLK,                                   \
                                                                              .sda_pin_func = I2C1_DAT,                                   \
                                                                                      .periph = APBPeriph_I2C1,                                 \
                                                                                              .periph_clock = APBPeriph_I2C1_CLOCK,                     \
                                                                                                      .actived = false,                   \
    }



uint32_t drv_i2c0_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c0_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c1_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size);

uint32_t drv_i2c1_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size);


void hw_i2c_init(void);

#endif  /* __DRV_I2C_H__ */
