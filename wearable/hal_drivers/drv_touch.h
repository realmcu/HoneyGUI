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

#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"
#include "drv_gpio.h"


#ifdef TOUCH_GT911
#include "touch_gt911.h"
#endif

bool drv_touch_read(uint16_t *x, uint16_t *y, bool *pressing);
void drv_touch_init(void);

#endif  /* __DRV_TOUCH_H__ */

/******************* end of file ***************/
