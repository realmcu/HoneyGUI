#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"

#include "def_3d.h"
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
        rot_angle += tp->deltaX / 10.0f;
    }
}

static void face_cb(gui_3d_t *this, size_t face/*face offset*/, gui_3d_world_t *world,
                    gui_3d_camera_t *camera, gui_3d_light_t *light)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 3, 60), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(&object_matrix, 0, 25, 120, 0, 0, 0,
                             5);

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                             rot_angle,
                             1);

    *world = gui_3d_matrix_multiply(face_matrix, object_matrix);
}


static void app_ui_design(gui_app_t *app)
{
    gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", (void *)_acdesc_1454, 0, 0, 480,
                                      480);

    extern void gui_fps_create(void *parent);
    gui_fps_create(&(app->screen));

    gui_3d_set_shape_transform_cb(test_3d, 0, face_cb);

    gui_3d_set_animate(test_3d, 10000, -1, update_face_animation, NULL);

    return;

}



static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design,
    .active_ms = 1000 * 60 * 60,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


