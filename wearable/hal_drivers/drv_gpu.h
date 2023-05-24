/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-06-29     supperthomas  first version
 */

#ifndef __DRV_GPU_H__
#define __DRV_GPU_H__

#include "board.h"
#include "stdbool.h"
#include "rtl_uart.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "drv_dlps.h"

#ifdef __cplusplus
extern "C" {
#endif


void drv_gpu_init(void);
void drv_gpu_deinit(void);

#ifdef __cplusplus
}
#endif
#endif  /* __DRV_GPU_H__ */

