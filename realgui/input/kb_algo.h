/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __KB_ALGO_H__
#define __KB_ALGO_H__
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"

/** @brief  input device key board structure */
typedef struct gui_indev_kb
{
    gui_obj_t base;

    bool *state;                         /* Current state of the KeyBoard , true means key is being pressed */
    uint32_t *timestamp_ms_press;            /* The timestamp when pressed, update once */
    uint32_t *timestamp_ms_release;          /* The timestamp when when release, update once*/
    // uint32_t *timestamp_ms_pressing;         /* The timestamp when pressing, update periodically*/

    uint32_t last_processed_release;     /* Last processed release timestamp to avoid duplicate event handling */

} gui_indev_kb_t;

/* KB event */
kb_info_t *kb_algo_process(gui_obj_t *object);

gui_indev_kb_t *gui_kb_create(char *name, bool *state, uint32_t *timestamp_ms_press,
                              uint32_t *timestamp_ms_release);


#endif
