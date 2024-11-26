#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_canvas.h"
#include "gui_text.h"
#include "gui_page.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include "gui_win.h"
#include "gui_canvas_img.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"

#define __WIN_NAME "win_timecard"
#define __IMG_NAME    "timecard"
#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define SCREEN_X_OFF 21
#define SCREEN_Y_OFF 18
#define CARD_HEIGHT 157 + 10

char *month[12] =
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
char *day[7] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

extern void *text_num_array[];
extern struct tm *timeinfo;
static char time_content[10] = "00:00", date_content[20] = "January0\nSUN";
static char time_timecard_content[10] =  "00:00", date_timecard_content[10] = "SUN 0";
static gui_text_t *timecard_date_text, *timecard_time_text, *date_text;

static void *font_size_48_bin_addr = SOURCEHANSANSSC_SIZE48_BITS1_FONT_BIN;
static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;

static void display_time()
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
        gui_text_convert_to_img(date_text, RGB565);

        GUI_WIDGET_POINTER_BY_NAME(img_hour_decimal, "circle_hour_decimal");
        gui_img_set_attribute((gui_img_t *)img_hour_decimal, img_hour_decimal->name,
                              text_num_array[timeinfo->tm_hour / 10], img_hour_decimal->x, img_hour_decimal->y);
        GUI_WIDGET_POINTER_BY_NAME(img_hour_single, "circle_hour_single");
        gui_img_set_attribute((gui_img_t *)img_hour_single, img_hour_single->name,
                              text_num_array[timeinfo->tm_hour % 10], img_hour_single->x, img_hour_single->y);

        GUI_WIDGET_POINTER_BY_NAME(img_minute_decimal, "circle_minute_decimal");
        gui_img_set_attribute((gui_img_t *)img_minute_decimal, img_minute_decimal->name,
                              text_num_array[timeinfo->tm_min / 10], img_minute_decimal->x, img_minute_decimal->y);
        GUI_WIDGET_POINTER_BY_NAME(img_minute_single, "circle_minute_single");
        gui_img_set_attribute((gui_img_t *)img_minute_single, img_minute_single->name,
                              text_num_array[timeinfo->tm_min % 10], img_minute_single->x, img_minute_single->y);

    }
}

// draw timecard and not display
static void draw_timecard(void *parent_widget)
{
    gui_win_t *win = gui_win_create(parent_widget, __WIN_NAME, 0, 0, SCREEN_WIDTH, 60);
    gui_canvas_round_rect_t *canvas_timecard = gui_canvas_round_rect_create(GUI_BASE(win), "",
                                                                            14 + SCREEN_X_OFF, 0, 340, 60, 20, gui_rgba(39, 43, 44, 255 * 0.7));

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
    GUI_BASE(win)->not_show = 0;
}

static void cv_cb(gui_cardview_t *cv)
{
    if (cv->animate->Beginning_frame)
    {
        display_time();
    }
    gui_win_t *win = 0;
    gui_img_t *img = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), __WIN_NAME, (void *)&win);
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), __IMG_NAME, (void *)&img);
    int offset = cv->hold_y;
    if (offset + CARD_HEIGHT + 50 < 0)
    {
        GUI_BASE(win)->not_show = 0;
        GUI_BASE(img)->not_show = 1;
    }
    else
    {
        GUI_BASE(win)->not_show = 1;
        GUI_BASE(img)->not_show = 0;
    }
    gui_obj_t *obj = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "appview", (void *)&obj);
    if (offset <= -500 && offset >= -581)
    {
        obj->y = 81 + offset + 500;
    }
    else if (offset < -581)
    {
        obj->y = 0;
    }
}

void curtain_down_design(void *parent_widget)
{
    // draw background
    gui_canvas_rect_create(parent_widget, "background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(0, 0,
                           0));

    //clock circle
    gui_img_t *img_clock = gui_img_create_from_mem(parent_widget, __IMG_NAME, UI_CARD_CLOCKCIRCLE_BIN,
                                                   17 + SCREEN_X_OFF, 30,
                                                   0, 0);
    gui_img_set_mode(img_clock, IMG_SRC_OVER_MODE);

    // text
    // time_text = gui_text_create(img_clock, "time_text",  -40, 50, 0, 0);
    // gui_text_set(time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
    //              strlen(time_content),
    //              64);
    // gui_text_type_set(time_text, SOURCEHANSANSSC_SIZE64_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
    // gui_text_mode_set(time_text, RIGHT);
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
    gui_text_convert_to_img(date_text, RGB565);

    gui_cardview_t *cv = gui_cardview_create(parent_widget, "cardview", SCREEN_X_OFF, 0,
                                             SCREEN_WIDTH - SCREEN_X_OFF, SCREEN_HEIGHT);
    draw_timecard(parent_widget);

    gui_cardview_set_style(cv, REDUCTION);
    // change timecard img and canvas
    gui_cardview_set_animate(cv, 2000, -1, cv_cb, cv);
    // gui_cardview_set_bottom_space(cv, 30);

    gui_card_t *cv_no_dispaly = gui_card_create(cv, "cv_no_dispaly", 0, 0, 0,
                                                250); // adjust card position
    gui_card_t *cv_calendar = gui_card_create(cv, "cv_valendar",   0, 0, 0, CARD_HEIGHT);
    gui_card_t *cv_weather = gui_card_create(cv, "cv_weather",     0, 0, 0, CARD_HEIGHT);
    gui_card_t *cv_activity = gui_card_create(cv, "cv_activity",   0, 0, 0, CARD_HEIGHT);
    gui_card_t *cv_app = gui_card_create(cv, "cv_app",             0, 0, 0, CARD_HEIGHT);
    gui_card_t *cv_appview = gui_card_create(cv, "cv_appview",     0, 0, 0, CARD_HEIGHT);

    gui_img_t *img = gui_img_create_from_mem(cv_calendar, "calendar", UI_CARD_CALENDAR_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    img = gui_img_create_from_mem(cv_weather, "weather", UI_CARD_WEATHER_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    img = gui_img_create_from_mem(cv_activity, "activity", UI_CARD_ACTIVITY_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    img = gui_img_create_from_mem(cv_app, "app", UI_CARD_APP_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    img = gui_img_create_from_mem(cv_appview, "appview", UI_CARD_APPVIEW_BIN, 38, 81, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
}
