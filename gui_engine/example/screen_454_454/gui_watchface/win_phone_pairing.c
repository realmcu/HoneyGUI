#ifdef _ENABLE_RTK_SOC_WATCH_
#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"
#include "gui_common.h"
//for bt
#include "app_task.h"
#include "app_cfg.h"
#include "gap.h"

gui_switch_t *switch_back_menu_conn_new_phone = NULL;
gui_text_t *text_phone_pairing_title = NULL;
gui_text_t *text_notification = NULL;
gui_text_t *text_watch_device_name = NULL;
gui_text_t *text_watch_device_addr = NULL;

char bt_bd_addr_text[20];

extern gui_win_t *win_menu_phone_pairing;
extern gui_win_t *win_menu_phone;

static void switch_back_menu_conn_new_phone_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_back_menu_conn_new_phone_touch_cb, event = %d\n", event);

    gui_obj_tree_free(win_menu_phone_pairing);
    win_menu_phone_pairing = NULL;

    if (win_menu_phone == NULL)
    {
        gui_app_t *app = get_app_watch_ui();
        win_menu_phone = gui_win_create(&(app->screen), "win_menu_phone", 0, 0, 454, 454);
        extern void design_win_menu_phone(void *parent);
        design_win_menu_phone(win_menu_phone);
    }
}

void design_win_menu_phone_pairing(void *parent)
{
    switch_back_menu_conn_new_phone = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                        ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_conn_new_phone,
                         (gui_event_cb_t)switch_back_menu_conn_new_phone_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_conn_new_phone,
                         (gui_event_cb_t)switch_back_menu_conn_new_phone_touch_cb,
                         GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    text_phone_pairing_title = gui_text_create(parent, "text_phone_pairing_title", 172, 24, 160,
                                               font_size);
    gui_text_set(text_phone_pairing_title, "连接新手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 15,
                 font_size);
    gui_text_type_set(text_phone_pairing_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_notification = gui_text_create(parent, "text_btext_notificationuds_device", 104, 120, 256,
                                        font_size);
    gui_text_set(text_notification, "已可以连接新手机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 24,
                 font_size);
    gui_text_type_set(text_notification, SIMKAI_SIZE32_BITS1_FONT_BIN);

    /*to do some changes when combining BT*/
    text_watch_device_name = gui_text_create(parent, "text_watch_device_name", 131, 206, 192,
                                             font_size);
    gui_text_set(text_watch_device_name, (char *)app_cfg_nv.device_name_legacy, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen((const char *)app_cfg_nv.device_name_legacy), font_size);
    gui_text_type_set(text_watch_device_name, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_watch_device_addr = gui_text_create(parent, "text_watch_device_addr", 131, 302, 221,
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
