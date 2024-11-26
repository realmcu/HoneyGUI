#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"
#include "gui_interface.h"
#include "call_mgr.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#include "app_hfp.h"
#include "app_audio_if.h"
#endif

#define GUI_CALLING_NUM_MAX 24

extern gui_tabview_t *tabview_call_mgr;
static gui_switch_t *switch_calling_mute = NULL;
static gui_switch_t *switch_calling_hangup = NULL;
static bool calling_mute_flag = false;

static void gui_call_status_update_cb(void *obj, uint16_t event, void *param)
{
    gui_log("gui_call_status_update_cb %d\n", event);
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
}

static void curtainview_set_done_cb_call_vol(gui_curtainview_t *this)
{
//    gui_log("curtainview_set_done_cb_call_vol\n");
    if (this->cur_curtain == CURTAIN_MIDDLE)
    {
        switch_calling_mute->touch_disable = true;
        switch_calling_hangup->touch_disable = true;
        gui_log("curtainview_set_done_cb_call_vol: CURTAIN_MIDDLE\n");
    }
    else
    {
        switch_calling_mute->touch_disable = false;
        switch_calling_hangup->touch_disable = false;
        if (calling_mute_flag)
        {
            gui_switch_is_on(switch_calling_mute);
        }
        else
        {
            gui_switch_is_off(switch_calling_mute);
        }
        gui_log("curtainview_set_done_cb_call_vol: CURTAIN_DOWN\n");
    }
}

static void switch_calling_hangup_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_calling_hangup_touch_cb\n");

#ifndef _WIN32
    //send msg to app task
    T_IO_MSG end_call_msg;
    end_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    end_call_msg.subtype = IO_MSG_MMI;
    end_call_msg.u.param = MMI_HF_REJECT_CALL;
    app_send_msg_to_apptask(&end_call_msg);
#endif
}

static void switch_calling_mute_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_calling_mute_touch_cb\n");
#ifndef _WIN32
    T_IO_MSG mic_mute_msg;
#endif
    switch (event)
    {
    // switch is off(touch to do some turn-on action)
    case GUI_EVENT_1:
        gui_log("switch_calling_mute_touch_cb mic mute on\n");
        calling_mute_flag = true;
#ifndef _WIN32
        //send msg to app task
        mic_mute_msg.type = IO_MSG_TYPE_WRISTBNAD;
        mic_mute_msg.subtype = IO_MSG_MMI;
        mic_mute_msg.u.param = MMI_DEV_MIC_MUTE;
        app_send_msg_to_apptask(&mic_mute_msg);
#endif
        break;
    // switch is on(touch to do some turn-off action)
    case GUI_EVENT_2:
        gui_log("switch_calling_mute_touch_cb mic mute off\n");
        calling_mute_flag = false;
#ifndef _WIN32
        mic_mute_msg.type = IO_MSG_TYPE_WRISTBNAD;
        mic_mute_msg.subtype = IO_MSG_MMI;
        mic_mute_msg.u.param = MMI_DEV_MIC_UNMUTE;
        app_send_msg_to_apptask(&mic_mute_msg);
#endif
        break;
    default:
        break;
    }
}

void design_tab_call_outgoing(void *parent)
{
#ifndef _WIN32
    char *string_dial_num = get_dial_num();
#else
    char *string_dial_num = "123456";//for GUI simulator
#endif
    gui_log("string_dial_num = %s\n", string_dial_num);
    gui_text_t *text_calling_num = gui_text_create(parent, "text_calling_num", 0, 59,
                                                   LCD_W, FONT_H_32);
    gui_text_set(text_calling_num, string_dial_num, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_dial_num), FONT_H_32);
    gui_text_mode_set(text_calling_num, CENTER);

    gui_img_t *img_calling = gui_img_create_from_mem(parent, "img_calling", ICON_CALLER_BIN, 165, 111,
                                                     124, 124);

    switch_calling_hangup = gui_switch_create(parent, 60, 297, 80, 80, ICON_HANGUP_BIN,
                                              ICON_HANGUP_BIN);
    switch_calling_hangup->off_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    switch_calling_hangup->on_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_hangup, (gui_event_cb_t)switch_calling_hangup_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_hangup, (gui_event_cb_t)switch_calling_hangup_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_calling_mute = gui_switch_create(parent, 314, 297, 80, 80,
                                            ICON_MIC_MUTE_OFF_BIN,
                                            ICON_MIC_MUTE_ON_BIN);
    switch_calling_mute->off_hl_pic_addr = ICON_MIC_MUTE_OFF_TOUCH_BIN;
    switch_calling_mute->on_hl_pic_addr = ICON_MIC_MUTE_ON_TOUCH_BIN;

    gui_obj_add_event_cb(switch_calling_mute, (gui_event_cb_t)switch_calling_mute_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_calling_mute, (gui_event_cb_t)switch_calling_mute_touch_cb, GUI_EVENT_2,
                         NULL);
    gui_img_create_from_mem(parent, "img_slide_vol", ICON_DISCONNECT_BIN, 88, 373, 277, 81);
    gui_img_create_from_mem(parent, "img_spk", ICON_VOLUME_BIN, 205, 386, 36, 48);

    gui_curtainview_t *curtainview_call_vol = gui_curtainview_create(parent, "curtainview_call_vol", 0,
                                                                     0, LCD_W, LCD_H);
    gui_curtainview_set_done_cb(curtainview_call_vol, curtainview_set_done_cb_call_vol);
    extern void design_curtain_call_vol(void *parent);
    design_curtain_call_vol(curtainview_call_vol);

    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_ACTIVE,
                         NULL);
    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_END, NULL);
}
