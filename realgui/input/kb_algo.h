/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __KB_ALGO_H__
#define __KB_ALGO_H__
#include <string.h>
#include "guidef.h"


/* KB event */
#define GUI_KB_EVENT_NONE     0                /* KB none */
#define GUI_KB_EVENT_UP       1                /* KB up event */
#define GUI_KB_EVENT_DOWN     2                /* KB down event */
kb_info_t *kb_get_info(void);
struct kb_info *kb_algo_process(gui_kb_port_data_t *kb_raw);


#endif
