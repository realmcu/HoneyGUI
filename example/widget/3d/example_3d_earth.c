/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* 3d earth demo start*/
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
#include "earth3d/desc_earth_tria.txt"
#include "earth3d/desc_earth_tria_no_mtl.txt"

static float rot_x_angle = 0.0f;
static float rot_y_angle = 0.0f;

static void update_earth_animation(void *param)
{
    (void)param;
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_x_angle += tp->deltaY / 10.0f;
        rot_y_angle += tp->deltaX / 10.0f;
    }
    rot_y_angle++;
}

static void earth_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 10), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 10, rot_x_angle, rot_y_angle, 0, 5);
}

static int app_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();
#if 1
    l3_model_base_t *earth_3d = l3_create_model((void *)_acdesc_earth_tria, L3_DRAW_FRONT_ONLY, 0, 0,
                                                dc->screen_width,
                                                dc->screen_height);
#else
    l3_model_base_t *earth_3d = l3_create_model((void *)_acdesc_earth_tria_no_mtl, L3_DRAW_FRONT_ONLY,
                                                0, 0,
                                                dc->screen_width, dc->screen_height);
#endif

    l3_set_global_transform(earth_3d, (l3_global_transform_cb)earth_global_cb);

    gui_lite3d_t *lite3d_earth = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget", earth_3d,
                                                   0, 0, 480, 480);

    gui_obj_create_timer(GUI_BASE(lite3d_earth), 17, true, update_earth_animation);


    return 0;

}

/* 3d earth demo end*/

GUI_INIT_APP_EXPORT(app_init);


