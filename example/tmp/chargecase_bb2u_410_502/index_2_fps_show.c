/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_view_macro.h"
#include "gui_text.h"
#include "gui_rect.h"


/* FPS */
static char fps[10];
static char widget_count_string[20];
static char mem_string[20];
static char low_mem_string[20];
void fps_create(void *parent);
static void fps_show_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch in\n");
    gui_view_switch_on_event(view, "anti_aliasing_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, "vector_graphic_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    // fps_create(view);
    extern void lock_view_design(gui_view_t *view);
    lock_view_design(view);
}

static void fps_show_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("fps_show_view switch out\n");
}

GUI_VIEW_INSTANCE("fps_show_view", 0, fps_show_switch_in, fps_show_switch_out);
static void gui_fps_cb(void *p)
{
    int fps_num = gui_fps();
    gui_obj_t *fps_rect = GUI_BASE(p);
    sprintf(fps, "FPS:%d", fps_num);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_fps, "t_fps", fps_rect);
    gui_text_content_set((gui_text_t *)t_fps, fps, strlen(fps));
    int widget_count_number = gui_get_obj_count();
    sprintf(widget_count_string, "WIDGETS:%d", widget_count_number);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(widget_count, "widget_count", fps_rect);
    gui_text_content_set((gui_text_t *)widget_count, widget_count_string, strlen(widget_count_string));
    uint32_t mem_number =  gui_mem_used();
    uint32_t low_mem_number =  gui_low_mem_used();
    sprintf(mem_string, "RAM:%dKB", (int)mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(mem, "mem", fps_rect);
    gui_text_content_set((gui_text_t *)mem, mem_string, strlen(mem_string));
    sprintf(low_mem_string, "lowRAM:%dKB", (int)low_mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(low_mem, "low_mem", fps_rect);
    gui_text_content_set((gui_text_t *)low_mem, low_mem_string, strlen(low_mem_string));
}


void fps_create(void *parent)
{
    char *text;
    int font_size = 20;
    gui_rounded_rect_t *fps_rect = gui_rect_create(parent, "rect_fps",
                                                   gui_get_screen_width() / 2 - 140 / 2, 0, 140,
                                                   70, 0,
                                                   APP_COLOR_GRAY_OPACITY(150));
    gui_obj_create_timer(GUI_BASE(fps_rect), 10, true, gui_fps_cb);
    sprintf(fps, "FPS:%d", gui_fps());
    text = fps;
    gui_text_t *t_fps = gui_text_create(fps_rect, "t_fps", 10, 0, gui_get_screen_width(), font_size);
    gui_text_set(t_fps, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(t_fps, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *widget_count = gui_text_create(fps_rect, "widget_count", 10, 16, gui_get_screen_width(),
                                               font_size);
    gui_text_set(widget_count, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(widget_count, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *mem = gui_text_create(fps_rect, "mem", 10, 16 * 2, gui_get_screen_width(), font_size);
    gui_text_set(mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(mem, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *low_mem = gui_text_create(fps_rect, "low_mem", 10, 16 * 3, gui_get_screen_width(),
                                          font_size);
    gui_text_set(low_mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(low_mem, HEADING_1_BIN, FONT_SRC_MEMADDR);
}





