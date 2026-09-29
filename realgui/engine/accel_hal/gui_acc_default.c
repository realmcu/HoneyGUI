/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_acc_default.h"
#include "gui_api_acc.h"

#if !defined(CONFIG_REALTEK_GUI_ACCELERATOR_SW) && \
    !defined(CONFIG_REALTEK_GUI_ACCELERATOR_CUSTOM)
#define CONFIG_REALTEK_GUI_ACCELERATOR_SW
#endif

#ifdef CONFIG_REALTEK_GUI_ACCELERATOR_SW
#include "acc_sw_entry.h"
#include "acc_sw_idu.h"

extern gui_blur_ops_t sw_arm2d_blur_ops;

static const gui_blit_ops_t sw_blit_ops =
{
    .init = sw_acc_init,
    .process = sw_acc_blit,
    .deinit = sw_acc_deinit,
};

extern void *gui_acc_decode(void *in);
static const gui_idu_ops_t sw_idu_ops =
{
    .load = gui_acc_decode,
    .release = gui_free,
};

static acc_engine_t sw_acc =
{
    .blit = &sw_blit_ops,
    .blur = &sw_arm2d_blur_ops,
    .idu = &sw_idu_ops,
};
#endif

void gui_acc_default_init(void)
{
#ifdef CONFIG_REALTEK_GUI_ACCELERATOR_SW
    if (gui_get_acc() == NULL)
    {
        gui_acc_info_register(&sw_acc);
    }
#endif
}
