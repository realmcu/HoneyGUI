/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include <math.h>
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_soccer.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "soccer_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void soccer_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = soccer_app,
};

/* Soccer Management */
static uint32_t *gui_soccer_array[20];
// {
//     SOCCER_P0001_CALL_BIN,
//     SOCCER_P0002_SPORTS_BIN,
//     SOCCER_P0003_HEARTRATE_BIN,
//     SOCCER_P0004_SLEEP_BIN,
//     SOCCER_P0005_WEATHER_BIN,
//     SOCCER_P0006_ACTIVITIES_BIN,
//     SOCCER_P0007_STRESS_BIN,
//     SOCCER_P0008_SPO2_BIN,
//     SOCCER_P0009_MUSIC_BIN,
//     SOCCER_P0010_VOICE_BIN,
//     SOCCER_P0011_NOTIFICATION_BIN,
//     SOCCER_P0012_SETTINGS_BIN,
//     SOCCER_P0013_SPORT_CORECD_BIN,
//     SOCCER_P0014_MEASURE_BIN,
//     SOCCER_P0015_MOOD_BIN,
//     SOCCER_P0016_BREATHE_BIN,
//     SOCCER_P0017_ALARM_BIN,
//     SOCCER_P0018_PERIOD_BIN,
//     SOCCER_P0019_HOME_BIN,
//     SOCCER_P0020_MORE_BIN,
// };

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

void click_button_back_2_watchface_or_menu(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    if (strcmp(e->indev_name, "Menu") == 0 || strcmp(e->indev_name, "Back") == 0)
    {
        gui_view_switch_direct(obj, "menu_view", SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(e->indev_name, "Home") == 0)
    {
        gui_view_switch_direct(obj, "watchface_view", SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}

void slide_back_2_menu(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    gui_view_switch_direct(obj, "menu_view", SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void soccer_app(gui_view_t *view)
{
    gui_view_t *view_c = gui_view_get_current();
    if (view_c && view_c->descriptor == gui_view_descriptor_get("menu_view"))
    {
        gui_obj_add_event_cb(view, click_button_back_2_watchface_or_menu, GUI_EVENT_KB_SHORT_PRESSED, NULL);
        gui_obj_add_event_cb(view, slide_back_2_menu, GUI_EVENT_TOUCH_RIGHT_SLIDE_QUICK, NULL);
        gui_obj_add_event_cb(view, slide_back_2_menu, GUI_EVENT_TOUCH_LEFT_SLIDE_QUICK, NULL);
        gui_obj_focus_set(view);
    }

    if (gui_soccer_array[0] == NULL)
    {
        uint32_t *soccer_array[20] =
        {
            SOCCER_P0001_CALL_BIN,
            SOCCER_P0002_SPORTS_BIN,
            SOCCER_P0003_HEARTRATE_BIN,
            SOCCER_P0004_SLEEP_BIN,
            SOCCER_P0005_WEATHER_BIN,
            SOCCER_P0006_ACTIVITIES_BIN,
            SOCCER_P0007_STRESS_BIN,
            SOCCER_P0008_SPO2_BIN,
            SOCCER_P0009_MUSIC_BIN,
            SOCCER_P0010_VOICE_BIN,
            SOCCER_P0011_NOTIFICATION_BIN,
            SOCCER_P0012_SETTINGS_BIN,
            SOCCER_P0013_SPORT_CORECD_BIN,
            SOCCER_P0014_MEASURE_BIN,
            SOCCER_P0015_MOOD_BIN,
            SOCCER_P0016_BREATHE_BIN,
            SOCCER_P0017_ALARM_BIN,
            SOCCER_P0018_PERIOD_BIN,
            SOCCER_P0019_HOME_BIN,
            SOCCER_P0020_MORE_BIN,
        };
        memcpy(gui_soccer_array, soccer_array, sizeof(soccer_array));
    }

    gui_obj_t *obj = GUI_BASE(view);
    gui_dispdev_t *dc = gui_get_dc();

    gui_soccer_t *soccer = gui_soccer_create(obj, "soccer", gui_soccer_array, 0, 0);
    gui_soccer_set_center(soccer, dc->screen_width / 2, dc->screen_height / 2);


}