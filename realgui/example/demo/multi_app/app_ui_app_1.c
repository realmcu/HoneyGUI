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
#include "..\tiger_blue.txt"
#include "..\tiger_grey.txt"


static void app_1_button_cb(void *obj, gui_event_t e);


static void app_1_ui_design(gui_app_t *app)
{
    gui_img_t *img = gui_img_create_from_mem(&(app->screen),  "img_1_test",
                                             (void *)_actiger_grey, 0, 0, 0, 0);

    gui_obj_add_event_cb(img, (gui_event_cb_t)app_1_button_cb, GUI_EVENT_TOUCH_PRESSED, NULL);
    return;
}

static void loop(void *tihs)
{
    while ((1))
    {
        gui_thread_mdelay(1000);
        gui_log("I am in gui app 1 task \n");
    }
}

gui_app_t realgui_base_demo_app_1 =
{
    .screen =
    {
        .name = "app_1",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_1_ui_design,
    .active_ms = 1000 * 60 * 60,
    // .thread_entry = loop,
};

static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&realgui_base_demo_app_1);
    return 0;
}

static void app_1_button_cb(void *obj, gui_event_t e)
{
    gui_log("gui_app_switch_to lvgl line = %d \n", __LINE__);
    extern gui_app_t realgui_base_demo_app_0;
    gui_app_switch(&realgui_base_demo_app_1, &realgui_base_demo_app_0);
}

