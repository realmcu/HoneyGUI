/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "cJSON.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_canvas.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_canvas.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "gui_list.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "app_bottom_view"
#define __WIN1_NAME    "win_timecard"
#define __WIN0_NAME    "win_timecircle"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void bottom_view_design(gui_view_t *view);
static void clear_bottom_view(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *fruit_ninja_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *box2d_ring_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = bottom_view_design,
    .on_switch_out = clear_bottom_view,
};

const char *month[12] =
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};
const char *day[7] =
{
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

extern void *text_num_array[];
extern struct tm *timeinfo;
static char time_content[10] = "00:00";
static char date_content[20] = "January0\nSun";
static char time_timecard_content[10] =  "00:00";
static char date_timecard_content[10] = "Sun 0";

static char calendar_name[10] = "Calendar";
static char calendar_content[30] = "No proximity schedules.";

static uint8_t *img_data_activity = NULL;
static char *move_content = NULL;
static char *ex_content = NULL;
static char *stand_content = NULL;

static gui_list_t *list = NULL;
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
    watchface_view = gui_view_descriptor_get("watchface_view");
    fruit_ninja_view = gui_view_descriptor_get("fruit_ninja_view");
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    box2d_ring_view = gui_view_descriptor_get("box2d_ring_view");
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void clear_bottom_view(gui_view_t *view)
{
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
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
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "timecard_time", current_view);
            sprintf(time_timecard_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
            gui_text_content_set((gui_text_t *)obj, time_timecard_content, strlen(time_timecard_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "timecard_date_1", current_view);
            sprintf(date_timecard_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
            gui_text_content_set((gui_text_t *)obj, date_timecard_content, strlen(date_timecard_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "timecard_date_2", current_view);
            sprintf(date_content, "%s%d\n%s", month[timeinfo->tm_mon], timeinfo->tm_mday,
                    day[timeinfo->tm_wday]);
            gui_text_content_set((gui_text_t *)obj, date_content, strlen(date_content));
        }

        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "circle_hour_decimal",
                                        current_view);
        gui_img_set_attribute((gui_img_t *)img_hour_decimal, img_hour_decimal->name,
                              text_num_array[timeinfo->tm_hour / 10], img_hour_decimal->x, img_hour_decimal->y);
        gui_img_refresh_size((gui_img_t *)img_hour_decimal);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "circle_hour_single",
                                        current_view);
        gui_img_set_attribute((gui_img_t *)img_hour_single, img_hour_single->name,
                              text_num_array[timeinfo->tm_hour % 10], img_hour_single->x, img_hour_single->y);
        gui_img_refresh_size((gui_img_t *)img_hour_single);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "circle_minute_decimal",
                                        current_view);
        gui_img_set_attribute((gui_img_t *)img_minute_decimal, img_minute_decimal->name,
                              text_num_array[timeinfo->tm_min / 10], img_minute_decimal->x, img_minute_decimal->y);
        gui_img_refresh_size((gui_img_t *)img_minute_decimal);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "circle_minute_single",
                                        current_view);
        gui_img_set_attribute((gui_img_t *)img_minute_single, img_minute_single->name,
                              text_num_array[timeinfo->tm_min % 10], img_minute_single->x, img_minute_single->y);
        gui_img_refresh_size((gui_img_t *)img_minute_single);
    }
}

// draw timecard and not display
static void draw_timecard(void *parent)
{
    // gui_win_t *win = gui_win_create(parent, __WIN1_NAME, 0, 0, SCREEN_WIDTH, 60);
    gui_canvas_round_rect_t *canvas_timecard = gui_canvas_round_rect_create(GUI_BASE(parent),
                                                                            __WIN1_NAME,
                                                                            35, 0, 340, 60, 20, gui_rgba(39, 43, 44, 255 * 0.7));

    // text
    gui_text_t *timecard_date_text = gui_text_create(canvas_timecard, "timecard_date_1",  15, 20, 0, 0);
    gui_text_set(timecard_date_text, (void *)date_timecard_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_timecard_content),
                 32);
    gui_text_type_set(timecard_date_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_date_text, LEFT);

    gui_text_t *timecard_time_text = gui_text_create(canvas_timecard, "timecard_time",  -10, 20, 0,
                                                     0);
    gui_text_set(timecard_time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 32);
    gui_text_type_set(timecard_time_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_time_text, RIGHT);
    GUI_BASE(canvas_timecard)->not_show = 1;
}

static void timer_cb(void *obj)
{
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
    }
    else
    {
        img_timecard->not_show = 0;
        canvas_timecard->not_show = 1;
    }
}

static void switch_app_cb(void *obj)
{
    const char *obj_name = ((gui_obj_t *)obj)->name;
    if (strcmp(obj_name, "RING") == 0)
    {
        gui_view_switch_direct(current_view, box2d_ring_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "FJ") == 0)
    {
        gui_view_switch_direct(current_view, fruit_ninja_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "HR") == 0)
    {
        gui_view_switch_direct(current_view, heartrate_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "app_menu") == 0)
    {
        gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}

static void img_bg_cb(NVGcontext *vg)
{
    nvgRoundedRect(vg, 0, 0, 352, 157, 20);
    nvgFillColor(vg, nvgRGB(98, 101, 102));
    nvgFill(vg);
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

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * move->valueint / 20000.0f), NVG_CW);  // cap 20000 steps
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
                   M_PI_F * (1.5f + 2.0f * stand->valueint / 30.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);

            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(move_text, "ac_move", current_view);
                sprintf(move_content, "Move: %d/20000 steps", move->valueint);
                gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(ex_text, "ac_ex", current_view);
                sprintf(ex_content, "Exercise: %d/60 min", ex->valueint);
                gui_text_content_set((gui_text_t *)ex_text, ex_content, strlen(ex_content));
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(stand_text, "ac_stand", current_view);
                sprintf(stand_content, "Stand: %d/30 times", stand->valueint);
                gui_text_content_set((gui_text_t *)stand_text, stand_content, strlen(stand_content));
            }
        }
    }
    // clear
    cJSON_Delete(root);
}
static void note_design(gui_obj_t *obj, void *p)
{
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;

    if (index == 0)
    {
        // note_calendar
        gui_img_t *img = gui_img_create_from_mem(obj, "calendar", UI_CARD_WHITE_BG_BIN, offset_X,
                                                 0, 0, 0);
        gui_text_t *text = gui_text_create(img, 0, 15, 15, 0, 0);
        gui_text_set(text, (void *)calendar_name, GUI_FONT_SRC_BMP, gui_rgb(0, 0, 0),
                     strlen(calendar_name), 48);
        gui_text_type_set(text, SOURCEHANSANSSC_SIZE48_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        // gui_text_rendermode_set(text, 2);

        text = gui_text_create(img, 0, 15, 65, 0, 0);
        gui_text_set(text, (void *)calendar_content, GUI_FONT_SRC_BMP, gui_rgb(40, 40, 40),
                     strlen(calendar_content), 32);
        gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        // gui_text_rendermode_set(text, 2);

    }
    else if (index == 1)
    {
        // note_weather
        gui_img_t *img = gui_img_create_from_mem(obj, "weather", UI_CARD_WEATHER_BIN, offset_X, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    else if (index == 2)
    {
        // note_activity
        {
            gui_img_t *canvas = gui_img_create_from_mem(obj, "note_ac", UI_CARD_GREY_BG_BIN, offset_X,
                                                        0, 0, 0);
            gui_img_set_mode(canvas, IMG_SRC_OVER_MODE);
            gui_img_t *img = gui_img_create_from_mem(canvas, "ac_bg", UI_BG_ICON_BIN, 22, 33, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_img_scale(img, 0.9f, 0.9f);

            if (move_content == NULL)
            {
                move_content = (char *)gui_malloc(30);
                ex_content = (char *)gui_malloc(30);
                stand_content = (char *)gui_malloc(30);
            }
            sprintf(move_content, "Move: 0/20000 steps");
            gui_text_t *move_text = gui_text_create(canvas, "ac_move", 134, 20, 0, 0);
            gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_BMP, gui_rgb(230, 67, 79),
                         strlen(move_content), 24);
            gui_text_type_set(move_text, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(move_text, LEFT);

            sprintf(ex_content, "Exercise: 0/60 min");
            gui_text_t *ex_text = gui_text_create(canvas, "ac_ex", 134, 57, 0, 0);
            gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_BMP, gui_rgb(186, 253, 79),
                         strlen(ex_content), 24);
            gui_text_type_set(ex_text, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(ex_text, LEFT);


            sprintf(stand_content, "Stand: 0/30 times");
            gui_text_t *stand_text = gui_text_create(canvas, "ac_stand", 134, 94, 0, 0);
            gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_BMP, gui_rgb(117, 230, 229),
                         strlen(stand_content), 24);
            gui_text_type_set(stand_text, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
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
                img = gui_img_create_from_mem(canvas, "activity", img_data_activity, 17, 28, 0, 0);
                img->base.w = image_w;
                img->base.h = image_h;
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb,
                                                  img_data_activity);
            }
        }
    }
    else if (index == 3)
    {
        // note_app
        gui_img_t *canvas_app = gui_img_create_from_mem(obj, "note_ac", UI_CARD_GREY_BG_BIN, offset_X,
                                                        0, 0, 0);
        gui_img_t *img = gui_img_create_from_mem(canvas_app, "RING", UI_CLOCK_BOX2D_RING_ICON_BIN, 17, 28,
                                                 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        img = gui_img_create_from_mem(canvas_app, "FJ", UI_CLOCK_FRUIT_NINJA_ICON_BIN, 17 + 109, 28, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        img = gui_img_create_from_mem(canvas_app, "HR", UI_CLOCK_HEARTRATE_ICON_BIN, 17 + 109 * 2, 28, 0,
                                      0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    else if (index == 4)
    {
        // note_app_menu
        // gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(note_appview), "app_menu",
        //                                                                57, 0, 292, 76, 40, gui_rgb(100, 100, 100));
        // img = gui_img_create_from_mem(canvas, 0, APP_MENU_ICON_BIN, 38, 20, 0, 0);
        // gui_img_set_mode(img, IMG_SRC_OVER_MODE);

        // gui_text_t *text = gui_text_create(canvas, 0, 90, 20, 0, 0);
        // gui_text_set(text, (void *)"ALL APP", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 7, 32);
        // gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        // gui_text_mode_set(text, LEFT);

        gui_img_t *img = gui_img_create_from_mem(obj, "app_menu", UI_APPVIEW_CARD_BIN, 76, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void bottom_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(parent, "bg", 0, 0, SCREEN_WIDTH,
                                                          SCREEN_HEIGHT, gui_rgb(0, 0, 0));
    gui_obj_create_timer(GUI_BASE(canvas_bg), 100, true, timer_cb);

    //clock circle
    gui_win_t *win = gui_win_create(parent, __WIN0_NAME, 38, 30, 335, 180);
    gui_img_t *img_clock = gui_img_create_from_mem(win, 0, UI_CARD_CLOCKCIRCLE_BIN, 157, 0, 0, 0);
    gui_img_set_mode(img_clock, IMG_SRC_OVER_MODE);
    {
        int text_w = 26;
        gui_img_t *img = gui_img_create_from_mem(win, "circle_hour_decimal", text_num_array[0], 185,
                                                 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
        img = gui_img_create_from_mem(win, "circle_hour_single", text_num_array[0], 185 + text_w, 67,
                                      0, 0);
        gui_img_scale(img, 0.7, 0.7);
        img = gui_img_create_from_mem(win, "colon", text_num_array[10], 185 + text_w * 2 + 5, 67 + 5,
                                      0, 0);
        gui_img_scale(img, 0.7, 0.7);
        img = gui_img_create_from_mem(win, "circle_minute_decimal", text_num_array[0],
                                      185 + text_w * 2 + 17, 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
        img = gui_img_create_from_mem(win, "circle_minute_single", text_num_array[0],
                                      185 + text_w * 3 + 17, 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
    }
    gui_text_t *date_text = gui_text_create(win, "timecard_date_2",  0, 42, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_content),
                 32);
    gui_text_type_set(date_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, MULTI_LEFT);

    // Card list
    int space = 10;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CARD);
    gui_list_set_note_num(list, 5);
    gui_list_set_offset(list, 215);

    draw_timecard(parent);

    time_update_cb();
}
