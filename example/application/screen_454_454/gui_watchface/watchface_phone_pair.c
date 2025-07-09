#ifndef _WIN32
#include "root_image/ui_resource.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "app_gui_main.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "watchface_mgr.h"
//for bt
#include "app_task.h"
#include "app_cfg.h"
#include "gap.h"

char bt_bd_addr_text[20];

static void switch_back_menu_conn_new_phone_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_back_menu_conn_new_phone_touch_cb, event = %d\n", event);
    app_watchface_switch_tabs(WATCHFACE_PHONE);
}

void design_tab_phone_pair(void *parent)
{
    gui_switch_t *switch_back_menu_conn_new_phone = gui_switch_create(parent, 131, 24, 48, 48,
                                                                      ICON_BACK_BIN,
                                                                      ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_conn_new_phone,
                         (gui_event_cb_t)switch_back_menu_conn_new_phone_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_conn_new_phone,
                         (gui_event_cb_t)switch_back_menu_conn_new_phone_touch_cb,
                         GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    gui_text_t *text_phone_pairing_title = gui_text_create(parent, "text_phone_pairing_title", 172, 24,
                                                           160,
                                                           font_size);
    gui_text_set(text_phone_pairing_title, "连接新手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15,
                 font_size);

    gui_text_t *text_notification = gui_text_create(parent, "text_btext_notificationuds_device", 104,
                                                    120, 256,
                                                    font_size);
    gui_text_set(text_notification, "已可以连接新手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 24,
                 font_size);

    /*to do some changes when combining BT*/
    gui_text_t *text_watch_device_name = gui_text_create(parent, "text_watch_device_name", 131, 206,
                                                         192,
                                                         font_size);
    gui_text_set(text_watch_device_name, (char *)app_cfg_nv.device_name_legacy, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen((const char *)app_cfg_nv.device_name_legacy), font_size);

    gui_text_t *text_watch_device_addr = gui_text_create(parent, "text_watch_device_addr", 131, 302,
                                                         221,
                                                         font_size);
    uint8_t bt_bd_addr[6];
    gap_get_param(GAP_PARAM_BD_ADDR, bt_bd_addr);
    sprintf(bt_bd_addr_text, "%02x:%02x:%02x:%02x:%02x:%02x",
            bt_bd_addr[5], bt_bd_addr[4], bt_bd_addr[3],
            bt_bd_addr[2], bt_bd_addr[1], bt_bd_addr[0]
           );
    gui_text_set(text_watch_device_addr, bt_bd_addr_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(bt_bd_addr_text),
                 font_size);

}
#endif
