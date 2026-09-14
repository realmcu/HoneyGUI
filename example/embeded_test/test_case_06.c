/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_fb.h"
#include "gui_list.h"
#include "gui_obj.h"
#include "gui_rect.h"
#include "test_cases.h"
#include "tp_algo.h"

/*
 * Calibration mode: run on the EVB, then set this to
 * max(P99 * 1.20, P99 + 2 ms) from at least 30 complete runs.
 */
#define TEST_CASE_06_EXPECTED_RENDER_TIME_MS 20U
#define TEST_CASE_06_NOTE_COUNT  30U
#define TEST_CASE_06_NOTE_HEIGHT 68U
static gui_list_t *test_case_06_list;



static void test_case_06_note_design(gui_obj_t *obj, void *param)
{
    const uint16_t index = ((gui_list_note_t *)obj)->index;
    GUI_UNUSED(param);

    gui_rounded_rect_t *card = gui_rect_create(obj, "list_card", 12, 4,
                                               gui_get_screen_width() - 24,
                                               TEST_CASE_06_NOTE_HEIGHT - 8, 10,
                                               gui_rgba(30 + (index % 5) * 18,
                                                        70 + (index % 4) * 20,
                                                        120 + (index % 3) * 28, 255));
    // gui_rect_set_opacity(card, 230);
}

uint32_t test_case_06(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_log("test_case_06 start!\n");

    gui_obj_child_free(gui_obj_get_root());
    gui_rect_create(gui_obj_get_root(), "background", 0, 0,
                    dc->screen_width, dc->screen_height, 0,
                    gui_rgba(12, 16, 24, 255));

    test_case_06_list = gui_list_create(gui_obj_get_root(), "virtual_list",
                                        0, 0, dc->screen_width, dc->screen_height,
                                        TEST_CASE_06_NOTE_HEIGHT, 8, VERTICAL,
                                        test_case_06_note_design, NULL, false);
    gui_list_set_style(test_case_06_list, LIST_HELIX);
    gui_list_set_note_num(test_case_06_list, TEST_CASE_06_NOTE_COUNT);

    gui_list_enable_area_display(test_case_06_list, true);
    gui_list_set_offset(test_case_06_list, -(int16_t)(TEST_CASE_06_NOTE_HEIGHT * 18U));

    gui_log("test_case_06 end!\n");

    return TEST_CASE_06_EXPECTED_RENDER_TIME_MS;
}
