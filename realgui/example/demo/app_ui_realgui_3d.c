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
#include "gui_canvas.h"


#include "tiger_blue.txt"
#include "def_3d.h"
#include "gui_3d.h"

#include "butterfly/desc.txt"
#include "math.h"


static int frame_counter = 0;
static float wing_angle = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;

void update_animation()
{
    frame_counter++;
    wing_angle = 70.0f * sinf(frame_counter * 0.1f);

    float radius = 4.0f;
    float theta = frame_counter * 0.01f;

    butterfly_x = radius * cosf(theta);
    butterfly_y = radius * sinf(theta);

    butterfly_z = 6.0f + 0.5f * sinf(frame_counter * 0.05f);

    butterfly_rz = theta * (180.0f / M_PI);
}

void cb(gui_3d_t *this, size_t s, gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 1), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);
    if (s == 0)
    {
        gui_3d_world_inititalize(world, butterfly_x, butterfly_y, butterfly_z, -wing_angle, 0, butterfly_rz,
                                 1);
    }
    else if (s == 1)
    {
        gui_3d_world_inititalize(world, butterfly_x, butterfly_y, butterfly_z, wing_angle, 0, butterfly_rz,
                                 1);
    }
    else if (s == 2)
    {
        gui_3d_world_inititalize(world, butterfly_x, butterfly_y, butterfly_z, -wing_angle, 10,
                                 butterfly_rz, 1);
    }
    else if (s == 3)
    {
        gui_3d_world_inititalize(world, butterfly_x, butterfly_y, butterfly_z, wing_angle, 10, butterfly_rz,
                                 1);
    }
    else
    {
        gui_3d_world_inititalize(world, butterfly_x, butterfly_y, butterfly_z, 0, 0, butterfly_rz, 1);
    }

}



static void app_ui_design(gui_app_t *app)
{

    gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);

    gui_3d_set_shape_transform_cb(test_3d, 0, cb);

    gui_3d_set_animate(test_3d, 10000, -1, update_animation, NULL);


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


