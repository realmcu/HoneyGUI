#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "watchface_mgr.h"
#ifndef _WIN32
//for bt
#include "app_task.h"
#endif

#define GUI_MAX_DEVICE_NAME_LEN 30

static gui_switch_t *switch_text_base_phone = NULL;
static gui_switch_t *switch_media_audio = NULL;
static gui_switch_t *switch_disconnect_phone = NULL;
static gui_text_t *text_phone_device_name = NULL;
static gui_text_t *text_phone_media_audio = NULL;

char *txet_disconnect_phone = "确认断开连接？";
static uint8_t app_bond_phone_index = 0xff;

static void tab_phone_update_cb(void *obj, uint16_t event, void *param)
{
    gui_log("tab_phone_update_cb\n %d", event);
#ifndef _WIN32
    //check phone connection status
    switch (event)
    {
    case GUI_EVENT_PHONE:
        app_bond_phone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_PHONE);
        if (app_bond_phone_index != 0xff)
        {
            gui_text_set(text_phone_device_name, app_db.bond_device[app_bond_phone_index].device_name,
                         GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                         app_db.bond_device[app_bond_phone_index].device_name_len * 2,
                         FONT_H_32);
            gui_text_encoding_set(text_phone_device_name, UNICODE_ENCODING);
            gui_obj_show(switch_text_base_phone, true);
            if (app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE) != 0xff)
            {
                if (app_audio_cfg.support_sink)
                {
                    gui_obj_show(switch_media_audio, true);
                    gui_obj_show(text_phone_media_audio, true);
                    T_APP_BR_LINK *p_link = app_find_br_link(app_db.bond_device[app_bond_phone_index].bd_addr);
                    if (app_db.audio_play_mode == MODE_APP_A2DP_SNK &&
                        (p_link->connected_profile & A2DP_PROFILE_MASK))//the second condition is for sink only
                    {
                        gui_switch_is_on(switch_media_audio);
                    }
                }
                else
                {
                    gui_obj_show(switch_media_audio, false);
                    gui_obj_show(text_phone_media_audio, false);
                }
                gui_switch_is_on(switch_text_base_phone);
                gui_obj_show(switch_disconnect_phone, true);
            }
            else
            {
                gui_switch_is_off(switch_text_base_phone);
                gui_obj_show(switch_media_audio, false);
                gui_obj_show(text_phone_media_audio, false);
                gui_obj_show(switch_disconnect_phone, false);
            }
        }
        else
        {
            gui_text_set(text_phone_device_name, "无手机设备", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15,
                         FONT_H_32);
            gui_obj_show(switch_text_base_phone, false);
            gui_obj_show(switch_media_audio, false);
            gui_obj_show(text_phone_media_audio, false);
            gui_obj_show(switch_disconnect_phone, false);
        }
        break;
    default:
        break;
    }

#endif
}

static void switch_back_menu_phone_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_back_menu_phone_touch_cb, event = %d\n", event);
    app_watchface_switch_tabs(WATCHFACE_MAIN);
}

static void switch_disconnect_yes_action(void *obj)
{
    gui_log("switch_disconnect_yes_action\n");
    app_watchface_switch_tabs(WATCHFACE_PHONE);

#ifndef _WIN32
    //disconnect phone
    app_bond_phone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE);
    if (app_bond_phone_index != 0xff)
    {
        app_bt_policy_discon_all_profiles_by_addr(app_db.bond_device[app_bond_phone_index].bd_addr);
    }
    gui_update_by_event(GUI_EVENT_PHONE, NULL, false);
#endif
}

static void switch_disconnect_no_action(void *obj)
{
    gui_log("switch_disconnect_no_action\n");
    app_watchface_switch_tabs(WATCHFACE_PHONE);
}

static void switch_text_base_phone_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_text_base_phone_touch_cb, event = %d\n", event);
#ifndef _WIN32
    switch (event)
    {
    case GUI_EVENT_1:
        {
            app_bond_phone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_PHONE);
            if (!(app_db.bond_device[app_bond_phone_index].used) &&
                app_db.bond_device[app_bond_phone_index].exist_addr_flag)
            {
                T_IO_MSG con_msg;
                con_msg.type = IO_MSG_TYPE_WRISTBNAD;
                con_msg.subtype = IO_MSG_CONNECT_PHONE;
                con_msg.u.buf = app_db.bond_device[app_bond_phone_index].bd_addr;
                app_send_msg_to_apptask(&con_msg);
            }
        }
        break;
    case GUI_EVENT_2:
        {
            set_confirm_yes(switch_disconnect_yes_action, obj);
            set_confirm_no(switch_disconnect_no_action, obj);
            set_confirm_text(txet_disconnect_phone, 123, 131, 7);

            app_watchface_switch_tabs(WATCHFACE_CONFIRM);
        }
        break;
    default:
        break;
    }
#endif
}

static void switch_text_base_connect_new_phone_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_text_base_connect_new_phone_touch_cb, event = %d\n", event);

#ifndef _WIN32
    app_watchface_switch_tabs(WATCHFACE_PHONE_PAIR);

    app_bond_phone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_PHONE);
    if (app_bond_phone_index != 0xff)
    {
        app_bt_policy_discon_all_profiles_by_addr(app_db.bond_device[app_bond_phone_index].bd_addr);
        app_bt_bond_del_bond_device(app_db.bond_device[app_bond_phone_index].bd_addr);
    }
#endif
}

static void switch_switch_media_audio_off_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_switch_media_audio_off_touch_cb, event = %d\n", event);
    gui_switch_is_off(switch_media_audio);

#ifndef _WIN32
    if (app_audio_cfg.support_local_source)
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
        set_mode_msg.u.param = MODE_APP_PLAYBACK;
        app_send_msg_to_apptask(&set_mode_msg);
    }
    else
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_A2DP_RECORD_CONTROL;
        set_mode_msg.u.param = A2DP_RECORD_DELETE;
        app_send_msg_to_apptask(&set_mode_msg);
    }
#endif
}

static void switch_switch_media_audio_on_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_switch_media_audio_on_touch_cb, event = %d\n", event);
    gui_switch_is_on(switch_media_audio);

#ifndef _WIN32
    if (app_audio_cfg.support_local_source)
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
        set_mode_msg.u.param = MODE_APP_A2DP_SNK;
        app_send_msg_to_apptask(&set_mode_msg);
    }
    else
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_A2DP_RECORD_CONTROL;
        set_mode_msg.u.param = A2DP_RECORD_ADD;
        app_send_msg_to_apptask(&set_mode_msg);
    }
#endif
}

static void switch_disconnect_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_disconnect_touch_cb, event = %d\n", event);

    set_confirm_yes(switch_disconnect_yes_action, obj);
    set_confirm_no(switch_disconnect_no_action, obj);
    set_confirm_text(txet_disconnect_phone, 123, 131, 7);

    app_watchface_switch_tabs(WATCHFACE_CONFIRM);
}


void design_tab_phone(void *parent)
{
    //add animate for update
    gui_obj_add_event_cb(parent, (gui_event_cb_t)tab_phone_update_cb, (gui_event_t)GUI_EVENT_PHONE,
                         NULL);

    gui_switch_t *switch_back_menu_phone = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                             ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_phone, (gui_event_cb_t)switch_back_menu_phone_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_phone, (gui_event_cb_t)switch_back_menu_phone_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_phone = gui_switch_create(parent, 83, 111, 288, 64, ICON_TEXT_BASE_DARK_BIN,
                                               ICON_TEXT_BASE_BLUE_BIN);
    switch_text_base_phone->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_phone->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_text_base_phone,
                         (gui_event_cb_t)switch_text_base_phone_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_phone,
                         (gui_event_cb_t)switch_text_base_phone_touch_cb, GUI_EVENT_2, NULL);

    gui_switch_t *switch_text_base_connect_new_phone = gui_switch_create(parent, 83, 193, 288, 64,
                                                                         ICON_TEXT_BASE_DARK_BIN,
                                                                         ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_connect_new_phone->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_connect_new_phone->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_text_base_connect_new_phone,
                         (gui_event_cb_t)switch_text_base_connect_new_phone_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_connect_new_phone,
                         (gui_event_cb_t)switch_text_base_connect_new_phone_touch_cb, GUI_EVENT_2, NULL);

    switch_media_audio = gui_switch_create(parent, 277, 282, 100, 60, ICON_SWITCH_OFF_BIN,
                                           ICON_SWITCH_ON_BIN);
    gui_obj_add_event_cb(switch_media_audio,
                         (gui_event_cb_t)switch_switch_media_audio_on_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_media_audio,
                         (gui_event_cb_t)switch_switch_media_audio_off_touch_cb, GUI_EVENT_2, NULL);

    switch_disconnect_phone = gui_switch_create(parent, 88, 373, 277, 81, ICON_DISCONNECT_BIN,
                                                ICON_DISCONNECT_BIN);
    switch_disconnect_phone->off_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    switch_disconnect_phone->on_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    gui_obj_add_event_cb(switch_disconnect_phone, (gui_event_cb_t)switch_disconnect_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_disconnect_phone, (gui_event_cb_t)switch_disconnect_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    int16_t font_size = 32;
    gui_text_t *text_phone_title = gui_text_create(parent, "text_phone_title", 179, 24, 128, font_size);
    gui_text_set(text_phone_title, "连接手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    /*to do some changes when combining BT*/
    text_phone_device_name = gui_text_create(parent, "text_buds_device", 128, 122, 192, font_size);
    gui_text_set(text_phone_device_name, "无手机设备", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15,
                 font_size);

    gui_text_t *text_connect_to_new_phone = gui_text_create(parent, "text_search_buds", 147, 204, 160,
                                                            font_size);
    gui_text_set(text_connect_to_new_phone, "连接新手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15,
                 font_size);
    /*when phone is connected , to display*/
    text_phone_media_audio = gui_text_create(parent, "text_search_buds", 128, 289, 128, font_size);
    gui_text_set(text_phone_media_audio, "媒体音频", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_t *text_disconnect_phone = gui_text_create(switch_disconnect_phone,
                                                        "text_disconnect_phone", 75, 19,
                                                        128, font_size);
    gui_text_set(text_disconnect_phone, "断开连接", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);

#ifndef _WIN32
    app_bond_phone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_PHONE);
    if (app_bond_phone_index != 0xff)
    {
        gui_text_set(text_phone_device_name, app_db.bond_device[app_bond_phone_index].device_name,
                     GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     app_db.bond_device[app_bond_phone_index].device_name_len * 2,
                     FONT_H_32);
        gui_text_encoding_set(text_phone_device_name, UNICODE_ENCODING);
        if (app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_PHONE) != 0xff)
        {
            if (app_audio_cfg.support_sink)
            {
                gui_obj_show(switch_media_audio, true);
                gui_obj_show(text_phone_media_audio, true);
                T_APP_BR_LINK *p_link = app_find_br_link(app_db.bond_device[app_bond_phone_index].bd_addr);
                if (app_db.audio_play_mode == MODE_APP_A2DP_SNK &&
                    (p_link->connected_profile & A2DP_PROFILE_MASK))//the second condition is for sink only
                {
                    gui_switch_is_on(switch_media_audio);
                }
            }
            else
            {
                gui_obj_show(switch_media_audio, false);
                gui_obj_show(text_phone_media_audio, false);
            }
            gui_switch_is_on(switch_text_base_phone);
            gui_obj_show(switch_disconnect_phone, true);
        }
        else
        {
            gui_obj_show(switch_media_audio, false);
            gui_obj_show(text_phone_media_audio, false);
            gui_obj_show(switch_disconnect_phone, false);
        }
    }
    else
    {
        gui_obj_show(switch_text_base_phone, false);
        gui_obj_show(switch_media_audio, false);
        gui_obj_show(text_phone_media_audio, false);
        gui_obj_show(switch_disconnect_phone, false);
    }
#endif
}

