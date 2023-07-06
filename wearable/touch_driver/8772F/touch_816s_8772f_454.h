/*
 * Copyright (c) 2006-2020, Watch Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     howie  first version
 */

#ifndef __TOUCH_816S_8772F_454_H
#define __TOUCH_816S_8772F_454_H


#include "board.h"
#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_816S_RST                                     P5_0
#define TOUCH_816S_INT                                     P5_1
#define TOUCH_816S_SCL                                     P5_5
#define TOUCH_816S_SDA                                     P5_3

#define TOUCH_816S_ADDR                                      0x15
#define TOUCH_CST816_CHIP_ID                                 0xA3     //chip ID 


void rtk_touch_hal_init(void);
bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing);

#ifdef __cplusplus
}
#endif

#endif /* __TOUCH_816S_8772F_454_H */
