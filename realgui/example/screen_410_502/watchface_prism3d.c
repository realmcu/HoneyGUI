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
#include "gui_3d_rect.h"
#include "math.h"
#include "root_image_hongkong/ui_resource.h"

#include "gui_prism_mirror3d.h"
#include "tp_algo.h"
#include "app_hongkong.h"

const static gui_view_descriptor_t *watchface_view = NULL;
extern uint8_t watchface_index;
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void app_cb(void *obj);
void _APP_WATCHFACE_PRISM3D_ui_design(gui_view_t *view)
{
    gui_3d_description_t *desc = gui_get_3d_desc((void *)
                                                 DESC_PRISM3D_BIN); //add triangel or rectangle head
    gui_prism_mirror3d_t *prism_watchface = gui_prism_mirror3d_create(GUI_BASE(view), "prism_3d",
                                                                      desc, 0, 0,
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

static void app_cb(void *obj)
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
    gui_view_switch_direct(gui_view_get_current_view(), watchface_view, VIEW_ANIMATION_NULL,
                           VIEW_ANIMATION_5);
}

