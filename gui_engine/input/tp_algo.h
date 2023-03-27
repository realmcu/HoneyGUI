/*
 * Copyright (c) 2015-2020, RealTek Wristband Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-11-18     howie  first version
 */
#ifndef __TP_ALGO_H__
#define __TP_ALGO_H__
//#include "touch.h"
#include <string.h>
#include "guidef.h"

#define SAME_POINT_THR      5
#define HALF_SCREEN_SIZE    (320/2)


/* Touch event */
#define GUI_TOUCH_EVENT_NONE     0                /* Touch none */
#define GUI_TOUCH_EVENT_UP       1                /* Touch up event */
#define GUI_TOUCH_EVENT_DOWN     2                /* Touch down event */


struct touch_info *tp_algo_process(struct gui_touch_data *raw_data);
struct touch_info *tp_get_info(void);


#endif
