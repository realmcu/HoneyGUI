/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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
#include "flag_gltf/gltf_desc_flag.txt"

static float rot_angle = 0.0f;

static void update_flag_animation(void *param)
{
    (void)param;


    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;

        if (rot_angle > 45.0f)
        {
            rot_angle = 45.0f;
        }
        else if (rot_angle < -45.0f)
        {
            rot_angle = -45.0f;
        }
    }
}

static void flag_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 1), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, -5, 0, 18, 0, rot_angle, 0, 5);
}


static int app_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    l3_model_base_t *flag_3d = l3_create_model((void *)_acgltf_desc_flag, L3_DRAW_FRONT_AND_SORT, 0, 0,
                                               dc->screen_width,
                                               dc->screen_height);

    l3_set_global_transform(flag_3d, (l3_global_transform_cb)flag_global_cb);

    gui_lite3d_t *lite3d_flag = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget",
                                                  flag_3d, 0, 0, 0, 0);

    gui_obj_create_timer(GUI_BASE(lite3d_flag), 20, true, update_flag_animation);


    return 0;

}

GUI_INIT_APP_EXPORT(app_init);


