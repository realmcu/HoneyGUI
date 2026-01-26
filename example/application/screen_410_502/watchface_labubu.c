/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "labubu_digital_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void create_watchface_labubu(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = create_watchface_labubu,
    .keep = false,
};

void *text_num_black_array[11] = {0};

static gui_video_t *video = NULL;

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

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static void time_update_cb(void *p)
{
    (void)p;
    extern struct tm *timeinfo;
    if (!timeinfo)
    {
        return;
    }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_black_array[timeinfo->tm_hour / 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_black_array[timeinfo->tm_hour % 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_single);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal,
                           text_num_black_array[timeinfo->tm_min / 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_black_array[timeinfo->tm_min % 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_single);
}

static void create_watchface_labubu(gui_view_t *view)
{
    if (text_num_black_array[0] == NULL)
    {
        void *text_num_black[11] =
        {
            UI_TEXT_0_B_BIN,
            UI_TEXT_1_B_BIN,
            UI_TEXT_2_B_BIN,
            UI_TEXT_3_B_BIN,
            UI_TEXT_4_B_BIN,
            UI_TEXT_5_B_BIN,
            UI_TEXT_6_B_BIN,
            UI_TEXT_7_B_BIN,
            UI_TEXT_8_B_BIN,
            UI_TEXT_9_B_BIN,
            UI_TEXT_COLON_B_BIN,
        };
        memcpy(text_num_black_array, text_num_black, sizeof(text_num_black));
    }

    gui_win_t *win = gui_win_create(view, "win", 0, 0, 0, 0);
    video = gui_video_create_from_mem(win, "labubu", LABUBU_MJPG, 0, 0, 410,
                                      502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, 0);
    //time
    {
        int text_w = 40;
        gui_img_t *img = gui_img_create_from_mem(win, "watch_hour_decimal", text_num_black_array[0],
                                                 120, 50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_hour_single", text_num_black_array[0],
                                      120 + text_w, 50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "colon", text_num_black_array[10],
                                      120 + text_w * 2 + 5, 50 + 5, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_decimal", text_num_black_array[0],
                                      120 + text_w * 2 + 20, 50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_single", text_num_black_array[0],
                                      120 + text_w * 3 + 20, 50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    gui_obj_create_timer(GUI_BASE(win), 30000, true, time_update_cb);

    gui_view_t *pre_view = gui_view_get_current();
    if (pre_view && strcmp(GUI_BASE(gui_view_get_current())->name, "menu_view") == 0)
    {
        gui_obj_hidden(&(gui_view_get_current()->base), true);
        // gui_obj_create_timer(GUI_BASE(view), 17, true, return_timer_cb);
        gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    else
    {
        gui_view_switch_on_event(view, watchface_view->name, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_TOUCH_CLICKED);
        gui_view_switch_on_event(view, watchface_view->name, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
}