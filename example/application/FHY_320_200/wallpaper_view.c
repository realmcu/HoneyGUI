/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME WALLPAPER_VIEW


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void wallpaper_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = wallpaper_view_design,
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static void timer_cb(void *p)
{
    GUI_UNUSED(p);
}

static void wallpaper_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 400);
    gui_view_switch_on_event(view, QUICK_VIEW, SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, CHARGING_VIEW, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION, GUI_EVENT_TOUCH_CLICKED);

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *img = gui_img_create_from_mem(parent, 0, WALLPAPER_1_BIN, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
}