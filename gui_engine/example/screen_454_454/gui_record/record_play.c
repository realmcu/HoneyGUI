#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_switch.h"
#include "record_mgr.h"
#ifndef _WIN32
#include "app_mic_record.h"
#include "app_task.h"
#include "app_mmi.h"
#include "app_msg.h"
#endif

gui_switch_t *switch_record_play_back = NULL;
gui_switch_t *switch_record_play_pause = NULL;
gui_text_t *text_record_play_time = NULL;
extern uint8_t record_status;
uint8_t play_record_status = 0;

static void tab_record_update_cb(void *obj, uint16_t event)
{
    gui_log("tab_record_update_cb  event = 0x%x\n", event);
    gui_obj_show(switch_record_play_back, true);
#ifndef _WIN32
    if (play_record_status)
    {
        gui_switch_is_on(switch_record_play_pause);
    }
    else
    {
        gui_switch_is_off(switch_record_play_pause);
    }
#endif
}

static void record_play_back_touch_cb(void *obj, uint16_t event)
{
    gui_log("record_play_back_touch_cb\n");
    app_record_switch_tabs(RECORD_FILES);
}

void switch_record_play_pause_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_record_play_pause_touch_cb, event = %d\n", event);

#ifndef _WIN32
    switch (event)
    {
    // touch to play
    case GUI_EVENT_1:
        {
//            switch_record_play_back->base.not_show = true;
            gui_obj_show(switch_record_play_back, false);
            gui_fb_change();
            T_IO_MSG record_msg;
            if (record_status == 0 && play_record_status == 0)
            {
                record_msg.type = IO_MSG_TYPE_WRISTBNAD;
                record_msg.subtype = IO_MSG_MMI;
                record_msg.u.param = MMI_RECORD_PLAY_START;
                app_send_msg_to_apptask(&record_msg);
                play_record_status = 1;
            }
        }
        break;
    case GUI_EVENT_2:
        {
            gui_obj_show(switch_record_play_back, true);
            gui_fb_change();
//            switch_record_play_back->base.not_show = false;
            T_IO_MSG record_msg;
            if (record_status == 0 && play_record_status == 1)
            {
                record_msg.type = IO_MSG_TYPE_WRISTBNAD;
                record_msg.subtype = IO_MSG_MMI;
                record_msg.u.param = MMI_RECORD_PLAY_STOP;
                app_send_msg_to_apptask(&record_msg);
                play_record_status = 0;
            }
        }
        break;

    default:
        break;
    }
#endif

}

static void text_record_play_time_animate_cb(void *obj)
{
    gui_log("text_record_play_time_animate_cb\n");
}

void design_tab_record_play(void *parent)
{
    gui_log("design_tab_record_play\n");
    gui_obj_add_event_cb(parent, (gui_event_cb_t)tab_record_update_cb, (gui_event_t)GUI_EVENT_RECORD,
                         NULL);
    switch_record_play_back = gui_switch_create(parent, 129, 24, 48, 48, ICON_BACK_BIN,
                                                ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_record_play_back, (gui_event_cb_t)record_play_back_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_record_play_back, (gui_event_cb_t)record_play_back_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    switch_record_play_pause = gui_switch_create(parent, 134, 71, 192, 192,
                                                 ICON_RECORD_PLAY_BIN,
                                                 ICON_RECORD_PAUSE_BIN);
    switch_record_play_pause->off_hl_pic_addr = ICON_RECORD_PLAY_TOUCH_BIN;
    switch_record_play_pause->on_hl_pic_addr = ICON_RECORD_PAUSE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_record_play_pause, (gui_event_cb_t)switch_record_play_pause_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record_play_pause, (gui_event_cb_t)switch_record_play_pause_touch_cb,
                         GUI_EVENT_2, NULL);

    char *string_record_file = "record.dat";
    gui_text_t *text_record_file = gui_text_create(parent, "text_title_record_files", 170, 24, 160,
                                                   FONT_H_32);
    gui_text_set(text_record_file, string_record_file, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_record_file), FONT_H_32);


    char *string_record_play_time = "00:00:00/00:00:00";
    text_record_play_time = gui_text_create(parent, "text_record_play_time", 108, 281, 248, FONT_H_32);
    gui_text_set(text_record_play_time, string_record_play_time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_record_play_time), FONT_H_32);
    gui_text_set_animate(text_record_play_time, 1000, 0, text_record_play_time_animate_cb,
                         text_record_play_time);
}


