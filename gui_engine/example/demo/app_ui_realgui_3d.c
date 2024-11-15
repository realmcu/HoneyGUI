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


#include "3d/cube_obj.txt"
#include "3d/cube_mtl.txt"
#include "tiger_blue.txt"
#include "def_3d.h"
#include "gui_3d.h"













static void app_ui_design(gui_app_t *app)
{

    gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", 0, 0, 480, 480);

    gui_3d_set_obj(test_3d, (void *)_accube_obj, sizeof(_accube_obj));
    gui_3d_set_mtl(test_3d, (void *)_accube_mtl, sizeof(_accube_mtl));


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


