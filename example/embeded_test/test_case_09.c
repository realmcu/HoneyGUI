/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_lite3d.h"
#include "gui_obj.h"
#include "test_cases.h"
#include "tp_algo.h"

#include "dog3d/desc_dog.txt"

/*
 * Calibrate this limit on the target EVB after collecting at least 30 runs.
 * It is intentionally higher than the 2D cases because it covers model setup
 * and the first Lite3D rasterisation.
 */
#define TEST_CASE_09_EXPECTED_RENDER_TIME_MS 40U

static float test_case_09_rot_angle;

static void test_case_09_update_dog_animation(void *param)
{
    touch_info_t *tp = tp_get_info();

    GUI_UNUSED(param);

    if (tp->pressed || tp->pressing)
    {
        test_case_09_rot_angle += tp->deltaX / 5.0f;
    }
}

static void test_case_09_dog_global_cb(l3_model_base_t *model)
{
    l3_camera_UVN_initialize(&model->camera, l3_4d_point(0, 0, 0),
                             l3_4d_point(0, 3, 30), 1, 32767, 90,
                             model->viewPortWidth, model->viewPortHeight);
    l3_world_initialize(&model->world, 0, 15, 30, 0, test_case_09_rot_angle, 0, 5);
}

uint32_t test_case_09(void)
{
    l3_model_base_t *dog_model;
    gui_lite3d_t *lite3d_dog;

    gui_log("test_case_09 start!\n");

    gui_obj_child_free(gui_obj_get_root());

    test_case_09_rot_angle = 0.0f;

    dog_model = l3_create_model((void *)_acdesc_dog, L3_DRAW_FRONT_AND_SORT,
                                50, 50, 380, 380);
    l3_set_global_transform(dog_model, test_case_09_dog_global_cb);
    lite3d_dog = gui_lite3d_create(gui_obj_get_root(), "lite3d_dog", dog_model,
                                   0, 0, 410, 502);
    gui_obj_create_timer(GUI_BASE(lite3d_dog), 17, true,
                         test_case_09_update_dog_animation);

    gui_log("test_case_09 end!\n");

    return TEST_CASE_09_EXPECTED_RENDER_TIME_MS;
}
