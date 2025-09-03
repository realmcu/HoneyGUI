/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include "app_main_dashboard.h"
#include "gui_view.h"
#include "gui_canvas.h"


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

#define RADIUS_OUTER 92
#define RADIUS_INNER 65

#define SPEED_MAX 160 // km/h
#define POWER_MAX 160 // kW

#define WIN_DAIL_LINE_TARGET_X 26
#define WIN_DAIL_LINE_TARGET_Y 53
#define WIN_DAIL_LINE_INIT_X 331
#define WIN_DAIL_LINE_INIT_Y 53
#define WIN_DAIL_LINE_INIT_W 138
#define WIN_DAIL_LINE_INIT_H 100
#define WIN_DAIL_LINE_TARGET_W 750
#define WIN_DAIL_LINE_TARGET_H 415

#define ANIMATION_DURATION 20

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void dashboard_design(gui_view_t *view);

static void update_text_display();
static void update_inform(void *p);
static void dashboard_exit_animation(void *p);
static void win_speed_animation(void *p);
static void win_power_animation(void *p);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *start_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = dashboard_design,
};

// Common dashboard
static uint8_t scene_flag = 0; // 0: start_engine, 1: dail, 2: digital
static gui_win_t *win_speed_diplay = NULL;
static gui_win_t *win_power_diplay = NULL;
static char top_infor_str[32] = {0};
static char loction_str[12] = "Suzhou";
static char temp_str[8] = "25°C";
static char time_str[8] = "00:00";

static uint16_t speed_val = 0; // km/h
static uint16_t power_val = 0; // kW
static char speed_str[4] = "0";
static char power_str[4] = "0";

static uint16_t odo_val = 0; // km/h
static uint16_t soc_val = 0; // kW
static char odo_str[12] = "ODO 0km";
static char soc_str[12] = "BATT 0%";

struct tm *timeinfo = NULL;

// Digital dashboard
static gui_win_t *win_digital = NULL;

// Dail dashboard
static gui_win_t *win_dail = NULL;
static gui_win_t *win_dail_line = NULL;
static void *speed_indicator_data = NULL;
static void *power_indicator_data = NULL;

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
        end_rad = start_rad - 2.0f * M_PI * (float)power_val / (float)POWER_MAX;
        dir = NVG_CCW;
    }

    NVGcolor start_color = nvgRGBA(0, 0, 0, 0);
    NVGcolor end_color = nvgRGBA(0, 100, 140, 255);
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

static void dashboard_dail_exit_animation(void *p)
{
    static int16_t offset = 0;
    int16_t step = 10;
    if (scene_flag != 1) {step = -10;}

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
        }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "line_left", GUI_BASE(win_dail_line));
        GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "line_right", GUI_BASE(win_dail_line));
        left->x = WIN_DAIL_LINE_TARGET_X - win_dail_line->base.x;
        right->x = win_dail_line->base.w / 2 - 2;
    }
}

static void dashboard_dail_entrance_animation(void *p)
{
    static int16_t offset = 0;
    int16_t step = 10;
    if (scene_flag != 1) {step = -10;}

    if (win_dail_line->base.x > WIN_DAIL_LINE_TARGET_X)
    {
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
        right->x = win_dail_line->base.w / 2 - 2;
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
        }
    }
}

static void exit_dashboard_dail()
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
    win_dail = gui_win_create(parent, 0, 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win_dail), 10, true, dashboard_dail_entrance_animation);
    win_dail_line = gui_win_create(win_dail, 0, WIN_DAIL_LINE_INIT_X, WIN_DAIL_LINE_INIT_Y,
                                   WIN_DAIL_LINE_INIT_W, WIN_DAIL_LINE_INIT_H);
    gui_img_t *line_left = gui_img_create_from_mem(win_dail_line, "line_left", LEFTLINEV1_BIN,
                                                   WIN_DAIL_LINE_TARGET_X - WIN_DAIL_LINE_INIT_X, 0, 0, 0);
    gui_img_t *line_right = gui_img_create_from_mem(win_dail_line, "line_right", RIGHTLINEV1_BIN,
                                                    WIN_DAIL_LINE_INIT_W / 2 - 2, 0, 0, 0);
    line_left->need_clip = true;
    line_right->need_clip = true;

    gui_img_t *gloom_left = gui_img_create_from_mem(win_dail, 0, LEFTBOTTOMGLOOMV1_BIN, 40, 353, 0, 0);
    gui_img_t *gloom_right = gui_img_create_from_mem(win_dail, 0, RIGHTBOTTOMGLOOMV1_BIN, 661, 353, 0,
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

static void press_icon(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    if (strcmp(img->base.name, "switch_icon") == 0)
    {
        gui_img_set_image_data(img, SWITCHICON_PRESS_BIN);
    }
    else
    {
        gui_img_set_image_data(img, OFFICON_PRESS_BIN);
    }
}

static void release_icon(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    touch_info_t *tp = tp_get_info();
    bool valid_touch = (gui_obj_point_in_obj_rect(obj, tp->x + tp->deltaX - GUI_BASE(obj)->x,
                                                  tp->y + tp->deltaY - GUI_BASE(obj)->y) == true);
    if (strcmp(img->base.name, "switch_icon") == 0)
    {
        gui_img_set_image_data(img, SWITCHICON_RELEASE_BIN);
        if (valid_touch)
        {
            //change_scene
        }
    }
    else
    {
        gui_img_set_image_data(img, OFFICON_RELEASE_BIN);
        if (valid_touch && scene_flag != 0)
        {
            if (scene_flag == 1)
            {
                exit_dashboard_dail();
            }
            else
            {
                //
            }
            scene_flag = 0;
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
    zoom_animation(GUI_BASE(win_speed_diplay), scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        if (scene_flag == 0)
        {
            gui_obj_hidden(GUI_BASE(win_speed_diplay), true);
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
    zoom_animation(GUI_BASE(win_power_diplay), scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        if (scene_flag == 0)
        {
            gui_obj_hidden(GUI_BASE(win_power_diplay), true);
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
        gui_obj_create_timer(GUI_BASE(win_speed_diplay), 10, true, win_speed_animation);
        gui_obj_create_timer(GUI_BASE(win_power_diplay), 10, true, win_power_animation);
        gui_obj_start_timer(GUI_BASE(win_speed_diplay));
        gui_obj_start_timer(GUI_BASE(win_power_diplay));
    }
    else if (cnt >= cnt_max + 50)
    {
        cnt = 0;
        gui_view_set_animate_step(current_view, 800);
        gui_view_switch_direct(current_view, start_view, SWITCH_OUT_ANIMATION_FADE,
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
    if (cnt < cnt_max)
    {
        return; //waiting
    }
    else if (cnt == cnt_max)
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(win_common, "win_common", obj);
        gui_obj_hidden(GUI_BASE(win_common), false);
        return;
    }
    else if (cnt == (cnt_max + cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", obj);
        gui_img_set_image_data(((gui_img_t *)led2), LED2_ON_BIN);
        gui_img_set_image_data(((gui_img_t *)led5), LED5_ON_BIN);
    }
    else if (cnt == (cnt_max + 2 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", obj);
        gui_img_set_image_data(((gui_img_t *)led1), LED1_ON_BIN);
        gui_img_set_image_data(((gui_img_t *)led4), LED4_ON_BIN);
        return;
    }
    else if (cnt == (cnt_max + 3 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", obj);
        gui_img_set_image_data(((gui_img_t *)led0), LED0_ON_BIN);
        gui_img_set_image_data(((gui_img_t *)led3), LED3_ON_BIN);
        return;
        return;
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
        return;
    }
    else if (cnt == (cnt_max + 5 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led0, "led0", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led3, "led3", obj);
        gui_img_set_image_data(((gui_img_t *)led0), LED0_OFF_BIN);
        gui_img_set_image_data(((gui_img_t *)led3), LED3_OFF_BIN);
        return;
    }
    else if (cnt == (cnt_max + 6 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led1, "led1", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led4, "led4", obj);
        gui_img_set_image_data(((gui_img_t *)led1), LED1_OFF_BIN);
        gui_img_set_image_data(((gui_img_t *)led4), LED4_OFF_BIN);
        return;
    }
    else if (cnt == (cnt_max + 7 * cnt_max / 7))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led2, "led2", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(led5, "led5", obj);
        gui_img_set_image_data(((gui_img_t *)led2), LED2_OFF_BIN);
        gui_img_set_image_data(((gui_img_t *)led5), LED5_OFF_BIN);
        map_design(); //show map

        GUI_WIDGET_POINTER_BY_NAME_ROOT(switch_icon, "switch_icon", obj);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(off_icon, "off_icon", obj);
        gui_img_set_image_data(((gui_img_t *)switch_icon), SWITCHICON_RELEASE_BIN);
        gui_img_set_image_data(((gui_img_t *)off_icon), OFFICON_RELEASE_BIN);
        return;
    }
    else if (cnt >= (cnt_max + 8 * cnt_max / 7))
    {
        cnt = 0;
        gui_obj_create_timer(obj, 10, true, update_inform);
        gui_obj_start_timer(obj);
    }
}

static void update_text_display()
{
    sprintf(speed_str, "%d", speed_val);
    sprintf(power_str, "%d", power_val);
    sprintf(odo_str, "ODO %dkm", odo_val);
    sprintf(soc_str, "BATT %d%%", soc_val);

    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "speed_text", win_speed_diplay);
        gui_text_content_set((gui_text_t *)text, (void *)speed_str, strlen(speed_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "power_text", win_power_diplay);
        gui_text_content_set((gui_text_t *)text, (void *)power_str, strlen(power_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "odo_text", win_speed_diplay);
        gui_text_content_set((gui_text_t *)text, (void *)odo_str, strlen(odo_str));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "soc_text", win_power_diplay);
        gui_text_content_set((gui_text_t *)text, (void *)soc_str, strlen(soc_str));
    }
}

static void update_inform(void *p)
{
#if defined __WIN32
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif
    // Top information
    if (timeinfo)
    {
        sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }
    sprintf(top_infor_str, "%s %s\n%s", loction_str, temp_str, time_str);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "top_infor", current_view);
    gui_text_content_set((gui_text_t *)text, top_infor_str, strlen(top_infor_str));

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
    update_text_display();
}

static void dashboard_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    scene_flag = 1;

    // Dashboard style
    gui_win_t *win_dashboard = gui_win_create(parent, 0, 0, 0, 0, 0);
    dashboard_dail_design(GUI_BASE(win_dashboard));

    // Common part
    gui_win_t *win_common = gui_win_create(parent, "win_common", 0, 0, 0, 0);
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

    gui_img_t *led_0 = gui_img_create_from_mem(win_common, "led0", LED0_OFF_BIN, DAIL_LED0_X,
                                               DAIL_LED0_Y, 0, 0);
    gui_img_t *led_1 = gui_img_create_from_mem(win_common, "led1", LED1_OFF_BIN, DAIL_LED1_X,
                                               DAIL_LED1_Y, 0, 0);
    gui_img_t *led_2 = gui_img_create_from_mem(win_common, "led2", LED2_OFF_BIN, DAIL_LED2_X,
                                               DAIL_LED2_Y, 0, 0);
    gui_img_t *led_3 = gui_img_create_from_mem(win_common, "led3", LED3_OFF_BIN, DAIL_LED3_X,
                                               DAIL_LED3_Y, 0, 0);
    gui_img_t *led_4 = gui_img_create_from_mem(win_common, "led4", LED4_OFF_BIN, DAIL_LED4_X,
                                               DAIL_LED4_Y, 0, 0);
    gui_img_t *led_5 = gui_img_create_from_mem(win_common, "led5", LED5_OFF_BIN, DAIL_LED5_X,
                                               DAIL_LED5_Y, 0, 0);

    // Speed display
    {
        sprintf(speed_str, "%d", speed_val);
        win_speed_diplay = gui_win_create(parent, 0, 38, 64, 294, 294);
        gui_obj_create_timer(GUI_BASE(win_speed_diplay), 10, true, win_speed_animation);
        gui_text_t *text = gui_text_create(win_speed_diplay, "speed_text", 0, 106, 294, 50);
        gui_text_set(text, (void *)speed_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(speed_str), 40);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        text = gui_text_create(win_speed_diplay, 0, 0, 141, 294, 30);
        gui_text_set(text, "km/h", GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen("km/h"), 26);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        gui_img_t *img = gui_img_create_from_mem(win_speed_diplay, 0, DIVIDING_LINE_BIN, 96, 170, 0, 0);

        text = gui_text_create(win_speed_diplay, "odo_text", 0, 176, 294, 26);
        gui_text_set(text, odo_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(odo_str), 14);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);
    }

    // Power display
    {
        sprintf(power_str, "%d", power_val);
        win_power_diplay = gui_win_create(parent, 0, 470, 64, 294, 294);
        gui_obj_create_timer(GUI_BASE(win_power_diplay), 10, true, win_power_animation);
        gui_text_t *text = gui_text_create(win_power_diplay, "power_text", 0, 106, 294, 50);
        gui_text_set(text, (void *)power_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(power_str), 40);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        text = gui_text_create(win_power_diplay, 0, 0, 141, 294, 30);
        gui_text_set(text, "kW", GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen("kW"), 26);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);

        gui_img_t *img = gui_img_create_from_mem(win_power_diplay, 0, DIVIDING_LINE_BIN, 96, 170, 0, 0);

        text = gui_text_create(win_power_diplay, "soc_text", 0, 176, 294, 26);
        gui_text_set(text, soc_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(soc_str), 14);
        gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_text_rendermode_set(text, 2);
    }
    gui_obj_create_timer(parent, 10, true, dashboard_entrance_animation);
}