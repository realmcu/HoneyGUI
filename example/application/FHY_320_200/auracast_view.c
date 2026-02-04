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
#include "gui_win.h"
#include "gui_text.h"
#include "gui_list.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME AURACAST_VIEW

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void auracast_view_design(gui_view_t *view);

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
    .on_switch_in = auracast_view_design,
};

static char warning_str[] = "You're about to restart\nAuracast to enable\npassword.";
static char title[] = "Restart Auracast";

static const char *descriptor_rec = NULL;

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

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    f_status.password = !f_status.password; // do not restart auracast
    if (!descriptor_rec) { return; }
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_button_restart(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    // to do: Restart Auracast
    if (!descriptor_rec) { return; }
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_obj_t *parent = obj;
    gui_color_t font_color;
    gui_color_t bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        bg_color = BG_THEME1_BRIGHT_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        bg_color = BG_THEME1_BRIGHT_DARK;
    }
    gui_text_t *t = gui_text_create(parent, 0, 12, 10, 296, 60);
    gui_text_set(t, (void *)warning_str, GUI_FONT_SRC_BMP, font_color, strlen(warning_str), 20);
    gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);

    gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_240_80_BIN, 40, 84, 0, 0);
    gui_img_a8_recolor(bg, bg_color.color.argb_full);
    t = gui_text_create(bg, 0, 0, 0, 240, 80);
    gui_text_set(t, title, GUI_FONT_SRC_BMP, font_color, 7, 28);
    gui_text_type_set(t, CAPTION_2_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MID_CENTER);
    gui_obj_add_event_cb(bg, click_button_restart, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void auracast_view_design(gui_view_t *view)
{
    if (gui_view_get_current())
    {
        descriptor_rec = gui_view_get_current()->descriptor->name;
    }

    gui_view_set_animate_step(view, 10);
    gui_obj_t *parent = GUI_BASE(view);

    gui_color_t font_color;
    gui_color_t mask_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        mask_color = SCREEN_BG_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        mask_color = SCREEN_BG_DARK;
    }

    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, 174, 0,
                                       VERTICAL, note_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 1);
    gui_list_set_out_scope(list, 20);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(mask, mask_color.color.argb_full);

    gui_text_t *t = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(t, title, GUI_FONT_SRC_BMP, font_color, strlen(title), 30);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MID_CENTER);
    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_a8_recolor(icon_back, font_color.color.argb_full);
}
