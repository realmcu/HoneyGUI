/**
 * @file components.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-11-08
 *
 * @copyright Copyright (c) 2023
 *
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

