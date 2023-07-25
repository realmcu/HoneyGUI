/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_TOUCH_H__
#define __DRV_TOUCH_H__

#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"
#include "drv_gpio.h"
#include "drv_i2c.h"


#ifdef TOUCH_GT911
#include "touch_gt911.h"
#endif


void rtk_touch_hal_init(void);
bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing);
void rtk_touch_hal_set_indicate(void (*indicate)(void *));
void rtk_touch_hal_int_config(bool enable);
bool drv_touch_read(uint16_t *x, uint16_t *y, bool *pressing);
void drv_touch_init(void);

#endif  /* __DRV_TOUCH_H__ */

/******************* end of file ***************/
