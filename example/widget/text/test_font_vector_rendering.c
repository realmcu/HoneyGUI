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
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "test_font_vector_rendering"
#define FONT_NUM 4

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *left_view = NULL;
static const gui_view_descriptor_t *right_view = NULL;

static void ui_text_font(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = ui_text_font,
    .on_switch_out = NULL,
};
static char *text_string[FONT_NUM] =
{
    "abcdefg",
    "1234567",
    "!@#$%^&",
    "QWERTYU",
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    left_view = gui_view_descriptor_get("test_font_bitmap_normal_rendering");
    right_view = gui_view_descriptor_get("test_font_vector_rendering_benchmark");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void ui_text_font(gui_view_t *view)
{
    gui_color_t color[FONT_NUM];
    color[0] = APP_COLOR_WHITE;
    color[1] = APP_COLOR_RED;
    color[2] = APP_COLOR_GREEN;
    color[3] = APP_COLOR_BLUE;
    gui_text_t *text1[FONT_NUM];
    gui_text_t *text2[FONT_NUM];
    gui_text_t *text4[FONT_NUM];
    gui_text_t *text8[FONT_NUM];
    struct gui_dispdev *dc = gui_get_dc();
    uint32_t font_size = 32;
    for (int i = 0; i < FONT_NUM; i++)
    {
        text1[i] = gui_text_create(view, "text1", i % 2 * dc->screen_width / 2,
                                   font_size * (i / 2) + font_size * FONT_NUM * 0 / 2, 300, font_size);
        gui_text_set(text1[i], text_string[i], GUI_FONT_SRC_TTF, color[i], strlen(text_string[i]),
                     font_size);
        gui_text_type_set(text1[i], font32vb4, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text1[i], 1);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text2[i] = gui_text_create(view, "text2", i % 2 * dc->screen_width / 2,
                                   font_size * (i / 2) + font_size * FONT_NUM * 1 / 2, 300, font_size);
        gui_text_set(text2[i], text_string[i], GUI_FONT_SRC_TTF, color[i], strlen(text_string[i]),
                     font_size);
        gui_text_type_set(text2[i], font32vb4, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text1[i], 2);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text4[i] = gui_text_create(view, "text4", i % 2 * dc->screen_width / 2,
                                   font_size * (i / 2) + font_size * FONT_NUM * 2 / 2, 300, font_size);
        gui_text_set(text4[i], text_string[i], GUI_FONT_SRC_TTF, color[i], strlen(text_string[i]),
                     font_size);
        gui_text_type_set(text4[i], font32vb4, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text1[i], 4);
    }
    for (int i = 0; i < FONT_NUM; i++)
    {
        text8[i] = gui_text_create(view, "text8", i % 2 * dc->screen_width / 2,
                                   font_size * (i / 2) + font_size * FONT_NUM * 3 / 2, 300, font_size);
        gui_text_set(text8[i], text_string[i], GUI_FONT_SRC_TTF, color[i], strlen(text_string[i]),
                     font_size);
        gui_text_type_set(text8[i], font32vb4, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text1[i], 8);
    }

    gui_text_t *test_name = gui_text_create(view, "test_name", 0, dc->screen_height - font_size,
                                            dc->screen_width, font_size);
    gui_text_set(test_name, (void *)descriptor.name, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(descriptor.name), font_size);
    gui_text_type_set(test_name, font32vb4, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(test_name, 2);

    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view->name, SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/


