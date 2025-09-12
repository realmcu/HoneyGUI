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
#include "earth3d/desc.txt"
#include "earth3d/desc_no_mtl.txt"

static float rot_x_angle = 0.0f;
static float rot_y_angle = 0.0f;

static void update_earth_animation(void *param)
{
    (void)param;
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_x_angle += tp->deltaY / 5.0f;
        rot_y_angle += tp->deltaX / 5.0f;
    }
    rot_y_angle++;
}

static void earth_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 10), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 10, rot_x_angle, rot_y_angle, 0, 5);
}

static int app_init(void)
{
#if 0
    l3_model_t *earth_3d = l3_create_model((void *)_acdesc, L3_DRAW_FRONT_AND_SORT, 0, 46, 410, 410);
#else
    l3_model_t *earth_3d = l3_create_model((void *)_acdesc_no_mtl, L3_DRAW_FRONT_AND_SORT, 0, 46, 410,
                                           410);
#endif

    l3_set_global_transform(earth_3d, (l3_global_transform_cb)earth_global_cb);

    gui_lite3d_t *lite3d_earth = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget", earth_3d,
                                                   0, 0, 480, 480);

    gui_obj_create_timer(GUI_BASE(lite3d_earth), 17, true, update_earth_animation);


    return 0;

}

/* 3d earth demo end*/

GUI_INIT_APP_EXPORT(app_init);


