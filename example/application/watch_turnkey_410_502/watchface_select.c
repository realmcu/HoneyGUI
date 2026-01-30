/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_list.h"
#include "gui_text.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "watchface_select_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void watchface_select_design(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, watchface_select_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list = NULL;

static const char *watchface_names[] =
{
    "Big Number",
    "Sport",
    "Video",
    "Add Watchface"
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void switch_app_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    int16_t index = *((int16_t *)param) - 1;
    switch (index)
    {
    case 0:
        {
            current_watchface_type = WATCHFACE_BIG_NUM;
            gui_view_switch_direct(gui_view_get_current(), "watchface_view",
                                   SWITCH_OUT_ANIMATION_ZOOM,
                                   SWITCH_IN_ANIMATION_ZOOM);
        }
        break;
    case 1:
        {
            current_watchface_type = WATCHFACE_SPORT;
            gui_view_switch_direct(gui_view_get_current(), "watchface_view",
                                   SWITCH_OUT_ANIMATION_ZOOM,
                                   SWITCH_IN_ANIMATION_ZOOM);
        }
        break;
    case 2:
        {
            current_watchface_type = WATCHFACE_VIDEO;
            gui_view_switch_direct(gui_view_get_current(), "watchface_view",
                                   SWITCH_OUT_ANIMATION_ZOOM,
                                   SWITCH_IN_ANIMATION_ZOOM);
        }
        break;
    case 3:
        gui_log("developing!!!\n");
        break;

    default:
        break;
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    int16_t index = ((gui_list_note_t *)obj)->index - 1;

    void *watchface_icons[] = {WATCHFACE_BIG_NUM_BIN, WATCHFACE_SPORT_BIN, WATCHFACE_VIDEO_BIN, ADD_WATCHFACE_BIN,};

    uint16_t offset_x = 0;
    uint16_t offset_y = 110;
    if (index >= 0 && index < 4)
    {
        gui_img_t *img = gui_img_create_from_mem(obj, "BIG_NUM", watchface_icons[index], offset_x, offset_y,
                                                 0, 0);

        gui_text_t *label = gui_text_create(obj, "watchface_big_num", 0, offset_y - 30, 200,
                                            40);
        gui_text_set(label, (void *)watchface_names[index], GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(watchface_names[index]), 20);
        gui_text_type_set(label, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(label, CENTER);

        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED,
                             &(((gui_list_note_t *)obj)->index));
    }
}
static void watchface_select_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);

    int space = 10;
    int length = 200;
    list = gui_list_create(parent, "list", -100, 0, 0, 0, length, space, HORIZONTAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_auto_align(list, true);
    gui_list_set_out_scope(list, 50);
    list->total_length += 100; //for auto align
    gui_list_set_offset(list, -((uint8_t)current_watchface_type) * (length + space));
}