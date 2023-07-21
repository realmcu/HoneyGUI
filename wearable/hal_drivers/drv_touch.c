/*
 * Copyright (c) 2006-2020, Watch Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     howie  first version
 */


#include "trace.h"
#include "rtl_hal_peripheral.h"
#include "app_section.h"
#include "drv_i2c.h"
#include "drv_gpio.h"
#include "drv_touch.h"
#include "os_timer.h"
#include "board.h"


bool drv_touch_read(uint16_t *x, uint16_t *y, bool *pressing)
{
    return rtk_touch_hal_read_all(x, y, pressing);
}

void drv_touch_set_indicate(void (*indicate)(void *))
{
    rtk_touch_hal_set_indicate(indicate);
}

void drv_touch_int_config(bool enable)
{
    rtk_touch_hal_int_config(enable);
}

void drv_touch_init(void)
{
    DBG_DIRECT("Drv touch init");
    rtk_touch_hal_init();
}



/************** end of file ********************/
