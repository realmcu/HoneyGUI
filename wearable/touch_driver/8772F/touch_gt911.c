/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     tyustli  first version
 */

#include "wristband_sdk_config.h"

#ifdef BSP_USING_TOUCH
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_touch.h"
#include "drv_lcd.h"
#include "touch_gt911.h"
#include "string.h"
#include "trace.h"
#include "utils.h"


const uint8_t GT911_CFG_TBL[] =
{
    0X60, 0XE0, 0X01, 0XE0, 0X01, 0X05, 0X35, 0X00, 0X02, 0X08,
    0X1E, 0X08, 0X50, 0X3C, 0X0F, 0X05, 0X00, 0X00, 0XFF, 0X67,
    0X50, 0X00, 0X00, 0X18, 0X1A, 0X1E, 0X14, 0X89, 0X28, 0X0A,
    0X30, 0X2E, 0XBB, 0X0A, 0X03, 0X00, 0X00, 0X02, 0X33, 0X1D,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X32, 0X00, 0X00,
    0X2A, 0X1C, 0X5A, 0X94, 0XC5, 0X02, 0X07, 0X00, 0X00, 0X00,
    0XB5, 0X1F, 0X00, 0X90, 0X28, 0X00, 0X77, 0X32, 0X00, 0X62,
    0X3F, 0X00, 0X52, 0X50, 0X00, 0X52, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0F,
    0X0F, 0X03, 0X06, 0X10, 0X42, 0XF8, 0X0F, 0X14, 0X00, 0X00,
    0X00, 0X00, 0X1A, 0X18, 0X16, 0X14, 0X12, 0X10, 0X0E, 0X0C,
    0X0A, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X29, 0X28, 0X24, 0X22, 0X20, 0X1F, 0X1E, 0X1D,
    0X0E, 0X0C, 0X0A, 0X08, 0X06, 0X05, 0X04, 0X02, 0X00, 0XFF,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF,
};


uint8_t GT911_WR_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t send[198];
    send[0] = reg >> 8;
    send[1] = reg & 0xff;
    if (len > 200)
    {
        return 0;
    }

    memcpy(send + 2, buf, len);

    i2c_config[TOUCH_I2C_INDEX].write(TOUCH_GT911_ADDR, send, 2 + len);

    return 0;
}

void GT911_RD_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t iic_write_buf[2] = {0};
    iic_write_buf[0] = reg >> 8;
    iic_write_buf[1] = reg & 0xff;

    i2c_config[TOUCH_I2C_INDEX].write(TOUCH_GT911_ADDR, iic_write_buf, 2);

    i2c_config[TOUCH_I2C_INDEX].read(TOUCH_GT911_ADDR, buf, len);
}

uint8_t GT911_Send_Cfg(uint8_t mode)
{
    uint8_t buf[2];
    uint8_t i = 0;
    buf[0] = 0;
    buf[1] = mode;
    for (i = 0; i < sizeof(GT911_CFG_TBL); i++)
    {
        buf[0] += GT911_CFG_TBL[i];
    }
    buf[0] = (~buf[0]) + 1;
    GT911_WR_Reg(GT_CFGS_REG, (uint8_t *)GT911_CFG_TBL, sizeof(GT911_CFG_TBL));
    GT911_WR_Reg(GT_CHECK_REG, buf, 2);
    return 0;
}

uint32_t GT911_read(uint8_t *buf, uint32_t len)
{
    uint8_t *read = buf;
    uint8_t temp[10] = {0};

    uint8_t mode = 0;
    uint8_t point_num = 0;
    uint8_t reset = 0;
    GT911_RD_Reg(GT_GSTID_REG, &mode, 1);
    read[0] = mode;
    if (mode & 0x80)
    {
        GT911_WR_Reg(GT_GSTID_REG, &reset, 1);// clear flags
    }
    else
    {
        return 0;
    }
    point_num = mode & 0x0F;
    if (point_num == 0)
    {
        return 0;
    }

    GT911_RD_Reg(0x814F, read + 1, 7);

    GT911_WR_Reg(GT_GSTID_REG, &reset, 1);

    return len;
}

bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing)
{
    *pressing = false;
    uint8_t buf[10] = {0};
    *x = 480;
    *y = 480;
    if (GT911_read(buf, 0))
    {
        *pressing = true;
        *x = (buf[2] | (buf[3] << 8));
        *y = (buf[4] | (buf[5] << 8));
    }
    return true;
}

void rtk_touch_hal_init(struct rtl_touch_config *touch_cfg)
{
    DBG_DIRECT("touch_gt911_init line = %d GPIO_TOUCH_INT %d\n", __LINE__, touch_cfg->int_pin);

    gpio_config.pin_mode(touch_cfg->int_pin, PIN_MODE_OUTPUT);
    gpio_config.pin_write(touch_cfg->int_pin, 0);
    gpio_config.pin_mode(touch_cfg->rst_pin, PIN_MODE_OUTPUT);
    gpio_config.pin_write(touch_cfg->rst_pin, 0);
    platform_delay_ms(10);
    gpio_config.pin_write(touch_cfg->int_pin, 1);
    platform_delay_ms(10);
    gpio_config.pin_write(touch_cfg->rst_pin, 1);
    platform_delay_ms(30);
    gpio_config.pin_mode(touch_cfg->int_pin, PIN_MODE_INPUT);

    uint8_t iic_write_buf[2] = {0x81, 0x40};
    uint8_t iic_read_buf[5] = {0};

    i2c_config[TOUCH_I2C_INDEX].write(TOUCH_GT911_ADDR, iic_write_buf, 2);

    i2c_config[TOUCH_I2C_INDEX].read(TOUCH_GT911_ADDR, iic_read_buf, 4);

    DBG_DIRECT("CTP ID:%s\r\n", iic_read_buf);

    uint8_t cmd_init[1] = {2};
    GT911_WR_Reg(GT_CTRL_REG, (uint8_t *)cmd_init, 1);
    GT911_RD_Reg(GT_CFGS_REG, (uint8_t *)cmd_init, 1);
    if (cmd_init[0] < 0x60)
    {
        GT911_Send_Cfg(1);
    }
    platform_delay_us(200);
    cmd_init[0] = 0;
    GT911_WR_Reg(GT_CTRL_REG, (uint8_t *)cmd_init, 1);


    uint8_t x_low = 0;
    uint8_t x_high = 0;
    uint8_t y_low = 0;
    uint8_t y_high = 0;

    GT911_RD_Reg(0x8146, &x_low, 1);
    GT911_RD_Reg(0x8147, &x_high, 1);
    GT911_RD_Reg(0x8148, &y_low, 1);
    GT911_RD_Reg(0x8149, &y_high, 1);
    uint16_t x = x_low + (x_high << 8);
    uint16_t y = y_low + (y_high << 8);
    DBG_DIRECT("x = %d\r\n", x);
    DBG_DIRECT("y = %d\r\n", y);
}

#endif /* BSP_USING_TOUCH */
