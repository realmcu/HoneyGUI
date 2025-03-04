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
#include "app_hongkong.h"

const static gui_view_descriptor_t *watchface_view = NULL;

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void app_cb(void *obj, gui_event_t e, void *param)
{
    extern uint8_t watchface_index;
    extern char *defaultPath;
    extern char watchface_path[];
    gui_view_t *view = (gui_view_t *)GUI_BASE(obj)->parent;
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
    gui_view_switch_direct(view, watchface_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
}

void WATCHFACE_CUBE_ui_design(gui_view_t *view)
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
    gui_cube_t *cube = gui_cube_create(view, "cube", &imgfile, 0, 0);
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

