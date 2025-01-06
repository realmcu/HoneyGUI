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

void callback_touch_clike_return();
void app_cb(void *p);

static void _APP_WATCHFACE_PRISM3D_ui_design(gui_app_t *app)
{
    gui_prism_mirror3d_t *prism_demo = gui_prism_mirror3d_create(&(app->screen), "prism_3d",
                                                                 DESC_PRISM3D_BIN, 0, 0,
                                                                 410,
                                                                 502);

    prism_demo->raw_state = (gui_prism_transform_state_t) {.worldPosition = {0, 23, 60}, .cameraPosition = {0, 4, 4}, .rot_x = 90, .rot_y = 0, .rot_z = 0, .scale = 22};
    prism_demo->target_state = (gui_prism_transform_state_t) {.worldPosition = {0, 22, 60}, .cameraPosition = {0, 0, 4}, .rot_x = 90, .rot_y = 0, .rot_z = 0, .scale = 29};
    gui_prism_mirror3d_click_switch_app_add_event(prism_demo, (gui_event_cb_t)app_cb);
    gui_prism_mirror3d_enter_animate(prism_demo);

}
// static void app_ui_design_switch(gui_app_t *app)
// {
//     gui_img_t *image = NULL;
//     if (face_nums_flags == 0)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", WF1PREVIEW_BIN, 0, 0, 410,
//                                         502);
//     }
//     if (face_nums_flags == 1)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", UI_CLOCK_FACE_MAIN_BIN, 0, 0, 410,
//                                         502);
//     }
//     if (face_nums_flags == 2)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", WATCHFACE_BUTTERFLY_BIN, 0, 0, 410,
//                                         502);
//     }
//     if (face_nums_flags == 3)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", WATCHFACE_RING_BIN, 0, 0, 410,
//                                         502);
//     }
//     if (face_nums_flags == 4)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", WF3PREVIEW_BIN, 0, 0, 410,
//                                         502);
//     }
//     if (face_nums_flags == 5)
//     {
//         image = gui_img_create_from_mem(&(app->screen), "image0", WF2PREVIEW_BIN, 0, 0, 410,
//                                         502);
//     }

//     gui_obj_add_event_cb(image, (gui_event_cb_t)callback_touch_clike_return, GUI_EVENT_1, NULL);

//     return;
// }

static int app_init(void)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502\\root_image_hongkong\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502/root_image_hongkong/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(_get_app_APP_WATCHFACE_PRISM3D_handle());
    return 0;
}

// static gui_app_t rtk_gui_demo_switch_image1 =
// {
//     .screen = {
//         .name = "rtk_gui_demo_switch_image1",
//         .x    = 0,
//         .y    = 0,
//     },
//     .ui_design = app_ui_design_switch,
//     .active_ms = 1000 * 60 * 60,
// };

void app_cb(void *p)
{
    extern uint8_t watchface_index;
    extern char *defaultPath;
    extern char watchface_path[];
    switch (gui_prism_mirror3d_get_enter_face())
    {
    case 0:
        watchface_index = 3;
        sprintf(watchface_path, "%sapp/watchface1/watchface1.xml", GUI_ROOT_FOLDER);
        break;
    case 1:
        watchface_index = 0;
        break;
    case 2:
        watchface_index = 2;
        break;
    case 3:
        watchface_index = 1;
        break;
    case 4:
        watchface_index = 3;
        sprintf(watchface_path, "%sapp/watchface3/watchface3.xml", GUI_ROOT_FOLDER);
        break;
    case 5:
        watchface_index = 3;
        sprintf(watchface_path, "%sapp/watchface2/watchface2.xml", GUI_ROOT_FOLDER);
        break;
    default:
        break;
    }
    extern void *get_app_hongkong(void);
    gui_app_switch(gui_current_app(), (gui_app_t *)get_app_hongkong());
    // gui_app_switch(gui_current_app(), &rtk_gui_demo_switch_image1);
}

void callback_touch_clike_return()
{
    gui_app_switch(gui_current_app(), _get_app_APP_WATCHFACE_PRISM3D_handle());
}

// GUI_INIT_APP_EXPORT(app_init);
