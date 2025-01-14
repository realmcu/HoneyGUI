#include "guidef.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_app.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"
#include "def_3d.h"
#include "gui_3d.h"
#include "math.h"
#include "root_image_hongkong/ui_resource.h"

#include "gui_prism_mirror3d.h"
#include <tp_algo.h>

static void _APP_WATCHFACE_PRISM3D_ui_design(gui_app_t *);
static gui_app_t _app_APP_WATCHFACE_PRISM3D =
{
    .screen =
    {
        .name = "APP_WATCHFACE_PRISM3D",
        .magic = 0x0b,
    },
    .ui_design = _APP_WATCHFACE_PRISM3D_ui_design,
    .active_ms = 1000000,
};
gui_app_t *_get_app_APP_WATCHFACE_PRISM3D_handle(void) { return &_app_APP_WATCHFACE_PRISM3D; }

extern uint8_t watchface_index;
static void app_cb(void *p);
static void _APP_WATCHFACE_PRISM3D_ui_design(gui_app_t *app)
{
    gui_prism_mirror3d_t *prism_watchface = gui_prism_mirror3d_create(&(app->screen), "prism_3d",
                                                                      DESC_PRISM3D_BIN, 0, 0,
                                                                      410,
                                                                      502, NULL);
    float raw_world_position[3] = {0, 10, 100};
    float raw_camera_position[3] = {0, 3, 60};
    gui_prism_mirror3d_set_raw_state(prism_watchface, raw_world_position, raw_camera_position, 0, 0, 0,
                                     19);

    float target_world_position[3] = {0, 11, 109};
    float target_camera_position[3] = {0, 0, 80};
    gui_prism_mirror3d_set_target_state(prism_watchface, target_world_position, target_camera_position,
                                        0, 0,
                                        0, 20);
    prism_watchface->conf.auto_rotation = true;
    prism_watchface->conf.face_nums = 6;
    prism_watchface->conf.sensitivity = 0.05f;

    int16_t *face = gui_prism_mirror3d_get_enter_face();
    *face = watchface_index;
    gui_prism_mirror3d_click_switch_app_add_event(prism_watchface, (gui_event_cb_t)app_cb);
    gui_prism_mirror3d_enter_animate(prism_watchface);
}

static void app_cb(void *p)
{
    extern char *defaultPath;
    extern char watchface_path[];
    switch (*gui_prism_mirror3d_get_enter_face())
    {
    case 0:
        watchface_index = 0;
        sprintf(watchface_path, "%sapp/watchface1/watchface1.xml", GUI_ROOT_FOLDER);
        break;
    case 1:
        watchface_index = 1;
        break;
    case 2:
        watchface_index = 2;
        break;
    case 3:
        watchface_index = 3;
        break;
    case 4:
        watchface_index = 4;
        sprintf(watchface_path, "%sapp/watchface3/watchface3.xml", GUI_ROOT_FOLDER);
        break;
    case 5:
        watchface_index = 5;
        sprintf(watchface_path, "%sapp/watchface2/watchface2.xml", GUI_ROOT_FOLDER);
        break;
    default:
        break;
    }
    extern void *get_app_hongkong(void);
    gui_app_switch(gui_current_app(), (gui_app_t *)get_app_hongkong());
}

