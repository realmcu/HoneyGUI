/* canvas example start */
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "gui_canvas.h"

static void draw_circle_callback(gui_canvas_t *canvas)
{
    nvgBeginPath(canvas->vg);
    nvgCircle(canvas->vg, 100, 100, 50);
    nvgFillColor(canvas->vg, nvgRGBA(255, 255, 0, 255));
    nvgFill(canvas->vg);

    nvgBeginPath(canvas->vg);
    nvgEllipse(canvas->vg, 200, 100, 80, 40);
    nvgStrokeColor(canvas->vg, nvgRGBA(0, 0, 255, 255));
    nvgStrokeWidth(canvas->vg, 2.0f);
    nvgStroke(canvas->vg);
}

void test_circle_drawing(void)
{
    gui_canvas_t *canvas = gui_canvas_create(gui_obj_get_root(), "test_circle", NULL, 0, 0, 300, 200);
    gui_canvas_set_canvas_cb(canvas, draw_circle_callback);
}
static void canvas_rect_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 150, 150, 200, 180, 20);

    nvgStrokeWidth(vg, 8.0f);
    nvgStrokeColor(vg, nvgRGB(255, 0, 0));
    nvgStroke(vg);

    NVGpaint gradient = nvgLinearGradient(vg, 150, 150, 350, 330, nvgRGB(255, 0, 0), nvgRGBA(0, 255, 0,
                                          255));
    nvgFillPaint(vg, gradient);
    nvgFill(vg);
}
void test_rect_drawing(void)
{

    gui_canvas_t *canvas = gui_canvas_create(gui_obj_get_root(), "canvas", 0, 0, 0, 480, 480);

    gui_canvas_set_canvas_cb(canvas, canvas_rect_cb);
}
static void arc_cb(gui_canvas_t *canvas)
{
    static float  progress;
    progress += 0.01f;
    nvgArc(canvas->vg, 480 / 2, 480 / 2, 150, 0, 3.14f * (sinf(progress) + 1), NVG_CCW);
    nvgStrokeWidth(canvas->vg, 20);
    nvgStrokeColor(canvas->vg, nvgRGB(178, 34, 34));
    nvgStroke(canvas->vg);
}

void test_arc_drawing(void)
{
    gui_canvas_t *canvas = gui_canvas_create(gui_obj_get_root(), "test_arc", NULL, 0, 0, 480, 480);
    gui_canvas_set_canvas_cb(canvas, arc_cb);
    canvas->render = 1;
}


static int app_init(void)
{
    test_rect_drawing();
    test_arc_drawing();
    test_circle_drawing();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* canvas example end */










