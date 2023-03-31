#if 0
#include "guidef.h"
#include "gui_img.h"
#include "gui_magic_img.h"
#include "gui_dynamic_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "rtk_gui_resource.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_stb_img.h"
#include "gui_button.h"
#include "gui_scroll_wheel.h"
#include "gui_page.h"
#include "gui_canvas.h"
#include "tiger.txt"
gui_app_t *get_rtk_gui_demo(void);


static void app_dialing_ui_design(gui_app_t *app)
{
    //gui_img_t *img = gui_img_create_from_mem(&app->screen,  "center", (void *)_actiger, 0, 0, 0, 0);

    gui_magic_img_t *img = gui_magic_img_create_from_mem(&app->screen,  "center", (void *)_actiger, 0,
                                                         0, 0, 0);
    gui_img_scale(img, 0.5, 0.5);
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


#endif

