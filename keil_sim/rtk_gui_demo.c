#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_canvas.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "tiger.txt"
#include "gui_server.h"
#include "gui_components_init.h"

// static void canvas_cb_black(gui_canvas_t *canvas)
// {
//    nvgRect(canvas->vg, 0, 0, 368, 448);
//    nvgFillColor(canvas->vg, nvgRGBA(0, 0xFF, 0, 128));
//    nvgFill(canvas->vg);
// }

static void app_dialing_ui_design(gui_app_t *app)
{
    gui_tabview_t *tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tab_t *tab_1 = gui_tab_create(tv, "tb_1",    0, 0, 0, 0, 0, 0);
    gui_tab_t *tab_2 = gui_tab_create(tv, "tb_2",    0, 0, 0, 0, 1, 0);

    gui_img_t *img_1 = gui_img_create_from_mem(tab_1,  "img_1", (void *)_actiger, 0, 0, 0, 0);
    gui_img_t *img_2 = gui_img_create_from_mem(tab_2,  "img_2", (void *)_actiger, 0, 0, 0, 0);
//    gui_canvas_t *canvas = gui_canvas_create(&app->screen, "canvas", 0, 0, 0, 368, 448);
//    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
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


static int app_init(void)
{
    rtgui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);




