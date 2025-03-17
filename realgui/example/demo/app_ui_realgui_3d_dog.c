/* 3d dog demo start*/
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

#include "gui_3d.h"
#include "tp_algo.h"
#include "dog3d/desc.txt"

static float rot_angle = 0.0f;

void update_dog_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_angle += tp->deltaX / 5.0f;
    }
}

static void dog_cb(void *this, gui_3d_world_t *world,
                   gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 3, 60), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(world, 0, 15, 90, 0, rot_angle, 0, 5);
}
static void app_ui_design(gui_app_t *app)
{
    gui_3d_description_t *desc = gui_get_3d_desc((void *)_acdesc);

    void *dog_3d = gui_3d_create(&(app->screen), "3d-widget", desc, 0, 0, 480, 480);
    gui_3d_set_global_shape_transform_cb(dog_3d, (gui_3d_shape_transform_cb)dog_cb);
    gui_3d_set_animate(dog_3d, 10000, -1, update_dog_animation, NULL);

//    extern void gui_fps_create(void *parent);
//    gui_fps_create(&(app->screen));

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


