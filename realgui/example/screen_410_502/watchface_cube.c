#include "guidef.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_app.h"
#include "gui_components_init.h"
#include "math.h"
#include "root_image_hongkong/ui_resource.h"
#include "gui_cube.h"

static void _APP_WATCHFACE_CUBE_ui_design(gui_app_t *app);
static gui_app_t _app_APP_WATCHFACE_CUBE =
{
    .screen =
    {
        .name = "APP_WATCHFACE_CUBE",
        .magic = 0x0b,
    },
    .ui_design = _APP_WATCHFACE_CUBE_ui_design,
    .active_ms = 1000000,
    .shutdown_animation_flag = GUI_APP_ANIMATION_7,
    // .startup_animation_flag = GUI_APP_ANIMATION_4,
};
gui_app_t *_get_app_APP_WATCHFACE_CUBE_handle(void) { return &_app_APP_WATCHFACE_CUBE; }

static void app_cb(void *obj, gui_event_t e, void *param)
{
    extern uint8_t watchface_index;
    extern char *defaultPath;
    extern char watchface_path[];
    switch (e)
    {
    case GUI_EVENT_1:
        watchface_index = 1;
        break;
    case GUI_EVENT_2:
        watchface_index = 5;
        sprintf(watchface_path, "%sapp/watchface2/watchface2.xml", GUI_ROOT_FOLDER);
        break;
    case GUI_EVENT_3:
        watchface_index = 2;
        break;
    case GUI_EVENT_4:
        watchface_index = 0;
        sprintf(watchface_path, "%sapp/watchface1/watchface1.xml", GUI_ROOT_FOLDER);
        break;
    case GUI_EVENT_5:
        watchface_index = 3;
        break;
    case GUI_EVENT_6:
        watchface_index = 4;
        sprintf(watchface_path, "%sapp/watchface3/watchface3.xml", GUI_ROOT_FOLDER);
        break;
    default:
        break;
    }
    extern void *get_app_hongkong(void);
    gui_app_switch(gui_current_app(), (gui_app_t *)get_app_hongkong());
}

void _APP_WATCHFACE_CUBE_ui_design(gui_app_t *app)
{
    gui_cube_imgfile_t imgfile =
    {
        .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
        .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
        .data_addr.data_addr_front = UI_CLOCK_FACE_MAIN_BIN,
        .data_addr.data_addr_back = WF2PREVIEW_BIN,
        .data_addr.data_addr_up =  WATCHFACE_BUTTERFLY_BIN,
        .data_addr.data_addr_down = WF1PREVIEW_BIN,
        .data_addr.data_addr_left = WATCHFACE_RING_BIN,
        .data_addr.data_addr_right = WF3PREVIEW_BIN
    };
    gui_cube_t *cube = gui_cube_create(app->window, "cube", &imgfile, 0, 0);
    gui_cube_auto_rotation_by_y(cube, 100, 3.0f);
    gui_cube_set_mode(cube, CUBE_SIDE_DOWN, IMG_SRC_OVER_MODE);
    gui_cube_set_size(cube, 120);
    gui_cube_set_center(cube, 200, 250);
    gui_cube_cb_t cube_cb =
    {
        .click_cb_back = app_cb,
        .click_cb_left = app_cb,
        .click_cb_right = app_cb,
        .click_cb_front = app_cb,
        .click_cb_up = app_cb,
        .click_cb_down = app_cb,
    };
    gui_cube_add_click_cb(cube, cube_cb);
}

