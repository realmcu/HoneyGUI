/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"

#include "font_mem.h"
#include "font_custom.h"

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    gui_log("font_load\n");
    gui_font_mem_load(text, rect);
}

static void font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    gui_log("font_draw\n");
    gui_font_mem_draw(text, rect);
}

static void font_unload(gui_text_t *text)
{
    GUI_UNUSED(text);

    gui_log("font_unload\n");
    gui_font_mem_unload(text);
}

static void font_destroy(gui_text_t *text)
{
    GUI_UNUSED(text);

    gui_log("font_destroy\n");
    gui_font_mem_destroy(text);
}

static void free_widget_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    gui_obj_tree_free((gui_obj_t *)e->user_data);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Custom font rendering demo
 */
/* gui custom font rendering demo start*/
void custom_font_rendering_demo(void)
{
    gui_log("custom_font_rendering_demo\n");
    gui_font_custom_set_callbacks(font_load, font_draw, font_unload, font_destroy);

    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, "HoneyGUI", GUI_FONT_SRC_CUS, APP_COLOR_WHITE, 8, 32);
    gui_text_type_set(text, font32b1, FONT_SRC_MEMADDR);

    gui_text_t *free_text = gui_text_create(gui_obj_get_root(), "free_text", 0, 430, 0, 48);
    gui_text_set(free_text, "DESTROY", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, 7, 48);
    gui_text_type_set(free_text, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(free_text, RIGHT);
    gui_obj_add_event_cb(free_text, free_widget_cb, GUI_EVENT_TOUCH_CLICKED, text);
}
/* gui custom font rendering demo end*/
