/* 3d donut demo start*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_lite3d.h"
#include "tp_algo.h"
#include "donut3d/desc_donut.txt"

static float rot_angle = 0.0f;
static float shift_z = 20.0f;
static float animation_time = 0.0f;

static void update_donut_animation(void *param)
{
    gui_lite3d_t *this = (gui_lite3d_t *)param;
    GUI_UNUSED(this);
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
    }
    else
    {
        rot_angle += 2.0f;
        animation_time += 0.02f;
        shift_z = 15.0f - 5.0f * sinf(animation_time);
    }
}

static void donut_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 1), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, shift_z, 0, rot_angle, 0, 5);
}

static int app_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    l3_model_base_t *donut_3d = l3_create_model((void *)_acdesc_donut, L3_DRAW_FRONT_AND_SORT, 0, 0,
                                                dc->screen_width, dc->screen_height);

    l3_set_global_transform(donut_3d, (l3_global_transform_cb)donut_global_cb);

    gui_lite3d_t *lite3d_donut = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget", donut_3d,
                                                   0, 0, 480, 480);

    gui_obj_create_timer(GUI_BASE(lite3d_donut), 17, true, update_donut_animation);


    return 0;

}

/* 3d donut demo end*/

GUI_INIT_APP_EXPORT(app_init);


