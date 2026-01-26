/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* 3d dog demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#include "gui_lite3d.h"
#include "tp_algo.h"
#include "dog3d/desc_dog.txt"

static float rot_angle = 0.0f;

static void update_dog_animation(void *param)
{
    GUI_UNUSED(param);
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
    }
}

static void dog_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 3, 30), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 15, 30, 0, rot_angle, 0, 5);
}

static int app_init(void)
{
    l3_model_base_t *dog_3d = l3_create_model((void *)_acdesc_dog, L3_DRAW_FRONT_AND_SORT, 50, 50, 380,
                                              380);

    l3_set_global_transform(dog_3d, (l3_global_transform_cb)dog_global_cb);

    gui_lite3d_t *lite3d_dog = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget", dog_3d,
                                                 0, 0, 480, 480);

    gui_obj_create_timer(GUI_BASE(lite3d_dog), 17, true, update_dog_animation);


    return 0;

}

/* 3d dog demo end*/

GUI_INIT_APP_EXPORT(app_init);


