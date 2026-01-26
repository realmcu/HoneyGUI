/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_img.h"
#include "gui_text.h"
#include "gui_view.h"
#include "gui_message.h"
#include "root_image/ui_resource.h"
#include "gui_view_macro.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "charging"

#define ICON_CHARGING_Y             215
#define ICON_CHARGING_START_X       162
#define ICON_CHARGING_STOP_X        220

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void charging_view_design(gui_view_t *view);

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
    .on_switch_in = charging_view_design,
};
static uint8_t animation_cnt = 0;
static char battery_tx_str[8] = {0};
static uint8_t battery_tx_val = 0;

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

static void switch_charging_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *view_c = gui_view_get_current();
    if (view_c)
    {
        gui_view_set_animate_step(view_c, 400);
        gui_view_switch_direct(view_c, descriptor.name, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
}

static void battery_update_cb(void *p)
{
    gui_text_t *text = (gui_text_t *)p;
    battery_tx_val++;
    if (battery_tx_val >= 100)
    {
        battery_tx_val = 0;
    }
    sprintf(battery_tx_str, "%d%%", battery_tx_val);
    gui_text_content_set(text, battery_tx_str, strlen(battery_tx_str));
}

static void entrance_animation(void *p)
{
    GUI_UNUSED(p);
    animation_cnt++;
    uint8_t cnt_max_0 = 5;
    uint8_t cnt_max_1 = 15 + cnt_max_0;
    uint8_t cnt_max_2 = 30 + cnt_max_0;
    uint8_t cnt_max_3 = 45 + cnt_max_0;
    uint8_t cnt_max_4 = 60 + cnt_max_0;
    gui_img_t *bg = (gui_img_t *)p;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(current_view)->child_list.prev, gui_obj_t,
                                                  brother_list);

    if (animation_cnt <= cnt_max_0)
    {
        gui_obj_hidden((void *)bg, true);
        return;
    }
    else if (animation_cnt <= cnt_max_1) //fade in
    {
        gui_obj_hidden((void *)bg, false);
        gui_img_set_opacity(bg, UINT8_MAX * (animation_cnt - cnt_max_0) / (cnt_max_1 - cnt_max_0));
        gui_img_set_opacity(icon, UINT8_MAX * (animation_cnt - cnt_max_0) / (cnt_max_1 - cnt_max_0));
    }
    else if (animation_cnt <= cnt_max_2) //wait
    {
        return;
    }
    else if (animation_cnt <= cnt_max_3) //scale
    {
        float scale = 1.f - 0.2f * (animation_cnt - cnt_max_2) / (cnt_max_3 - cnt_max_2);
        gui_img_scale(icon, scale, scale);
    }
    else if (animation_cnt <= cnt_max_4) //move
    {
        gui_img_set_opacity(bg, UINT8_MAX * (cnt_max_4 - animation_cnt) / (cnt_max_4 - cnt_max_3));
        float t_x = (float)(ICON_CHARGING_STOP_X - ICON_CHARGING_START_X) * (animation_cnt - cnt_max_3) /
                    (cnt_max_4 - cnt_max_3);
        gui_img_translate(icon, t_x, 0);
    }
    if (animation_cnt == cnt_max_4)
    {
        gui_obj_hidden((void *)bg, true);
        gui_obj_stop_timer(GUI_BASE(p));
        sprintf(battery_tx_str, "%d%%", battery_tx_val);
        gui_text_t *text = gui_text_create(GUI_BASE(p)->parent, 0, 180, 0, 40, 502);
        gui_text_set(text, battery_tx_str, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(battery_tx_str),
                     40);
        gui_text_type_set(text, NOTO_SANS_SC_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, VERTICAL_LEFT_MID);
        gui_text_rendermode_set(text, 2);
        gui_obj_create_timer(GUI_BASE(text), 30, true, battery_update_cb);
        animation_cnt = 0;
    }
}

static void charging_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 400);
    gui_view_switch_on_event(view, "fps_show_view", SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);

    animation_cnt = 0;

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", BG_CHARGE_BIN, 0, 0, 0,
                                            0);
    gui_img_set_mode(bg, IMG_COVER_MODE);
    gui_img_set_opacity(bg, 1);

    float focus_x = 36.f;
    float focus_y = 87.f / 2;
    gui_img_t *charging_icon = gui_img_create_from_mem(parent, "icon",
                                                       ICON_CHARGING_BIN,
                                                       ICON_CHARGING_START_X + focus_x, ICON_CHARGING_Y + focus_y, 0, 0);
    gui_img_set_focus(charging_icon, focus_x, focus_y);
    gui_img_set_opacity(charging_icon, 1);
    gui_img_set_mode(bg, IMG_SRC_OVER_MODE);
    gui_img_set_quality(charging_icon, true);
    gui_obj_create_timer(GUI_BASE(bg), 10, true, entrance_animation);
}