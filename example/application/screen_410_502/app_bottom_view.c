#include "root_image_hongkong/ui_resource.h"
#include "app_hongkong.h"
#include "gui_canvas.h"
#include "gui_text.h"
#include <time.h>
#include "gui_win.h"
#include "gui_canvas_img.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "cJSON.h"
#include "gui_list.h"
#include "tp_algo.h"


#define __WIN_NAME    "win_timecard"
#define __IMG_NAME    "timecard"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define CURRENT_VIEW_NAME "app_bottom_view"
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *fruit_ninja_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *box2d_ring_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void bottom_view_design(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = bottom_view_design,
};

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
static char time_content[10] = "00:00", date_content[20] = "January0\nSUN";
static char time_timecard_content[10] =  "00:00", date_timecard_content[10] = "SUN 0";
static gui_text_t *timecard_date_text, *timecard_time_text, *date_text;

static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;

static gui_list_t *list;

static uint8_t *img_data_activity;
static char *move_content, *ex_content, *stand_content;
static uint8_t *img_data_bg = NULL;

static size_t buffer_size = 0;

void clear_down_view(void)
{
    if (img_data_bg)
    {
        gui_lower_free(img_data_bg);
        img_data_bg = NULL;
    }
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
    }
}

static void display_time(void)
{
    if (!timeinfo)
    {
        return;
    }
    else
    {
        sprintf(time_timecard_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        gui_text_content_set(timecard_time_text, time_timecard_content, strlen(time_timecard_content));

        sprintf(date_timecard_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
        gui_text_content_set(timecard_date_text, date_timecard_content, strlen(date_timecard_content));

        sprintf(date_content, "%s%d\n%s", month[timeinfo->tm_mon], timeinfo->tm_mday,
                day[timeinfo->tm_wday]);
        gui_text_content_set(date_text, date_content, strlen(date_content));
        // gui_text_convert_to_img(date_text, RGB565);

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
    // gui_win_t *win = gui_win_create(parent, __WIN_NAME, 0, 0, SCREEN_WIDTH, 60);
    gui_canvas_round_rect_t *canvas_timecard = gui_canvas_round_rect_create(GUI_BASE(parent),
                                                                            __WIN_NAME,
                                                                            35, 0, 340, 60, 20, gui_rgba(39, 43, 44, 255 * 0.7));

    // text
    timecard_date_text = gui_text_create(canvas_timecard, "timecard_date_1",  15, 20, 0, 0);
    gui_text_set(timecard_date_text, (void *)date_timecard_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_timecard_content),
                 32);
    gui_text_type_set(timecard_date_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_date_text, LEFT);

    timecard_time_text = gui_text_create(canvas_timecard, "timecard_time",  -10, 20, 0,
                                         0);
    gui_text_set(timecard_time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 32);
    gui_text_type_set(timecard_time_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_time_text, RIGHT);
    GUI_BASE(canvas_timecard)->not_show = 1;
}

static void timer_cb(void *obj)
{
    static uint16_t count = 0;
    count++;
    if (count >= 300)
    {
        display_time();
        count = 0;
    }

    // control timecard display
    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_timecard, __IMG_NAME, current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(canvas_timecard, __WIN_NAME, current_view);
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
    gui_log("enter switch_app_cb\n");
    extern void close_box2d_ring(void);
    close_box2d_ring();
    extern void clear_activity(void);
    clear_activity();

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
    else if (strcmp(obj_name, "appview") == 0)
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
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * move->valueint / 20000.0f), NVG_CW);  // cap 20000 steps
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(30, 55, 25));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(22, 50, 47));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * stand->valueint / 30.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);

            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(move_text, "ac_move", current_view);
                sprintf(move_content, "Move: %d/20000 steps", move->valueint);
                gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
                // gui_text_convert_to_img((gui_text_t *)move_text, RGB565);
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(ex_text, "ac_ex", current_view);
                sprintf(ex_content, "Exercise: %d/60 min", ex->valueint);
                gui_text_content_set((gui_text_t *)ex_text, ex_content, strlen(ex_content));
                // gui_text_convert_to_img(ex_text, RGB565);
            }
            {
                GUI_WIDGET_POINTER_BY_NAME_ROOT(stand_text, "ac_stand", current_view);
                sprintf(stand_content, "Stand: %d/30 times", stand->valueint);
                gui_text_content_set((gui_text_t *)stand_text, stand_content, strlen(stand_content));
                // gui_text_convert_to_img(stand_text, RGB565);
            }
        }
    }
    // clear
    cJSON_Delete(root);
}

static void activity_timer_cb(void *obj)
{
    gui_img_t *img = (gui_img_t *)obj;
    uint8_t *img_data = (void *)gui_img_get_image_data(img);
    memset(img_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb, img_data);
    gui_img_set_image_data(img, img_data);
}

void bottom_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(parent, "background", 0, 0, SCREEN_WIDTH,
                                                          SCREEN_HEIGHT, gui_rgb(0, 0,
                                                                                 0));
    gui_obj_create_timer(GUI_BASE(canvas_bg), 100, true, timer_cb);
    // gui_obj_start_timer(GUI_BASE(canvas_bg));
    //clock circle
    gui_img_t *img_clock = gui_img_create_from_mem(parent, __IMG_NAME, UI_CARD_CLOCKCIRCLE_BIN,
                                                   38, 30,
                                                   0, 0);
    gui_img_set_mode(img_clock, IMG_SRC_OVER_MODE);
    {
        int text_w = 26;
        gui_img_t *img = gui_img_create_from_mem(img_clock, "circle_hour_decimal", text_num_array[0], 185,
                                                 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(img_clock, "circle_hour_single", text_num_array[0], 185 + text_w, 67,
                                      0, 0);
        gui_img_scale(img, 0.7, 0.7);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(img_clock, "colon", text_num_array[10], 185 + text_w * 2 + 5, 67 + 5,
                                      0, 0);
        gui_img_scale(img, 0.7, 0.7);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(img_clock, "circle_minute_decimal", text_num_array[0],
                                      185 + text_w * 2 + 17, 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(img_clock, "circle_minute_single", text_num_array[0],
                                      185 + text_w * 3 + 17, 67, 0, 0);
        gui_img_scale(img, 0.7, 0.7);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    date_text = gui_text_create(img_clock, "timecard_date_2",  0, 42, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_content),
                 32);
    gui_text_type_set(date_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, MULTI_LEFT);

    // gui_text_convert_to_img(date_text, RGB565);

    int space = 10;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL);
    gui_list_set_style(list, LIST_CARD);
    gui_list_set_offset(list, 215);

    draw_timecard(parent);

    int16_t offset_X = 29;
    gui_list_note_t *note_calendar = gui_list_add_note(list, false);
    gui_list_note_t *note_weather = gui_list_add_note(list, false);
    gui_list_note_t *note_activity = gui_list_add_note(list, false);
    gui_list_note_t *note_app = gui_list_add_note(list, false);
    gui_list_note_t *note_appview = gui_list_add_note(list, false);

    gui_img_t *img = gui_img_create_from_mem(note_calendar, "calendar", UI_CARD_CALENDAR_BIN, offset_X,
                                             0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    img = gui_img_create_from_mem(note_weather, "weather", UI_CARD_WEATHER_BIN, offset_X, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);

    {
        int image_h = 157;
        int image_w = 352;
        int pixel_bytes = 4;
        size_t bg_buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_bg)
        {
            img_data_bg = gui_lower_malloc(bg_buffer_size);
        }
        memset(img_data_bg, 0, bg_buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, img_bg_cb,
                                          img_data_bg);
    }

    // note_activity
    {
        gui_img_t *canvas = gui_img_create_from_mem(note_activity, "note_ac", (void *)img_data_bg, offset_X,
                                                    0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(canvas, "ac_bg", UI_BG_ICON_BIN, 22, 33, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_scale(img, 0.9f, 0.9f);

        if (move_content == NULL)
        {
            move_content = (char *)gui_lower_malloc(30);
            ex_content = (char *)gui_lower_malloc(30);
            stand_content = (char *)gui_lower_malloc(30);
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
            buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
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


    // note_app
    {
        gui_img_t *canvas_app = gui_img_create_from_mem(note_app, "note_ac", (void *)img_data_bg, offset_X,
                                                        0, 0, 0);
        img = gui_img_create_from_mem(canvas_app, "RING", UI_CLOCK_BOX2D_RING_ICON_BIN, 17, 28, 0, 0);
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

    img = gui_img_create_from_mem(note_appview, "appview", UI_CARD_APPVIEW_BIN, 57, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    display_time();
}
