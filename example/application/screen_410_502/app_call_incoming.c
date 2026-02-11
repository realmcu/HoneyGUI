/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "app_main_watch.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_win.h"
#include "gui_text.h"
#include "root_image/ui_resource.h"
#include "watch_adapt.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "call_incoming_view"

#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define DIAL_LENGTH 16
/*============================================================================*
*                             Types
*============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_call_incoming_design(gui_view_t *view);
void app_calling_design(gui_obj_t *parent);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *last_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_call_incoming_design,
    .on_switch_out = NULL,

    .keep = false,
};

static gui_win_t *win_call_in = NULL;

static char call_number[DIAL_LENGTH + 1] = "1234567890";
static char call_time_str[10] = "00:00:00";
static uint32_t call_time = 0;

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

static void format_time(uint32_t seconds, char *str)
{
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;
    sprintf(str, "%02u:%02u:%02u", h, m, s);
}

static void click_speaker_cb(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    gui_call_t *call = gui_get_call();
    bool status = true;
    if (img->src.data == SPEAKER_ON_ICON_BIN)
    {
        img->src.data = SPEAKER_OFF_ICON_BIN;
        status = false;
    }
    else
    {
        img->src.data = SPEAKER_ON_ICON_BIN;
    }
    if (call && call->call_set_speaker)
    {
        call->call_set_speaker(status);
    }
}

static void click_mic_cb(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    gui_call_t *call = gui_get_call();
    bool status = true;
    if (img->src.data == MIC_ON_ICON_BIN)
    {
        img->src.data = MIC_OFF_ICON_BIN;
        status = false;
    }
    else
    {
        img->src.data = MIC_ON_ICON_BIN;
    }
    if (call && call->call_set_mic)
    {
        call->call_set_mic(status);
    }
}

static void click_call_end_cb(void *obj, gui_event_t *e)
{
    (void)e;

    gui_call_t *call = gui_get_call();
    if (call && call->call_end)
    {
        call->call_end();
    }

    if (current_view)
    {
        gui_view_switch_direct(current_view, last_view->name, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
    else
    {
        gui_obj_tree_free_async(GUI_BASE(obj)->parent);
    }
}

static void click_call_incoming_cb(void *obj, gui_event_t *e)
{
    (void)obj;
    (void)e;

    gui_call_t *call = gui_get_call();
    if (call && call->call_start)
    {
        call->call_start();
    }

    app_calling_design(GUI_BASE(current_view));
    gui_obj_hidden(GUI_BASE(win_call_in), true);
}

static void win_scroll_cb(void *obj, gui_event_t *e)
{
    (void)obj;
    (void)e;

    // Disable view scroll
}

static void on_calling(void *p)
{
    call_time += 1;
    format_time(call_time, call_time_str);
    gui_text_t *t = GUI_TYPE(gui_text_t, p);
    gui_text_content_set(t, call_time_str, strlen(call_time_str));
}

static void scale_img(void *p)
{
    gui_img_t *img = (gui_img_t *)p;
    static float scale = 1.0f;
    static float positive = 1.0f;
    if (scale >= 1.2f)
    {
        positive = -1.0f;
    }
    else if (scale <= 0.8f)
    {
        positive = 1.0f;
    }
    scale += 0.01f * positive;
    gui_img_scale(img, scale, scale);
}

static void app_call_incoming_design(gui_view_t *view)
{
    if (gui_view_get_current())
    {
        last_view = gui_view_get_current()->descriptor;
        gui_view_set_animate_step(view, 1000);
    }

    // call
    win_call_in = gui_win_create(view, "win_call_in", 0, 0, 0, 0);

    gui_text_t *text = gui_text_create(win_call_in, 0, 0, 50, SCREEN_WIDTH, 60);
    gui_text_set(text, "Call", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Call"), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    text = gui_text_create(win_call_in, 0, 0, 110, SCREEN_WIDTH, 50);
    gui_text_set(text, call_number, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(call_number), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);


    gui_img_t *img_person = gui_img_create_from_mem(win_call_in, 0, CALL_PERSON_ICON_BIN, 155 + 50,
                                                    230 + 50, 0, 0);
    gui_img_set_focus(img_person, 50, 50);
    gui_img_set_quality(img_person, true);
    gui_obj_create_timer(GUI_BASE(img_person), 20, true, scale_img);

    gui_img_t *img_call_reject = gui_img_create_from_mem(win_call_in, 0, CALL_REJECT_ICON_BIN, 90,
                                                         380, 0, 0);
    gui_img_set_quality(img_call_reject, true);

    gui_img_t *img_call = gui_img_create_from_mem(win_call_in, 0, CALL_ICON_BIN, 260,
                                                  380, 0, 0);
    gui_img_set_quality(img_call, true);

    gui_obj_add_event_cb(img_call_reject, click_call_end_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(img_call, click_call_incoming_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_call_t *call = gui_get_call();
    if (call && call->call_set_mic)
    {
        call->call_set_mic(true); // set mic on
    }
    if (call && call->call_set_speaker)
    {
        call->call_set_speaker(true); // set speaker on
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void app_calling_design(gui_obj_t *parent)
{
    // call
    gui_win_t *win_calling = gui_win_create(parent, "win_calling", 0, 0, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(win_calling), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
                         NULL); // Disable view horizontal scroll
    gui_obj_add_event_cb(GUI_BASE(win_calling), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_VERTICAL,
                         NULL); // Disable view vertical scroll

    gui_text_t *text = gui_text_create(win_calling, 0, 0, 50, SCREEN_WIDTH, 60);
    gui_text_set(text, call_number, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(call_number), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    gui_img_t *img_person = gui_img_create_from_mem(win_calling, 0, CALL_PERSON_ICON_BIN, 155,
                                                    120, 0, 0);
    gui_img_set_quality(img_person, true);

    call_time = 0;
    format_time(call_time, call_time_str);
    text = gui_text_create(win_calling, 0, 0, 250, SCREEN_WIDTH, 50);
    gui_text_set(text, call_time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(call_time_str), 46);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);
    gui_obj_create_timer(GUI_BASE(text), 1000, true, on_calling);
    gui_obj_start_timer(GUI_BASE(text));

    gui_img_t *img_speaker = gui_img_create_from_mem(win_calling, 0, SPEAKER_ON_ICON_BIN, 50,
                                                     380, 0, 0);
    gui_img_t *img_call_end = gui_img_create_from_mem(win_calling, 0, CALL_REJECT_ICON_BIN, 176,
                                                      410, 0, 0);
    gui_img_t *img_mic = gui_img_create_from_mem(win_calling, 0, MIC_ON_ICON_BIN, 300,
                                                 380, 0, 0);
    gui_img_set_quality(img_speaker, true);
    gui_img_set_quality(img_call_end, true);
    gui_img_set_quality(img_mic, true);
    gui_obj_add_event_cb(img_speaker, click_speaker_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(img_call_end, click_call_end_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(img_mic, click_mic_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

void gui_update_call_number(char *number)
{
    snprintf(call_number, sizeof(call_number), "%s", number);
}