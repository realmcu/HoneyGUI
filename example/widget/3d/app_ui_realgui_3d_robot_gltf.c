/* 3d robot demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#include "gui_lite3d_gltf.h"
#include "gui_lite3d.h"
#include "tp_algo.h"
#include "robot_gltf/gltf_desc_robot.txt"

static float rot_angle = 0.0f;


static void update_robot_animation(void *param)
{
    GUI_UNUSED(param);
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
    }
}

static void robot_global_cb(l3_gltf_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 1), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 5, 8, 90, rot_angle, 0, 5);
}


static int app_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    l3_gltf_model_t *robot_3d = l3_create_gltf_model((void *)_acgltf_desc_robot, LITE_RGB565, 0, 0,
                                                     dc->screen_width,
                                                     dc->screen_height);

    l3_gltf_set_global_transform(robot_3d, (l3_gltf_global_transform_cb)robot_global_cb);

    gui_lite3d_gltf_t *lite3d_robot = gui_lite3d_gltf_create(gui_obj_get_root(), "lite3d-widget",
                                                             robot_3d, 0, 0, 0, 0);

    gui_obj_create_timer(GUI_BASE(lite3d_robot), 20, true, update_robot_animation);


    return 0;

}

GUI_INIT_APP_EXPORT(app_init);

/* 3d robot demo end*/
