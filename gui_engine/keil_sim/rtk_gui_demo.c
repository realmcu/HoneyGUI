#include "guidef.h"
#include "gui_img.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "tiger.txt"


gui_app_t *get_rtk_gui_demo(void);


static void app_dialing_ui_design(gui_app_t *app)
{
    gui_img_t *img = gui_img_create_from_mem(&app->screen,  "center", (void *)_actiger, 0, 0, 0, 0);
}


static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dialing_ui_design,
};

gui_app_t *get_rtk_gui_demo(void)
{
    return &rtk_gui_demo;
}




