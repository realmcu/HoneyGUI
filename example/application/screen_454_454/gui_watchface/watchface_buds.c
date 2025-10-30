#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "app_gui_main.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "draw_font.h"
#include "watchface_mgr.h"
#ifndef _WIN32
//for bt buds
#include "app_bond.h"
#include "app_gap.h"
#include "app_task.h"
#endif

static gui_switch_t *switch_disconnect = NULL;
static gui_text_t *text_buds_device = NULL;
static gui_switch_t *switch_text_base_buds_device = NULL;
//for bt buds
static uint8_t app_bond_earphone_index = 0xff;

char *text_disconnect = "确认断开连接？";
char bond_bd_addr[20];
//todo:
static void tab_buds_update_cb(void *obj, uint16_t event, void *param)
{
#ifndef _WIN32
    //todo:
    switch (event)
    {
    case GUI_EVENT_BUDS:
        app_bond_earphone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_EARPHONE);
        if (app_bond_earphone_index != 0xff)
        {
            if (app_db.bond_device[app_bond_earphone_index].device_name_len == 0)
            {
                sprintf(bond_bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                        app_db.bond_device[app_bond_earphone_index].bd_addr[5],
                        app_db.bond_device[app_bond_earphone_index].bd_addr[4],
                        app_db.bond_device[app_bond_earphone_index].bd_addr[3],
                        app_db.bond_device[app_bond_earphone_index].bd_addr[2],
                        app_db.bond_device[app_bond_earphone_index].bd_addr[1],
                        app_db.bond_device[app_bond_earphone_index].bd_addr[0]);
                gui_text_set(text_buds_device, bond_bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, FONT_H_32);
            }
            else
            {
                gui_text_set(text_buds_device, app_db.bond_device[app_bond_earphone_index].device_name,
                             GUI_FONT_SRC_BMP, APP_COLOR_WHITE, app_db.bond_device[app_bond_earphone_index].device_name_len * 2,
                             FONT_H_32);
                gui_text_encoding_set(text_buds_device, UNICODE_ENCODING);
            }
            app_bond_earphone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EARPHONE);
            if (app_bond_earphone_index != 0xff)
            {
                gui_obj_show(switch_disconnect, true);
                gui_switch_is_on(switch_text_base_buds_device);
            }
            else
            {
                gui_obj_show(switch_disconnect, false);
                gui_switch_is_off(switch_text_base_buds_device);
            }
        }
        else
        {
            gui_text_set(text_buds_device, "无耳机设备", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15, FONT_H_32);
            gui_obj_show(switch_disconnect, false);
            gui_switch_is_off(switch_text_base_buds_device);
        }
        break;

    default:
        break;
    }
#endif
}

static void switch_back_menu_buds_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_back_menu_buds_touch_cb, event = %d\n", event);
    app_watchface_switch_tabs(WATCHFACE_MAIN);
}

static void switch_text_base_buds_device_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_text_base_buds_device_touch_cb, event = %d\n", event);
#ifndef _WIN32
    app_watchface_switch_tabs(WATCHFACE_BUDS_DEVICE);
#endif
}

static void switch_text_base_search_buds_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_text_base_search_buds_touch_cb, event = %d\n", event);
#ifndef _WIN32
    app_watchface_switch_tabs(WATCHFACE_BUDS_SEARCH);
#endif
    //start search
    //stop: only happens at init state
    //finish: search complete
#ifndef _WIN32
    if (get_search_status() != SEARCH_START)
    {
        T_IO_MSG inquiry_msg;
        inquiry_msg.type = IO_MSG_TYPE_WRISTBNAD;
        inquiry_msg.subtype = IO_MSG_INQUIRY_START;
        app_send_msg_to_apptask(&inquiry_msg);
    }
#endif
}

static void switch_disconnect_yes_action(void *obj)
{
    gui_log("switch_disconnect_yes_action\n");
#ifndef _WIN32
    //disconnect buds
    app_bond_earphone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EARPHONE);
    if (app_bond_earphone_index != 0xff)
    {
        T_IO_MSG discon_msg;
        discon_msg.type = IO_MSG_TYPE_WRISTBNAD;
        discon_msg.subtype = IO_MSG_DISCONNECT_BREDR_DEVICE;
        discon_msg.u.buf = app_db.bond_device[app_bond_earphone_index].bd_addr;
        app_send_msg_to_apptask(&discon_msg);
    }
#endif
    app_watchface_switch_tabs(WATCHFACE_BUDS);
}

static void switch_disconnect_no_action(void *obj)
{
    gui_log("switch_disconnect_no_action\n");
    app_watchface_switch_tabs(WATCHFACE_BUDS);
#ifndef _WIN32
    gui_update_by_event(GUI_EVENT_BUDS, NULL, false);
#endif
}

static void switch_disconnect_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_disconnect_touch_cb\n");

    set_confirm_yes(switch_disconnect_yes_action, obj);
    set_confirm_no(switch_disconnect_no_action, obj);
    set_confirm_text(text_disconnect, 123, 131, 7);

    app_watchface_switch_tabs(WATCHFACE_CONFIRM);
}

void design_tab_buds(void *parent)
{
    /*update event*/
    gui_obj_add_event_cb(parent, (gui_event_cb_t)tab_buds_update_cb, (gui_event_t)GUI_EVENT_BUDS, NULL);

    gui_switch_t *switch_back_menu_buds = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                            ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_buds, (gui_event_cb_t)switch_back_menu_buds_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_buds, (gui_event_cb_t)switch_back_menu_buds_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_buds_device = gui_switch_create(parent, 83, 111, 288, 64,
                                                     ICON_TEXT_BASE_DARK_BIN,
                                                     ICON_TEXT_BASE_BLUE_BIN);
    switch_text_base_buds_device->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_buds_device->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    gui_switch_t *switch_text_base_search_buds = gui_switch_create(parent, 83, 193, 288, 64,
                                                                   ICON_TEXT_BASE_DARK_BIN,
                                                                   ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_text_base_search_buds,
                         (gui_event_cb_t)switch_text_base_search_buds_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds,
                         (gui_event_cb_t)switch_text_base_search_buds_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds,
                         (gui_event_cb_t)switch_text_base_search_buds_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    switch_disconnect = gui_switch_create(parent, 88, 373, 277, 81, ICON_DISCONNECT_BIN,
                                          ICON_DISCONNECT_BIN);
    switch_disconnect->off_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    switch_disconnect->on_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    gui_obj_add_event_cb(switch_disconnect, (gui_event_cb_t)switch_disconnect_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_disconnect, (gui_event_cb_t)switch_disconnect_touch_cb, GUI_EVENT_2,
                         NULL);

    int16_t font_size = 32;
    gui_text_t *text_buds_title = gui_text_create(parent, "text_buds_title", 179, 24, 128, font_size);
    gui_text_set(text_buds_title, "连接耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);

    text_buds_device = gui_text_create(switch_text_base_buds_device, "text_buds_device", 64,
                                       11, 192, font_size);
    gui_text_set(text_buds_device, "耳机设备名称", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 18,
                 font_size);

    gui_text_t *text_search_buds = gui_text_create(switch_text_base_search_buds, "text_search_buds", 80,
                                                   11, 128, font_size);
    gui_text_set(text_search_buds, "搜索耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);

    gui_text_t *text_disconnect = gui_text_create(switch_disconnect, "text_disconnect", 75, 19, 128,
                                                  font_size);
    gui_text_set(text_disconnect, "断开连接", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);

#ifndef _WIN32
    //check if already connected buds
    app_bond_earphone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_EARPHONE);
    if (app_bond_earphone_index != 0xff)
    {
        if (app_db.bond_device[app_bond_earphone_index].device_name_len == 0)
        {
            sprintf(bond_bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    app_db.bond_device[app_bond_earphone_index].bd_addr[5],
                    app_db.bond_device[app_bond_earphone_index].bd_addr[4],
                    app_db.bond_device[app_bond_earphone_index].bd_addr[3],
                    app_db.bond_device[app_bond_earphone_index].bd_addr[2],
                    app_db.bond_device[app_bond_earphone_index].bd_addr[1],
                    app_db.bond_device[app_bond_earphone_index].bd_addr[0]);
            gui_text_set(text_buds_device, bond_bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, font_size);
        }
        else
        {
            gui_text_set(text_buds_device, app_db.bond_device[app_bond_earphone_index].device_name,
                         GUI_FONT_SRC_BMP, APP_COLOR_WHITE, \
                         app_db.bond_device[app_bond_earphone_index].device_name_len * 2, font_size);
            gui_text_encoding_set(text_buds_device, UNICODE_ENCODING);
        }
        app_bond_earphone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EARPHONE);
        if (app_bond_earphone_index != 0xff)
        {
            gui_switch_is_on(switch_text_base_buds_device);
            gui_obj_show(switch_disconnect, true);
        }
        else
        {
            gui_switch_is_off(switch_text_base_buds_device);
            gui_obj_show(switch_disconnect, false);
        }
    }
    else
    {
        gui_text_set(text_buds_device, "无耳机设备", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15, font_size);
        gui_obj_show(switch_disconnect, false);
    }
#endif
}
