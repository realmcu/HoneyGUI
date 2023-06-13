/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_i2c.h"
#include "trace.h"
#include "os_sync.h"


static bool i2c0_actived = false;
static bool i2c1_actived = false;


void drv_i2c0_init(uint8_t scl_pin, uint8_t sda_pin)
{
    Pad_Config(scl_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(sda_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(scl_pin, I2C0_CLK);
    Pinmux_Config(sda_pin, I2C0_DAT);

    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
    I2C_InitTypeDef  I2C_InitStructure;
    I2C_InitStructure.I2C_Clock = 40000000;
    I2C_InitStructure.I2C_ClockSpeed   = 400000;
    I2C_InitStructure.I2C_DeviveMode   = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode  = I2C_AddressMode_7BIT;
    I2C_InitStructure.I2C_Ack = ENABLE;
    I2C_Init(I2C0, &I2C_InitStructure);
    I2C_Cmd(I2C0, ENABLE);
    i2c0_actived = true;
}

void drv_i2c1_init(uint8_t scl_pin, uint8_t sda_pin)
{
    Pad_Config(scl_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(sda_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(scl_pin, I2C1_CLK);
    Pinmux_Config(sda_pin, I2C1_DAT);

    RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);
    I2C_InitTypeDef  I2C_InitStructure;
    I2C_InitStructure.I2C_Clock = 40000000;
    I2C_InitStructure.I2C_ClockSpeed   = 400000;
    I2C_InitStructure.I2C_DeviveMode   = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode  = I2C_AddressMode_7BIT;
    I2C_InitStructure.I2C_Ack = ENABLE;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
    i2c1_actived = true;
}


uint32_t drv_i2c0_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c0_actived == false)
    {
        DBG_DIRECT("NOT INIT IIC BUS!!!!");
        while (1);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(I2C0, slave_addr);
    abort_status = I2C_MasterRead(I2C0, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c0_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c0_actived == false)
    {
        DBG_DIRECT("NOT INIT IIC BUS!!!!");
        while (1);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(I2C0, slave_addr);
    abort_status = I2C_MasterWrite(I2C0, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c1_read(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c1_actived == false)
    {
        DBG_DIRECT("NOT INIT IIC BUS!!!!");
        while (1);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(I2C1, slave_addr);
    abort_status = I2C_MasterRead(I2C1, buffer, size);
    if (abort_status != I2C_Success)
    {
        return 0;
    }
    return size;
}

uint32_t drv_i2c1_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    if (i2c1_actived == false)
    {
        DBG_DIRECT("NOT INIT IIC BUS!!!!");
        while (1);
    }
    I2C_Status abort_status = I2C_ERR_TIMEOUT;
    I2C_SetSlaveAddress(I2C1, slave_addr);
    abort_status = I2C_MasterWrite(I2C1, buffer, size);
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
