/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* gui list widget example start*/

#include "guidef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_img.h"
#include "gui_list.h"
#include "gui_obj_focus.h"

/* Use the following macro to check out the different style lists */
#define RUN_VERTICAL_CIRCLE_STYLE
// #define RUN_VERTICAL_CLASSIC_STYLE
// #define RUN_VERTICAL_FADE_STYLE

#define NOTE_NUM 8
#define SCREEN_W 480
#define SCREEN_H 480

int length = 100;

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_color_t color[] =
    {
        APP_COLOR_WHITE_OPACITY,
        APP_COLOR_GRAY,
        APP_COLOR_SILVER_OPACITY(150),
        APP_COLOR_WHITE,
        APP_COLOR_GRAY_OPACITY(150),
        APP_COLOR_RED,
        APP_COLOR_GREEN,
        APP_COLOR_BLUE,
    };
    uint16_t index = ((gui_list_note_t *)obj)->index % (sizeof(color) / sizeof(gui_color_t));

    int16_t w = 0;
    int16_t h = 0;
#ifdef RUN_VERTICAL_CIRCLE_STYLE
    w = SCREEN_W;
    h = length;
#endif
#ifdef RUN_VERTICAL_CLASSIC_STYLE
    w = SCREEN_W;
    h = SCREEN_H;
#endif
#ifdef RUN_VERTICAL_FADE_STYLE
    w = SCREEN_W;
    h = SCREEN_H - 20;
#endif

    /* Solid-colour note. Geometry is not part of nano, so the tile is an image
     * built in place instead of a rounded rect. */
    gui_img_t *tile = gui_img_create_from_mem(obj, "note", gui_rle_solid_color_create(w, h,
                                              color[index]),
                                              0, 0, w, h);
    GUI_UNUSED(tile);
}

static void list_event_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    GUI_UNUSED(obj);
    gui_list_t *list = (gui_list_t *)obj;
    static int8_t index = 0;
    if (strcmp(e->indev_name, "Home") == 0)
    {
        index += 2;
        index %= NOTE_NUM;
        gui_list_scroll_to_note(list, index);
    }
    else if (strcmp(e->indev_name, "Back") == 0)
    {
        index -= 2;
        index %= NOTE_NUM;
        gui_list_jump_to_note(list, index);
    }
}

static int app_init(void)
{
    uint16_t space = 10;

    /* VERTICAL CIRCLE STYLE */
#ifdef RUN_VERTICAL_CIRCLE_STYLE

    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, SCREEN_H, length, space,
                                       VERTICAL,
                                       note_design, NULL, false);
    gui_list_set_note_num(list, NOTE_NUM);
    gui_list_set_style(list, LIST_CIRCLE);
    gui_list_set_out_scope(list, 100);
    gui_list_set_circle_radius(list, SCREEN_H / 2);
    gui_list_set_auto_align(list, true);
    gui_list_enable_loop(list, true);
    gui_list_enable_scroll(list, false);
    gui_obj_add_event_cb(list, (gui_event_cb_t)list_event_cb, GUI_EVENT_KB_SHORT_PRESSED, NULL);
    gui_obj_focus_set(list);
    return 0;
#endif

    /* VERTICAL CLASSIC STYLE */
#ifdef RUN_VERTICAL_CLASSIC_STYLE
    int classic_length = SCREEN_H;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, classic_length, space,
                                       VERTICAL, note_design, NULL, false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 10);
    gui_list_set_auto_align(list, true);
    gui_list_set_offset(list, -classic_length * 5);
    gui_list_set_inertia(list, false);
    return 0;
#endif

    /* VERTICAL FADE STYLE */
#ifdef RUN_VERTICAL_FADE_STYLE
    int fade_length = SCREEN_H;
    gui_list_t *list = gui_list_create(gui_obj_get_root(), "list", 0, 0, 0, 0, fade_length, space,
                                       VERTICAL, note_design, NULL, false);
    gui_list_set_style(list, LIST_FADE);
    gui_list_set_note_num(list, 10);
    gui_list_set_auto_align(list, true);
    gui_list_set_inertia(list, false);
    gui_list_set_out_scope(list, fade_length / 2);
    gui_list_set_offset(list, -fade_length * 5);
    return 0;
#endif

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui list widget example end*/
