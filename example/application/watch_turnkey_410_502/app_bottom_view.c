/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "cJSON.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_list.h"
#include "gui_view_instance.h"
#include "gui_lite_geometry_round_rect.h"
#include "gui_canvas.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "bottom_view"
#define __WIN1_NAME    "win_timecard"
#define __WIN0_NAME    "win_timecircle"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void bottom_view_design(gui_view_t *view);
static void clear_bottom_view(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, bottom_view_design, clear_bottom_view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
const char *month[12] =
{
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
};
const char *day[7] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};


static char date_content[] = "WED\nNOV\n19";
static char date_timecard_content[10] = "Sun 0";

static uint8_t *img_data_activity = NULL;
static char *move_content = NULL;
static char *ex_content = NULL;
static char *stand_content = NULL;
uint8_t battery_level = 0;
uint8_t *img_data_battery = NULL;
static gui_list_t *list = NULL;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void clear_bottom_view(gui_view_t *view)
{
    (void)view;
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
    }
    if (img_data_battery)
    {
        gui_lower_free(img_data_battery);
        img_data_battery = NULL;
    }
    if (move_content)
    {
        gui_free(move_content);
        gui_free(ex_content);
        gui_free(stand_content);
        move_content = NULL;
        ex_content = NULL;
        stand_content = NULL;
    }
}

static void time_update_cb(void)
{
    if (!timeinfo)
    {
        return;
    }
    else
    {
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "time_s", current_view);
            gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "date_s", current_view);
            sprintf(date_timecard_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
            gui_text_content_set((gui_text_t *)obj, date_timecard_content, strlen(date_timecard_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "date_b", current_view);
            sprintf(date_content, "%s\n%s\n%d", day[timeinfo->tm_wday], month[timeinfo->tm_mon],
                    timeinfo->tm_mday);
            gui_text_content_set((gui_text_t *)obj, date_content, strlen(date_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", current_view);
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}

// draw timecard and not display
static void draw_timecard(void *parent)
{
    gui_win_t *win = gui_win_create(parent, __WIN1_NAME, 0, 0, SCREEN_WIDTH, 60);
    gui_lite_round_rect_t *rect_timecard = gui_lite_round_rect_create(GUI_BASE(win), __WIN1_NAME, 35, 0,
                                                                      340, 60, 20, gui_rgba(39, 43, 44, 255 * 0.7));
    // text
    gui_text_t *timecard_date_text = gui_text_create(rect_timecard, "date_s",  15, 20, 0, 0);
    gui_text_set(timecard_date_text, (void *)date_timecard_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_timecard_content),
                 32);
    gui_text_type_set(timecard_date_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_date_text, LEFT);

    gui_text_t *timecard_time_text = gui_text_create(rect_timecard, "time_s",  -10, 20, 0,
                                                     0);
    gui_text_set(timecard_time_text, (void *)time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(time_str),
                 32);
    gui_text_type_set(timecard_time_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_time_text, RIGHT);
    GUI_BASE(win)->not_show = 1;
    gui_win_enable_blur(win, false);
}

static void timer_cb(void *obj)
{
    (void)obj;
    static uint16_t count = 0;
    count++;
    if (count >= 300)
    {
        time_update_cb();
        count = 0;
    }

    // control timecard display
    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_timecard, __WIN0_NAME, current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(canvas_timecard, __WIN1_NAME, current_view);
    if (list->offset < 93)
    {
        img_timecard->not_show = 1;
        canvas_timecard->not_show = 0;
        gui_win_enable_blur((void *)canvas_timecard, true);
    }
    else
    {
        img_timecard->not_show = 0;
        canvas_timecard->not_show = 1;
        gui_win_enable_blur((void *)canvas_timecard, false);
    }
}

static void switch_app_cb(void *obj)
{
    GUI_UNUSED(obj);
    const char *obj_name = ((gui_obj_t *)obj)->name;
    if (strcmp(obj_name, "note_weather") == 0)
    {
        gui_view_switch_direct(current_view, gui_view_descriptor_get("weather_view"),
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "note_ac") == 0)
    {
        gui_view_switch_direct(current_view, gui_view_descriptor_get("activity_view"),
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "MUSIC") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        gui_view_switch_direct(current_view, gui_view_descriptor_get("music_view"),
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "BATTERY") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        gui_view_switch_direct(current_view, gui_view_descriptor_get("battery_view"),
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "MESSAGE") == 0)
    {
        // gui_view_switch_direct(current_view, gui_view_descriptor_get("app_message_view"), SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
        //                        SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "APP_MENU") == 0)
    {
        gui_view_set_animate_step(current_view, 100);
        gui_view_switch_direct(current_view, gui_view_descriptor_get("menu_view"),
                               SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}
static int steps_to_calories(int steps)
{
    return (int)(steps * 0.04f);
}
static void arc_activity_cb(NVGcontext *vg)
{
    cJSON *root;
    extern char *cjson_content;
    if (!cjson_content)
    {
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse activity array
    cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
    if (cJSON_IsArray(activity_array))
    {
        cJSON *act = cJSON_GetArrayItem(activity_array, 0);
        if (!act)
        {
            gui_log("get activity_array unsuccessful\n");
        }
        else
        {
            cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
            cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
            cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");

            int calories = steps_to_calories(move->valueint);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * calories / 500.0f), NVG_CW);  // cap 500 kcol
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(30, 55, 25));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(22, 50, 47));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * stand->valueint / 6.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);

            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(move_text, "ac_move", current_view);
                sprintf(move_content, "%d/500 kcol",  calories);
                gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(ex_text, "ac_ex", current_view);
                sprintf(ex_content, "%d/60 min", ex->valueint);
                gui_text_content_set((gui_text_t *)ex_text, ex_content, strlen(ex_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(stand_text, "ac_stand", current_view);
                sprintf(stand_content, "%d/6 h", stand->valueint);
                gui_text_content_set((gui_text_t *)stand_text, stand_content, strlen(stand_content));
            }
        }
    }
    // clear
    cJSON_Delete(root);
}
static void draw_battery_arc(NVGcontext *vg)
{
    float cx = 45.0f;
    float cy = 45.0f;
    float outer_radius = 39.6f;
    // float inner_radius = 32.4f;
    float stroke_width = 7.2f;
    battery_level = 75;

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, outer_radius);
    nvgFillColor(vg, nvgRGBA(50, 50, 50, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, outer_radius);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, cx, cy, outer_radius - stroke_width / 2, 0, 2 * M_PI_F, NVG_CW);

    nvgStrokeColor(vg, battery_level < 10 ? nvgRGBA(255, 0, 0, 80) : nvgRGBA(0, 200, 0, 80));
    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);

    float start_angle = -M_PI_F / 2;
    float sweep_angle = (battery_level / 100.0f) * 2 * M_PI_F;

    nvgBeginPath(vg);
    nvgArc(vg, cx, cy, outer_radius - stroke_width / 2, start_angle, start_angle + sweep_angle, NVG_CW);


    if (battery_level < 10.0f)
    {
        nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));
    }
    else
    {
        nvgStrokeColor(vg, nvgRGBA(0, 200, 0, 255));
    }

    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;

    if (index == 0)
    {
        // note_weather
        gui_img_t *img = gui_img_create_from_mem(obj, "note_weather", UI_CARD_WEATHER_BIN, offset_X, 0, 0,
                                                 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    }
    else if (index == 1)
    {
        // note_activity
        {

            gui_lite_round_rect_t *rect_activity = gui_lite_round_rect_create(GUI_BASE(obj), "note_ac",
                                                                              offset_X, 0, 352, 157, 20, gui_rgba(98, 101, 98, 255 * 0.7));
            if (move_content == NULL)
            {
                move_content = (char *)gui_malloc(30);
                ex_content = (char *)gui_malloc(30);
                stand_content = (char *)gui_malloc(30);
            }
            sprintf(move_content, "0/500 kcol");
            gui_text_t *move_text = gui_text_create(rect_activity, "ac_move", 134, 20, 0, 0);
            gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_TTF, gui_rgb(230, 67, 79),
                         strlen(move_content), 24);
            gui_text_type_set(move_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(move_text, LEFT);

            sprintf(ex_content, "0/60 min");
            gui_text_t *ex_text = gui_text_create(rect_activity, "ac_ex", 134, 57, 0, 0);
            gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_TTF, gui_rgb(186, 253, 79),
                         strlen(ex_content), 24);
            gui_text_type_set(ex_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(ex_text, LEFT);


            sprintf(stand_content, "0/6 h");
            gui_text_t *stand_text = gui_text_create(rect_activity, "ac_stand", 134, 94, 0, 0);
            gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_TTF, gui_rgb(117, 230, 229),
                         strlen(stand_content), 24);
            gui_text_type_set(stand_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(stand_text, LEFT);
            // activity icon
            {
                int image_h = 100;
                int image_w = 100;
                int pixel_bytes = 4;
                size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                if (!img_data_activity)
                {
                    img_data_activity = gui_lower_malloc(buffer_size);
                }
                memset(img_data_activity, 0, buffer_size);
                gui_img_t *img = gui_img_create_from_mem(rect_activity, "activity", img_data_activity, 17, 28, 0,
                                                         0);
                img->base.w = image_w;
                img->base.h = image_h;
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb,
                                                  img_data_activity);
            }
            gui_obj_add_event_cb(rect_activity, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else if (index == 2)
    {
        gui_lite_round_rect_t *rect_app = gui_lite_round_rect_create(GUI_BASE(obj), "APP_CANVAS", offset_X,
                                                                     0, 352, 157, 20, gui_rgba(98, 101, 98, 255 * 0.7));
        // Information Statistics Quick Bar
        //music
        {
            gui_img_t *img = gui_img_create_from_mem(rect_app, "MUSIC", APP_MUSIC_ICON_BIN, 17, 28,
                                                     0, 0);
            gui_img_scale(img, 0.85, 0.85);
            gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
        //battery
        {
            battery_level = 75;

            int image_h = 100;
            int image_w = 100;
            int pixel_bytes = 4;
            size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);

            if (!img_data_battery)
            {
                img_data_battery = gui_lower_malloc(buffer_size);
            }
            memset(img_data_battery, 0, buffer_size);



            gui_img_t *img_battery = gui_img_create_from_mem(rect_app, "BATTERY", img_data_battery, 17 + 109,
                                                             28, 0, 0);

            img_battery->base.w = image_w;
            img_battery->base.h = image_h;

            gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_battery_arc,
                                              img_data_battery);
            gui_img_set_mode(img_battery, IMG_SRC_OVER_MODE);

            char battery_content[3];

            sprintf(battery_content, "%d", battery_level);

            gui_text_t *battery_text = gui_text_create(rect_app, "battery_text", 145, 42, 0, 0);
            gui_text_set(battery_text, "75", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(battery_content), 50);
            gui_text_type_set(battery_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(battery_text, LEFT);

            gui_obj_add_event_cb(img_battery, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
        //message
        {

            gui_img_t *img = gui_img_create_from_mem(rect_app, "MESSAGE", APP_MESSAGE_ICON_BIN, 17 + 109 * 2,
                                                     28, 0,
                                                     0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_img_scale(img, 0.85, 0.85);
            // gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else if (index == 3)
    {
        gui_lite_round_rect_t *rect_menu_bg = gui_lite_round_rect_create(obj, "APP_MENU", 76, 0, 258, 76,
                                                                         38, gui_rgba(98, 101, 98, 255 * 0.7));
        gui_text_t *text = gui_text_create(rect_menu_bg, 0, 70, 20, 0, 0);
        gui_text_set(text, "APP MENU", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("APP MENU"), 32);
        gui_text_type_set(text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        gui_obj_add_event_cb(rect_menu_bg, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void bottom_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, gui_view_descriptor_get("watchface_view"),
                             SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_set_opacity(view, 200);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    // gui_canvas_rect_t *canvas_bg = NULL;
    gui_lite_round_rect_t *rect_menu_bg = gui_lite_round_rect_create(parent, "bottom_bg", 0, 0,
                                                                     SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgba(0, 0, 0, 255));

    if (current_watchface_type == WATCHFACE_BIG_NUM)
    {
        gui_lite_round_rect_set_color(rect_menu_bg, gui_rgba(219, 122, 147, 255));
    }

    gui_obj_create_timer(GUI_BASE(rect_menu_bg), 100, true, timer_cb);

    //clock circle
    gui_win_t *win = gui_win_create(parent, __WIN0_NAME, 38, 30, 335, 180);
    gui_img_t *img_clock = gui_img_create_from_mem(win, 0, UI_CARD_CLOCKCIRCLE_BIN, 163, 0, 0, 0);
    gui_img_set_mode(img_clock, IMG_SRC_OVER_MODE);
    gui_text_t *t_time = gui_text_create(img_clock, "time_b", 0, 0, 190, 190);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 60);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);

    gui_text_t *date_text = gui_text_create(win, "date_b",  0, 22, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_content),
                 48);
    gui_text_type_set(date_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, MULTI_LEFT);

    // Card list
    int space = 10;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CARD);
    gui_list_set_note_num(list, 4);
    gui_list_set_offset(list, 250);
    gui_list_set_card_stack_location(list, 20);

    draw_timecard(parent);

    time_update_cb();
}
