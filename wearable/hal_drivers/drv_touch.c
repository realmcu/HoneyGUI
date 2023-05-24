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
#include "utils.h"
#include "app_section.h"
#include "drv_i2c.h"
#include "drv_gpio.h"
#include "drv_touch.h"
#include "os_timer.h"



bool drv_touch_read(uint16_t *x, uint16_t *y, bool *pressing)
{
    return rtk_touch_hal_read_all(x, y, pressing);
}


static void touch_enter_dlps(void *drv_io)
{
}

static void touch_exit_dlps(void *drv_io)
{

}

static bool touch_allowed_enter_dlps_check(void *drv_io)
{
    return false;
}

static bool touch_system_wakeup_dlps_check(void *drv_io)
{
    return false;
}



void drv_touch_init(void)
{
    DBG_DIRECT("Drv touch init");
    rtk_touch_hal_init();

}



/************** end of file ********************/
