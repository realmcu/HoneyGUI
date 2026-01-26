/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __WHEEL_ALGO_H__
#define __WHEEL_ALGO_H__
#include <string.h>
#include "guidef.h"

#ifdef __cplusplus
extern "C" {
#endif

touch_info_t *wheel_get_info(void);
touch_info_t *wheel_algo_process(gui_wheel_port_data_t *wheel_raw);

#ifdef __cplusplus
}
#endif

#endif
