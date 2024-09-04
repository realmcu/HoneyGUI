#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"

static void canvas_cb_black(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;


    nvgRect(vg, 20, 20, 200, 180);

    nvgStrokeWidth(vg, 8.0f);
    nvgStrokeColor(vg, nvgRGB(255, 0, 0));
    nvgStroke(vg);


    NVGpaint gradient = nvgLinearGradient(vg, 20, 20, 220, 200, nvgRGB(255, 0, 0), nvgRGBA(0, 255, 0,
                                          255));
    nvgFillPaint(vg, gradient);

    // nvgFillColor(vg, nvgRGBA(0, 255, 0, 200));

    nvgFill(vg);

}

static void app_nanovg_ui_design(gui_app_t *app)
{

    gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 454, 454);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);

}

static gui_app_t nanovg_demo =
{
    .screen =
    {
        .name = "nanovg_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_nanovg_ui_design,
    .active_ms = 1000 * 60 * 60,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&nanovg_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);










