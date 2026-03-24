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
#define CURRENT_VIEW_NAME "test_font_noto_v2_rendering"

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;

static void ui_noto_v2(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = ui_noto_v2,
    .on_switch_out = NULL,
};

static char *test_lines[] =
{
    "The quick brown fox",
    "jumps over the lazy dog.",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "abcdefghijklmnopqrstuvwxyz",
    "0123456789 !@#$%^&*()",
    "Hello, HoneyGUI V2 Font!",
};
#define LINE_COUNT (sizeof(test_lines) / sizeof(test_lines[0]))

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static void ui_noto_v2(gui_view_t *view)
{
    struct gui_dispdev *dc = gui_get_dc();
    uint32_t font_size = 32;

    gui_color_t colors[] =
    {
        APP_COLOR_WHITE,
        APP_COLOR_RED,
        gui_rgba(0, UINT8_MAX, 0, 0xC0),
        gui_rgba(0, 0, UINT8_MAX, 0x80),
        gui_rgba(UINT8_MAX, UINT8_MAX, 0, 0xFF),
        gui_rgba(0, UINT8_MAX, UINT8_MAX, 0xFF),
    };

    for (uint32_t i = 0; i < LINE_COUNT; i++)
    {
        gui_text_t *t = gui_text_create(view, "noto_v2", 10,
                                        font_size * i * 2, dc->screen_width - 20, 0);
        gui_text_set(t, test_lines[i], GUI_FONT_SRC_BMP,
                     colors[i % LINE_COUNT], strlen(test_lines[i]), font_size);
        gui_text_type_set(t, fontnotov2, FONT_SRC_MEMADDR);
    }

    /* label */
    gui_text_t *label = gui_text_create(view, "label", 0,
                                        dc->screen_height - font_size, dc->screen_width, 0);
    gui_text_set(label, (void *)descriptor.name, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, strlen(descriptor.name), font_size);
    gui_text_type_set(label, fontnotov2, FONT_SRC_MEMADDR);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void text_noto_v2_rendering_test(void)
{
    gui_view_create(gui_obj_get_root(), descriptor.name, 0, 0, 0, 0);
}
