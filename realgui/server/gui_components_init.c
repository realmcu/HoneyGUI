/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include "gui_components_init.h"

static const gui_init_fn_t __honeygui_init_start[] GUI_APP_SECTION(".honeygui$init0") GUI_APP_USED
GUI_FN_ALIGN = { 0 };
static const gui_init_fn_t __honeygui_init_end[]   GUI_APP_SECTION(".honeygui$initZ") GUI_APP_USED
GUI_FN_ALIGN = { 0 };



void gui_components_init(void)
{
    static bool inited = false;
    if (inited)
    {
        return;
    }
    inited = true;
    volatile const gui_init_fn_t *fn_ptr;

    // cppcheck-suppress comparePointers
    for (fn_ptr = __honeygui_init_start + 1; fn_ptr < __honeygui_init_end; ++fn_ptr)
    {
        if (*fn_ptr) { (*fn_ptr)(); }
    }
}

