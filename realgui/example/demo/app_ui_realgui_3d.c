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


#include "3d/rect_obj.txt"
#include "3d/rect_mtl.txt"
#include "tiger_blue.txt"
#include "def_3d.h"
#include "gui_3d.h"








void cb(gui_3d_t *this, size_t s, gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();

    if (s == 0)
    {
        gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 1), 1, 32767, 90,
                                     dc->screen_width, dc->screen_height);
        gui_3d_world_inititalize(world, 0, 0, 5.0f, 0, 40, 0, 1);
    }
    else
    {
        gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 1), 1, 32767, 90,
                                     dc->screen_width, dc->screen_height);
        gui_3d_world_inititalize(world, 0, 0, 5.0f, 0, 60, 0, 1);
    }




}




static void app_ui_design(gui_app_t *app)
{

    gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", 0, 0, 480, 480);

    gui_3d_set_obj(test_3d, (void *)_acrect_obj, sizeof(_acrect_obj));
    gui_3d_set_mtl(test_3d, (void *)_acrect_mtl, sizeof(_acrect_mtl));

    gui_3d_set_shape_transform_cb(test_3d, 0, cb);


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


