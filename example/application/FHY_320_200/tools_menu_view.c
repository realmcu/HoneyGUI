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
typedef struct note_design_param
{
    void **page_design;
    uint32_t **img_data_array;
} note_design_param_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME TOOLS_MENU_VIEW

#define LIST_Y  60

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void tools_menu_view_design(gui_view_t *view);
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
    .on_switch_in = tools_menu_view_design,
    .on_switch_out = clear,
};

static note_design_param_t *design_p = NULL;
static int16_t list_offset_his = 0;
static gui_img_t *bg_note = NULL;
static const char *title[] =
{
    "Tools",
    "Outils",
    "Werkzeuge",
    "Herramientas",
    "Strumenti",
    "Gereedschap",
    "Ferramentas",
    "Инструменты",
    "工具",
    "ツール",
    "툴",
    "เครื่องมือ",
    "Công cụ"
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

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(current_view, MENU_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void note_timer_cb(void *p)
{
    gui_obj_stop_timer(p);
    gui_view_switch_direct(current_view, DETAIL_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void switch_page(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    if (GUI_BASE(obj)->y < -28) { return; }
    detail_page_design_func = param;
    gui_obj_move(GUI_BASE(bg_note), 0, GUI_BASE(obj)->y + LIST_Y);
    gui_obj_hidden((void *)bg_note, false);
    gui_obj_create_timer(GUI_BASE(obj), 800, true, note_timer_cb);
}

static void note_design(gui_obj_t *obj, void *p)
{
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;

    gui_img_t *img = gui_img_create_from_mem(note, 0, design_p->img_data_array[index], 8, 8, 0, 0);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_img_a8_recolor(img, FG_THEME2_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_img_a8_recolor(img, FG_THEME2_DARK.color.argb_full);
        font_color = FG_1_DARK;
    }
    char *text = (char *)page_name_array[language_index][index + 11];
    int font_size = 30;
    uint16_t text_length = strlen(text);
    if (text_length < 23)
    {
        gui_text_t *t = gui_text_create(note, 0, 56, 15, 244, 56);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, LEFT);
    }
    else
    {
        gui_scroll_text_t *t = gui_scroll_text_create(note, 0, 56, 15, 244, 56);
        gui_scroll_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_scroll_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(t, SCROLL_X, 260, 260, 5000, 0);
    }
    if (design_p->page_design[index] != NULL)
    {
        gui_obj_add_event_cb(note, switch_page, GUI_EVENT_TOUCH_CLICKED,
                             design_p->page_design[index]);
    }
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
    int range = list->base.h - list->total_length;
    int t_y = list_offset_his * (SCROLL_BAR_BG_H - SCROLL_BAR_L_H) / range;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void tools_menu_view_design(gui_view_t *view)
{
    if (gui_view_get_current() && !strcmp(gui_view_get_current()->descriptor->name, MENU_VIEW))
    {
        list_offset_his = 0;
    }
    gui_view_set_animate_step(view, 10);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    bg_note = gui_img_create_from_mem(parent, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    uint32_t *img_data_array[] =
    {
        ICON_TIMER_BIN,
        ICON_FLASHLIGHT_BIN,
        ICON_SILENTNOW_BIN,
        ICON_VOLUME_UNIT_METER_BIN,
        ICON_ICON_APP_BIN,
    };

    int array_size = sizeof(img_data_array) / sizeof(img_data_array[0]);
    void *page_design[] =
    {
        page_timer_design,
        page_flashlight_design,
        page_silentnow_design,
        page_volume_unit_meter_design,
        page_qrcode_design,
    };
    design_p = gui_malloc(sizeof(note_design_param_t));
    void **func_cb = gui_malloc(array_size * sizeof(void *));
    memset(func_cb, 0, array_size * sizeof(void *));
    memcpy(func_cb, page_design, sizeof(page_design));
    uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
    memcpy(data_array, img_data_array, array_size * sizeof(void *));
    design_p->page_design = func_cb;
    design_p->img_data_array = data_array;
    gui_list_t *list = gui_list_create(view, 0, 0, LIST_Y, 0, 0, 56, 0,
                                       VERTICAL, note_design, design_p, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, array_size);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);

    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        gui_img_a8_recolor(mask, SCREEN_BG_DARK.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_DARK.color.argb_full);
        font_color = FG_1_DARK;
    }
    gui_img_a8_mix_alpha(bg_note, bg_note->fg_color_set >> 24);

    gui_text_t *text = gui_text_create(parent, 0, 0, 0, 320, 60);;
    gui_text_set(text, (void *)title[language_index], GUI_FONT_SRC_BMP, font_color,
                 strlen(title[language_index]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 62, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 62, 0, 0);
    gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(icon_back, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
    }
}

static void clear(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (design_p)
    {
        gui_free(design_p->page_design);
        gui_free(design_p->img_data_array);
        gui_free(design_p);
        design_p = NULL;
    }
}