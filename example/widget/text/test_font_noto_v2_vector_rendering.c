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
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "test_font_noto_v2_vector_rendering"

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *left_view = NULL;

static void ui_noto_v2_vector(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = ui_noto_v2_vector,
    .on_switch_out = NULL,
};

static char *multiline_text =
    "The quick brown fox\n"
    "jumps over the lazy dog.\n"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
    "abcdefghijklmnopqrstuvwxyz\n"
    "0123456789 !@#$%^&*()\n"
    "Hello, Vector V2 Font!";

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
    left_view = gui_view_descriptor_get("test_font_noto_v2_rendering");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void ui_noto_v2_vector(gui_view_t *view)
{
    struct gui_dispdev *dc = gui_get_dc();
    uint32_t font_size = 32;

    /* multi-line text block */
    gui_text_t *t = gui_text_create(view, "noto_v2_vec", 10, 0,
                                    dc->screen_width - 20, dc->screen_height - font_size * 2);
    gui_text_set(t, multiline_text, GUI_FONT_SRC_TTF,
                 APP_COLOR_WHITE, strlen(multiline_text), font_size);
    gui_text_type_set(t, fontnotov2vec, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);

    /* label at bottom */
    char *label_str = "Vector V2  <<< swipe >>>";
    gui_text_t *label = gui_text_create(view, "label", 0,
                                        dc->screen_height - font_size * 2,
                                        dc->screen_width, font_size * 2);
    gui_text_set(label, label_str, GUI_FONT_SRC_TTF,
                 gui_rgba(UINT8_MAX, UINT8_MAX, 0, UINT8_MAX),
                 strlen(label_str), font_size);
    gui_text_type_set(label, fontnotov2vec, FONT_SRC_MEMADDR);
    gui_text_mode_set(label, CENTER);

    /* swipe right -> bitmap V2 view */
    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_RIGHT);
}
