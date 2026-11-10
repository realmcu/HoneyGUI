/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"

/*============================================================================*
 *                            Variables
 *============================================================================*/

static gui_scroll_text_t *loop_x;
static gui_scroll_text_t *loop_x_reverse;
static gui_scroll_text_t *loop_y;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void loop_stop_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    GUI_UNUSED(obj);
    gui_scroll_text_stop(loop_x);
    gui_scroll_text_stop(loop_x_reverse);
    gui_scroll_text_stop(loop_y);
}

static void loop_reset_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    GUI_UNUSED(obj);
    gui_scroll_text_reset(loop_x);
    gui_scroll_text_reset(loop_x_reverse);
    gui_scroll_text_reset(loop_y);
}

static void loop_pause_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    GUI_UNUSED(obj);
    gui_scroll_text_pause(loop_x);
    gui_scroll_text_pause(loop_x_reverse);
    gui_scroll_text_pause(loop_y);
}

static void loop_resume_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    GUI_UNUSED(obj);
    gui_scroll_text_resume(loop_x);
    gui_scroll_text_resume(loop_x_reverse);
    gui_scroll_text_resume(loop_y);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void text_font_scroll_loop_test(void)
{
    char *test_text =
        "Everyone has the right to freedom of thought, conscience and religion.";

    /* SCROLL_X loop */
    loop_x = gui_scroll_text_create(gui_obj_get_root(), "loop_x", 40, 0, 400, 32);
    gui_scroll_text_set(loop_x, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text), 32);
    gui_scroll_text_scroll_set(loop_x, SCROLL_X, 0, 0, 8000, 0);
    gui_scroll_text_type_set(loop_x, font32b2, FONT_SRC_MEMADDR);
    gui_scroll_text_loop_set(loop_x, true, 32);
    gui_scroll_text_scroll_pause_set(loop_x, 2000);

    /* SCROLL_X_REVERSE loop */
    loop_x_reverse = gui_scroll_text_create(gui_obj_get_root(), "loop_x_rev", 40, 40, 400, 32);
    gui_scroll_text_set(loop_x_reverse, test_text, GUI_FONT_SRC_BMP, APP_COLOR_CYAN, strlen(test_text),
                        32);
    gui_scroll_text_scroll_set(loop_x_reverse, SCROLL_X_REVERSE, 0, 0, 8000, 0);
    gui_scroll_text_type_set(loop_x_reverse, font32b2, FONT_SRC_MEMADDR);
    gui_scroll_text_loop_set(loop_x_reverse, true, 32);

    /* SCROLL_Y loop */
    loop_y = gui_scroll_text_create(gui_obj_get_root(), "loop_y", 40, 100, 400, 60);
    gui_scroll_text_set(loop_y, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text), 32);
    gui_scroll_text_scroll_set(loop_y, SCROLL_Y, 0, 0, 8000, 0);
    gui_scroll_text_type_set(loop_y, font32b2, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(&loop_y->base, true);
    gui_scroll_text_loop_set(loop_y, true, 0);

    /* SCROLL_Y_REVERSE loop - TODO: not yet supported */

    /* control buttons */
    gui_text_t *stop = gui_text_create(gui_obj_get_root(), "stop", 100, 400, 0, 0);
    gui_text_set(stop, "STOP", GUI_FONT_SRC_BMP, APP_COLOR_TOMATO, 4, 32);
    gui_text_type_set(stop, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(stop, (gui_event_cb_t)loop_stop_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *reset = gui_text_create(gui_obj_get_root(), "reset", 100, 440, 0, 0);
    gui_text_set(reset, "RESET", GUI_FONT_SRC_BMP, APP_COLOR_TOMATO, 5, 32);
    gui_text_type_set(reset, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(reset, (gui_event_cb_t)loop_reset_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *pause = gui_text_create(gui_obj_get_root(), "pause", 300, 400, 0, 0);
    gui_text_set(pause, "PAUSE", GUI_FONT_SRC_BMP, APP_COLOR_CYAN, 5, 32);
    gui_text_type_set(pause, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(pause, (gui_event_cb_t)loop_pause_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *resume = gui_text_create(gui_obj_get_root(), "resume", 300, 440, 0, 0);
    gui_text_set(resume, "RESUME", GUI_FONT_SRC_BMP, APP_COLOR_CYAN, 6, 32);
    gui_text_type_set(resume, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(resume, (gui_event_cb_t)loop_resume_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}
