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
#define CURRENT_VIEW_NAME "activity_exercise_view"
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
// void activity_exercise_design(gui_view_t *view);
void activity_exercise_design(gui_obj_t *obj);
static void clear_activity_exercise(gui_view_t *view);
extern void time_update_cb(void *p);
/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
// const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = activity_exercise_design,
    .on_switch_out = clear_activity_exercise,
};

extern char *cjson_content;
static uint8_t *img_ring_data = NULL;
static uint8_t *img_chart_data = NULL;
static gui_img_t *img_chart;
extern gui_img_t *img_ring;
extern gui_img_t *img_dot;
extern uint8_t *img_dot_data;

static uint16_t count = COUNT_MAX; //for timer
static bool draw_flag = 0; //0: get new json data
// static char exercise_content[30] = {0};
static char ex_content[30] = {0};

static char time_text_content[10];
extern gui_text_t *time_text;
static char exercise_text_content[10];
static char per_text_content[10];
static char cal_text_content[10];
static char calTotal_text_content[10];
static char calUnit_text_content[10];
static char am_text_content[10];
extern uint8_t activeIndex;
extern gui_win_t *win_activity;
extern gui_win_t *win_menu_activity;
extern bool has_draw_bg; // only draw background once
extern gui_win_t *win_activity_arc;
extern const char *majorLabels[];
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
    // menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void clear_activity_exercise(gui_view_t *view)
{
    (void)view;
    if (img_ring_data)
    {
        gui_lower_free(img_ring_data);
        img_ring_data = NULL;

        count = COUNT_MAX;
    }
}
// half-hour data structure
typedef struct
{
    int exercise;
    int timeIndex;
} HalfHourData;

// main time segment structure
typedef struct
{
    // const char* label;  // time label: "12AM", "6AM", "12PM", "6PM"
    HalfHourData halfHours[12]; // half-hour data for the segment
    int totalExerciseTime;     // taotal exercise for the segment
} TimeSegment;

static void drawGroupedBarChart(NVGcontext *vg, float x, float y, float width, float height)
{
    TimeSegment segments[4] = {0};

    // segments[0].label = "12AM";
    segments[0].halfHours[0].exercise = 0.5f;
    segments[0].halfHours[1].exercise = 0;
    segments[0].halfHours[2].exercise = 0;
    segments[0].halfHours[3].exercise = 0;
    segments[0].halfHours[4].exercise = 0;
    segments[0].halfHours[5].exercise = 0;
    segments[0].halfHours[6].exercise = 0;
    segments[0].halfHours[7].exercise = 0;
    segments[0].halfHours[8].exercise = 0;
    segments[0].halfHours[9].exercise = 0;
    segments[0].halfHours[10].exercise = 0;
    segments[0].halfHours[11].exercise = 0;

    // segments[1].label = "6AM";
    segments[1].halfHours[0].exercise = 1;
    segments[1].halfHours[1].exercise = 0;
    segments[1].halfHours[2].exercise = 5;
    segments[1].halfHours[3].exercise = 6;
    segments[1].halfHours[4].exercise = 0;
    segments[1].halfHours[5].exercise = 15;
    segments[1].halfHours[6].exercise = 6;
    segments[1].halfHours[7].exercise = 9;
    segments[1].halfHours[8].exercise = 15;
    segments[1].halfHours[9].exercise = 0;
    segments[1].halfHours[10].exercise = 0;
    segments[1].halfHours[11].exercise = 0;

    // segments[2].label = "12PM";
    segments[2].halfHours[0].exercise = 0;
    segments[2].halfHours[1].exercise = 0;
    segments[2].halfHours[2].exercise = 0;
    segments[2].halfHours[3].exercise = 0;
    segments[2].halfHours[4].exercise = 0;
    segments[2].halfHours[5].exercise = 0;
    segments[2].halfHours[6].exercise = 0;
    segments[2].halfHours[7].exercise = 0;
    segments[2].halfHours[8].exercise = 0;
    segments[2].halfHours[9].exercise = 0;
    segments[2].halfHours[10].exercise = 0;
    segments[2].halfHours[11].exercise = 0;

    // segments[3].label = "6PM";
    segments[3].halfHours[0].exercise = 0;
    segments[3].halfHours[1].exercise = 0;
    segments[3].halfHours[2].exercise = 0;
    segments[3].halfHours[3].exercise = 0;
    segments[3].halfHours[4].exercise = 0;
    segments[3].halfHours[5].exercise = 0;
    segments[3].halfHours[6].exercise = 0;
    segments[3].halfHours[7].exercise = 0;
    segments[3].halfHours[8].exercise = 0;
    segments[3].halfHours[9].exercise = 0;
    segments[3].halfHours[10].exercise = 0;
    segments[3].halfHours[11].exercise = 0;

    int segmentCount = 4;
    int halfHoursPerSegment = 12;
    float maxStepsPerHalfHour = 30.0f;

    float segmentWidth = width / segmentCount;
    float barSpacing = segmentWidth / halfHoursPerSegment;

    for (int seg = 0; seg < segmentCount; seg++)
    {
        float segmentStartX = x + seg * segmentWidth;

        for (int halfHour = 0; halfHour < halfHoursPerSegment; halfHour++)
        {
            float barX = segmentStartX + barSpacing / 2 + halfHour * barSpacing;
            float barHeight = (float)segments[seg].halfHours[halfHour].exercise / maxStepsPerHalfHour *
                              (height * 1.0f);
            float barY = y + height * 0.9f - barHeight;
            gui_log("seg %d barHeight %f \n", seg, barHeight);
            nvgBeginPath(vg);
            nvgMoveTo(vg, barX, y + height * 0.9f);  // buttom
            nvgLineTo(vg, barX, barY);               // top

            nvgStrokeColor(vg, nvgRGB(0xB3, 0xFC, 0x4F));  // yellow-green line
            nvgStrokeWidth(vg, barSpacing * 0.5f);
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgCircle(vg, barX + (barSpacing * 0.1f), y + height * 0.89f, barSpacing * 0.25f); // buttom circle
            nvgFillColor(vg, nvgRGB(0xB3, 0xFC, 0x4F));
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
            nvgLineTo(vg, lineX, y + height * 1.1f);
            nvgStrokeColor(vg, nvgRGB(0xCC, 0xFF, 0x80));
            nvgStrokeWidth(vg, 3.0f);
            nvgStroke(vg);
        }
    }
    gui_fb_change();

}


static void drawStepDistributionChart(NVGcontext *vg, int width, int height)
{
    nvgBeginFrame(vg, width, height, 1.0f);

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
static void activity_exercise_timer_cb(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_timer_t *timer = img_ring->base.timer;

    count += timer->interval_ms;

    float progress = count / (float)COUNT_MAX;
    float arc_scale = 1  - progress;

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

static void enter_timer_cb(void *obj)
{
    {
        count = 0;
        has_draw_bg = false;
        activeIndex = 2;
        draw_flag = 0;

        gui_obj_create_timer(GUI_BASE(img_ring), 10, true, activity_exercise_timer_cb);
        gui_obj_start_timer(GUI_BASE(img_ring));
        gui_obj_stop_timer(GUI_BASE(obj));
    }
}
// void activity_exercise_design(gui_view_t *view)
void activity_exercise_design(gui_obj_t *obj)
{
    // gui_obj_t *obj = GUI_BASE(view);
    has_draw_bg = false;
    activeIndex = 2;
    draw_flag = 0;

    gui_img_create_from_mem(obj, "bg_exercise", (void *)ACTIVITY_APP_BG2_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(obj, "icon2", (void *)ACTIVITY_RIGHT_CONTROL_BIN, 326, 397, 0, 0);


    // win_activity = gui_win_create(obj, "ring_win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // img_ring ring
    int ring_image_h = RADIUS_SCALE * 2;
    int ring_image_w = RADIUS_SCALE * 2;
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
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96, drawCircles_cb,
                                          img_dot_data);
        gui_img_refresh_size(img_dot);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 366, 214, drawStepBarChart_cb,
                                          img_chart_data);
        gui_img_refresh_size(img_chart);
        gui_obj_create_timer(obj, 10, true, enter_timer_cb);
    }

    sprintf(exercise_text_content, "Exercise");
    gui_text_t *exercise_text = gui_text_create(obj, "exercise_text", -30, 70, 0, 0);
    gui_text_set(exercise_text, (void *)exercise_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xB3, 0xFC,
                 0x4F),
                 strlen(exercise_text_content), 36);
    gui_text_type_set(exercise_text, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(exercise_text, RIGHT);
    gui_text_rendermode_set(exercise_text, 2);

    sprintf(cal_text_content, "19/");
    gui_text_t *text_cal = gui_text_create(obj, "cal_text", -280, 330, 0, 0);
    gui_text_set(text_cal, (void *)cal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xB3, 0xFC, 0x4F),
                 strlen(cal_text_content), 90);
    gui_text_type_set(text_cal, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal, RIGHT);
    gui_text_rendermode_set(text_cal, 2);

    sprintf(calTotal_text_content, "30");
    gui_text_t *text_cal_total = gui_text_create(obj, "calTotal_text", -230, 340, 0, 0);
    gui_text_set(text_cal_total, (void *)calTotal_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xB3, 0xFC,
                 0x4F),
                 strlen(calTotal_text_content), 42);
    gui_text_type_set(text_cal_total, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_cal_total, RIGHT);
    gui_text_rendermode_set(text_cal_total, 2);

    sprintf(calUnit_text_content, "MIN");
    gui_text_t *text_cal_unit = gui_text_create(obj, "calUnit_text", -210, 375, 0, 0);
    gui_text_set(text_cal_unit, (void *)calUnit_text_content, GUI_FONT_SRC_TTF, gui_rgb(0xB3, 0xFC,
                 0x4F),
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
