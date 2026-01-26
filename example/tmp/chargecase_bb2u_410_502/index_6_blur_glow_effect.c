/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_view_macro.h"
#include "gui_canvas.h"
#include "nanovg.h"
#include <math.h>
#include "gui_img.h"

static float glow_time = 1.3f;

static void update_glow(float dt)
{
    //glow_time += dt;
    GUI_UNUSED(dt);
}

static void draw_glowing_sphere(
    NVGcontext *vg,
    float cx, float cy, float radius
)
{
    float t = glow_time;

    float pulse = 0.5f + 0.5f * 1;
    float glow_radius = radius + 80.0f + 120.0f * pulse;
    float glow_alpha = 0.4f + 0.4f * pulse;
    float hue = fmodf(t * 45.0f, 360.0f);

    nvgSave(vg);

    NVGcolor outer_glow = nvgHSLA(hue / 360.0f, 0.9f, 0.7f, (unsigned char)(255 * glow_alpha));
    NVGcolor transparent = nvgRGBA(0, 0, 0, 0);

    NVGpaint glow_paint = nvgRadialGradient(
                              vg, cx, cy,
                              radius * 0.8f, glow_radius,
                              outer_glow, transparent
                          );

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, glow_radius);
    nvgFillPaint(vg, glow_paint);
    nvgFill(vg);

    NVGcolor mid_glow = nvgHSLA(hue / 360.0f, 0.9f, 0.8f, (unsigned char)(255 * glow_alpha * 0.7f));
    NVGpaint mid_paint = nvgRadialGradient(
                             vg, cx, cy,
                             radius * 0.6f, radius * 2.5f,
                             mid_glow, transparent
                         );

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, radius * 2.5f);
    nvgFillPaint(vg, mid_paint);
    nvgFill(vg);

    NVGcolor dark = nvgRGBA(80, 80, 100, 255);
    NVGcolor highlight = nvgRGBA(255, 255, 255, 200);

    NVGpaint sphere_paint = nvgRadialGradient(
                                vg, cx - radius * 0.3f, cy - radius * 0.3f,
                                radius * 0.2f, radius,
                                highlight, dark
                            );

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, radius);
    nvgFillPaint(vg, sphere_paint);
    nvgFill(vg);

    float hx = cx - radius * 0.35f;
    float hy = cy - radius * 0.4f;
    float hs = radius * 0.25f;

    NVGpaint hl_paint = nvgRadialGradient(
                            vg, hx, hy, 0, hs,
                            nvgRGBA(255, 255, 255, 180), nvgRGBA(255, 255, 255, 0)
                        );

    nvgBeginPath(vg);
    nvgCircle(vg, hx, hy, hs);
    nvgFillPaint(vg, hl_paint);
    nvgFill(vg);

    nvgRestore(vg);
}
static void render(NVGcontext *vg)
{
    float dt = 0.001f;
    if (dt <= 0) { dt = 0.016f; }

    update_glow(dt);

    draw_glowing_sphere(vg, 200, 300, 80.0f);
}
static void canvas_rect_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    render(vg);
}
#define CANVAS_W2 480
#define CANVAS_H2 480
#define CANVAS_BUFFER_LENGTH2 CANVAS_W2*CANVAS_H2*4
static uint8_t *buffer = 0;
static void blur_glow_effect_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view); GUI_UNUSED(buffer);
    gui_log("blur_glow_effect_view switch in\n");
    // gui_canvas_t *canvas = gui_canvas_create(view, "canvas", 0, 0, 0, CANVAS_W2, CANVAS_H2);
    // gui_canvas_set_canvas_cb(canvas, canvas_rect_cb222);

    buffer = gui_lower_malloc(CANVAS_W2 * CANVAS_H2 * 4);
    memset(buffer, 0, CANVAS_W2 * CANVAS_H2 * 4);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 480, 480, render, buffer);
    gui_img_t *img = gui_img_create_from_mem(view, 0, buffer, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);



    gui_view_switch_on_event(view, "gif_apng_support_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, "inner_shadow_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void blur_glow_effect_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("blur_glow_effect_view switch out\n");
    if (buffer)
    {
        gui_lower_free(buffer);
    }
}

GUI_VIEW_INSTANCE("blur_glow_effect_view", 1, blur_glow_effect_switch_in,
                  blur_glow_effect_switch_out);

