/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
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
/* VIEW */
const static gui_view_descriptor_t *test_view = NULL;
const static gui_view_descriptor_t *lock_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *quick_view = NULL;
const static gui_view_descriptor_t *flashlight_view = NULL;
const static gui_view_descriptor_t *timer_view = NULL;
const static gui_view_descriptor_t *charging_view = NULL;

/* FPS */
static char fps[10] __attribute__((unused));
static char widget_count_string[20] __attribute__((unused));
static char mem_string[20] __attribute__((unused));
static char low_mem_string[20] __attribute__((unused));

#ifdef _WIN32
unsigned char *resource_root = NULL;
#endif

static uint8_t tp_click_cnt = 0;
static uint8_t kb_click_cnt = 0;
static uint32_t last_click_time = 0;
static uint16_t sleep_cnt = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    test_view = gui_view_descriptor_get("quick_view");
    lock_view = gui_view_descriptor_get("lock_view");
    menu_view = gui_view_descriptor_get("menu_view");
    quick_view = gui_view_descriptor_get("quick_view");
    flashlight_view = gui_view_descriptor_get("flashlight_view");
    timer_view = gui_view_descriptor_get("timer_view");
    charging_view = gui_view_descriptor_get("charging_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

#ifdef __WIN32
static void gui_fps_cb(void *p)
{
    int fps_num = gui_fps();
    gui_obj_t *fps_rect = GUI_BASE(p);
    sprintf(fps, "FPS:%d", fps_num);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_fps, "t_fps", fps_rect);
    gui_text_content_set((gui_text_t *)t_fps, fps, strlen(fps));
    int widget_count_number = gui_get_obj_count();
    sprintf(widget_count_string, "WIDGETS:%d", widget_count_number);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(widget_count, "widget_count", fps_rect);
    gui_text_content_set((gui_text_t *)widget_count, widget_count_string, strlen(widget_count_string));
    uint32_t mem_number =  gui_mem_used();
    uint32_t low_mem_number =  gui_low_mem_used();
    sprintf(mem_string, "RAM:%dKB", (int)mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(mem, "mem", fps_rect);
    gui_text_content_set((gui_text_t *)mem, mem_string, strlen(mem_string));
    sprintf(low_mem_string, "lowRAM:%dKB", (int)low_mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(low_mem, "low_mem", fps_rect);
    gui_text_content_set((gui_text_t *)low_mem, low_mem_string, strlen(low_mem_string));
}

// Show the FPS, widget count, memory usage, and low memory usage
static void fps_create(void *parent)
{
    char *text;
    int font_size = 20;
    gui_obj_create_timer(GUI_BASE(parent), 10, true, gui_fps_cb);
    sprintf(fps, "FPS:%d", gui_fps());
    text = fps;
    gui_text_t *t_fps = gui_text_create(parent, "t_fps", 10, 0, gui_get_screen_width(), font_size);
    gui_text_set(t_fps, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(t_fps, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *widget_count = gui_text_create(parent, "widget_count", 10, 16, gui_get_screen_width(),
                                               font_size);
    gui_text_set(widget_count, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(widget_count, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *mem = gui_text_create(parent, "mem", 10, 16 * 2, gui_get_screen_width(), font_size);
    gui_text_set(mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(mem, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_t *low_mem = gui_text_create(parent, "low_mem", 10, 16 * 3, gui_get_screen_width(),
                                          font_size);
    gui_text_set(low_mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(low_mem, HEADING_1_BIN, FONT_SRC_MEMADDR);
}
#endif

static void button_2_view(uint8_t index)
{
    switch (index)
    {
    case 0:
        {
            gui_log("Wake up/dim off screen \n");
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), lock_view, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    case 1:
        {
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), quick_view, SWITCH_OUT_NONE_ANIMATION,
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
            gui_view_switch_direct(gui_view_get_current(), flashlight_view, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_NONE_ANIMATION);
        }
        break;
    case 4:
        {
            gui_view_set_animate_step(gui_view_get_current(), 400);
            gui_view_switch_direct(gui_view_get_current(), timer_view, SWITCH_OUT_NONE_ANIMATION,
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
    kb_info_t *kb = kb_get_info();
    touch_info_t *tp = tp_get_info();

    if (tp_click_cnt != 0 && (gui_ms_get() - last_click_time) > 200)
    {
        switch (tp_click_cnt)
        {
        case 1 :
            gui_log("tp click\n");
            break;
        case 2 :
            gui_log("tp double click\n");
            break;
        case 3 :
            gui_log("tp triple click\n");
            break;
        default:
            break;
        }
        tp_click_cnt = 0;
        return;
    }
    else if (kb_click_cnt != 0 && (gui_ms_get() - last_click_time) > 450)
    {
        switch (kb_click_cnt)
        {
        case 1 :
            button_2_view(case_button_customize_index_array[0]);
            break;
        case 2 :
            button_2_view(case_button_customize_index_array[1]);
            break;
        default:
            break;
        }
        kb_click_cnt = 0;
        return;
    }

    if (tp->pressed || tp->pressing || kb->pressed || kb->released)
    {
        sleep_cnt = 0;
    }
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

#if defined __WIN32
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
            extern const gui_view_descriptor_t *timer_descriptor_rec;
            timer_descriptor_rec = lock_view;
            return;
        }
        gui_view_t *current_view = gui_view_get_current();
        if (current_view->descriptor == charging_view) { return; }
        gui_view_set_animate_step(current_view, 400);
        gui_view_switch_direct(gui_view_get_current(), lock_view, SWITCH_OUT_NONE_ANIMATION,
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
#ifdef _WIN32
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
    gui_obj_add_event_cb(win_touch, kb_click_cb, GUI_EVENT_KB_SHORT_CLICKED, NULL);
    gui_obj_add_event_cb(win_touch, touch_long_cb, GUI_EVENT_KB_LONG_CLICKED, NULL);
    gui_obj_create_timer(GUI_BASE(win_touch), 10, true, timer_touch_cb);

    gui_win_t *win_view = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    // fps_create(gui_obj_get_root());
    gui_obj_create_timer(GUI_BASE(win_view), 1000, true, time_update_cb);
    gui_view_create(win_view, test_view, 0, 0, 0, 0);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);

