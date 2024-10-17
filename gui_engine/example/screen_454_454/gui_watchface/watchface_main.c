#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_curtainview.h"
#include <time.h>
#include <stdio.h>
#include "gui_win.h"
#include "gui_tab.h"
#include "gui_obj.h"
#include "watchface_mgr.h"
#include "gui_interface.h"
#ifndef _WIN32
#include "hub_clock.h"
#include "module_global_data.h"
#endif

#define MY_PI   (180)

void *gui_watchface_turn_around_timer = NULL;

static gui_curtainview_t *curtainview_menu = NULL;
static gui_img_t *watchface_hour;
static gui_img_t *watchface_minute;
static gui_img_t *watchface_second;
static float angle_hour;
static float angle_min;
static float angle_sec;

static void img_animate_watchface_callback(void *obj)
{
    extern bool gui_send_msg_to_server(gui_msg_t *msg);
#ifndef _WIN32
    uint16_t seconds = get_system_clock_second();
    uint16_t minute = RtkWristbandSys.Global_Time.minutes;
    uint16_t hour = RtkWristbandSys.Global_Time.hour;
    int millisecond = 0;
#else
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    uint16_t seconds = timeinfo->tm_sec;
    uint16_t minute = timeinfo->tm_min;
    uint16_t hour = timeinfo->tm_hour;
    int millisecond = 0;
    struct timespec spec;
    const float angle_per_second = 360.0f / 60.0f;
    clock_gettime(CLOCK_REALTIME, &spec);
    millisecond = spec.tv_nsec / 1000000;
#endif
    angle_hour = (hour % 12) * MY_PI / 6 + minute * MY_PI / 360;
    angle_min  = minute * MY_PI / 30 + seconds * MY_PI / 1800;
    angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
    gui_img_translate(watchface_hour, watchface_hour->base.w / 2, watchface_hour->base.h);
    gui_img_translate(watchface_minute, watchface_minute->base.w / 2, watchface_minute->base.h);
    gui_img_translate(watchface_second, watchface_second->base.w / 2, watchface_second->base.h);

    gui_img_rotation(watchface_hour, angle_hour,  watchface_hour->base.w / 2,
                     watchface_hour->base.h);
    gui_img_rotation(watchface_minute, angle_min, watchface_minute->base.w / 2,
                     watchface_minute->base.h);
    gui_img_rotation(watchface_second, angle_sec, watchface_second->base.w / 2,
                     watchface_second->base.h);

    gui_msg_t msg;
    msg.event = GUI_EVENT_WATCHFACE_UPDATE;
    gui_send_msg_to_server(&msg);
}

static void tab_watchface_update_cb(void *obj, uint16_t event)
{
    extern gui_tab_t *tab_watchface;
    switch (event)
    {
    case GUI_EVENT_WATCHFACE_UPDATE:
        gui_tab_update_preload((gui_obj_t *)tab_watchface);
        break;
    default:
        break;
    }
}

static void win_switch_to_perspective_app_touch_cb(void *obj, uint16_t event)
{
    static uint32_t first_click = 0;
    static uint32_t second_click = 0;
    static bool is_first_click = 1;
    switch (event)
    {
    case GUI_EVENT_TOUCH_CLICKED:
        if (is_first_click)
        {
            first_click = gui_ms_get();
            is_first_click = 0;
        }
        else
        {
            second_click = gui_ms_get();
            is_first_click = 1;
        }

        if (second_click - first_click < DOUBLE_CLICK_INTERVAL && second_click != 0)
        {
            // app_watchface_switch_tabs(WATCHFACE_PERSPECTIVE);  //double click bug
            is_first_click = 1;
            first_click = 0;
            second_click = 0;
        }
        else if (second_click - first_click >= DOUBLE_CLICK_INTERVAL && second_click != 0)
        {
            is_first_click = 0;
            first_click = second_click;
            second_click = 0;
        }

        break;
    default:
        break;

    }
}

void design_tab_watchface_main(void *parent)
{
    gui_img_t *img_clock = gui_img_create_from_mem(parent, "img_clock", CLOCK_BASE_BIN, 0, 0, 454, 454);

    watchface_hour = gui_img_create_from_mem(parent, "hour", ICON_CLOCK_HOUR_HAND_BIN, 220, 129, 12,
                                             100);
    watchface_minute = gui_img_create_from_mem(parent, "minute", ICON_CLOCK_MINUTE_HAND_BIN, 220, 97,
                                               12,
                                               132);
    watchface_second = gui_img_create_from_mem(parent, "second", ICON_CLOCK_SECOND_HAND_BIN, 222, 81, 8,
                                               148);
    gui_img_t *watchface_hand = gui_img_create_from_mem(parent, "hand", ICON_CLOCK_HAND_DOT_BIN, 218,
                                                        218, 0, 0);
    gui_img_set_quality(watchface_hour, 1);
    gui_img_set_quality(watchface_second, 1);
    gui_img_set_quality(watchface_minute, 1);
    gui_img_set_quality(img_clock, 1);

    gui_img_set_animate(watchface_second, 1000, -1, img_animate_watchface_callback, watchface_second);
    gui_obj_add_event_cb(watchface_second, (gui_event_cb_t)tab_watchface_update_cb,
                         (gui_event_t)GUI_EVENT_WATCHFACE_UPDATE, NULL);

    curtainview_menu = gui_curtainview_create(parent, "curtainview_menu", 0, 0, 454, 454);
    void design_curtain_menu(void *parent);
    design_curtain_menu(curtainview_menu);

    gui_win_t *win_switch_to_perspective_app = gui_win_create(parent, "win_switch_to_perspective_app",
                                                              0, 0, 454, 454);
    gui_win_click(win_switch_to_perspective_app, (gui_event_cb_t)win_switch_to_perspective_app_touch_cb,
                  NULL);
}
