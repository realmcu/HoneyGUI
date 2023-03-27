#if 1
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
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"

gui_app_t *get_vg_lite_clock_demo(void);



static void app_vg_lite_ui_design(gui_app_t *app)
{

    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(&app->screen,  "path_clock", NULL, 0, 0,
                                                               454, 454);

}


static gui_app_t vg_lite_clock_demo =
{
    .screen =
    {
        .name = "vg_lite_clock_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_vg_lite_ui_design,
};

gui_app_t *get_vg_lite_clock_demo(void)
{
    return &vg_lite_clock_demo;
}


#endif

