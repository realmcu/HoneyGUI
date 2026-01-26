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
#include "gui_scroll_text.h"
#include "gui_list.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME NOTIFICATION_VIEW

#define LIST_Y  60

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void notification_view_design(gui_view_t *view);
static void clear(gui_view_t *view);

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
    .on_switch_in = notification_view_design,
};

static int16_t list_offset_his = 0;
static uint8_t scroll_bar_h = 0;
static gui_img_t *bg_note = NULL;
static bool detail = 0;
static uint8_t message_index = 0;
static const char preview_off_str[] = "You got a message on your mobile device.";

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
    if (detail)
    {
        detail = 0;
        msg_2_regenerate_current_view();
    }
    else
    {
        list_offset_his = 0;
        gui_view_switch_direct(current_view, INFORM_CENTER_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
    }
}

static void note_timer_cb(void *p)
{
    GUI_UNUSED(p);
    detail = 1;
    msg_2_regenerate_current_view();
}

static void switch_page(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    if (GUI_BASE(obj)->y < -28) { return; }
    gui_obj_move(GUI_BASE(bg_note), 0, GUI_BASE(obj)->y + LIST_Y);
    gui_obj_hidden((void *)bg_note, false);
    gui_obj_create_timer(GUI_BASE(obj), 800, true, note_timer_cb);
    message_index = ((gui_list_note_t *)obj)->index;
}

static void notification_note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint8_t index = note->index;
    gui_color_t fg_1_color;
    if (theme_bg_white)
    {
        fg_1_color = FG_1_LIGHT;
    }
    else
    {
        fg_1_color = FG_1_DARK;
    }
    gui_img_t *img = gui_img_create_from_mem(note, 0, ICON_MESSAGE_BIN, 8, 8, 0, 0);
    gui_img_a8_recolor(img, fg_1_color.color.argb_full);

    char *text;
    if (!f_status.message_preview || language_index != ENGLISH)
    {
        text = (char *)preview_off_str;
    }
    else
    {
        text = notification_content[index].content;
    }
    int font_size = 30;
    gui_text_t *t = gui_text_create(note, 0, 56, 0, 244, 56);
    gui_text_set(t, text, GUI_FONT_SRC_BMP, fg_1_color, 20, font_size);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MID_LEFT);
    gui_obj_add_event_cb(note, switch_page, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void detail_note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    gui_color_t fg_1_color;
    if (theme_bg_white)
    {
        fg_1_color = FG_1_LIGHT;
    }
    else
    {
        fg_1_color = FG_1_DARK;
    }
    char *text;
    if (!f_status.message_preview || language_index != ENGLISH)
    {
        text = (char *)preview_off_str;
    }
    else
    {
        text = notification_content[message_index].content;
    }
    uint16_t text_length = strlen(text);
    int font_size = 30;
    gui_text_t *t = gui_text_create(note, 0, 12, 0, 288, note->base.h);
    gui_text_set(t, text, GUI_FONT_SRC_BMP, fg_1_color, text_length, font_size);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    if (!detail)
    {
        list_offset_his = list->offset;
    }
    int range = list->base.h - list->total_length;
    int t_y = list->offset * (SCROLL_BAR_BG_H - scroll_bar_h) / range;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void detail_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t scroll_color;
    bg_note = gui_img_create_from_mem(parent, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    gui_list_t *list = gui_list_create(parent, 0, 0, 40, 0, 0, 200, 0,
                                       VERTICAL, detail_note_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 1);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_320_40_BIN, 0, 0, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
        scroll_color = FG_BLACK;
    }
    else
    {
        gui_img_a8_recolor(mask, SCREEN_BG_DARK.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_DARK.color.argb_full);
        font_color = FG_1_DARK;
        scroll_color = FG_WHITE;
    }
    gui_img_a8_mix_alpha(bg_note, bg_note->fg_color_set >> 24);

    char *text = (char *)notification_content[message_index].source;
    uint16_t text_length = strlen(text);
    gui_text_t *title = gui_text_create(parent, 0, 0, 0, 320, 40);
    gui_text_set(title, text, GUI_FONT_SRC_BMP, font_color, text_length, 30);
    gui_text_type_set(title, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 40);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(icon_back, font_color.color.argb_full);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    scroll_bar_h = SCROLL_BAR_L_H;
    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 72, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 72, 0, 0);
    gui_img_a8_recolor(scrollbar_bg, scroll_color.color.argb_full);
    gui_img_a8_recolor(scrollbar, scroll_color.color.argb_full);
}

static void notification_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t scroll_color;
    bg_note = gui_img_create_from_mem(parent, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    gui_list_t *list = gui_list_create(parent, 0, 0, LIST_Y, 0, 0, 56, 0,
                                       VERTICAL, notification_note_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, f_status.notification ? message_num_val : 0);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 20);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
        scroll_color = FG_BLACK;
    }
    else
    {
        gui_img_a8_recolor(mask, SCREEN_BG_DARK.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_DARK.color.argb_full);
        font_color = FG_1_DARK;
        scroll_color = FG_WHITE;
    }
    gui_img_a8_mix_alpha(bg_note, bg_note->fg_color_set >> 24);

    gui_text_t *title = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(title, (void *)page_name_array[language_index][NOTIFICATION], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen(page_name_array[language_index][NOTIFICATION]), 30);
    gui_text_type_set(title, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_img_a8_recolor(icon_back, font_color.color.argb_full);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (f_status.notification && message_num_val > 2)
    {
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
        void *img_data;
        if (message_num_val < 4)
        {
            scroll_bar_h = SCROLL_BAR_L_H;
            img_data = SCROLLBAR_L_BIN;
        }
        else if (message_num_val < 8)
        {
            scroll_bar_h = SCROLL_BAR_M_H;
            img_data = SCROLLBAR_M_BIN;
        }
        else
        {
            scroll_bar_h = SCROLL_BAR_S_H;
            img_data = SCROLLBAR_S_BIN;
        }
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 72, 0, 0);
        gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, img_data, 310, 72, 0, 0);
        gui_img_a8_recolor(scrollbar_bg, scroll_color.color.argb_full);
        gui_img_a8_recolor(scrollbar, scroll_color.color.argb_full);
    }
}

static void notification_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);
    gui_obj_t *parent = GUI_BASE(view);

    if (detail)
    {
        detail_design(parent);
    }
    else
    {
        notification_design(parent);
    }
}