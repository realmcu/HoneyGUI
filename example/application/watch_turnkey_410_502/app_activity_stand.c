/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <time.h>
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "cJSON.h"
#include "gui_canvas.h"
#include "guidef.h"
#include "gui_view.h"
#include "app_main_watch.h"
#include "nanovg.h"
#include <math.h>
/*============================================================================*
 *                            Macros
 *============================================================================*/
#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "activity_stand_view"
#ifdef COUNT_MAX
#undef COUNT_MAX
#define COUNT_MAX 250
#else
#define COUNT_MAX 250
#endif

#ifdef RADIUS_SCALE
#undef
#define RADIUS_SCALE 36
#else
#define RADIUS_SCALE 36
#endif
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
extern void time_update_cb(void *p);
/*============================================================================*
 *                            Variables
 *============================================================================*/

extern char *cjson_content;
static uint8_t *img_chart_data = NULL;
static gui_img_t *img_ring;
extern gui_img_t *img_dot;
extern uint8_t *img_dot_data;
static gui_img_t *img_chart;

static uint16_t count = COUNT_MAX; //for timer
static bool draw_flag = 0; //0: get new json data
extern gui_text_t *time_text;
static char stand_text_content[10];
static char per_text_content[10];
static char cal_text_content[10];
static char calTotal_text_content[10];
static char calUnit_text_content[10];

extern uint8_t activeIndex;
extern gui_win_t *win_activity;
extern gui_win_t *win_menu_activity;
extern bool has_draw_bg; // only draw background once
extern gui_win_t *win_activity_arc;
extern const char *majorLabels[];
// extern void arc_activity_ring_cb(NVGcontext *vg);
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
typedef struct
{
    int standCompleted;    // Has the standing goal been completed for this hour? 1=completed, 0=not completed
    int hourIndex;
    const char *hourLabel;
} HourStandData;
static void drawRoundedBar(NVGcontext *vg, float x, float y, float width, float height,
                           NVGcolor color)
{
    nvgLineCap(vg, NVG_ROUND);
    nvgLineJoin(vg, NVG_ROUND);

    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x, y - height);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, width);
    nvgStroke(vg);
}
static void drawStandDistributionChart(NVGcontext *vg, float x, float y, float width, float height)
{
    HourStandData hours[24];
    int completedCount = 0;

    for (int i = 0; i < 24; i++)
    {
        hours[i].hourIndex = i;

        // if (i == 0) { hours[i].hourLabel = "12AM"; }
        // else if (i < 12) { hours[i].hourLabel = (i < 10) ? "" : ""; }
        // else if (i == 12) { hours[i].hourLabel = "12PM"; }
        // else { hours[i].hourLabel = (i - 12 < 10) ? "" : ""; }

        // Simulate the completed standing duration (8 hours)
        if ((i >= 8 && i <= 11) ||  // 8:00 AM to 11:00 AM
            (i >= 14 && i <= 17) || // 2:00 PM to 5:00 PM
            (i >= 19 && i <= 20))   // 7-8 p.m.
        {
            hours[i].standCompleted = 1;
            completedCount++;
        }
        else
        {
            hours[i].standCompleted = 0;
        }
    }

    int totalHours = 24;
    float hourWidth = width / totalHours;
    float maxBarHeight = height * 0.6f;
    float barWidth = hourWidth * 0.6f;

    for (int i = 0; i < totalHours; i++)
    {
        float barX = x + i * hourWidth + (hourWidth - barWidth) / 2;
        float barBottomY = y + height * 0.85f;

        if (hours[i].standCompleted)
        {
            float barHeight = maxBarHeight;
            drawRoundedBar(vg, barX + barWidth / 2, barBottomY, barWidth, barHeight, nvgRGB(0x74, 0xFA, 0xF2));
        }
        else
        {
            float barHeight = maxBarHeight * 0.15f;
            drawRoundedBar(vg, barX + barWidth / 2, barBottomY, barWidth, barHeight, nvgRGB(0x55, 0x55, 0x55));
        }
    }

    const int majorHours[] = {0, 6, 12, 18, 24};
    // const char *majorLabels[] = {"12AM", "6AM", "12PM", "6PM", ""};

    for (int i = 0; i < 5; i++)
    {
        float lineX = x + majorHours[i] * hourWidth;

        nvgBeginPath(vg);
        nvgMoveTo(vg, lineX, y);
        nvgLineTo(vg, lineX, y + height);
        nvgStrokeColor(vg, nvgRGB(0x9E, 0xFF, 0xFD));
        nvgStrokeWidth(vg, 2.0f);
        nvgStroke(vg);
    }

    gui_fb_change();
}

static void drawStepBarChart_cb(NVGcontext *vg)
{
    drawStandDistributionChart(vg, 20, 0, 346, 180);
}
extern void drawCircles_cb(NVGcontext *vg) ;
static void activity_stand_timer_cb(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_timer_t *timer = img_ring->base.timer;

    count += timer->interval_ms;

    float progress = count / (float)COUNT_MAX;
    float arc_scale = 1  - progress;

    gui_img_scale(img_ring, 1.0f * arc_scale, 1.0f * arc_scale);
    gui_img_translate(img_ring, 100 * arc_scale, -150.f * arc_scale);

    // dot img
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96,
                                      drawCircles_cb, img_dot_data);
    gui_img_set_image_data(img_dot, img_dot_data);
    gui_img_refresh_size(img_dot);

    // chart img
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 366, 214,
                                      drawStepBarChart_cb, img_chart_data);
    gui_img_set_image_data(img_chart, img_chart_data);
    gui_img_refresh_size(img_chart);

    if ((count >= COUNT_MAX) || arc_scale < 0.20f)
    {
        count = 0;
        gui_obj_stop_timer(GUI_BASE(img_ring));
    }
}

static void enter_timer_cb(void *obj)
{
    gui_view_t *view = gui_view_get_current();
    if (strcmp(GUI_BASE(view)->name, CURRENT_VIEW_NAME) == 0)
    {
        count = 0;
        has_draw_bg = false;
        activeIndex = 2;
        draw_flag = 0;
        uint8_t *img_ring_data = (void *)gui_img_get_image_data(img_ring);

        int image_h = RADIUS_SCALE * 2;
        int image_w = RADIUS_SCALE * 2;
        int pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        memset(img_ring_data, 0, buffer_size);
        gui_obj_create_timer(GUI_BASE(img_ring), 10, true, activity_stand_timer_cb);
        gui_obj_start_timer(GUI_BASE(img_ring));
        gui_obj_stop_timer(GUI_BASE(obj));
    }
}
// void activity_stand_design(gui_view_t *view)
void activity_stand_design(gui_obj_t *obj)
{
    // gui_obj_t *obj = GUI_BASE(view);
    has_draw_bg = false;
    activeIndex = 3;
    draw_flag = 0;

    gui_img_create_from_mem(obj, "bg_stand", (void *)ACTIVITY_APP_BG3_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(obj, "icon2", (void *)ACTIVITY_RIGHT_CONTROL_BIN, 326, 397, 0, 0);

    // img_ring ring
    int pixel_bytes = 4;
    size_t buffer_size = 0;

    // img_ring dot
    buffer_size = 12 * 96 * pixel_bytes + sizeof(gui_rgb_data_head_t);
    img_dot_data = gui_lower_malloc(buffer_size);
    if (img_dot_data == NULL)
    {
        img_dot_data = gui_lower_malloc(buffer_size);
    }
    memset(img_dot_data, 0, buffer_size);
    img_dot = gui_img_create_from_mem(win_menu_activity, "dot_img", (void *)img_dot_data,
                                      393, 82, 0, 0);
    gui_img_set_mode(img_dot, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_dot, true);

    // image chart
    buffer_size = 366 * 214 * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_chart_data == NULL)
    {
        img_chart_data = gui_lower_malloc(buffer_size);
    }
    memset(img_chart_data, 0, buffer_size);
    img_chart = gui_img_create_from_mem(obj, "chart_img", (void *)img_chart_data,
                                        0, 133, 0, 0);
    gui_img_set_mode(img_chart, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_chart, true);

    {

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96, drawCircles_cb,
                                          img_dot_data);
        gui_img_refresh_size(img_dot);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 366, 214, drawStepBarChart_cb,
                                          img_chart_data);
        gui_img_refresh_size(img_chart);
        gui_obj_create_timer(&(win_activity_arc->base), 10, true, enter_timer_cb);
    }

    sprintf(stand_text_content, "Stand");
    gui_text_t *stand_text = gui_text_create(obj, "stand_text", -30, 70, 0, 0);
    gui_text_set(stand_text, (void *)stand_text_content, GUI_FONT_SRC_TTF, gui_rgb(0x74, 0xFA, 0xF2),
                 strlen(stand_text_content), 36);
    gui_text_type_set(stand_text, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(stand_text, RIGHT);
    gui_text_rendermode_set(stand_text, 2);

    sprintf(cal_text_content, "10/");
    gui_text_t *text_cal = gui_text_create(obj, "cal_text", -280, 330, 0, 0);
    gui_text_set(text_cal, (void *)cal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0x74, 0xFA, 0xF2),
                 strlen(cal_text_content), 90);
    gui_text_type_set(text_cal, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal, RIGHT);
    gui_text_rendermode_set(text_cal, 2);

    sprintf(calTotal_text_content, "12");
    gui_text_t *text_cal_total = gui_text_create(obj, "calTotal_text", -240, 340, 0, 0);
    gui_text_set(text_cal_total, (void *)calTotal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0x74, 0xFA,
                 0xF2),
                 strlen(calTotal_text_content), 42);
    gui_text_type_set(text_cal_total, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal_total, RIGHT);
    gui_text_rendermode_set(text_cal_total, 2);

    sprintf(calUnit_text_content, "HR");
    gui_text_t *text_cal_unit = gui_text_create(obj, "calUnit_text", -220, 375, 0, 0);
    gui_text_set(text_cal_unit, (void *)calUnit_text_content, GUI_FONT_SRC_TTF, gui_rgb(0x74, 0xFA,
                 0xF2),
                 strlen(calUnit_text_content), 42);
    gui_text_type_set(text_cal_unit, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal_unit, RIGHT);
    gui_text_rendermode_set(text_cal_unit, 2);

    sprintf(per_text_content, "67%%");
    gui_text_t *text_per = gui_text_create(obj, "per_text", -328, 430, 0, 0);
    gui_text_set(text_per, (void *)per_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(per_text_content), 36);
    gui_text_type_set(text_per, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_per, RIGHT);
    gui_text_rendermode_set(text_per, 2);

    gui_text_t *time_12AM = gui_text_create(obj, "12AM_text", -315, 290, 0, 0);
    gui_text_set(time_12AM, (void *)majorLabels[0], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[0]), 28);
    gui_text_type_set(time_12AM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_12AM, RIGHT);
    gui_text_rendermode_set(time_12AM, 2);

    gui_text_t *time_6AM = gui_text_create(obj, "6AM_text", -240, 290, 0, 0);
    gui_text_set(time_6AM, (void *)majorLabels[1], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[1]), 28);
    gui_text_type_set(time_6AM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_6AM, RIGHT);
    gui_text_rendermode_set(time_6AM, 2);

    gui_text_t *time_12PM = gui_text_create(obj, "12PM_text", -145, 290, 0, 0);
    gui_text_set(time_12PM, (void *)majorLabels[2], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[2]), 28);
    gui_text_type_set(time_12PM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_12PM, RIGHT);
    gui_text_rendermode_set(time_12PM, 2);

    gui_text_t *time_6PM = gui_text_create(obj, "6PM_text", -65, 290, 0, 0);
    gui_text_set(time_6PM, (void *)majorLabels[3], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[3]), 28);
    gui_text_type_set(time_6PM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_6PM, RIGHT);
    gui_text_rendermode_set(time_6PM, 2);
    time_update_cb(NULL);
    return;
}
