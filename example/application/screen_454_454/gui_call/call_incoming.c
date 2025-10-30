#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"
#include "call_mgr.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#endif

static void gui_call_status_update_cb(void *obj, uint16_t event, void *param)
{
    gui_log("gui_call_status_update_cb %d\n", event);
#ifndef _WIN32
    switch (event)
    {
    case GUI_EVENT_CALL_ACTIVE:
        {
            app_call_switch_tabs(CALL_ACTIVE);
        }
        break;
    case GUI_EVENT_CALL_END:
        {
            gui_tabview_jump_tab(tabview_main, 0, 0);
            app_call_switch_tabs(CALL_MAIN);
        }
        break;
    default:
        break;
    }
#endif
}

static void switch_incoming_call_answer_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_incoming_call_answer_touch_cb\n");

#ifndef _WIN32
    //send to app task
    T_IO_MSG answer_call_msg;
    answer_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    answer_call_msg.subtype = IO_MSG_MMI;
    answer_call_msg.u.param = MMI_HF_ANSWER_CALL;
    app_send_msg_to_apptask(&answer_call_msg);
#endif
}

static void switch_incoming_call_hangup_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_incoming_call_hangup_touch_cb\n");
#ifndef _WIN32
    //send to app task
    T_IO_MSG reject_call_msg;
    reject_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    reject_call_msg.subtype = IO_MSG_MMI;
    reject_call_msg.u.param = MMI_HF_REJECT_CALL;
    app_send_msg_to_apptask(&reject_call_msg);
#endif
}

void design_tab_call_incoming(void *parent)
{
    char *string_incoming_call_title = "来电";
    gui_text_t *text_incoming_call_title = gui_text_create(parent, "text_incoming_call_title", 195, 24,
                                                           strlen(string_incoming_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_incoming_call_title, string_incoming_call_title, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_incoming_call_title), FONT_H_32);
#ifndef _WIN32
    //get incoming call number
    char *string_dial_num = get_dial_num();
#else
    char *string_dial_num = "123456";//for GUI simulator
#endif
    gui_log("string_dial_num = %s\n", string_dial_num);
    gui_text_t *text_incoming_call_num = gui_text_create(parent, "text_incoming_call_num", 0, 59,
                                                         LCD_W, FONT_H_32);
    gui_text_set(text_incoming_call_num, string_dial_num, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_dial_num), FONT_H_32);
    gui_text_mode_set(text_incoming_call_num, CENTER);
    //incoming call image
    gui_img_t *img_incoming_call = gui_img_create_from_mem(parent, "img_incoming_call", ICON_CALLER_BIN,
                                                           165, 191,
                                                           124, 124);
    //incoming call answer
    gui_switch_t *switch_incoming_call_answer = gui_switch_create(parent, 285, 285, 80, 80,
                                                                  ICON_ANSWER_BIN,
                                                                  ICON_ANSWER_BIN);
    switch_incoming_call_answer->off_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    switch_incoming_call_answer->on_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    gui_obj_add_event_cb(switch_incoming_call_answer,
                         (gui_event_cb_t)switch_incoming_call_answer_touch_cb, GUI_EVENT_1, string_dial_num);
    gui_obj_add_event_cb(switch_incoming_call_answer,
                         (gui_event_cb_t)switch_incoming_call_answer_touch_cb, GUI_EVENT_2, string_dial_num);
    //incoming call hangup
    gui_switch_t *switch_incoming_call_hangup = gui_switch_create(parent, 89, 285, 80, 80,
                                                                  ICON_HANGUP_BIN,
                                                                  ICON_HANGUP_BIN);
    switch_incoming_call_hangup->off_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    switch_incoming_call_hangup->on_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_incoming_call_hangup,
                         (gui_event_cb_t)switch_incoming_call_hangup_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_incoming_call_hangup,
                         (gui_event_cb_t)switch_incoming_call_hangup_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_ACTIVE,
                         NULL);
    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_END, NULL);
}
