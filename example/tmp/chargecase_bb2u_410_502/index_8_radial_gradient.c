#include "gui_view_macro.h"
#include "gui_canvas.h"
#include "nanovg.h"
#include <math.h>

static float ease_sin01(float t)
{

    return 0.5f * (sinf(t) + 1.0f);
}


static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}


static void draw_dynamic_radial_circle(NVGcontext *vg,
                                       float cx, float cy, float baseRadius,
                                       float t)
{

    float pulse = ease_sin01(t * 1.2f);
    float innerR = baseRadius * (0.18f + 0.08f * pulse);
    float outerR = baseRadius * (0.95f + 0.15f * pulse);


    float offsetR = baseRadius * 0.10f;
    float cx_anim = cx + cosf(t * 0.7f) * offsetR;
    float cy_anim = cy + sinf(t * 0.9f) * offsetR;


    uint8_t coreAlpha = (uint8_t)clampf(180 + 60 * sinf(t * 1.5f), 60, 255);
    uint8_t edgeAlpha = (uint8_t)clampf(10 + 30 * ease_sin01(t * 1.0f), 6, 80);

    NVGcolor innerColor = nvgRGBA(255, 255, 255, coreAlpha);
    NVGcolor outerColor = nvgRGBA(0, 128, 255, edgeAlpha);

    NVGpaint paint = nvgRadialGradient(vg, cx_anim, cy_anim, innerR, outerR, innerColor, outerColor);

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, baseRadius);
    nvgFillPaint(vg, paint);
    nvgFill(vg);
}


static void draw_dynamic_radial_roundrect(NVGcontext *vg,
                                          float x, float y, float w, float h, float r,
                                          float t)
{
    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    float maxR = fmaxf(w, h) * 0.5f;
    float pulse = ease_sin01(t * 0.8f);

    float innerR = maxR * (0.20f + 0.10f * pulse);
    float outerR = maxR * (0.85f + 0.15f * pulse);

    float offsetR = fminf(w, h) * 0.12f;
    float cx_anim = cx + cosf(t * 0.6f) * offsetR;
    float cy_anim = cy + sinf(t * 0.6f) * offsetR;

    uint8_t coreAlpha = (uint8_t)clampf(200 + 40 * sinf(t * 1.2f), 80, 255);
    uint8_t edgeAlpha = (uint8_t)clampf(16 + 32 * ease_sin01(t * 0.9f), 8, 96);

    NVGcolor innerColor = nvgRGBA(255, 255, 255, coreAlpha);
    NVGcolor outerColor = nvgRGBA(30, 30, 30, edgeAlpha);

    NVGpaint paint = nvgRadialGradient(vg, cx_anim, cy_anim, innerR, outerR, innerColor, outerColor);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, x, y, w, h, r);
    nvgFillPaint(vg, paint);
    nvgFill(vg);
}


static void canvas_rect_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;


    static double t = 0.0;

    t += 1.0 / 300.0;


    nvgBeginPath(vg);

    nvgFillColor(vg, nvgRGB(240, 242, 247));
    nvgFill(vg);


    draw_dynamic_radial_circle(vg, 160.0f, 140.0f, 80.0f, (float)t);


    draw_dynamic_radial_roundrect(vg, 100.0f, 300.0f, 240.0f, 160.0f, 20.0f, (float)t);
}


static void radial_gradient_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("radial_gradient_view switch in\n");
    gui_canvas_t *canvas = gui_canvas_create(view, "canvas", 0, 0, 0, 480, 480);
    gui_canvas_set_canvas_cb(canvas, canvas_rect_cb);
    gui_view_switch_on_event(view, gui_view_descriptor_get("inner_shadow_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("multiple_layer_animation_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void radial_gradient_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("radial_gradient_view switch out\n");
}

GUI_VIEW_INSTANCE("radial_gradient_view", false, radial_gradient_switch_in,
                  radial_gradient_switch_out);

