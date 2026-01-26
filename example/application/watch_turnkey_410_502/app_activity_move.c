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
#define CURRENT_VIEW_NAME "activity_move_view"
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
void activity_move_design(gui_obj_t *obj);
extern void time_update_cb(void *p);
/*============================================================================*
 *                            Variables
 *============================================================================*/

extern char *cjson_content;
static uint8_t *img_chart_data = NULL;
extern gui_img_t *img_ring;
extern gui_img_t *img_dot;
extern uint8_t *img_dot_data;
static gui_img_t *img_chart;

static uint16_t count = 0; //for timer
static bool draw_flag = 0; //0: get new json data


extern gui_text_t *time_text;
static char move_text_content[10];
static char per_text_content[10];
static char cal_text_content[10];
static char calTotal_text_content[10];
static char calUnit_text_content[10];
const char *majorLabels[] = {"12AM", "6AM", "12PM", "6PM", ""};
extern uint8_t activeIndex;
extern gui_win_t *win_activity;
extern gui_win_t *win_menu_activity;
extern bool has_draw_bg; // only draw background once
extern gui_win_t *win_activity_arc;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/


// half-hour data structure
typedef struct
{
    int steps;
    int timeIndex;
} HalfHourData;

// main time segment structure
typedef struct
{
    // const char* label;  // time label: "12AM", "6AM", "12PM", "6PM"
    HalfHourData halfHours[12]; // half-hour data for the segment
    int totalSteps;     // taotal steps for the segment
} TimeSegment;

// chat drawing function
static void drawGroupedBarChart(NVGcontext *vg, float x, float y, float width, float height)
{
    TimeSegment segments[4] = {0};

    // 12AM time segment data
    // segments[0].label = "12AM";
    segments[0].halfHours[0].steps = 1;
    segments[0].halfHours[1].steps = 0;
    segments[0].halfHours[2].steps = 0;
    segments[0].halfHours[3].steps = 0;
    segments[0].halfHours[4].steps = 0;
    segments[0].halfHours[5].steps = 0;
    segments[0].halfHours[6].steps = 0;
    segments[0].halfHours[7].steps = 0;
    segments[0].halfHours[8].steps = 0;
    segments[0].halfHours[9].steps = 0;
    segments[0].halfHours[10].steps = 0;
    segments[0].halfHours[11].steps = 0;

    // 6AM time segment data
    // segments[1].label = "6AM";
    segments[1].halfHours[0].steps = 200;
    segments[1].halfHours[1].steps = 0;
    segments[1].halfHours[2].steps = 400;
    segments[1].halfHours[3].steps = 350;
    segments[1].halfHours[4].steps = 250;
    segments[1].halfHours[5].steps = 100;
    segments[1].halfHours[6].steps = 110;
    segments[1].halfHours[7].steps = 0;
    segments[1].halfHours[8].steps = 140;
    segments[1].halfHours[9].steps = 0;
    segments[1].halfHours[10].steps = 170;
    segments[1].halfHours[11].steps = 0;

    // 12PM time segment data
    // segments[2].label = "12PM";
    segments[2].halfHours[0].steps = 180;
    segments[2].halfHours[1].steps = 220;
    segments[2].halfHours[2].steps = 0;
    segments[2].halfHours[3].steps = 320;
    segments[2].halfHours[4].steps = 380;
    segments[2].halfHours[5].steps = 420;
    segments[2].halfHours[6].steps = 110;
    segments[2].halfHours[7].steps = 0;
    segments[2].halfHours[8].steps = 140;
    segments[2].halfHours[9].steps = 0;
    segments[2].halfHours[10].steps = 170;
    segments[2].halfHours[11].steps = 0;

    // 6PM time segment data
    // segments[3].label = "6PM";
    segments[3].halfHours[0].steps = 0;
    segments[3].halfHours[1].steps = 0;
    segments[3].halfHours[2].steps = 0;
    segments[3].halfHours[3].steps = 0;
    segments[3].halfHours[4].steps = 0;
    segments[3].halfHours[5].steps = 0;
    segments[3].halfHours[6].steps = 0;
    segments[3].halfHours[7].steps = 0;
    segments[3].halfHours[8].steps = 0;
    segments[3].halfHours[9].steps = 0;
    segments[3].halfHours[10].steps = 0;
    segments[3].halfHours[11].steps = 0;

    int segmentCount = 4;
    int halfHoursPerSegment = 12;
    float maxStepsPerHalfHour = 500.0f;

    float segmentWidth = width / segmentCount;
    float barSpacing = segmentWidth / halfHoursPerSegment;

    for (int seg = 0; seg < segmentCount; seg++)
    {
        float segmentStartX = x + seg * segmentWidth;

        for (int halfHour = 0; halfHour < halfHoursPerSegment; halfHour++)
        {
            float barX = segmentStartX + barSpacing / 2 + halfHour * barSpacing;
            float barHeight = (float)segments[seg].halfHours[halfHour].steps / maxStepsPerHalfHour *
                              (height * 1.0f);
            float barY = y + height * 0.9f - barHeight;

            nvgBeginPath(vg);
            nvgMoveTo(vg, barX, y + height * 0.9f);  // bottom
            nvgLineTo(vg, barX, barY);               // top

            nvgStrokeColor(vg, nvgRGB(0xE4, 0x36, 0x4B));  // red line
            nvgStrokeWidth(vg, barSpacing * 0.5f);
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgCircle(vg, barX + (barSpacing * 0.1f), y + height * 0.89f, barSpacing * 0.25f); // bottom circle
            nvgFillColor(vg, nvgRGB(0xE4, 0x36, 0x4B));
            nvgFill(vg);
        }

    }
    // if(draw_flag == 0)
    {
        draw_flag = 1;
        // main time segment vertical lines
        for (int seg = 0; seg < segmentCount; seg++)
        {
            float lineX = x + seg * segmentWidth;
            nvgBeginPath(vg);
            nvgMoveTo(vg, lineX, y);
            nvgLineTo(vg, lineX, y + height * 1.1f);//vertical lines height
            nvgStrokeColor(vg, nvgRGB(245, 130, 140));
            nvgStrokeWidth(vg, 3.0f);
            nvgStroke(vg);
        }
    }
    gui_fb_change();

}


static void drawStepDistributionChart(NVGcontext *vg, int width, int height)
{
    nvgBeginFrame(vg, width, height, 1.0f);

    // chat area
    float chartX = 22;
    float chartY = height * 0.1f;
    float chartWidth = width * 1.0f;
    float chartHeight = height * 0.7f;

    drawGroupedBarChart(vg, chartX, chartY, chartWidth, chartHeight);

    nvgEndFrame(vg);
}

static void drawStepBarChart_cb(NVGcontext *vg)
{
    drawStepDistributionChart(vg, 366, 214);
}
extern void drawCircles_cb(NVGcontext *vg) ;
extern bool ring_flag_enter;
static void activity_move_timer_cb(void *obj)
{
    GUI_UNUSED(obj);

    gui_obj_timer_t *timer = img_ring->base.timer;

    count += timer->interval_ms;
    if (count >= COUNT_MAX)
    {
        count = 0;
        gui_obj_stop_timer(GUI_BASE(img_ring));
        return;
    }

    float progress = count / (float)COUNT_MAX;
    float arc_scale = 1 - progress;

    gui_img_scale(img_ring, 1.0f * arc_scale, 1.0f * arc_scale);
    gui_img_translate(img_ring, 100 * arc_scale, -150.f * arc_scale);
    // uint8_t *img_ring_data = (void *)gui_img_get_image_data(img_ring);
    // gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, RADIUS_SCALE * 2, RADIUS_SCALE * 2,
    //                                   arc_activity_ring_cb, img_ring_data);
    // gui_img_set_image_data(img_ring, img_ring_data);
    // gui_img_refresh_size(img_ring);

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

void activity_move_design(gui_obj_t *obj)
{
    has_draw_bg = false;
    // ring_flag_enter = true;
    activeIndex = 1;
    draw_flag = 0;

    gui_img_create_from_mem(obj, "bg_move", (void *)ACTIVITY_APP_BG1_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(obj, "icon2", (void *)ACTIVITY_RIGHT_CONTROL_BIN, 326, 397, 0, 0);

    int pixel_bytes = 4;
    size_t buffer_size = 0;

    // img_ring dot
    buffer_size = 12 * 96 * pixel_bytes + sizeof(gui_rgb_data_head_t);
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
    // img_chart_data = gui_lower_malloc(buffer_size);
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
        // gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, ring_image_w, ring_image_h, arc_activity_ring_cb,
        //                                   img_ring_data);
        // gui_img_refresh_size(img_ring);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96, drawCircles_cb,
                                          img_dot_data);
        gui_img_refresh_size(img_dot);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 366, 214, drawStepBarChart_cb,
                                          img_chart_data);
        gui_img_refresh_size(img_chart);
        if (ring_flag_enter == false)
        {
            ring_flag_enter = true;
            gui_obj_create_timer(GUI_BASE(img_ring), 10, true, activity_move_timer_cb);
            gui_obj_start_timer(GUI_BASE(img_ring));
        }

    }

    sprintf(move_text_content, "Move");
    gui_text_t *move_text = gui_text_create(obj, "move_text", -30, 70, 0, 0);
    gui_text_set(move_text, (void *)move_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xE4, 0x36, 0x4B),
                 strlen(move_text_content), 36);
    gui_text_type_set(move_text, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(move_text, RIGHT);
    gui_text_rendermode_set(move_text, 2);

    sprintf(cal_text_content, "375/");
    gui_text_t *text_cal = gui_text_create(obj, "cal_text", -230, 330, 0, 0);
    gui_text_set(text_cal, (void *)cal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xE4, 0x36, 0x4B),
                 strlen(cal_text_content), 90);
    gui_text_type_set(text_cal, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal, RIGHT);
    gui_text_rendermode_set(text_cal, 2);

    sprintf(calTotal_text_content, "500");
    gui_text_t *text_cal_total = gui_text_create(obj, "calTotal_text", -150, 340, 0, 0);
    gui_text_set(text_cal_total, (void *)calTotal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xE4, 0x36,
                 0x4B),
                 strlen(calTotal_text_content), 42);
    gui_text_type_set(text_cal_total, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal_total, RIGHT);
    gui_text_rendermode_set(text_cal_total, 2);

    sprintf(calUnit_text_content, "kcal");
    gui_text_t *text_cal_unit = gui_text_create(obj, "calUnit_text", -150, 375, 0, 0);
    gui_text_set(text_cal_unit, (void *)calUnit_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xE4, 0x36,
                 0x4B),
                 strlen(calUnit_text_content), 42);
    gui_text_type_set(text_cal_unit, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal_unit, RIGHT);
    gui_text_rendermode_set(text_cal_unit, 2);

    sprintf(per_text_content, "75%%");
    gui_text_t *text_per = gui_text_create(obj, "per_text", -328, 430, 0, 0);
    gui_text_set(text_per, (void *)per_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(per_text_content), 36);
    gui_text_type_set(text_per, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_per, RIGHT);
    gui_text_rendermode_set(text_per, 2);

    // sprintf(majorLabels[0], "12AM");
    gui_text_t *time_12AM = gui_text_create(obj, "12AM_text", -315, 290, 0, 0);
    gui_text_set(time_12AM, (void *)majorLabels[0], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[0]), 28);
    gui_text_type_set(time_12AM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_12AM, RIGHT);
    gui_text_rendermode_set(time_12AM, 2);

    gui_text_t *time_6AM = gui_text_create(obj, "6AM_text", -225, 290, 0, 0);
    gui_text_set(time_6AM, (void *)majorLabels[1], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[1]), 28);
    gui_text_type_set(time_6AM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_6AM, RIGHT);
    gui_text_rendermode_set(time_6AM, 2);

    gui_text_t *time_12PM = gui_text_create(obj, "12PM_text", -135, 290, 0, 0);
    gui_text_set(time_12PM, (void *)majorLabels[2], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[2]), 28);
    gui_text_type_set(time_12PM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_12PM, RIGHT);
    gui_text_rendermode_set(time_12PM, 2);

    gui_text_t *time_6PM = gui_text_create(obj, "6PM_text", -45, 290, 0, 0);
    gui_text_set(time_6PM, (void *)majorLabels[3], GUI_FONT_SRC_TTF, gui_rgb(0xA0, 0x77, 0x83),
                 strlen(majorLabels[3]), 28);
    gui_text_type_set(time_6PM, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_6PM, RIGHT);
    gui_text_rendermode_set(time_6PM, 2);

    time_update_cb(NULL);
    return;
}
