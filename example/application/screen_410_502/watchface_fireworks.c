/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "fireworks_clock_view"
// Function to convert degrees to radians
#define DEG_TO_RAD(angleInDegrees) ((angleInDegrees) * M_PI_F / 180.0f)
#define HOUR_HAND_LENGTH 0.5f
#define MINUTE_HAND_LENGTH 0.6f
#define SECOND_HAND_LENGTH 0.7f
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*============================================================================*
*                             Types
*============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_fireworks_design(gui_view_t *view);
static void cleanup_resources(gui_view_t *view);
static void draw_clock_hand(NVGcontext *vg, float centerX, float centerY, float length,
                            float angleInDegrees, NVGcolor color);
static void draw_tapered_line(NVGcontext *vg, float startX, float startY, float endX, float endY,
                              float startWidth, float endWidth, NVGcolor color);
static void firework_sim_cb(NVGcontext *vg);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_fireworks_design,
    .on_switch_out = cleanup_resources,

    .keep = false,
};

/* Draw Clock */
static gui_img_t *current_img = NULL;
static uint8_t *img_data = NULL;
static float firework_start = 50.0f;
static float firework_alpha = 255.0f;
static uint16_t screen_w = 0;
static uint16_t screen_h = 0;
static NVGcolor rgba_color[3] = {0};
static uint8_t currentColorIndex = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static void draw_hexagon(NVGcontext *vg, float centerX, float centerY, float radius,
                         float rotationAngle, NVGcolor color)
{
    nvgBeginPath(vg);
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * M_PI_F / 3.0f + rotationAngle;
        float x = centerX + radius * cosf(angle);
        float y = centerY + radius * sinf(angle);
        if (i == 0)
        {
            nvgMoveTo(vg, x, y);
        }
        else
        {
            nvgLineTo(vg, x, y);
        }
    }
    nvgClosePath(vg);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);
}

static void initialize_colors(void)
{
    rgba_color[0] = nvgRGBA(255, 255, 128, 230);
    rgba_color[1] = nvgRGBA(177, 255, 47, 230);
    rgba_color[2] = nvgRGBA(255, 69, 0, 230);
}

static void draw_background_ring(NVGcontext *vg, float centerX, float centerY, float radius)
{

    NVGpaint paint = nvgRadialGradient(vg, centerX, centerY, radius * 0.95f, radius,
                                       rgba_color[currentColorIndex],
                                       nvgRGBA(0, 0, 0, 0));

    nvgBeginPath(vg);
    nvgArc(vg, centerX, centerY, radius, 0, 2 * NVG_PI, NVG_CW);
    nvgArc(vg, centerX, centerY, radius * 0.85f, 0, 2 * NVG_PI, NVG_CCW);
    nvgPathWinding(vg, NVG_HOLE);

    nvgFillPaint(vg, paint);
    nvgFill(vg);
}

static void create_firework_clock_cb(NVGcontext *vg)
{
    float centerX = screen_w / 2.0f;
    float centerY = screen_h / 2.0f;
    float hexagonRadius = MIN(screen_w, screen_h) * 0.4f;
    float ringRadius = hexagonRadius + 1.0f;

    draw_background_ring(vg, centerX, centerY, ringRadius);

    NVGcolor mainHexColor = nvgRGB(255, 255, 255);
    draw_hexagon(vg, centerX, centerY, hexagonRadius, 0, mainHexColor);
    NVGcolor offsetHexColor = nvgRGB(255, 255, 255);
    draw_hexagon(vg, centerX, centerY, hexagonRadius, M_PI_F / 6, offsetHexColor);

    // Clock
    extern struct tm *timeinfo;

    int hours = timeinfo->tm_hour;
    int minutes = timeinfo->tm_min;
    int seconds = timeinfo->tm_sec;

    // Calculate angles for each hand
    float hourAngle = (hours % 12 + minutes / 60.0f) * 30;  // 360/12 = 30 degrees per hour
    float minuteAngle = (minutes + seconds / 60.0f) * 6;    // 360/60 = 6 degrees per minute
    float secondAngle = seconds * 6;                        // 360/60 = 6 degrees per second

    // Draw hour, minute, and second hands with different colors
    draw_clock_hand(vg, centerX, centerY, hexagonRadius * HOUR_HAND_LENGTH, hourAngle, nvgRGBA(255, 255,
                    0, 255));  // Yellow for hours
    draw_clock_hand(vg, centerX, centerY, hexagonRadius * MINUTE_HAND_LENGTH, minuteAngle, nvgRGBA(0,
                    255,
                    0, 255)); // Green for minutes
    draw_clock_hand(vg, centerX, centerY, hexagonRadius * SECOND_HAND_LENGTH, secondAngle, nvgRGBA(255,
                    0,
                    0, 255)); // Red for seconds
    // Firework cb
    {
        firework_sim_cb(vg);
    }
}

static void update_clock(void *param)
{
    (void)param;
    currentColorIndex = (currentColorIndex + 1) % 3;

    size_t buffer_size = screen_w * screen_h * 4 + sizeof(gui_rgb_data_head_t);
    memset(img_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, screen_w,
                                      screen_h, create_firework_clock_cb, img_data);
    // gui_log("currentColorIndex:%d\n",currentColorIndex);
}

static void cleanup_resources(gui_view_t *view)
{
    (void)view;
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
}

static void draw_clock_hand(NVGcontext *vg, float centerX, float centerY, float length,
                            float angleInDegrees, NVGcolor color)
{
    float angleInRadians = DEG_TO_RAD(angleInDegrees - 90); // Adjust to start from '12' position
    float xEnd = centerX + cosf(angleInRadians) * length;
    float yEnd = centerY + sinf(angleInRadians) * length;

    nvgBeginPath(vg);
    nvgMoveTo(vg, centerX, centerY);
    nvgLineTo(vg, xEnd, yEnd);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 10.0f);
    nvgStroke(vg);
}

static void draw_tapered_line(NVGcontext *vg, float startX, float startY, float endX, float endY,
                              float startWidth, float endWidth, NVGcolor color)
{
    float dirX = endX - startX;
    float dirY = endY - startY;
    float length = sqrtf(dirX * dirX + dirY * dirY);

    if (length > 0.0001f)
    {
        dirX /= length;
        dirY /= length;
    }
    float perpX = -dirY;
    float perpY = dirX;

    float halfStartWidth = startWidth * 0.5f;
    float halfEndWidth = endWidth * 0.5f;

    float startWidthX1 = startX + perpX * halfStartWidth;
    float startWidthY1 = startY + perpY * halfStartWidth;
    float startWidthX2 = startX - perpX * halfStartWidth;
    float startWidthY2 = startY - perpY * halfStartWidth;

    float endWidthX1 = endX + perpX * halfEndWidth;
    float endWidthY1 = endY + perpY * halfEndWidth;
    float endWidthX2 = endX - perpX * halfEndWidth;
    float endWidthY2 = endY - perpY * halfEndWidth;

// NVGcolor color_circle = color;
    nvgBeginPath(vg);
    nvgCircle(vg, startX, startY, halfStartWidth + 1);
    nvgFillColor(vg, color);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, startWidthX1, startWidthY1);
    nvgLineTo(vg, endWidthX1, endWidthY1);
    nvgLineTo(vg, endWidthX2, endWidthY2);
    nvgLineTo(vg, startWidthX2, startWidthY2);
    nvgClosePath(vg);

// color.a *= 0.8;
    nvgFillColor(vg, color);
    nvgFill(vg);

}

static void firework_sim_cb(NVGcontext *vg)
{
    float circleCenterX = screen_w / 2.0;
    float circleCenterY = screen_h / 2.0;

    if (firework_alpha <= 30.0f)
    {
        firework_start = 50.0f;
        firework_alpha = 255.0f;
    }
    else
    {
        firework_start += 50.0f;
        firework_alpha -= 35.0f;
    }
    for (int i = 0; i < 30; i++)
    {
        float baseAngle = (2 * M_PI_F * i) / 30.0f;

        float startX = circleCenterX + cosf(baseAngle) * firework_start;
        float startY = circleCenterY + sinf(baseAngle) * firework_start;

        float spreadAngle = baseAngle;

        //(25-60)
        float length = 25.0f + ((float)xorshift16() / (float)RAND_MAX) * 30.0f;

        float endX = startX + cosf(spreadAngle) * length;
        float endY = startY + sinf(spreadAngle) * length;

        int r = xorshift16() % 255;
        int g = xorshift16() % 255;
        int b = xorshift16() % 255;
        NVGcolor lineColor = nvgRGBA(r, g, b, firework_alpha);
        float startWidth = 5.0f + ((float)xorshift16() / (float)RAND_MAX) * 2.0f;
        float endWidth = 1.0f;

        draw_tapered_line(vg, endX, endY, startX, startY, startWidth, endWidth, lineColor);
    }
}

static void app_ui_fireworks_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    gui_dispdev_t *dc = gui_get_dc();
    screen_w = gui_get_screen_width();
    screen_h = gui_get_screen_height();

    gui_win_t *fireworks_win = gui_win_create(obj, "fireworks_win", 0, 0, dc->screen_width,
                                              dc->screen_height);
    initialize_colors();

    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
    size_t buffer_size = screen_w * screen_h * 4 + sizeof(gui_rgb_data_head_t);
    img_data = (uint8_t *)gui_lower_malloc(buffer_size);
    memset(img_data, 0, buffer_size);
    current_img = gui_img_create_from_mem(fireworks_win, 0, (void *)img_data, 0, 0, 0, 0);
    GUI_BASE(current_img)->w = screen_w;
    GUI_BASE(current_img)->h = screen_h;
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, screen_w,
                                      screen_h, create_firework_clock_cb, img_data);
    gui_obj_create_timer(&(fireworks_win->base), 300, true, update_clock);
    gui_obj_start_timer(&(fireworks_win->base));
}
