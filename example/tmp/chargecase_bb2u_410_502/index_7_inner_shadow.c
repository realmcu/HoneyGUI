/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_view_macro.h"
#include "gui_canvas.h"
#include "gui_img.h"
#include "tp_algo.h"
#include <math.h>

#define USE_IMG 1
#define CANVAS_W 410
#define CANVAS_H 502
#define CANVAS_BUFFER_LENGTH CANVAS_W*CANVAS_H*2
static void *buffer = NULL;

// Arc control point Y coordinate
static float arc_control_y = 0;
static float arc_control_x = 0;

#if USE_IMG
static void render(NVGcontext *ctx)
{
#else
static void render(gui_canvas_t *canvas)
{
    NVGcontext *ctx = canvas->vg;
#endif
    float transform[6];

    // Animation time control
    static uint32_t last_time = 0;
    static float animation_time = 0.0f;
    uint32_t current_time = gui_ms_get();

    // Calculate animation time (update every 50ms)
    if (current_time - last_time > 50)
    {
        animation_time += 0.15f;
        if (animation_time > 2.0f * NVG_PI)
        {
            animation_time = 0.0f;
        }
        last_time = current_time;
    }

    // Commented out water drop drawing code
    // nvgSave(ctx);
    // nvgTransformTranslate(transform, 200, 200);
    // nvgTransform(ctx, transform[0], transform[1], transform[2],
    //              transform[3], transform[4], transform[5]);

    // // Simplified water drop drawing (commented out)
    // nvgBeginPath(ctx);
    // float wave_amplitude = 20.0f * sinf(animation_time * 2.0f);
    // float rotation_angle = 0.2f * sinf(animation_time * 0.5f);
    // nvgRotate(ctx, rotation_angle);
    // nvgMoveTo(ctx, 0, -100);
    // nvgBezierTo(ctx, 60 + wave_amplitude, -80, 80, 0, 40, 80);
    // nvgBezierTo(ctx, 0, 100 + wave_amplitude * 0.5f, -40, 80, -60 - wave_amplitude, -80);
    // nvgLineTo(ctx, 0, -100);
    // nvgClosePath(ctx);

    // // Gradient color calculation (commented out)
    // float color_shift = sinf(animation_time * 0.8f) * 0.5f + 0.5f;
    // NVGpaint vertical_gradient = nvgLinearGradient(ctx, 0, -120, 0, 120,
    //                                               nvgRGBA(255 * color_shift, 255, 0, 255),
    //                                               nvgRGBA(0, 255, 255 * (1.0f - color_shift), 255));
    // nvgFillPaint(ctx, vertical_gradient);
    // nvgFill(ctx);

    // nvgRestore(ctx);

    // Draw gradient arc
    nvgSave(ctx);
    nvgTransformTranslate(transform, CANVAS_W / 2, CANVAS_H / 2);
    nvgTransform(ctx, transform[0], transform[1], transform[2],
                 transform[3], transform[4], transform[5]);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, -100, 0);
    nvgQuadTo(ctx, arc_control_x * 2.f, arc_control_y * 2.f, 100, 0);

    // Red to blue gradient
    NVGpaint arc_gradient = nvgLinearGradient(ctx, -100, 0, 100, 0,
                                              nvgRGBA(255, 0, 0, 255),
                                              nvgRGBA(0, 0, 255, 255));

    nvgStrokePaint(ctx, arc_gradient);
    nvgStrokeWidth(ctx, 14.0f);
    nvgStroke(ctx);

    // Add shaking effect vertical ellipse ring
    nvgSave(ctx);

    // Shake effect parameters
    float shake_intensity = 1.5f * sinf(animation_time * 4.0f);  // Further reduce shake amplitude
    float scale_variation = 0.05f * sinf(animation_time * 2.0f) + 0.95f;  // Smaller scale variation

    // Apply transformation to arc midpoint - ensure ellipse ring precisely follows arc midpoint
    nvgTranslate(ctx, arc_control_x, arc_control_y);
    nvgTranslate(ctx, shake_intensity, 0);  // Horizontal shake

    // Draw vertical ellipse ring - adjust size and position
    nvgBeginPath(ctx);
    nvgEllipse(ctx, 0, 0, 15 * scale_variation,
               20 * scale_variation);  // Slightly increase ellipse size

    // Ellipse ring gradient color - adjust to more visible colors
    NVGpaint ellipse_gradient = nvgLinearGradient(ctx, 0, -25, 0, 25,
                                                  nvgRGBA(255, 0, 255, 255),  // Magenta
                                                  nvgRGBA(0, 255, 255, 255)); // Cyan

    nvgStrokePaint(ctx, ellipse_gradient);
    nvgStrokeWidth(ctx, 4.0f);  // Reduce line width for more precise following

    nvgStroke(ctx);
}

static void timer_cb(void *obj)
{
    GUI_UNUSED(obj);
    touch_info_t *tp = tp_get_info();

    // Check if touch is pressed
    if (tp->pressing && tp->type == TOUCH_HOLD_Y)
    {
        int16_t x = tp->x + tp->deltaX;
        int16_t y = tp->y + tp->deltaY;
        arc_control_x = x - CANVAS_W / 2;
        arc_control_y = y - CANVAS_H / 2;
    }
    else
    {
        // Spring effect when not pressing and control points are not zero
        if (arc_control_x != 0 || arc_control_y != 0)
        {
            // Spring damping factor (0.9 for smooth spring effect)
            float damping = -0.9f;

            // Apply spring effect to x coordinate
            if (arc_control_x != 0)
            {
                arc_control_x *= damping;
                // Add oscillation effect when close to zero
                if (fabsf(arc_control_x) < 1.0f)
                {
                    arc_control_x *= -0.5f; // Reverse direction for spring oscillation
                }
                // Stop oscillation when very close to zero
                if (fabsf(arc_control_x) < 0.1f)
                {
                    arc_control_x = 0;
                }
            }

            // Apply spring effect to y coordinate
            if (arc_control_y != 0)
            {
                arc_control_y *= damping;
                // Add oscillation effect when close to zero
                if (fabsf(arc_control_y) < 1.0f)
                {
                    arc_control_y *= -0.5f; // Reverse direction for spring oscillation
                }
                // Stop oscillation when very close to zero
                if (fabsf(arc_control_y) < 0.1f)
                {
                    arc_control_y = 0;
                }
            }
        }
    }
#if USE_IMG
    uint32_t size = CANVAS_W * CANVAS_H + 2;
    gui_memset32(buffer, 0, size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, CANVAS_W, CANVAS_H, render, buffer);
#endif
}

static void inner_shadow_switch_in(gui_view_t *view)
{
    gui_view_switch_on_event(view, "blur_glow_effect_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, "radial_gradient_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    GUI_UNUSED(view);
    if (!buffer)
    {
        uint32_t size = CANVAS_W * CANVAS_H * 4 + sizeof(gui_rgb_data_head_t);
        buffer = gui_malloc(size);
        memset(buffer, 0, size);
    }

#if USE_IMG
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, CANVAS_W, CANVAS_H, render, buffer);
    gui_img_t *img = gui_img_create_from_mem(view, 0, buffer, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    gui_obj_create_timer((void *)img, 10, -1, timer_cb);
    gui_obj_start_timer((void *)img);
#else
    gui_canvas_t *canvas = gui_canvas_create(view, 0, 0, 0, 0, CANVAS_W, CANVAS_H);
    gui_canvas_set_canvas_cb(canvas, render);
    gui_obj_create_timer((void *)canvas, 10, -1, timer_cb);
    gui_obj_start_timer((void *)canvas);
#endif
    gui_log("inner_shadow_view switch in\n");
}

static void inner_shadow_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("inner_shadow_view switch out\n");
}

GUI_VIEW_INSTANCE("inner_shadow_view", 1, inner_shadow_switch_in, inner_shadow_switch_out);