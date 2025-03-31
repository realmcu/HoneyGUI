/* 3d face demo start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#include "gui_3d.h"
#include "tp_algo.h"
#include "face3d/desc_1454.txt"
#include "face3d/desc_5822.txt"

static float rot_angle = 0.0f;

void update_face_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
    }
}


static void face_cb(void *this, gui_3d_world_t *world,
                    gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 3, 60), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);


    gui_3d_world_inititalize(world, 0, 25, 120, 0, rot_angle, 0, 5);
}



static int app_init(void)
{
    void *test_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc_1454, 0, 0, 480,
                                  480);
    gui_3d_set_global_shape_transform_cb(test_3d, (gui_3d_shape_transform_cb)face_cb);

    gui_obj_create_timer(&(((gui_3d_base_t *)test_3d)->base), 17, true, update_face_animation);
    gui_obj_start_timer(&(((gui_3d_base_t *)test_3d)->base));

    return 0;

}

/* 3d face demo end*/


GUI_INIT_APP_EXPORT(app_init);


