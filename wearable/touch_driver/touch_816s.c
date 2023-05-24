/*
 * Copyright (c) 2006-2020, Watch Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     howie  first version
 */


#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_touch.h"
#include "touch_816s.h"
#include "string.h"
#include "trace.h"
#include "utils.h"




bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing)
{
    uint8_t iic_write_buf[1] = {0};
    uint8_t buf[24] = {0};

    drv_i2c0_write(TOUCH_816S_ADDR, iic_write_buf, 1);
    platform_delay_us(100);

    drv_i2c0_read(TOUCH_816S_ADDR, buf, 24);

    if (buf[3] >> 6 == 2)
    {
        *pressing = true;
    }
    else
    {
        *pressing = false;
    }


    *x = 454 - (((buf[3] & 0x0f) << 8) | buf[4]);
    *y = 454 - ((buf[5] & 0x0f) << 8) | buf[6];

    return true;
}


void rtk_touch_hal_set_indicate(void *indicate)
{
    drv_pin_mode(TOUCH_INT, PIN_MODE_INPUT);
    drv_pin_attach_irq(TOUCH_INT, PIN_IRQ_MODE_RISING_FALLING, indicate,
                       NULL);
    drv_pin_irq_enable(TOUCH_INT, PIN_IRQ_DISABLE);
}

void rtk_touch_hal_int_config(bool enable)
{
    if (enable == true)
    {
        drv_pin_irq_enable(TOUCH_INT, PIN_IRQ_ENABLE);
    }
    else
    {
        drv_pin_irq_enable(TOUCH_INT, PIN_IRQ_DISABLE);
    }
}




void rtk_touch_hal_init(void)
{
    drv_pin_mode(TOUCH_RST, PIN_MODE_OUTPUT);
    drv_pin_write(TOUCH_RST, 0);
    platform_delay_ms(10);
    drv_pin_write(TOUCH_RST, 1);
    platform_delay_ms(100);



    uint8_t iic_write_buf[1] = {TOUCH_CST816_CHIP_ID};
    uint8_t iic_read_buf[1] = {0x33};

    drv_i2c0_write(TOUCH_816S_ADDR, iic_write_buf, 1);
    platform_delay_us(100);

    drv_i2c0_read(TOUCH_816S_ADDR, iic_read_buf, 1);

    DBG_DIRECT("CTP ID:0x%x\r\n", iic_read_buf[0]);



}

