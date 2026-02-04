/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "font_mem.h"
#include "time.h"
#include "tp_algo.h"
#include "app_main_dashboard.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "app_message_adapter.h"
#include "app_phone_adapter.h"
#include "app_map.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "dashboard_view"

#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()


#define DAIL_LED0_X 85
#define DAIL_LED0_Y 360
#define DAIL_LED1_X 65
#define DAIL_LED1_Y 400
#define DAIL_LED2_X 45
#define DAIL_LED2_Y 440
#define DAIL_LED3_X 690
#define DAIL_LED3_Y 360
#define DAIL_LED4_X 710
#define DAIL_LED4_Y 400
#define DAIL_LED5_X 730
#define DAIL_LED5_Y 440

#define DIGITAL_LED0_X 27
#define DIGITAL_LED0_Y 310
#define DIGITAL_LED1_X 40
#define DIGITAL_LED1_Y 355
#define DIGITAL_LED2_X 53
#define DIGITAL_LED2_Y 400
#define DIGITAL_LED3_X 747
#define DIGITAL_LED3_Y 310
#define DIGITAL_LED4_X 734
#define DIGITAL_LED4_Y 355
#define DIGITAL_LED5_X 721
#define DIGITAL_LED5_Y 400

#define RADIUS_OUTER 92
#define RADIUS_INNER 65

#define SPEED_MAX 160 // km/h
#define POWER_MAX 160 // kW

#define WIN_DAIL_LINE_TARGET_X 25
#define WIN_DAIL_LINE_TARGET_Y 53
#define WIN_DAIL_LINE_INIT_X 331
#define WIN_DAIL_LINE_INIT_Y 53
#define WIN_DAIL_LINE_INIT_W 138
#define WIN_DAIL_LINE_INIT_H 100
#define WIN_DAIL_LINE_TARGET_W 750
#define WIN_DAIL_LINE_TARGET_H 417

#define WIN_DIGITAL_LINE_TARGET_X 152
#define WIN_DIGITAL_LINE_INIT_X 331
#define WIN_DIGITAL_LINE_INIT_Y 112
#define WIN_DIGITAL_LINE_INIT_W 138
#define WIN_DIGITAL_LINE_TARGET_W 496

#define WIN_DIGITAL_NEEDLE_H 393
#define WIN_DIGITAL_NEEDLE_W 251
#define WIN_DIGITAL_NEEDLE_X 51
#define WIN_DIGITAL_NEEDLE_Y 60
#define DIGITAL_NEEDLE_Y 37
#define DIGITAL_NEEDLE_H 356

#define ANIMATION_DURATION 20
#define SWITCH_TO_DAIL_DURATION 40
#define SWITCH_TO_DIGITAL_DURATION 60

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void dashboard_design(gui_view_t *view);

static void update_text_display(void);
static void update_inform(void *p);
static void dashboard_exit_animation(void *p);
static void win_speed_animation(void *p);
static void win_power_animation(void *p);
/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    PHONE_STATUS_NONE = 0,
    PHONE_STATUS_ONGOING,
    PHONE_STATUS_ACCEPT,
} phone_status_t;

typedef struct
{
    uint8_t phone_number[11];
    uint8_t phone_number_len;
    phone_status_t status;
} phone_data_t;
/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *start_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = dashboard_design,
};

// Common dashboard
uint8_t scene_flag = 0; // 0: start_engine, 1: dail, 2: digital

static gui_win_t *win_common = NULL;
static gui_win_t *win_speed_display = NULL;
static gui_win_t *win_power_display = NULL;
static char top_infor_str[64] = {0};
static char loction_str[16] = "Suzhou";
static char temp_str[16] = "25°C";
static char time_str[8] = "00:00";

static uint16_t speed_val = 0; // km/h
static uint16_t power_val = 0; // kW
static char speed_str[8] = "0";
static char power_str[8] = "0";

static uint16_t odo_val = 0; // km
static uint16_t soc_val = 0; // %
static char odo_str[12] = "ODO 0km";
static char soc_str[12] = "BATT 0%";

struct tm *timeinfo = NULL;

static uint8_t switch_wait_cnt = 0;
static bool animate_flag = 0;

// Digital dashboard
static gui_win_t *win_digital = NULL;
static gui_win_t *win_digital_line = NULL;

// Dail dashboard
static gui_win_t *win_dail = NULL;
static gui_win_t *win_dail_line = NULL;

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
    start_view = gui_view_descriptor_get("start_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// Dashboard digital functions
static void dashboard_digital_exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 20;
    // gui_obj_t *obj = GUI_BASE(p);
    cnt++;

    static int16_t offset = 0;
    int16_t step = -10;

    if (win_digital_line->base.x < WIN_DIGITAL_LINE_INIT_X)
    {
        // hide line
        offset += step;
        win_digital_line->base.x = WIN_DIGITAL_LINE_TARGET_X - offset;
        win_digital_line->base.w = WIN_DIGITAL_LINE_TARGET_W + offset * 2;
        if (win_digital_line->base.x >= WIN_DIGITAL_LINE_INIT_X)
        {
            win_digital_line->base.x = WIN_DIGITAL_LINE_INIT_X;
            win_digital_line->base.w = WIN_DIGITAL_LINE_INIT_W;
            offset = 0;
            cnt = 0;
        }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(line, "line", GUI_BASE(win_digital_line));
        line->x = WIN_DIGITAL_LINE_TARGET_X - win_digital_line->base.x;
        return;
    }
    else if (cnt <= cnt_max)
    {
        // Img faing out
        uint8_t opacity = (cnt_max - cnt) * 255 / cnt_max;
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_wing, "l_wing", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_wing, "r_wing", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_gloom, "l_gloom", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_gloom, "r_gloom", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(battery, "battery", GUI_BASE(win_digital));
        gui_img_set_opacity((gui_img_t *)l_wing, opacity);
        gui_img_set_opacity((gui_img_t *)r_wing, opacity);
        gui_img_set_opacity((gui_img_t *)l_gloom, opacity);
        gui_img_set_opacity((gui_img_t *)r_gloom, opacity);
        gui_img_set_opacity((gui_img_t *)battery, opacity);

        // Img moving out
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_win_needle, "win_wing_left", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_win_needle, "win_wing_right", GUI_BASE(win_digital));
        l_win_needle->x = -WIN_DIGITAL_NEEDLE_W + (WIN_DIGITAL_NEEDLE_X + WIN_DIGITAL_NEEDLE_W) *
                          (cnt_max - cnt) / cnt_max;
        r_win_needle->x = SCREEN_WIDTH - (WIN_DIGITAL_NEEDLE_X + WIN_DIGITAL_NEEDLE_W) *
                          (cnt_max - cnt) / cnt_max;
        return;
    }
    cnt = 0;
    gui_obj_tree_free_async(GUI_BASE(win_digital));
    gui_obj_stop_timer(GUI_BASE(p));
}

static void dashboard_digital_entrance_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max_1 = switch_wait_cnt;
    uint16_t cnt_max_2 = cnt_max_1 + 20;
    cnt++;
    if (cnt <= cnt_max_1)
    {
        uint16_t cnt_led = 10;
        if (cnt <= cnt_led)
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", win_common);
            led0->x = DAIL_LED0_X + (DIGITAL_LED0_X - DAIL_LED0_X) * cnt / cnt_led;
            led0->y = DAIL_LED0_Y + (DIGITAL_LED0_Y - DAIL_LED0_Y) * cnt / cnt_led;
            led1->x = DAIL_LED1_X + (DIGITAL_LED1_X - DAIL_LED1_X) * cnt / cnt_led;
            led1->y = DAIL_LED1_Y + (DIGITAL_LED1_Y - DAIL_LED1_Y) * cnt / cnt_led;
            led2->x = DAIL_LED2_X + (DIGITAL_LED2_X - DAIL_LED2_X) * cnt / cnt_led;
            led2->y = DAIL_LED2_Y + (DIGITAL_LED2_Y - DAIL_LED2_Y) * cnt / cnt_led;
            led3->x = DAIL_LED3_X + (DIGITAL_LED3_X - DAIL_LED3_X) * cnt / cnt_led;
            led3->y = DAIL_LED3_Y + (DIGITAL_LED3_Y - DAIL_LED3_Y) * cnt / cnt_led;
            led4->x = DAIL_LED4_X + (DIGITAL_LED4_X - DAIL_LED4_X) * cnt / cnt_led;
            led4->y = DAIL_LED4_Y + (DIGITAL_LED4_Y - DAIL_LED4_Y) * cnt / cnt_led;
            led5->x = DAIL_LED5_X + (DIGITAL_LED5_X - DAIL_LED5_X) * cnt / cnt_led;
            led5->y = DAIL_LED5_Y + (DIGITAL_LED5_Y - DAIL_LED5_Y) * cnt / cnt_led;
        }
        return;
    }
    else if (cnt <= cnt_max_2)
    {
        gui_obj_hidden(GUI_BASE(win_digital), false);

        // Img faing in
        uint8_t opacity = (cnt - cnt_max_1) * 255 / (cnt_max_2 - cnt_max_1);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_wing, "l_wing", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_wing, "r_wing", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_gloom, "l_gloom", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_gloom, "r_gloom", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(battery, "battery", GUI_BASE(win_digital));
        gui_img_set_opacity((gui_img_t *)l_wing, opacity);
        gui_img_set_opacity((gui_img_t *)r_wing, opacity);
        gui_img_set_opacity((gui_img_t *)l_gloom, opacity);
        gui_img_set_opacity((gui_img_t *)r_gloom, opacity);
        gui_img_set_opacity((gui_img_t *)battery, opacity);

        // Img moving in
        GUI_WIDGET_POINTER_BY_NAME_ROOT(l_win_needle, "win_wing_left", GUI_BASE(win_digital));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(r_win_needle, "win_wing_right", GUI_BASE(win_digital));
        l_win_needle->x = -WIN_DIGITAL_NEEDLE_W + (WIN_DIGITAL_NEEDLE_X + WIN_DIGITAL_NEEDLE_W) *
                          (cnt - cnt_max_1) / (cnt_max_2 - cnt_max_1);
        r_win_needle->x = SCREEN_WIDTH - (WIN_DIGITAL_NEEDLE_X + WIN_DIGITAL_NEEDLE_W) *
                          (cnt - cnt_max_1) / (cnt_max_2 - cnt_max_1);
        return;
    }

    static int16_t offset = 0;
    int16_t step = 10;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(line, "line", GUI_BASE(win_digital_line));
    if (win_digital_line->base.x > WIN_DIGITAL_LINE_TARGET_X)
    {
        // Show line
        offset += step;
        win_digital_line->base.x = WIN_DIGITAL_LINE_INIT_X - offset;
        win_digital_line->base.w = WIN_DIGITAL_LINE_INIT_W + offset * 2;
        if (win_digital_line->base.x <= WIN_DIGITAL_LINE_TARGET_X)
        {
            win_digital_line->base.x = WIN_DIGITAL_LINE_TARGET_X;
            win_digital_line->base.w = WIN_DIGITAL_LINE_TARGET_W;
            offset = 0;
            ((gui_img_t *)line)->need_clip = false;
            gui_obj_stop_timer(GUI_BASE(p));
            animate_flag = 0;
            cnt = 0;
        }
        line->x = WIN_DIGITAL_LINE_TARGET_X - win_digital_line->base.x;
    }
}

static void win_needle_left_animation(void *p)
{
    gui_obj_t *win = gui_list_entry((&GUI_BASE(p)->child_list)->prev, gui_obj_t, brother_list);

    int16_t h = roundf((float)speed_val * DIGITAL_NEEDLE_H / SPEED_MAX);
    win->y = WIN_DIGITAL_NEEDLE_H - h;
    gui_obj_t *needle = gui_list_entry((&win->child_list)->next, gui_obj_t, brother_list);
    needle->y = h - DIGITAL_NEEDLE_H;
}

static void win_needle_right_animation(void *p)
{
    gui_obj_t *win = gui_list_entry((&GUI_BASE(p)->child_list)->prev, gui_obj_t, brother_list);
    int16_t h = (int16_t)roundf((float)power_val * DIGITAL_NEEDLE_H / SPEED_MAX);
    win->y = WIN_DIGITAL_NEEDLE_H - h;
    gui_obj_t *needle = gui_list_entry((&win->child_list)->next, gui_obj_t, brother_list);
    needle->y = h - DIGITAL_NEEDLE_H;
}

static void digital_draw_rect(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    NVGcolor color = nvgRGB(78, 181, 153);
    color.a *= (((gui_img_t *)canvas->base.parent)->opacity_value / 255.f);

    int16_t w = soc_val * 55 / 100;
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, w, canvas->base.h);
    nvgFillColor(vg, color);
    nvgFill(vg);
}

static void exit_dashboard_digital(void)
{
    gui_obj_create_timer(GUI_BASE(win_digital), 10, true, dashboard_digital_exit_animation);
    gui_obj_start_timer(GUI_BASE(win_digital));
    GUI_WIDGET_POINTER_BY_NAME_ROOT(line, "line", GUI_BASE(win_digital_line));
    ((gui_img_t *)line)->need_clip = true;
}

static void dashboard_digital_design(gui_obj_t *parent)
{
    scene_flag = 2;
    win_digital = gui_win_create(parent, 0, 0, 0, 0, 0);
    gui_obj_hidden(GUI_BASE(win_digital), true);
    gui_obj_create_timer(GUI_BASE(win_digital), 10, true, dashboard_digital_entrance_animation);
    win_digital_line = gui_win_create(win_digital, 0, WIN_DIGITAL_LINE_INIT_X, WIN_DIGITAL_LINE_INIT_Y,
                                      WIN_DIGITAL_LINE_INIT_W, 0);
    gui_img_t *line = gui_img_create_from_mem(win_digital_line, "line", BOARDERV2_BIN,
                                              WIN_DIGITAL_LINE_TARGET_X - WIN_DIGITAL_LINE_INIT_X, 0, 0, 0);
    line->need_clip = true;

    gui_img_t *gloom_left = gui_img_create_from_mem(win_digital, "l_gloom", LEFTGLOWV2_BIN, 112, 73, 0,
                                                    0);
    gui_img_t *gloom_right = gui_img_create_from_mem(win_digital, "r_gloom", RIGHTGLOWV2_BIN, 544, 73,
                                                     0,
                                                     0);
    gui_img_set_mode(gloom_left, IMG_SRC_OVER_MODE);
    gui_img_set_mode(gloom_right, IMG_SRC_OVER_MODE);
    gui_img_set_opacity(gloom_left, 0);
    gui_img_set_opacity(gloom_right, 0);

    gui_img_t *wing_left_2 = gui_img_create_from_mem(win_digital, "l_wing", LEFTWING2V2_BIN, 7, 40, 0,
                                                     0);
    gui_img_t *wing_right_2 = gui_img_create_from_mem(win_digital, "r_wing", RIGHTWING2V2_BIN, 626, 40,
                                                      0,
                                                      0);
    gui_img_set_opacity(wing_left_2, 0);
    gui_img_set_opacity(wing_right_2, 0);
    gui_img_t *battery_shell = gui_img_create_from_mem(win_digital, "battery", BATTERY_BIN, 380, 430, 0,
                                                       0);
    gui_img_set_opacity(battery_shell, 0);
    gui_canvas_t *canvas = gui_canvas_create(battery_shell, 0, NULL, 2, 2, 55, 18);
    gui_canvas_set_canvas_cb(canvas, digital_draw_rect);

    gui_win_t *win_wing_left = gui_win_create(win_digital, "win_wing_left", -WIN_DIGITAL_NEEDLE_W,
                                              WIN_DIGITAL_NEEDLE_Y, WIN_DIGITAL_NEEDLE_W, WIN_DIGITAL_NEEDLE_H);
    gui_win_t *win_wing_right = gui_win_create(win_digital, "win_wing_right",
                                               SCREEN_WIDTH + WIN_DIGITAL_NEEDLE_W, WIN_DIGITAL_NEEDLE_Y, WIN_DIGITAL_NEEDLE_W,
                                               WIN_DIGITAL_NEEDLE_H);
    gui_img_create_from_mem(win_wing_left, 0, LEFTWINGV2_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(win_wing_right, 0, RIGHTWINGV2_BIN, 0, 0, 0,
                            0);
    gui_win_t *win_needle_left = gui_win_create(win_wing_left, "win_needle_left", 0, DIGITAL_NEEDLE_Y,
                                                WIN_DIGITAL_NEEDLE_W, WIN_DIGITAL_NEEDLE_H);
    gui_win_t *win_needle_right = gui_win_create(win_wing_right, "win_needle_right", 0,
                                                 DIGITAL_NEEDLE_Y, WIN_DIGITAL_NEEDLE_W, WIN_DIGITAL_NEEDLE_H);
    gui_obj_create_timer(GUI_BASE(win_wing_left), 10, true, win_needle_left_animation);
    gui_obj_create_timer(GUI_BASE(win_wing_right), 10, true, win_needle_right_animation);
    gui_img_t *needle_left = gui_img_create_from_mem(win_needle_left, 0, LEFTWINGNEEDLEV2_BIN, 0, 0, 0,
                                                     0);
    gui_img_t *needle_right = gui_img_create_from_mem(win_needle_right, 0, RIGHTWINGNEEDLEV2_BIN, 118,
                                                      0, 0, 0);
    needle_left->need_clip = true;
    needle_right->need_clip = true;
}

// Dashboard dail functions
static void dail_speed_timer(void *p)
{
    gui_obj_t *parent = GUI_BASE(p);
    float degree = 360.f * (float)speed_val / (float)SPEED_MAX;
    gui_obj_t *obj = gui_list_entry((&parent->child_list)->prev, gui_obj_t, brother_list);
    gui_img_rotation((gui_img_t *)obj, degree);
}

static void dail_power_timer(void *p)
{
    gui_obj_t *parent = GUI_BASE(p);
    float degree = 360.f * (float)power_val / (float)POWER_MAX;
    gui_obj_t *obj = gui_list_entry((&parent->child_list)->prev, gui_obj_t, brother_list);
    gui_img_rotation((gui_img_t *)obj, -degree);
}

static void dail_draw_arc(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    float radius_inner = (float)RADIUS_INNER;
    float radius_outer = (float)RADIUS_OUTER;
    float cx = radius_outer;
    float cy = radius_outer;
    float start_rad = M_PI * 90 / 180.0f;
    float end_rad = start_rad + 2.0f * M_PI * (float)speed_val / (float)SPEED_MAX;
    enum NVGwinding dir = NVG_CW; // NVG_CCW = counter-clockwise, NVG_CW = clockwise

    if (strcmp(GUI_BASE(canvas)->name, "power") == 0)
    {
        if (power_val == 0) { return; }
        end_rad = start_rad - 2.0f * M_PI * (float)power_val / (float)POWER_MAX;
        dir = NVG_CCW;
    }
    else if (speed_val == 0)
    {
        return;
    }

    NVGcolor start_color = nvgRGBA(0, 0, 0, 0);
    NVGcolor end_color = nvgRGBA(0, 100, 140, 255);
    end_color.a *= (((gui_img_t *)canvas->base.parent)->opacity_value / 255.f);
    NVGpaint paint = nvgRadialGradient(vg, cx, cy, radius_inner, radius_outer, start_color, end_color);

    nvgBeginPath(vg);
    nvgArc(vg, cx, cy, radius_outer, start_rad, end_rad, dir);

    // Connect the end point of the outer arc to the end point of the inner arc. Calculate the coordinates of the inner arc's end point and draw a line to it.
    float innerEndX = cx + cosf(end_rad) * radius_inner;
    float innerEndY = cy + sinf(end_rad) * radius_inner;
    nvgLineTo(vg, innerEndX, innerEndY);

    // Draw the inner arc (counter-clockwise, back to the starting point). Note that the angle parameters are reversed (end_rad -> start_rad), and the direction is NVG_CCW = counter-clockwise.
    nvgArc(vg, cx, cy, radius_inner, end_rad, start_rad, dir);

    // Close the path, nvgClosePath() will automatically draw a straight line from the current point (the starting point of the inner arc) to the starting point of the entire path (the starting point of the outer arc).
    nvgClosePath(vg);

    nvgFillPaint(vg, paint);
    nvgFill(vg);
}

static void set_img_child_opacity(gui_obj_t *obj, uint8_t opacity)
{
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->type == IMAGE_FROM_MEM)
        {
            gui_img_set_opacity((gui_img_t *)o, opacity);
            set_img_child_opacity(o, opacity);
        }
    }
}

static void dashboard_dail_exit_animation(void *p)
{
    static int16_t offset = 0;
    int16_t step = -10;

    if (win_dail_line->base.h > WIN_DAIL_LINE_INIT_H)
    {
        offset += step;
        win_dail_line->base.h = WIN_DAIL_LINE_TARGET_H + offset;
        if (win_dail_line->base.h <= WIN_DAIL_LINE_INIT_H)
        {
            win_dail_line->base.h = WIN_DAIL_LINE_INIT_H;
            offset = 0;
        }
    }
    else if (win_dail_line->base.x < WIN_DAIL_LINE_INIT_X)
    {
        offset += step;
        win_dail_line->base.x = WIN_DAIL_LINE_TARGET_X - offset;
        win_dail_line->base.w = WIN_DAIL_LINE_TARGET_W + offset * 2;
        if (win_dail_line->base.x >= WIN_DAIL_LINE_INIT_X)
        {
            win_dail_line->base.x = WIN_DAIL_LINE_INIT_X;
            win_dail_line->base.w = WIN_DAIL_LINE_INIT_W;
            offset = 0;
            gui_obj_stop_timer(GUI_BASE(p));
            gui_obj_tree_free_async(GUI_BASE(p));
        }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "line_left", GUI_BASE(win_dail_line));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "line_right", GUI_BASE(win_dail_line));
        left->x = WIN_DAIL_LINE_TARGET_X - win_dail_line->base.x;
        right->x = win_dail_line->base.w / 2;
        // Img faing out
        uint8_t opacity = (WIN_DAIL_LINE_INIT_X - win_dail_line->base.x) * 255 /
                          (WIN_DAIL_LINE_INIT_X - WIN_DAIL_LINE_TARGET_X);
        set_img_child_opacity(GUI_BASE(win_dail), opacity);
    }
}

static void dashboard_dail_entrance_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = switch_wait_cnt;
    // gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    if (cnt <= cnt_max) // Move led position and wait exit animation
    {
        uint16_t cnt_led = 10;
        if (cnt <= cnt_led)
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", win_common);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", win_common);
            led0->x = DIGITAL_LED0_X + (DAIL_LED0_X - DIGITAL_LED0_X) * cnt / cnt_led;
            led0->y = DIGITAL_LED0_Y + (DAIL_LED0_Y - DIGITAL_LED0_Y) * cnt / cnt_led;
            led1->x = DIGITAL_LED1_X + (DAIL_LED1_X - DIGITAL_LED1_X) * cnt / cnt_led;
            led1->y = DIGITAL_LED1_Y + (DAIL_LED1_Y - DIGITAL_LED1_Y) * cnt / cnt_led;
            led2->x = DIGITAL_LED2_X + (DAIL_LED2_X - DIGITAL_LED2_X) * cnt / cnt_led;
            led2->y = DIGITAL_LED2_Y + (DAIL_LED2_Y - DIGITAL_LED2_Y) * cnt / cnt_led;
            led3->x = DIGITAL_LED3_X + (DAIL_LED3_X - DIGITAL_LED3_X) * cnt / cnt_led;
            led3->y = DIGITAL_LED3_Y + (DAIL_LED3_Y - DIGITAL_LED3_Y) * cnt / cnt_led;
            led4->x = DIGITAL_LED4_X + (DAIL_LED4_X - DIGITAL_LED4_X) * cnt / cnt_led;
            led4->y = DIGITAL_LED4_Y + (DAIL_LED4_Y - DIGITAL_LED4_Y) * cnt / cnt_led;
            led5->x = DIGITAL_LED5_X + (DAIL_LED5_X - DIGITAL_LED5_X) * cnt / cnt_led;
            led5->y = DIGITAL_LED5_Y + (DAIL_LED5_Y - DIGITAL_LED5_Y) * cnt / cnt_led;
        }
        return;
    }

    gui_obj_hidden(GUI_BASE(win_dail), false);
    static int16_t offset = 0;
    int16_t step = 10;
    if (win_dail_line->base.x > WIN_DAIL_LINE_TARGET_X)
    {
        // Show line
        offset += step;
        win_dail_line->base.x = WIN_DAIL_LINE_INIT_X - offset;
        win_dail_line->base.w = WIN_DAIL_LINE_INIT_W + offset * 2;
        if (win_dail_line->base.x <= WIN_DAIL_LINE_TARGET_X)
        {
            win_dail_line->base.x = WIN_DAIL_LINE_TARGET_X;
            win_dail_line->base.w = WIN_DAIL_LINE_TARGET_W;
            offset = 0;
        }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "line_left", GUI_BASE(win_dail_line));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "line_right", GUI_BASE(win_dail_line));
        left->x = WIN_DAIL_LINE_TARGET_X - win_dail_line->base.x;
        right->x = win_dail_line->base.w / 2;

        // Img faing in
        uint8_t opacity = (WIN_DAIL_LINE_INIT_X - win_dail_line->base.x) * 255 /
                          (WIN_DAIL_LINE_INIT_X - WIN_DAIL_LINE_TARGET_X);
        set_img_child_opacity(GUI_BASE(win_dail), opacity);
    }
    else if (win_dail_line->base.h < WIN_DAIL_LINE_TARGET_H)
    {
        offset += step;
        win_dail_line->base.h = WIN_DAIL_LINE_INIT_H + offset;
        if (win_dail_line->base.h >= WIN_DAIL_LINE_TARGET_H)
        {
            win_dail_line->base.h = WIN_DAIL_LINE_TARGET_H;
            offset = 0;
            GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "line_left", GUI_BASE(win_dail_line));
            GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "line_right", GUI_BASE(win_dail_line));
            ((gui_img_t *)left)->need_clip = false;
            ((gui_img_t *)right)->need_clip = false;
            gui_obj_stop_timer(GUI_BASE(p));
            cnt = 0;
            animate_flag = 0;
        }
    }
}

static void exit_dashboard_dail(void)
{
    gui_obj_create_timer(GUI_BASE(win_dail), 10, true, dashboard_dail_exit_animation);
    gui_obj_start_timer(GUI_BASE(win_dail));

    GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "line_left", GUI_BASE(win_dail_line));
    GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "line_right", GUI_BASE(win_dail_line));
    ((gui_img_t *)left)->need_clip = true;
    ((gui_img_t *)right)->need_clip = true;
}

static void dashboard_dail_design(gui_obj_t *parent)
{
    scene_flag = 1;

    win_dail = gui_win_create(parent, 0, 0, 0, 0, 0);
    gui_obj_hidden(GUI_BASE(win_dail), true);
    gui_obj_create_timer(GUI_BASE(win_dail), 10, true, dashboard_dail_entrance_animation);
    win_dail_line = gui_win_create(win_dail, 0, WIN_DAIL_LINE_INIT_X, WIN_DAIL_LINE_INIT_Y,
                                   WIN_DAIL_LINE_INIT_W, WIN_DAIL_LINE_INIT_H);
    gui_img_t *line_left = gui_img_create_from_mem(win_dail_line, "line_left", LEFTLINEV1_BIN,
                                                   WIN_DAIL_LINE_TARGET_X - WIN_DAIL_LINE_INIT_X, 0, 0, 0);
    gui_img_t *line_right = gui_img_create_from_mem(win_dail_line, "line_right", RIGHTLINEV1_BIN,
                                                    WIN_DAIL_LINE_INIT_W / 2, 0, 0, 0);
    line_left->need_clip = true;
    line_right->need_clip = true;

    gui_img_create_from_mem(win_dail, 0, LEFTBOTTOMGLOOMV1_BIN, 40, 353, 0, 0);
    gui_img_create_from_mem(win_dail, 0, RIGHTBOTTOMGLOOMV1_BIN, 661, 353, 0,
                            0);

    gui_img_t *dail_speed = gui_img_create_from_mem(win_dail, 0, DAIL_SPEED_BIN, 38, 64, 0, 0);
    gui_img_t *dail_power = gui_img_create_from_mem(win_dail, 0, DAIL_POWER_BIN, 470, 64, 0, 0);
    gui_img_set_mode(dail_speed, IMG_SRC_OVER_MODE);
    gui_img_set_mode(dail_power, IMG_SRC_OVER_MODE);

    gui_canvas_t *canvas = gui_canvas_create(dail_speed, "speed", NULL, 54, 54, RADIUS_OUTER * 2,
                                             RADIUS_OUTER * 2);
    gui_canvas_set_canvas_cb(canvas, dail_draw_arc);
    canvas = gui_canvas_create(dail_power, "power", NULL, 54, 54, RADIUS_OUTER * 2, RADIUS_OUTER * 2);
    gui_canvas_set_canvas_cb(canvas, dail_draw_arc);

    int16_t focus_x = 147;
    int16_t focus_y = 147;
    int16_t pos_x = 144;
    int16_t pos_y = 212;
    gui_img_t *dail_pointer_s = gui_img_create_from_mem(dail_speed, 0, DAIL_POINTER_BIN, focus_x,
                                                        focus_y, 0, 0);
    gui_img_t *dail_pointer_p = gui_img_create_from_mem(dail_power, 0, DAIL_POINTER_BIN, focus_x,
                                                        focus_y, 0, 0);
    gui_img_set_focus(dail_pointer_s, focus_x - pos_x, focus_y - pos_y);
    gui_img_set_focus(dail_pointer_p, focus_x - pos_x, focus_y - pos_y);
    gui_img_set_quality(dail_pointer_s, true);
    gui_img_set_quality(dail_pointer_p, true);
    gui_obj_create_timer(GUI_BASE(dail_speed), 10, true, dail_speed_timer);
    gui_obj_create_timer(GUI_BASE(dail_power), 10, true, dail_power_timer);
}


// Dashboard common part functions
static void press_icon(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;
    if (strcmp(img->base.name, "switch_icon") == 0)
    {
        gui_img_set_src(img, SWITCHICON_PRESS_BIN, IMG_SRC_MEMADDR);
    }
    else
    {
        gui_img_set_src(img, OFFICON_PRESS_BIN, IMG_SRC_MEMADDR);
    }
}

static void release_icon(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;
    touch_info_t *tp = tp_get_info();
    bool valid_touch = (gui_obj_point_in_obj_rect(obj, tp->x + tp->deltaX - GUI_BASE(obj)->x,
                                                  tp->y + tp->deltaY - GUI_BASE(obj)->y) == true);
    if (strcmp(img->base.name, "switch_icon") == 0)
    {
        gui_img_set_src(img, SWITCHICON_RELEASE_BIN, IMG_SRC_MEMADDR);
        if (valid_touch && !animate_flag)
        {
            //change_scene
            animate_flag = 1;
            if (scene_flag == 1)
            {
                dashboard_digital_design(GUI_BASE(win_dail)->parent);
                exit_dashboard_dail();
                switch_wait_cnt = SWITCH_TO_DIGITAL_DURATION;
            }
            else
            {
                dashboard_dail_design(GUI_BASE(win_digital)->parent);
                exit_dashboard_digital();
                switch_wait_cnt = SWITCH_TO_DAIL_DURATION;
            }
            // change_pos_indicator_color();
        }
    }
    else
    {
        gui_img_set_src(img, OFFICON_RELEASE_BIN, IMG_SRC_MEMADDR);
        if (valid_touch && scene_flag && !animate_flag)
        {
            if (scene_flag == 1)
            {
                exit_dashboard_dail();
            }
            else
            {
                exit_dashboard_digital();
            }
            scene_flag = 0;
            animate_flag = 1;
            gui_obj_create_timer(GUI_BASE(current_view), 10, true, dashboard_exit_animation);
            exit_map();

        }
    }
}

static void zoom_animation(gui_obj_t *obj, float scale)
{
    matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
    if (scale == 0)
    {
        matrix_scale(1.f, 1.f, obj->matrix);
    }
    else
    {
        matrix_scale(scale, scale, obj->matrix);
    }
    matrix_translate(obj->w / -2, obj->h / -2, obj->matrix);
}

static void win_speed_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = ANIMATION_DURATION;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    float scale = (float)cnt / cnt_max;
    if (scene_flag == 0) {scale = 1.0f - scale;}
    zoom_animation(GUI_BASE(win_speed_display), scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        if (scene_flag == 0)
        {
            gui_obj_hidden(GUI_BASE(win_speed_display), true);
        }
        gui_obj_stop_timer(obj);
    }
}

static void win_power_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = ANIMATION_DURATION;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    float scale = (float)cnt / cnt_max;
    if (scene_flag == 0) {scale = 1.0f - scale;}
    zoom_animation(GUI_BASE(win_power_display), scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        if (scene_flag == 0)
        {
            gui_obj_hidden(GUI_BASE(win_power_display), true);
        }
        gui_obj_stop_timer(obj);
    }
}

static void win_common_exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = ANIMATION_DURATION;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    obj->opacity_value = (cnt_max - cnt) * 255 / cnt_max;
    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_stop_timer(obj);
        gui_obj_hidden(obj, true);
    }
}

static void dashboard_exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 20;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    if (cnt == 1)
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(win_common, "win_common", obj);
        gui_obj_create_timer(win_common, 10, true, win_common_exit_animation);
        gui_obj_start_timer(win_common);
    }
    else if (cnt == cnt_max + 30)
    {
        gui_obj_create_timer(GUI_BASE(win_speed_display), 10, true, win_speed_animation);
        gui_obj_create_timer(GUI_BASE(win_power_display), 10, true, win_power_animation);
        gui_obj_start_timer(GUI_BASE(win_speed_display));
        gui_obj_start_timer(GUI_BASE(win_power_display));
    }
    else if (cnt >= cnt_max + 50)
    {
        cnt = 0;
        gui_view_set_animate_step(current_view, 800);
        gui_view_switch_direct(current_view, start_view->name, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
        speed_val = 0;
        power_val = 0;
    }

}

static void dashboard_entrance_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 70;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    animate_flag = 1;
    if (cnt < cnt_max)
    {
        return; //waiting
    }
    else if (cnt == cnt_max)
    {
        gui_obj_hidden(GUI_BASE(win_common), false);
    }
    else if (cnt == (cnt_max + cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led2), LED2_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led5), LED5_ON_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_ON_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt == (cnt_max + 2 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led1), LED1_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led4), LED4_ON_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_OFF_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt == (cnt_max + 3 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led0), LED0_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led3), LED3_ON_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_ON_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt == (cnt_max + 4 * cnt_max / 7))
    {
        static int16_t step = 5;
        speed_val += step;
        power_val += step;
        cnt--;
        if (speed_val >= SPEED_MAX)
        {
            step = -5;
        }
        if (speed_val <= 0)
        {
            speed_val = 0;
            power_val = 0;
            step = 5;
            cnt++;
        }
        update_text_display();
    }
    else if (cnt == (cnt_max + 5 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led0), LED0_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led3), LED3_OFF_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_OFF_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt == (cnt_max + 6 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led1), LED1_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led4), LED4_OFF_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_ON_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_ON_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt == (cnt_max + 7 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led2), LED2_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led5), LED5_OFF_BIN, IMG_SRC_MEMADDR);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_OFF_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_OFF_BIN, IMG_SRC_MEMADDR);

        map_design(); //show map

        GUI_WIDGET_POINTER_BY_NAME_ROOT(switch_icon, "switch_icon", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(off_icon, "off_icon", GUI_BASE(win_common));
        gui_img_set_src(((gui_img_t *)switch_icon), SWITCHICON_RELEASE_BIN, IMG_SRC_MEMADDR);
        gui_img_set_src(((gui_img_t *)off_icon), OFFICON_RELEASE_BIN, IMG_SRC_MEMADDR);
    }
    else if (cnt >= (cnt_max + 8 * cnt_max / 7))
    {
        cnt = 0;
        gui_obj_create_timer(obj, 20, true, update_inform);
        gui_obj_start_timer(obj);
        animate_flag = 0;
    }
}

static void update_text_display(void)
{
    sprintf(speed_str, "%d", speed_val);
    sprintf(power_str, "%d", power_val);
    sprintf(odo_str, "ODO %dkm", odo_val);
    sprintf(soc_str, "BATT %d%%", soc_val);

    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "speed_text", win_speed_display);
        gui_text_content_set((gui_text_t *)text, (void *)speed_str, strlen(speed_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "power_text", win_power_display);
        gui_text_content_set((gui_text_t *)text, (void *)power_str, strlen(power_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "odo_text", win_speed_display);
        gui_text_content_set((gui_text_t *)text, (void *)odo_str, strlen(odo_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "soc_text", win_power_display);
        gui_text_content_set((gui_text_t *)text, (void *)soc_str, strlen(soc_str));
    }
}

static void update_inform(void *p)
{
    (void)p;
    static uint8_t cnt = 0;
    cnt++;
#if defined _HONEYGUI_SIMULATOR_
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif
    gui_dashboard_t *dashboard_info = get_dashboard_info();

    // Top information
    if (timeinfo)
    {
        sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }
    if (dashboard_info)
    {
        sprintf(loction_str, "%s", dashboard_info->location);
        sprintf(temp_str, "%d°C", dashboard_info->temp_val);
    }
    sprintf(top_infor_str, "%s %s\n%s", loction_str, temp_str, time_str);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "top_infor", current_view);
    gui_text_content_set((gui_text_t *)text, top_infor_str, strlen(top_infor_str));

    if (dashboard_info)
    {
        speed_val = dashboard_info->speed_val;
        power_val = dashboard_info->power_val;
        odo_val = dashboard_info->odo_val;
        soc_val = dashboard_info->soc_val;

        GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_l, "led_turn_l", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led_turn_r, "led_turn_r", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(bt, "bt", GUI_BASE(win_common));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(wifi, "wifi", GUI_BASE(win_common));

        if (dashboard_info->bt_status)
        {
            gui_img_a8_recolor((gui_img_t *)bt, 0xFF00FF00);
        }
        else
        {
            gui_img_a8_recolor((gui_img_t *)bt, 0xFFFFFFFF);
        }
        if (dashboard_info->wifi_status)
        {
            gui_obj_hidden(wifi, false);
        }
        else
        {
            gui_obj_hidden(wifi, true);
        }
        if (dashboard_info->led_turn_l_status)
        {
            if (cnt % 10 == 0)
            {
                if (((gui_img_t *)led_turn_l)->data == TURNLEFT_OFF_BIN)
                {
                    gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_ON_BIN, IMG_SRC_MEMADDR);
                }
                else
                {
                    gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_OFF_BIN, IMG_SRC_MEMADDR);
                }
            }
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led_turn_l), TURNLEFT_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led_turn_r_status)
        {
            if (cnt % 10 == 0)
            {
                if (((gui_img_t *)led_turn_r)->data == TURNRIGHT_OFF_BIN)
                {
                    gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_ON_BIN, IMG_SRC_MEMADDR);
                }
                else
                {
                    gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_OFF_BIN, IMG_SRC_MEMADDR);
                }
            }
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led_turn_r), TURNRIGHT_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led0_status)
        {
            gui_img_set_src(((gui_img_t *)led0), LED0_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led0), LED0_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led1_status)
        {
            gui_img_set_src(((gui_img_t *)led1), LED1_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led1), LED1_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led2_status)
        {
            gui_img_set_src(((gui_img_t *)led2), LED2_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led2), LED2_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led3_status)
        {
            gui_img_set_src(((gui_img_t *)led3), LED3_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led3), LED3_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led4_status)
        {
            gui_img_set_src(((gui_img_t *)led4), LED4_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led4), LED4_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->led5_status)
        {
            gui_img_set_src(((gui_img_t *)led5), LED5_ON_BIN, IMG_SRC_MEMADDR);
        }
        else
        {
            gui_img_set_src(((gui_img_t *)led5), LED5_OFF_BIN, IMG_SRC_MEMADDR);
        }
        if (dashboard_info->map_data_update)
        {
            dashboard_info->map_data_update = 0;
            GUI_WIDGET_POINTER_BY_NAME_ROOT(map, "map", GUI_BASE(win_map));
            gui_img_set_src(((gui_img_t *)map),
                            dashboard_info->map_data[dashboard_info->map_data_index], ((gui_img_t *)map)->storage_type);
        }
    }
    else
    {
        // Speed information
        speed_val += 2;
        if (speed_val >= SPEED_MAX)
        {
            speed_val = 0;
        }

        // Power information
        power_val += 2;
        if (power_val >= POWER_MAX)
        {
            power_val = 0;
        }

        // Odo information
        odo_val += 1;
        if (odo_val >= 1000)
        {
            odo_val = 0;
        }

        // Soc information
        soc_val += 1;
        if (soc_val >= 100)
        {
            soc_val = 0;
        }
    }

    update_text_display();
}

static void dashboard_design(gui_view_t *view)
{
    gui_font_mem_init(HARMONYOS_SIZE28_BITS1_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS1_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE56_BITS1_FONT_BIN);

    gui_obj_t *parent = GUI_BASE(view);
    gui_obj_create_timer(parent, 10, true, dashboard_entrance_animation);

    switch_wait_cnt = 0;
    // Dashboard style
    gui_win_t *win_dashboard = gui_win_create(parent, 0, 0, 0, 0, 0);
    dashboard_dail_design(GUI_BASE(win_dashboard));
    // dashboard_digital_design(GUI_BASE(win_dashboard));

    // Speed display
    {
        sprintf(speed_str, "%d", speed_val);
        win_speed_display = gui_win_create(parent, 0, 38, 64, 294, 294);
        gui_obj_create_timer(GUI_BASE(win_speed_display), 10, true, win_speed_animation);
        gui_text_t *text = gui_text_create(win_speed_display, "speed_text", 0, 106, 294, 50);
        gui_text_set(text, (void *)speed_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(speed_str), 40);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        text = gui_text_create(win_speed_display, 0, 0, 141, 294, 30);
        gui_text_set(text, "km/h", GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen("km/h"), 26);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        gui_img_create_from_mem(win_speed_display, 0, DIVIDING_LINE_BIN, 96, 170, 0, 0);

        text = gui_text_create(win_speed_display, "odo_text", 0, 176, 294, 26);
        gui_text_set(text, odo_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(odo_str), 14);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);
    }

    // Power display
    {
        sprintf(power_str, "%d", power_val);
        win_power_display = gui_win_create(parent, 0, 470, 64, 294, 294);
        gui_obj_create_timer(GUI_BASE(win_power_display), 10, true, win_power_animation);
        gui_text_t *text = gui_text_create(win_power_display, "power_text", 0, 106, 294, 50);
        gui_text_set(text, (void *)power_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(power_str), 40);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        text = gui_text_create(win_power_display, 0, 0, 141, 294, 30);
        gui_text_set(text, "kW", GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen("kW"), 26);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        gui_img_create_from_mem(win_power_display, 0, DIVIDING_LINE_BIN, 96, 170, 0, 0);

        text = gui_text_create(win_power_display, "soc_text", 0, 176, 294, 26);
        gui_text_set(text, soc_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(soc_str), 14);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);
    }

    // Common part
    win_common = gui_win_create(parent, "win_common", 0, 0, 0, 0);
    gui_obj_hidden(GUI_BASE(win_common), true);
    gui_text_t *text = gui_text_create(win_common, "top_infor", 0, 20, 0, 0);
    sprintf(top_infor_str, "%s %s\n%s", loction_str, temp_str, time_str);
    gui_text_set(text, (void *)top_infor_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(top_infor_str),
                 26);
    gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_CENTER);
    gui_text_rendermode_set(text, 2);

    gui_img_t *switch_icon = gui_img_create_from_mem(win_common, "switch_icon", SWITCHICON_PRESS_BIN,
                                                     35, 30, 0, 0);
    gui_img_t *off_icon = gui_img_create_from_mem(win_common, "off_icon", OFFICON_PRESS_BIN, 740, 30, 0,
                                                  0);
    gui_obj_add_event_cb(switch_icon, press_icon, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_add_event_cb(switch_icon, release_icon, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_add_event_cb(off_icon, press_icon, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_add_event_cb(off_icon, release_icon, GUI_EVENT_TOUCH_RELEASED, NULL);

    gui_img_t *bt = gui_img_create_from_mem(win_common, "bt", BT_BIN, 294, 52, 0, 0);
    gui_img_a8_recolor(bt, 0xFFFFFFFF);

    gui_img_t *wifi = gui_img_create_from_mem(win_common, "wifi", WIFI_BIN, 478, 52, 0, 0);
    gui_obj_hidden(GUI_BASE(wifi), true);

    gui_img_create_from_mem(win_common, "led_turn_l", TURNLEFT_OFF_BIN, 20, 90, 0, 0);
    gui_img_create_from_mem(win_common, "led_turn_r", TURNRIGHT_OFF_BIN, 746, 90, 0, 0);

    gui_img_create_from_mem(win_common, "led0", LED0_OFF_BIN, DAIL_LED0_X,
                            DAIL_LED0_Y, 0, 0);
    gui_img_create_from_mem(win_common, "led1", LED1_OFF_BIN, DAIL_LED1_X,
                            DAIL_LED1_Y, 0, 0);
    gui_img_create_from_mem(win_common, "led2", LED2_OFF_BIN, DAIL_LED2_X,
                            DAIL_LED2_Y, 0, 0);
    gui_img_create_from_mem(win_common, "led3", LED3_OFF_BIN, DAIL_LED3_X,
                            DAIL_LED3_Y, 0, 0);
    gui_img_create_from_mem(win_common, "led4", LED4_OFF_BIN, DAIL_LED4_X,
                            DAIL_LED4_Y, 0, 0);
    gui_img_create_from_mem(win_common, "led5", LED5_OFF_BIN, DAIL_LED5_X,
                            DAIL_LED5_Y, 0, 0);
    app_phone_popup_create(parent);
    gui_message_popup_create(parent);
#ifdef _HONEYGUI_SIMULATOR_
    // Start navigation test after entrance animation completes
    extern void app_map_test_navigation(void);
    app_map_test_navigation();
#endif

}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Switch dashboard display mode (dail <-> digital)
 * Called from external key event handler
 */
void app_dashboard_switch_display_mode(void)
{
    if (current_view == NULL)
    {
        gui_log("Dashboard view not initialized, ignoring switch request\n");
        return;
    }

    if (animate_flag)
    {
        gui_log("Animation in progress, ignoring switch request\n");
        return;
    }

    animate_flag = 1;
    if (scene_flag == 1)
    {
        // Switch from dail to digital
        dashboard_digital_design(GUI_BASE(win_dail)->parent);
        exit_dashboard_dail();
        switch_wait_cnt = SWITCH_TO_DIGITAL_DURATION;
        gui_log("Switching from dail to digital mode\n");
    }
    else if (scene_flag == 2)
    {
        // Switch from digital to dail
        dashboard_dail_design(GUI_BASE(win_digital)->parent);
        exit_dashboard_digital();
        switch_wait_cnt = SWITCH_TO_DAIL_DURATION;
        gui_log("Switching from digital to dail mode\n");
    }
}

/**
 * @brief Exit dashboard and return to start view
 * Called from external key event handler
 */
void app_dashboard_exit_to_start(void)
{
    if (current_view == NULL)
    {
        gui_log("Dashboard view not initialized, ignoring exit request\n");
        return;
    }

    if (animate_flag || scene_flag == 0)
    {
        gui_log("Animation in progress or already exiting, ignoring exit request\n");
        return;
    }

    if (scene_flag == 1)
    {
        exit_dashboard_dail();
    }
    else if (scene_flag == 2)
    {
        exit_dashboard_digital();
    }

    scene_flag = 0;
    animate_flag = 1;
    gui_obj_create_timer(GUI_BASE(current_view), 10, true, dashboard_exit_animation);
    exit_map();

    gui_log("Exiting dashboard to start view\n");
}
