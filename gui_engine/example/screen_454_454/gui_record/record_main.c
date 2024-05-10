#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_obj.h"
#include "gui_switch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_curtainview.h"
#include "record_mgr.h"
#ifndef _WIN32
#include "app_mic_record.h"
#include "app_mmi.h"
#include "app_msg.h"
#include "app_task.h"
#include "hub_clock.h"
#endif

gui_text_t *text_record_title = NULL;
gui_text_t *text_record_tips = NULL;
gui_switch_t *switch_record_button = NULL;

char *string_record_time = "00: 00: 00";
uint8_t record_time_display[11];
uint32_t record_time_ms_init;
uint32_t record_time_ms;
uint8_t record_time_s;
uint8_t record_time_min;
uint8_t record_time_hour;
uint8_t record_status = 0;

static char *update_record_time(void)
{
    record_time_ms = gui_ms_get() - record_time_ms_init;
    record_time_s = record_time_ms / 1000;
    record_time_min = record_time_s / 60;
    record_time_hour = record_time_min / 60;
    record_time_display[0] = record_time_hour % 60 / 10 + 0x30;
    record_time_display[1] = record_time_hour % 60 % 10 + 0x30;
    record_time_display[2] = ':';
    record_time_display[3] = ' ';
    record_time_display[4] = record_time_min % 60 / 10 + 0x30;
    record_time_display[5] = record_time_min % 60 % 10 + 0x30;
    record_time_display[6] = ':';
    record_time_display[7] = ' ';
    record_time_display[8] = record_time_s % 60 / 10 + 0x30;
    record_time_display[9] = record_time_s % 60 % 10 + 0x30;
    record_time_display[10] = '\0';

    return (char *)record_time_display;
}
void reset_record_time(void)
{
    record_time_ms_init = gui_ms_get();
    record_time_ms = 0;
    record_time_s = 0;
    record_time_min = 0;
    record_time_hour = 0;
    record_time_display[0] = '0';
    record_time_display[1] = '0';
    record_time_display[2] = ':';
    record_time_display[3] = ' ';
    record_time_display[4] = '0';
    record_time_display[5] = '0';
    record_time_display[6] = ':';
    record_time_display[7] = ' ';
    record_time_display[8] = '0';
    record_time_display[9] = '0';
    record_time_display[10] = '\0';
}

static void text_record_tips_animate_cb(void *obj)
{
    gui_text_t *this = (gui_text_t *)obj;
    gui_log("text_record_tips_animate_cb\n");
    gui_log("text_record_tips_animate_cb, os_time_ms = %d\n", gui_ms_get());
    gui_log("text_record_tips_animate_cb, current_frame = %d, repeate_count = %d, progress_percent = %f\n",
            this->animate->current_frame, this->animate->current_repeat_count, this->animate->progress_percent);

    //update every 7 frame(about 210 ms)
    if (this->animate->current_frame % 7 == 0)
    {
        gui_log("text_record_tips_animate_cb update record time\n");
        string_record_time = update_record_time();
        gui_text_set(text_record_tips, string_record_time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_record_time), FONT_H_32);
    }
}

static void switch_record_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_record_touch_cb\n");

    char *string_record_title;
    char *string_record_tips = "轻触录音";

#ifndef _WIN32
    T_IO_MSG record_msg;
    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
        gui_log("switch_record_touch_cb GUI_EVENT_2\n");
        string_record_title = "录音";

        text_record_title->base.w = strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_title, 195, 24);
        gui_text_set(text_record_title, string_record_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_record_title), FONT_H_32);

        text_record_tips->base.w = strlen(string_record_tips) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_tips, 163, 314);
        gui_text_set(text_record_tips, string_record_tips, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_record_tips), FONT_H_32);
        //shut down animate
        text_record_tips->animate->animate = false;
        reset_record_time();

        gui_obj_show(switch_record_button, true);
        if (record_status == 1)
        {
            record_msg.type = IO_MSG_TYPE_WRISTBNAD;
            record_msg.subtype = IO_MSG_MMI;
#if F_APP_PLAY_AND_RECORD
            record_msg.u.param = MMI_RECORD_RECORD_AND_PLAY_STOP;
#else
            record_msg.u.param = MMI_RECORD_STOP;
#endif
            app_send_msg_to_apptask(&record_msg);
            record_status = 0;
        }
        break;


    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        gui_log("switch_record_touch_cb GUI_EVENT_1\n");
        string_record_title = "正在录音";

        text_record_title->base.w = strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_title, 165, 24);
        gui_text_set(text_record_title, string_record_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_record_title), FONT_H_32);

        string_record_time = (char *)record_time_display;
        reset_record_time();

        text_record_tips->base.w = strlen(string_record_time) * FONT_NUM_ALPHA_W;
        gui_text_move(text_record_tips, 142, 314);

        //gui_obj_set_update_att(text_record_tips, 250, true, NULL);
        gui_text_set_animate(text_record_tips, 1000, -1, text_record_tips_animate_cb, text_record_tips);
        text_record_tips->animate->animate = true;

        gui_obj_show(switch_record_button, false);

        if (record_status == 0)
        {
            record_msg.type = IO_MSG_TYPE_WRISTBNAD;
            record_msg.subtype = IO_MSG_MMI;
#if F_APP_PLAY_AND_RECORD
            record_msg.u.param = MMI_RECORD_RECORD_AND_PLAY_START;
#else
            record_msg.u.param = MMI_RECORD_START;
#endif
            app_send_msg_to_apptask(&record_msg);
            record_status = 1;
        }
        break;

    default:
        break;
    }
#endif

}

static void switch_record_button_touch_cb(void *obj, uint16_t event)
{
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        {
            gui_log("record_button_touch\n");
            app_record_switch_tabs(RECORD_FILES);
        }
        break;

    default:
        break;
    }
}

void design_tab_record_main(void *parent)
{
    gui_switch_t *switch_record = gui_switch_create(parent, 131, 78, 192, 192, ICON_RECORD_START_BIN,
                                                    ICON_RECORD_STOP_BIN);

    gui_obj_add_event_cb(switch_record, (gui_event_cb_t)switch_record_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record, (gui_event_cb_t)switch_record_touch_cb, GUI_EVENT_2, NULL);

    char *string_record_title = "录音";
    text_record_title = gui_text_create(parent, "text_record_title", 195, 24,
                                        strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_record_title, string_record_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_record_title), FONT_H_32);

    char *string_record_tips = "轻触录音";
    text_record_tips = gui_text_create(parent, "text_record_tips", 163, 314,
                                       strlen(string_record_tips) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_record_tips, string_record_tips, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_record_tips), FONT_H_32);

    gui_text_set_animate(text_record_tips, 1000, -1, text_record_tips_animate_cb, text_record_tips);
    text_record_tips->animate->animate = false;

    switch_record_button = gui_switch_create(parent, 88, 373, 277, 81, ICON_DISCONNECT_BIN,
                                             ICON_DISCONNECT_TOUCH_BIN);
    gui_obj_add_event_cb(switch_record_button, (gui_event_cb_t)switch_record_button_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record_button, (gui_event_cb_t)switch_record_button_touch_cb,
                         GUI_EVENT_2, NULL);
}
