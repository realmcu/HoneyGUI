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
#include "board.h"


bool drv_touch_read(uint16_t *x, uint16_t *y, bool *pressing)
{
    return rtk_touch_hal_read_all(x, y, pressing);
}


static void touch_enter_dlps(void)
{
    Pad_Config(TOUCH_816S_INT, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    System_WakeUpPinEnable(TOUCH_816S_INT, PAD_WAKEUP_POL_LOW, PAD_WAKEUP_DEB_DISABLE);
}

static void touch_exit_dlps(void)
{
    Pad_Config(TOUCH_816S_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
}

static bool touch_allowed_enter_dlps_check(void)
{
    return true;
}

static bool touch_system_wakeup_dlps_check(void)
{
    if (System_WakeUpInterruptValue(TOUCH_816S_INT) == SET)
    {
        Pad_ClearWakeupINTPendingBit(TOUCH_816S_INT);
        System_WakeUpPinDisable(TOUCH_816S_INT);
        DBG_DIRECT("Touch Wake up");
        return true;
    }
    return false;
}

void drv_touch_dlps_init(void)
{
    System_WakeUpPinEnable(TOUCH_816S_INT, PAD_WAKEUP_POL_LOW, PAD_WAKEUP_DEB_DISABLE);
    drv_dlps_exit_cbacks_register("touch", touch_exit_dlps);
    drv_dlps_enter_cbacks_register("touch", touch_enter_dlps);
    drv_dlps_wakeup_cbacks_register("touch", touch_system_wakeup_dlps_check);
    drv_dlps_check_cbacks_register("touch", touch_allowed_enter_dlps_check);
    uart4_allow_enter_dlps_timer_init();
}

void drv_touch_init(void)
{
    DBG_DIRECT("Drv touch init");
    rtk_touch_hal_init();
    drv_touch_dlps_init();
}



/************** end of file ********************/
