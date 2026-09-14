/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_circle.h"
#include "test_cases.h"

/* Simulator measures ~12 ms; keep headroom for slower targets. */
#define TEST_CASE_03_EXPECTED_RENDER_TIME_MS 30U

uint32_t test_case_03(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    int center_x = dc->screen_width / 2;
    int center_y = dc->screen_height / 2;

    gui_log("test_case_03 start!\n");

    gui_obj_child_free(gui_obj_get_root());

    /* Large solid circle, centered. */
    gui_circle_t *circle_solid = gui_circle_create(gui_obj_get_root(), "circle_solid",
                                                   center_x, center_y, 150,
                                                   gui_rgba(0, 90, 160, 255));
    gui_circle_set_opacity(circle_solid, 220);


    gui_log("test_case_03 end!\n");

    return TEST_CASE_03_EXPECTED_RENDER_TIME_MS;
}
