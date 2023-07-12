/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     tyustli  first version
 */

#ifndef __TOUCH_GT911_H
#define __TOUCH_GT911_H


#include "board.h"
#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TOUCH_GT911_ADDR                                      0x14

#define GT_CTRL_REG     0X8040
#define GT_CFGS_REG     0X8047
#define GT_CHECK_REG    0X80FF
#define GT_PID_REG      0X8140

#define GT_GSTID_REG    0X814E
#define GT_TP1_REG      0X8150
#define GT_TP2_REG      0X8158
#define GT_TP3_REG      0X8160
#define GT_TP4_REG      0X8168
#define GT_TP5_REG      0X8170
#define TOUCH_GT911_RST                                          P1_6
#define TOUCH_GT911_INT                                          P2_2
#define TOUCH_GT911_SCL                                          P2_3
#define TOUCH_GT911_SDA                                          P2_4
bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing);
#ifdef __cplusplus
}
#endif

#endif /* __TOUCH_GT911_H */
