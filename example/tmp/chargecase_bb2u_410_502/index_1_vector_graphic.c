/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_view_macro.h"
#include "gui_canvas.h"
#include "gui_img.h"

#include <math.h>

typedef struct
{
    float time;
    float scale;
    float angle;
    float corner_radius;
    float alpha;
    float offset_x;
    float offset_y;
} AnimState;

static AnimState anim = {0};

static uint8_t draw_type = 0;
static bool redraw = false;

static float w = 200;
static float h = 200;
static NVGcolor col1;
static NVGcolor col2;


static void update_animation(float dt)
{
    anim.time += dt;

    anim.scale = 1.0f + 0.3f * (0.5f + 0.5f * sinf(anim.time * 1.8f));

    anim.angle = anim.time * 30.0f;

    anim.corner_radius = 5.0f + 55.0f * (0.5f + 0.5f * sinf(anim.time * 2.5f));

    anim.alpha = 0.6f + 0.4f * (0.5f + 0.5f * sinf(anim.time * 1.2f));

    anim.offset_x = 8.0f * sinf(anim.time * 1.1f);
    anim.offset_y = 6.0f * cosf(anim.time * 1.4f);
}
static void render(NVGcontext *vg)
{
    if (draw_type == 0)
    {
        float dt = 0.01f;

        update_animation(dt);

        float center_x = 200.0f;  // 400/2
        float center_y = 200.0f;  // 400/2

        nvgSave(vg);

        nvgTranslate(vg, center_x, center_y);

        float rect_w = w * anim.scale;
        float rect_h = h * anim.scale;

        nvgBeginPath(vg);
        nvgRoundedRect(vg, -rect_w / 2, -rect_h / 2, rect_w, rect_h, anim.corner_radius);

        nvgStrokeWidth(vg, 8.0f * anim.scale);
        NVGcolor stroke_col = nvgRGB(255, 0, 0);
        stroke_col.a = anim.alpha;
        nvgStrokeColor(vg, stroke_col);
        nvgStroke(vg);

        // float hue = fmodf(anim.time * 40.0f, 360.0f);
        // NVGcolor col1 = nvgHSLA(hue / 360.0f, 0.8f, 0.6f, (unsigned char)(255 * anim.alpha));
        // NVGcolor col2 = nvgHSLA((hue + 90) / 360.0f, 0.8f, 0.6f, (unsigned char)(255 * anim.alpha));

        NVGpaint gradient = nvgLinearGradient(vg, -rect_w / 2, -rect_h / 2, rect_w / 2, rect_h / 2, col1,
                                              col2);
        nvgFillPaint(vg, gradient);
        nvgFill(vg);

        nvgRestore(vg);
    }
    else if (draw_type == 1)
    {
        nvgSave(vg);

        nvgTranslate(vg, 200, 200);

        nvgBeginPath(vg);
        nvgEllipse(vg, 0, 0, w / 2, h / 2);

        NVGpaint ellipse_gradient = nvgLinearGradient(vg, -w / 2, 0, w / 2, 0,
                                                      col1,
                                                      col2);
        nvgFillPaint(vg, ellipse_gradient);
        nvgFill(vg);

        nvgStrokeWidth(vg, 6.0f);
        nvgStrokeColor(vg, nvgRGB(0, 0, 0));
        nvgStroke(vg);

        nvgRestore(vg);
    }
}

#define CANVAS_W 400
#define CANVAS_H 400
#define CANVAS_BUFFER_LENGTH CANVAS_W*CANVAS_H*4
static uint8_t *buffer = 0;
static void vector_graphic_switch_in_timer(void *p)
{
    GUI_UNUSED(p); gui_log("111\n");
    memset(buffer, 0,  CANVAS_BUFFER_LENGTH);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 400, 400, render, buffer);
}
static int resource(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    extern const unsigned char _binary_root_0x704D1400_bin_start[];
    resource_root = (unsigned char *)_binary_root_0x704D1400_bin_start;
#endif
    return 0;
}
static void img_cb(void *obj)
{
    GUI_UNUSED(obj);
    if (!redraw) { return; }
    uint32_t size = CANVAS_W * CANVAS_H * 4 + sizeof(gui_rgb_data_head_t);
    memset(buffer, 0,  size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 400, 400, render, buffer);
    redraw = 0;
}
static void vector_graphic_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch in\n");
    // gui_canvas_t *canvas = gui_canvas_create(view, "canvas", 0, 0, 0, CANVAS_W, CANVAS_H);
    // gui_canvas_set_canvas_cb(canvas, canvas_rect_cb111);
    col1 = nvgRGB(255, 0, 0);
    col2 = nvgRGB(0, 0, 255);

    uint32_t size = CANVAS_W * CANVAS_H * 4 + sizeof(gui_rgb_data_head_t);
    if (!buffer)
    {
        buffer = gui_lower_malloc(size);
        memset(buffer, 0,  size);
    }
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 480, 480, render, buffer);
    gui_img_t *img = gui_img_create_from_mem(view, 0, buffer, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    gui_obj_create_timer((void *)img, 10, 1, img_cb);
    gui_obj_start_timer((void *)img);



    gui_view_switch_on_event(view, "transparent_gradient_animation_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, "fps_show_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void vector_graphic_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch out\n");
    if (buffer)
    {
        gui_lower_free(buffer);
    }

}


#include "shell.h"
void draw_info_update(int argc, char *argv[])
{
    GUI_UNUSED(argc);
    int i = 1;
    while (i <= argc - 1)
    {
        if (!strcmp(argv[i], "rect"))
        {
            draw_type = 0;
            redraw = 1;
        }
        else if (!strcmp(argv[i], "ellipse"))
        {
            draw_type = 1;
            redraw = 1;
        }
        else if (!strcmp(argv[i], "col1"))
        {
            col1.r = atoi(argv[++i]);
            redraw = 1;
        }
        else if (!strcmp(argv[i], "col2"))
        {
            col2.g = atoi(argv[++i]);
            redraw = 1;
        }
        else if (!strcmp(argv[i], "w"))
        {
            w = atof(argv[++i]);
            redraw = 1;
        }
        else if (!strcmp(argv[i], "h"))
        {
            h = atof(argv[++i]);
            redraw = 1;
        }
        ++i;
    }

}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
    draw, draw_info_update, test);


#include "gui_win.h"
GUI_VIEW_INSTANCE("vector_graphic_view", 1, vector_graphic_switch_in, vector_graphic_switch_out);
unsigned char *resource_root = 0;
void fps_create(void *parent);
static int chargecase_demo_entry(void)
{

    gui_win_t *win = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    gui_view_create(win, "vector_graphic_view", 0, 0, 0, 0);
    // gui_view_create(win, "inner_shadow_view", 0, 0, 0, 0);
    // gui_view_create(win, "fps_show_view", 0, 0, 0, 0);
    fps_create(gui_obj_get_root());
    return 0;
}
GUI_INIT_VIEW_DESCRIPTOR_REGISTER(resource);
GUI_INIT_APP_EXPORT(chargecase_demo_entry);