/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_i2c.h"
#include "trace.h"
#include "os_sync.h"

struct realtek_i2c_bus i2c_bus[] =
{
    I2C0_CONFIG,
    I2C1_CONFIG,
};

static void mcu_i2c_init(struct realtek_i2c_bus *i2c_cfg)
{
    Pad_Config(i2c_cfg->scl_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(i2c_cfg->sda_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(i2c_cfg->scl_pin, i2c_cfg->scl_pin_func);
    Pinmux_Config(i2c_cfg->sda_pin, i2c_cfg->sda_pin_func);

    RCC_PeriphClockCmd(i2c_cfg->periph, i2c_cfg->periph_clock, ENABLE);
    I2C_InitTypeDef  I2C_InitStructure;
    I2C_InitStructure.I2C_Clock = 40000000;
    I2C_InitStructure.I2C_ClockSpeed   = 400000;
    I2C_InitStructure.I2C_DeviveMode   = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode  = I2C_AddressMode_7BIT;
    I2C_InitStructure.I2C_Ack = ENABLE;
    I2C_Init(i2c_cfg->Instance, &I2C_InitStructure);
    I2C_Cmd(i2c_cfg->Instance, ENABLE);
    i2c_cfg->actived = true;
}

uint32_t drv_i2c0_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c_bus[0].actived == false)
    {
        mcu_i2c_init(&i2c_bus[0]);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(i2c_bus[0].Instance, slave_addr);
    abort_status = I2C_MasterRead(i2c_bus[0].Instance, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c0_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c_bus[0].actived == false)
    {
        mcu_i2c_init(&i2c_bus[0]);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(i2c_bus[0].Instance, slave_addr);
    abort_status = I2C_MasterWrite(i2c_bus[0].Instance, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c1_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c_bus[1].actived == false)
    {
        mcu_i2c_init(&i2c_bus[1]);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(i2c_bus[1].Instance, slave_addr);
    abort_status = I2C_MasterRead(i2c_bus[1].Instance, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c1_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c_bus[1].actived == false)
    {
        mcu_i2c_init(&i2c_bus[1]);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(i2c_bus[1].Instance, slave_addr);
    abort_status = I2C_MasterWrite(i2c_bus[1].Instance, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}





void hw_i2c_init(void)
{
    DBG_DIRECT("hw_i2c_init");
}

/************** end of file ********************/
