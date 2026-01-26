/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* gui lite geometry rect example start */
#include "gui_components_init.h"
#include "gui_rect.h"
#include "gui_view.h"
#include "gui_view_instance.h"
#define RECT_VIEW_NAME "rect_view"
static void app_rect_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_view_set_animate_step(view, 500);
    gui_view_switch_on_event(view, "rect_round_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    gui_rect_create(parent, "bg", 0, 0, 480, 480, 0, gui_rgba(255, 200, 200, 255));
    gui_rect_create(parent, "geometry2", 100, 200, 200, 200, 20, gui_rgba(0, 255, 0, 200));
    gui_rect_create(parent, "geometry3", 150, 250, 200, 200, 20, gui_rgba(0, 0, 255, 200));
    gui_rect_create(parent, "canvas_clear", 29, 10, 352, 100, 30, gui_rgba(39, 43, 44, 255));
}
GUI_VIEW_INSTANCE(RECT_VIEW_NAME, false, app_rect_design, 0);
/* gui lite geometry rect example end */
