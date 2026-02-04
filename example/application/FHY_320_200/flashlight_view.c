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
#define CURRENT_VIEW_NAME FLASHLIGHT_VIEW

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void flashlight_view_design(gui_view_t *view);

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
    .on_switch_in = flashlight_view_design,
};

static const char *descriptor_rec = NULL;
static int8_t brightness_val_rec = 0;

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


static void click_cancel(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    screen_brightness_val = brightness_val_rec;
    gui_view_set_animate_step(current_view, 400);
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void flashlight_view_design(gui_view_t *view)
{
    descriptor_rec = gui_view_get_current()->descriptor->name;
    gui_view_switch_on_event(view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION, SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);

    gui_set_bg_color(FG_WHITE);
    brightness_val_rec = screen_brightness_val;
    screen_brightness_val = 3;

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *icon = gui_img_create_from_mem(parent, 0, ICON_FLASHLIGHT_OFF_BIN, 143, 118, 0, 0);
    gui_img_a8_recolor(icon, theme_bg_white ? BG_THEME2_BRIGHT_LIGHT.color.argb_full :
                       BG_THEME2_BRIGHT_DARK.color.argb_full);
}