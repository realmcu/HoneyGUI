/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "test_font_assets.h"
#include "test_cases.h"

#define TEST_CASE_08_EXPECTED_RENDER_TIME_MS 60U
#define TEST_CASE_08_FONT_SIZE               32U

static char test_case_08_multiline_text[] =
    "The quick brown fox\n"
    "jumps over the lazy dog.\n"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
    "abcdefghijklmnopqrstuvwxyz\n"
    "0123456789 !@#$%^&*()\n"
    "Hello, HoneyGUI Typography!";

uint32_t test_case_08(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_log("test_case_08 start!\n");

    gui_obj_child_free(gui_obj_get_root());

    gui_text_t *text = gui_text_create(gui_obj_get_root(), "typo_vec", 10, 0,
                                       dc->screen_width - 20,
                                       dc->screen_height - TEST_CASE_08_FONT_SIZE * 2);
    gui_text_set(text, test_case_08_multiline_text, GUI_FONT_SRC_TTF,
                 APP_COLOR_WHITE, strlen(test_case_08_multiline_text),
                 TEST_CASE_08_FONT_SIZE);
    gui_text_type_set(text, (void *)test_font_noto_vector, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    char *label_text = "Vector V3  <<< swipe >>>";
    gui_text_t *label = gui_text_create(gui_obj_get_root(), "label", 0,
                                        dc->screen_height - TEST_CASE_08_FONT_SIZE * 2,
                                        dc->screen_width, TEST_CASE_08_FONT_SIZE * 2);
    gui_text_set(label, label_text, GUI_FONT_SRC_TTF,
                 gui_rgba(UINT8_MAX, UINT8_MAX, 0, UINT8_MAX),
                 strlen(label_text), TEST_CASE_08_FONT_SIZE);
    gui_text_type_set(label, (void *)test_font_noto_vector, FONT_SRC_MEMADDR);
    gui_text_mode_set(label, CENTER);
    gui_text_set_cache(label, true);

    gui_log("test_case_08 end!\n");

    return TEST_CASE_08_EXPECTED_RENDER_TIME_MS;
}
