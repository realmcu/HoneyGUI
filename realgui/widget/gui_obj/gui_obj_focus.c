/**
 * @file gui_obj_focus.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief Focus management implementation
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "gui_obj_focus.h"
#include "gui_obj_event.h"

static gui_obj_t *g_obj_focused = NULL;

void gui_obj_focus_set(gui_obj_t *obj)
{
    g_obj_focused = obj;
}

gui_obj_t *gui_obj_focus_get(void)
{
    return g_obj_focused;
}

bool gui_obj_focus_is_focused(gui_obj_t *obj)
{
    return (g_obj_focused == obj);
}
