/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_arc.h"
#include "gui_arc_group.h"
#include "test_cases.h"

/* Six stroked rings plus a gradient; simulator measures ~36 ms. */
#define TEST_CASE_04_EXPECTED_RENDER_TIME_MS 110U

#define TEST_CASE_04_RING_THICKNESS 22.0f

uint32_t test_case_04(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    int center_x = dc->screen_width / 2;
    int center_y = dc->screen_height / 2;

    gui_log("test_case_04 start!\n");

    gui_obj_child_free(gui_obj_get_root());



    // Calculate bounding box for the arc group
    float outer_radius = 163.0f;
    float outer_thickness = 22.0f;
    int box_size = (int)(outer_radius + outer_thickness / 2 + 2) * 2 + 4;
    int box_x = (int)center_x - box_size / 2;
    int box_y = (int)center_y - box_size / 2;

    // Background rings: static full rings, shared buffer via the arc group
    gui_arc_group_t *bg_group = gui_arc_group_create(gui_obj_get_root(), "bg_rings",
                                                     box_x, box_y, box_size, box_size);

    float group_cx = box_size / 2.0f, group_cy = group_cx;

    gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                          163.0f, 0, 360, 22.0f, gui_rgba(52, 14, 11, 255));
    gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                          136.5f, 0, 360, 22.0f, gui_rgba(16, 37, 17, 255));
    gui_arc_group_add_arc(bg_group, group_cx, group_cy,
                          110.0f, 0, 360, 22.0f, gui_rgba(17, 32, 52, 255));

    // Foreground arcs: independent widgets (these can change dynamically), solid colour
    gui_arc_create(gui_obj_get_root(), "fg_arc_move", center_x, center_y,
                   163.0f, 0, 252.0f, 22.0f, gui_rgba(250, 17, 79, 255));
    gui_arc_create(gui_obj_get_root(), "fg_arc_exercise", center_x, center_y,
                   136.5f, 0, 144.0f, 22.0f, gui_rgba(164, 255, 4, 255));
    gui_arc_create(gui_obj_get_root(), "fg_arc_stand", center_x, center_y,
                   110.0f, 0, 198.0f, 22.0f, gui_rgba(0, 200, 255, 255));

    GUI_UNUSED(bg_group);

    gui_log("test_case_04 end!\n");

    return TEST_CASE_04_EXPECTED_RENDER_TIME_MS;
}
