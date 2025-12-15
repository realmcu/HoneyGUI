
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
#include "pikachu_gltf/gltf_desc_Pikachu_resize.txt"

static float rot_angle = 0.0f;
const float DEFAULT_RADIUS = 10.0f;
const float DEFAULT_DEPTH = 1.0f;
const float DEFAULT_DURATION = 0.6f;

static void update_pikachu_animation(void *param)
{
    gui_lite3d_t *lite3d_pikachu = (gui_lite3d_t *)param;
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
        l3_model_trigger_deformation(lite3d_pikachu->model, tp->x, tp->y, DEFAULT_RADIUS, DEFAULT_DEPTH,
                                     DEFAULT_DURATION);
    }
}

static void pikachu_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 1), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 10, 28, 0, -30 + rot_angle, 0, 5);
}


static int app_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    l3_model_base_t *pikachu_3d = l3_create_model((void *)_acgltf_desc_Pikachu_resize,
                                                  L3_DRAW_FRONT_AND_SORT,
                                                  0, 0,
                                                  dc->screen_width,
                                                  dc->screen_height);

    l3_set_global_transform(pikachu_3d, (l3_global_transform_cb)pikachu_global_cb);

    gui_lite3d_t *lite3d_pikachu = gui_lite3d_create(gui_obj_get_root(), "lite3d-widget",
                                                     pikachu_3d, 0, 0, 0, 0);

    gui_obj_create_timer(GUI_BASE(lite3d_pikachu), 20, true, update_pikachu_animation);


    return 0;

}

GUI_INIT_APP_EXPORT(app_init);


