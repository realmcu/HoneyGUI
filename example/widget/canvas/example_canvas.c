/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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

/* canvas draw callback start */
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
/* canvas draw callback end */

/* canvas create start */
void test_circle_drawing(void)
{
    gui_canvas_t *canvas = gui_canvas_create(gui_obj_get_root(), "test_circle", NULL, 0, 0, 300, 200);
    gui_canvas_set_canvas_cb(canvas, draw_circle_callback);
}
/* canvas create end */
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
    // static float  progress;
    // progress += 0.01f;
    nvgArc(canvas->vg, 480 / 2, 480 / 2, 150, 0, -6.14f, NVG_CCW);
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

static void draw_pre_canvas_cb(NVGcontext *vg)
{
    /* Yellow filled circle */
    nvgBeginPath(vg);
    nvgCircle(vg, 100, 100, 50);
    nvgFillColor(vg, nvgRGBA(255, 255, 0, 255));
    nvgFill(vg);

    /* Blue ellipse outline */
    nvgBeginPath(vg);
    nvgEllipse(vg, 200, 100, 80, 40);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 255, 255));
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);

    /* Rounded rect: gradient fill + red stroke share the same path */
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 150, 150, 200, 180, 20);
    {
        NVGpaint gradient = nvgLinearGradient(vg, 150, 150, 350, 330,
                                              nvgRGB(255, 0, 0),
                                              nvgRGBA(0, 255, 0, 255));
        nvgFillPaint(vg, gradient);
        nvgFill(vg);
    }
    nvgStrokeWidth(vg, 8.0f);
    nvgStrokeColor(vg, nvgRGB(255, 0, 0));
    nvgStroke(vg);

    /* Dark red arc */
    nvgBeginPath(vg);
    nvgArc(vg, 480 / 2, 480 / 2, 150, 0, -6.14f, NVG_CCW);
    nvgStrokeWidth(vg, 20);
    nvgStrokeColor(vg, nvgRGB(178, 34, 34));
    nvgStroke(vg);
}

void test_pre_buffer_drawing(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint32_t image_w = dc->screen_width;
    uint32_t image_h = dc->screen_height;
    uint8_t *image_buffer = gui_lower_malloc(image_w * image_h * 4 + sizeof(gui_rgb_data_head_t));
    memset(image_buffer, 0, image_w * image_h * 4 + sizeof(gui_rgb_data_head_t));

    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_pre_canvas_cb,
                                      image_buffer);

    gui_img_create_from_mem(gui_obj_get_root(), "canvas_img", image_buffer, 0, 0, 0,
                            0);
}


static int app_init(void)
{
    // test_rect_drawing();
    // test_arc_drawing();
    // test_circle_drawing();
    test_pre_buffer_drawing();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* canvas example end */










