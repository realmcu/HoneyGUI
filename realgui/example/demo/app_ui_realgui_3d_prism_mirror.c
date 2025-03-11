/* 3d prism mirror demo start*/
#include "guidef.h"
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
#include "gui_3d.h"
#include "math.h"

#include "gui_prism_mirror3d.h"
#include <tp_algo.h>
#include "prism3d/desc.txt"

#include "prism3d/root/homelist_dog.c"
#include "prism3d/root/homelist_line_black.c"
#include "prism3d/root/homelist_line_orange.c"
#include "prism3d/root/homelist_number.c"
#include "prism3d/root/homelist_watch_black.c"
#include "prism3d/root/homelist_watch_white.c"


void callback_touch_clike_return();
void app_cb(gui_obj_t *obj, void *event_param);

static void app_ui_design(gui_app_t *app)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    gui_3d_description_t *desc = gui_get_3d_desc((void *)_acdesc);
    gui_prism_mirror3d_t *prism_demo = gui_prism_mirror3d_create(&(app->screen), "prism_3d",
                                                                 desc, 0, 0,
                                                                 dc->screen_width,
                                                                 dc->screen_height, NULL);
    float raw_world_position[3] = {0, 10, 100};
    float raw_camera_position[3] = {0, 3, 60};
    gui_prism_mirror3d_set_raw_state(prism_demo, raw_world_position, raw_camera_position, 0, 0, 0, 19);

    float target_world_position[3] = {0, 10.5, 109};
    float target_camera_position[3] = {0, 0, 80};
    gui_prism_mirror3d_set_target_state(prism_demo, target_world_position, target_camera_position, 0, 0,
                                        0, 19);
    prism_demo->conf.auto_rotation = true;
    prism_demo->conf.face_nums = 6;
    prism_demo->conf.sensitivity = 0.05f;
    gui_prism_mirror3d_click_switch_app_add_event(prism_demo, (gui_event_cb_t)app_cb);
    gui_prism_mirror3d_enter_animate(prism_demo);

}
uint16_t face_nums_flags = 0;
static void app_ui_design_switch(gui_app_t *app)
{
    touch_info_t *tp = tp_get_info();
    gui_img_t *image;
    if (face_nums_flags == 0)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_watch_white, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_nums_flags == 1)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_dog, 0, 0, 480,
                                        480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_nums_flags == 2)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_line_black, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_nums_flags == 3)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_line_orange, 0, 0,
                                        480,
                                        480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_nums_flags == 4)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_number, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_nums_flags == 5)
    {
        image = gui_img_create_from_mem(&(app->screen), "image0", (void *)_achomelist_watch_black, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }

    gui_obj_add_event_cb(image, callback_touch_clike_return, GUI_EVENT_1, NULL);

    return;

}
static gui_app_t rtk_gui_demo =
{
    .screen = {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design,
    .active_ms = 1000 * 60 * 60,
};

static gui_app_t rtk_gui_demo_switch_image1 =
{
    .screen = {
        .name = "rtk_gui_demo_switch_image1",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design_switch,
    .active_ms = 1000 * 60 * 60,
};

void *get_app_rtk_gui_demo(void)
{
    return &rtk_gui_demo;
}

static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}


GUI_INIT_APP_EXPORT(app_init);

void app_cb(gui_obj_t *obj, void *event_param)
{
    gui_prism_mirror3d_t *prism_3d = (gui_prism_mirror3d_t *)obj;
    face_nums_flags = *gui_prism_mirror3d_get_enter_face();
    switch (face_nums_flags)
    {
    case 0:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    case 1:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    case 2:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    case 3:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    case 4:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    case 5:
        gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
        break;
    default:
        break;
    }
}
void callback_touch_clike_return()
{
    gui_app_switch(gui_current_app(), &rtk_gui_demo);
}

/* 3d prism mirror demo end*/

