#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_canvas.h"

// static void cardlist_clock(void *parent)
// {
//     gui_img_create_from_mem(parent, "page0", CARD_HEALTH_BIN, 14, 0, 0, 0);
// }
// static void cardlist_activity(void *parent)
// {
//     gui_img_create_from_mem(parent, "page1", CARD_MORE_BIN, 14, 0, 0, 0);
// }
// static void cardlist_heart(void *parent)
// {
//     gui_img_create_from_mem(parent, "page2", CARD_MUSIC_BIN, 14, 0, 0, 0);
// }
// static void cardlist_blood(void *parent)
// {
//     gui_img_create_from_mem(parent, "page3", CARD_MUSIC1_BIN, 14, 0, 0, 0);
// }
// static void cardlist_weather(void *parent)
// {
//     gui_img_create_from_mem(parent, "page4", CARD_WEATHER1_BIN, 14, 0, 0, 0);
// }
// static void cardlist_music(void *parent)
// {
//     gui_img_create_from_mem(parent, "page5", CARD_MORE_BIN, 14, 0, 0, 0);
// }


// static void canvas_cb_black(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 224);
//     nvgFillColor(canvas->vg, nvgRGBA(0, 0, 128, 255));
//     nvgFill(canvas->vg);
// }

// static gui_cardview_t *cv;

// static void cv_status_cb(gui_cardview_t *this)
// {
//     //gui_log("cv release value = %d \n", this->release_y);
// }

// void curtain_down_design(void *parent_widget)
// {
//     gui_curtain_t *ct_card = parent_widget;
//     // gui_curtainview_t *curtainview = (gui_curtainview_t *)ct_card->base.parent;

//     gui_canvas_t *canvas = gui_canvas_create(parent_widget, "canvas", 0, 0, 0, 368, 448);
//     gui_canvas_set_canvas_cb(canvas, canvas_cb_black);

//     cv = gui_cardview_create(parent_widget, "cardview", 0, 0, 0, 0);

//     gui_cardview_status_cb(cv, cv_status_cb);



//     gui_card_t *tb_music = gui_card_create(cv, "tb_music",         0, 0, 0, 124);
//     gui_card_t *tb_weather = gui_card_create(cv, "tb_weather",     0, 0, 0, 124);
//     gui_card_t *tb_blood = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124);
//     gui_card_t *tb_heart = gui_card_create(cv, "tb_heart",         0, 0, 0, 124);
//     gui_card_t *tb_activity = gui_card_create(cv, "tb_activity",   0, 0, 0, 124);
//     gui_card_t *tb_clock = gui_card_create(cv, "tb_clock",         0, 0, 0, 124);

//     cardlist_clock(tb_clock);
//     cardlist_activity(tb_activity);
//     cardlist_heart(tb_heart);
//     cardlist_blood(tb_blood);
//     cardlist_weather(tb_weather);
//     cardlist_music(tb_music);

//     gui_card_t *tb_music_test = gui_card_create(cv, "tb_music",         0, 0, 0, 124);
//     gui_card_t *tb_weather_test = gui_card_create(cv, "tb_weather",     0, 0, 0, 124);
//     gui_card_t *tb_blood_test = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124);
//     gui_card_t *tb_heart_test = gui_card_create(cv, "tb_heart",         0, 0, 0, 124);
//     gui_card_t *tb_activity_test = gui_card_create(cv, "tb_activity",   0, 0, 0, 124);
//     gui_card_t *tb_clock_test = gui_card_create(cv, "tb_clock",         0, 0, 0, 124);

//     // cardlist_clock(tb_clock_test);
//     // cardlist_activity(tb_activity_test);
//     // cardlist_heart(tb_heart_test);
//     // cardlist_blood(tb_blood_test);
//     // cardlist_weather(tb_weather_test);
//     // cardlist_music(tb_music_test);
// }


/* Revised part */
#include "gui_text.h"
#include "gui_page.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include "gui_win.h"
#define __CANVAS_NAME "canvas_timecard"
#define __IMG_NAME    "timecard"
#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
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

extern struct tm *timeinfo;
static char time_content[10] = "00:00", date_content[20] = "October15\nThur";
static char time_timecard_content[10] =  "00:00", date_timecard_content[10] = "Thur 15";
static gui_text_t *timecard_date_text, *timecard_time_text, *date_text, *time_text;

static void *font_size_96_bin_addr = SOURCEHANSANSSC_SIZE96_BITS4_FONT_BIN;
static void *font_size_64_bin_addr = SOURCEHANSANSSC_SIZE64_BITS4_FONT_BIN;
static void *font_size_48_bin_addr = SOURCEHANSANSSC_SIZE48_BITS4_FONT_BIN;
static void *font_size_40_bin_addr = SOURCEHANSANSSC_SIZE40_BITS4_FONT_BIN;
static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;

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

        sprintf(time_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        gui_text_content_set(time_text, time_content, strlen(time_content));
    }
}

static void canvas_cb_draw_bg(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 368, 448, 50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 255 * 0.6)); //(255, 255, 255, 255 * 0.34)
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void canvas_cb_draw_timecard(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 340, 60, 20); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255 * 0.7));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}
// draw timecard and not display
static void draw_timecard(void *parent_widget)
{
    gui_canvas_t *canvas_timecard = gui_canvas_create(parent_widget, __CANVAS_NAME, 0, 14, 0, 368, 60);
    gui_canvas_set_canvas_cb(canvas_timecard, canvas_cb_draw_timecard);

    // text
    timecard_date_text = gui_text_create(canvas_timecard, "timecard_date_1",  15, 20, 0, 0);
    gui_text_set(timecard_date_text, (void *)date_timecard_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_timecard_content),
                 32);
    gui_text_type_set(timecard_date_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_date_text, LEFT);

    timecard_time_text = gui_text_create(canvas_timecard, "timecard_time",  -50, 20, 0, 0);
    gui_text_set(timecard_time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 32);
    gui_text_type_set(timecard_time_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_time_text, RIGHT);
    GUI_BASE(canvas_timecard)->not_show = 0;
}

static void cv_cb(gui_cardview_t *cv)
{
    if (cv->animate->Beginning_frame)
    {
        display_time();
    }
    gui_canvas_t *canvas = 0;
    gui_img_t *img = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), __CANVAS_NAME, (void *)&canvas);
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), __IMG_NAME, (void *)&img);
    int offset = cv->hold_y;
    if (offset + CARD_HEIGHT + 50 < 0)
    {
        GUI_BASE(canvas)->not_show = 0;
        GUI_BASE(img)->not_show = 1;
        // gui_img_set_attribute(img, "timecard", CARD_MUSIC_BIN, GUI_BASE(img)->x, GUI_BASE(img)->y);
        // gui_img_set_opacity(img, (timecard_height + offset) / timecard_height * 255);
    }
    else
    {
        GUI_BASE(canvas)->not_show = 1;
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
    // draw transparently black background
    gui_canvas_t *canvas_bg = gui_canvas_create(parent_widget, "background", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas_bg, canvas_cb_draw_bg);

    //clock circle
    gui_img_t *img = gui_img_create_from_mem(parent_widget, __IMG_NAME, UI_CARD_CLOCKCIRCLE_BIN, 17, 30,
                                             0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);

    // text
    date_text = gui_text_create(img, "timecard_date_2",  0, 42, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_content),
                 40);
    gui_text_type_set(date_text, font_size_40_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, MULTI_LEFT);

    time_text = gui_text_create(img, "time_text",  -40, 50, 0, 0);
    gui_text_set(time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 64);
    gui_text_type_set(time_text, font_size_64_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_text, RIGHT);


    gui_cardview_t *cv = gui_cardview_create(parent_widget, "cardview", 0, 0, 0, 448);
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

    gui_img_create_from_mem(cv_calendar, "calendar", UI_CARD_CALENDAR_BIN, 8, 0, 0, 0);
    img = gui_img_create_from_mem(cv_weather, "weather", UI_CARD_WEATHER_BIN, 8, 0, 0, 0);
    // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    img = gui_img_create_from_mem(cv_activity, "activity", UI_CARD_ACTIVITY_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    img = gui_img_create_from_mem(cv_app, "app", UI_CARD_APP_BIN, 8, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    img = gui_img_create_from_mem(cv_appview, "appview", UI_CARD_APPVIEW_BIN, 38, 81, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
}
