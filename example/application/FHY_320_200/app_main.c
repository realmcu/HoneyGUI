/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <time.h>
#include "root_image/ui_resource.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "guidef.h"
#include "kb_algo.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "app_main.h"
#include "common_data.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
#ifdef _HONEYGUI_SIMULATOR_
unsigned char *resource_root = NULL;
#endif

static uint8_t tp_click_cnt = 0;
static uint8_t kb_click_cnt = 0;
static uint32_t last_click_time = 0;
static uint16_t sleep_cnt = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void button_2_view(uint8_t index)
{
    switch (index)
    {
    case 0:
        {
            gui_log("Wake up/dim off screen \n");
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), LOCK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    case 1:
        {
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), QUICK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    case 2:
        {
            gui_log("Shut down/enable screen \n");
        }
        break;
    case 3:
        {
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), FLASHLIGHT_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    case 4:
        {
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), TIMER_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    default:
        break;
    }
}

static void timer_touch_cb(void *param)
{
    GUI_UNUSED(param);
    // kb_info_t *kb = kb_get_info();
    // touch_info_t *tp = tp_get_info();

    // if (tp_click_cnt != 0 && (gui_ms_get() - last_click_time) > 200)
    // {
    //     switch (tp_click_cnt)
    //     {
    //     case 1 :
    //         gui_log("tp click\n");
    //         break;
    //     case 2 :
    //         gui_log("tp double click\n");
    //         break;
    //     case 3 :
    //         gui_log("tp triple click\n");
    //         break;
    //     default:
    //         break;
    //     }
    //     tp_click_cnt = 0;
    //     return;
    // }
    // else if (kb_click_cnt != 0 && (gui_ms_get() - last_click_time) > 450)
    // {
    //     switch (kb_click_cnt)
    //     {
    //     case 1 :
    //         button_2_view(case_button_customize_index_array[0]);
    //         break;
    //     case 2 :
    //         button_2_view(case_button_customize_index_array[1]);
    //         break;
    //     default:
    //         break;
    //     }
    //     kb_click_cnt = 0;
    //     return;
    // }

    // if (tp->pressed || tp->pressing || kb->pressed || kb->released)
    // {
    //     sleep_cnt = 0;
    // }
}

// Update the watch time
static void time_update_cb(void *param)
{
    GUI_UNUSED(param);

    sleep_cnt++;
    if (f_status.timer && timer_val != 0)
    {
        timer_val--;
        sprintf(timer_str, "%02d:%02d", timer_val / 60, timer_val % 60);
    }

    if (f_status.call == CALL_ING)
    {
        calltime_val++;
        sprintf(calltime_str, "%02d:%02d", calltime_val / 60, calltime_val % 60);
    }

#if defined _HONEYGUI_SIMULATOR_
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#else
    tx_time.tm_min++;
    if (tx_time.tm_min == 60)
    {
        tx_time.tm_min = 0;
        tx_time.tm_hour++;
        if (tx_time.tm_hour == 24)
        {
            tx_time.tm_hour = 0;
        }
    }
#endif

    if (f_status.auto_dim_off_screen && sleep_cnt >= auto_dim_time_val)
    {
        sleep_cnt = 0;
        if (f_status.timer)
        {
            extern const char *timer_descriptor_rec;
            timer_descriptor_rec = LOCK_VIEW;
            return;
        }
        gui_view_t *current_view = gui_view_get_current();
        if (!strcmp(current_view->descriptor->name, CHARGING_VIEW)) { return; }
        gui_view_set_animate_step(current_view, 400);
        gui_view_switch_direct(gui_view_get_current(), LOCK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
}

static void touch_long_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    button_2_view(case_button_customize_index_array[2]);
}

static void tp_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_touch_port_data_t *touch_data = touchpad_get_data();
    if (tp_click_cnt == 0)
    {
        tp_click_cnt++;
        last_click_time = touch_data->timestamp_ms;
    }
    else if (tp_click_cnt < 3 && (touch_data->timestamp_ms - last_click_time) < 150)
    {
        tp_click_cnt++;
        last_click_time = touch_data->timestamp_ms;
    }
}

static void kb_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_touch_port_data_t *touch_data = touchpad_get_data();
    if (kb_click_cnt == 0)
    {
        kb_click_cnt++;
        last_click_time = touch_data->timestamp_ms;
    }
    else if (kb_click_cnt < 2 && (touch_data->timestamp_ms - last_click_time) < 400)
    {
        kb_click_cnt++;
    }
}

extern const unsigned char _binary_root_0x00950000_bin_start[];
extern const unsigned char _binary_root_0x00950000_bin_end[];
extern const unsigned char _binary_root_0x00950000_bin_size[];

static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = (unsigned char *)_binary_root_0x00950000_bin_start;
#endif
    theme_bg_white = false;
    detail_page_design_func = page_dark_light_design;
    timeinfo = &tx_time;

    f_status.bt = BT_CONNECT;
    f_status.ble = 1;
    f_status.music_input = 1;
    f_status.notification_new = 1;
    f_status.headband_connect = 1;

    info_center_func_cnt = 2;
    f_status.infor_center_func_0 = 1;
    f_status.infor_center_func_1 = 1;
    f_status.infor_center_func_2 = 0;
    f_status.infor_center_func_3 = 0;
    f_status.infor_center_func_4 = 0;
    f_status.infor_center_func_5 = 0;

    // f_status.call = CALL_ING;
    // f_status.timer = 1;
    // f_status.tx_charging = 1;
    // f_status.headband_charging = 1;
    // f_status.ota = OTA_DOING;

    quick_page_name[0] = page_name_array[language_index][0];
    quick_page_name[1] = page_name_array[language_index][1];
    quick_page_name[2] = page_name_array[language_index][2];
    quick_page_name[3] = page_name_array[language_index][3];
    detail_page_design_func = page_information_center_customize_design;

    gui_win_t *win_touch = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    // gui_obj_add_event_cb(win_touch, touch_long_cb, GUI_EVENT_TOUCH_LONG, NULL);
    // gui_obj_add_event_cb(win_touch, tp_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_touch, kb_click_cb, GUI_EVENT_KB_SHORT_PRESSED, NULL);
    gui_obj_add_event_cb(win_touch, touch_long_cb, GUI_EVENT_KB_LONG_PRESSED, NULL);
    gui_obj_create_timer(GUI_BASE(win_touch), 10, true, timer_touch_cb);

    gui_win_t *win_view = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win_view), 1000, true, time_update_cb);
    gui_view_create(win_view, QUICK_VIEW, 0, 0, 0, 0);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);

