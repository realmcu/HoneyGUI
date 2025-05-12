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


static int rti_start(void)
{
    return 0;
}
GUI_APP_INIT_EXPORT(rti_start, "0");

static int rti_board_start(void)
{
    return 0;
}
GUI_APP_INIT_EXPORT(rti_board_start, "0.end");

static int rti_board_end(void)
{
    return 0;
}
GUI_APP_INIT_EXPORT(rti_board_end, "1.end");

static int rti_end(void)
{
    return 0;
}
GUI_APP_INIT_EXPORT(rti_end, "7.end");


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
    for (fn_ptr = &__ui_init_rti_board_end; fn_ptr < &__ui_init_rti_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }
}

