/*
 * Copyright (c) 2015-2020, RealTek Wristband Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-11-18     howie  first version
 */
#ifndef __GUI_KB_H__
#define __GUI_KB_H__
#include <string.h>
#include "guidef.h"


kb_info_t *kb_get_info(void);
void kb_algo_process(gui_kb_port_data_t *kb_raw);


#endif
