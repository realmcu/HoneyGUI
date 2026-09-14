/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "test_assets.h"
#include "test_cases.h"

#define TEST_CASE_00_EXPECTED_RENDER_TIME_MS 68U

uint32_t test_case_00(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_log("test_case_00 start!\n");

    gui_obj_child_free(gui_obj_get_root());

    gui_img_t *img = gui_img_create_from_mem(gui_obj_get_root(), "img_test",
                                             (void *)test_asset_get_tiger_blue(),
                                             0, 0, 0, 0);

    gui_img_set_focus(img, gui_img_get_width(img) / 2, gui_img_get_height(img) / 2);
    gui_img_rotation(img, 0.0f);
    gui_img_translate(img, dc->screen_width / 2, dc->screen_height / 2);
    gui_img_rotation(img, 45.0f);

    gui_log("test_case_00 end!\n");

    return TEST_CASE_00_EXPECTED_RENDER_TIME_MS;
}
