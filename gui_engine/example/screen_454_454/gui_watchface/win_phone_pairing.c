#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "gui_obj.h"

gui_switch_t *switch_back_menu_conn_new_phone = NULL;
gui_text_t *text_phone_pairing_title = NULL;
gui_text_t *text_notification = NULL;
gui_text_t *text_watch_device_name = NULL;
gui_text_t *text_watch_device_addr = NULL;

extern gui_win_t *win_menu_phone_pairing;
extern gui_win_t *win_menu_phone;

static void switch_back_menu_conn_new_phone_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_menu_conn_new_phone_touch_cb, event = %d\n", event);
    win_menu_phone_pairing->base.not_show = true;
    win_menu_phone->base.not_show = false;

    gui_tree_free(win_menu_phone_pairing);
    win_menu_phone_pairing = NULL;
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
    gui_text_set(text_phone_pairing_title, "连接新手机", "rtk_font_mem", APP_COLOR_WHITE, 15,
                 font_size);
    gui_text_type_set(text_phone_pairing_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_notification = gui_text_create(parent, "text_btext_notificationuds_device", 104, 120, 256,
                                        font_size);
    gui_text_set(text_notification, "已可以连接新手机", "rtk_font_mem", APP_COLOR_WHITE, 24,
                 font_size);
    gui_text_type_set(text_notification, SIMKAI_SIZE32_BITS1_FONT_BIN);
    /*to do some changes when combining BT*/
    text_watch_device_name = gui_text_create(parent, "text_watch_device_name", 131, 206, 192,
                                             font_size);
    gui_text_set(text_watch_device_name, "手表设备名称", "rtk_font_mem", APP_COLOR_WHITE, 18,
                 font_size);
    gui_text_type_set(text_watch_device_name, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_watch_device_addr = gui_text_create(parent, "text_watch_device_addr", 160, 302, 134,
                                             font_size);
    gui_text_set(text_watch_device_addr, "MAC地址", "rtk_font_mem", APP_COLOR_WHITE, 9, font_size);
    gui_text_type_set(text_watch_device_addr, SIMKAI_SIZE32_BITS1_FONT_BIN);

}
