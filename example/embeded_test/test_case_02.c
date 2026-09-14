/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_rect.h"
#include "test_cases.h"

/* Simulator measures ~17 ms; keep headroom for slower targets. */
#define TEST_CASE_02_EXPECTED_RENDER_TIME_MS 25U

uint32_t test_case_02(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_log("test_case_02 start!\n");

    gui_obj_child_free(gui_obj_get_root());

    /* Opaque full screen background, radius 0 (fast path). */
    gui_rect_create(gui_obj_get_root(), "rect_bg",
                    0, 0, dc->screen_width, dc->screen_height, 0,
                    gui_rgba(20, 20, 30, 255));

    /* Rounded rect with solid fill and partial opacity. */
    gui_rounded_rect_t *rect_solid = gui_rect_create(gui_obj_get_root(), "rect_solid",
                                                     dc->screen_width / 2 - 100,
                                                     dc->screen_height / 2 - 160,
                                                     200, 140, 30,
                                                     gui_rgba(255, 0, 0, 255));
    gui_rect_set_opacity(rect_solid, 200);

    /* Rounded rect with a diagonal linear gradient. */
    gui_rounded_rect_t *rect_gradient = gui_rect_create(gui_obj_get_root(), "rect_gradient",
                                                        dc->screen_width / 2 - 100,
                                                        dc->screen_height / 2 + 20,
                                                        200, 140, 30,
                                                        gui_rgba(0, 128, 255, 255));
    gui_rect_set_linear_gradient(rect_gradient, RECT_GRADIENT_DIAGONAL_TL_BR);
    gui_rect_add_gradient_stop(rect_gradient, 0.0f, gui_rgba(0, 128, 255, 255));
    gui_rect_add_gradient_stop(rect_gradient, 0.5f, gui_rgba(255, 255, 255, 255));
    gui_rect_add_gradient_stop(rect_gradient, 1.0f, gui_rgba(255, 0, 128, 255));

    gui_log("test_case_02 end!\n");

    return TEST_CASE_02_EXPECTED_RENDER_TIME_MS;
}
