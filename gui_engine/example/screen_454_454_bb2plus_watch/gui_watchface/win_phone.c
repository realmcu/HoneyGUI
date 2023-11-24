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

gui_switch_t *switch_back_menu_phone = NULL;
gui_switch_t *switch_text_base_connect_new_phone = NULL;
gui_switch_t *switch_media_audio = NULL;
gui_switch_t *switch_disconnect_phone = NULL;
gui_text_t *text_phone_title = NULL;
gui_text_t *text_phone_device_name = NULL;
gui_text_t *text_connect_to_new_phone = NULL;
gui_text_t *text_phone_media_audio = NULL;
gui_text_t *text_disconnect_phone = NULL;
gui_win_t *win_menu_phone_pairing = NULL;

extern gui_tabview_t *tabview_main;
extern gui_win_t *win_menu_phone;
extern gui_win_t *win_confirm;

char *txet_disconnect_phone = "确认断开连接？";

static void switch_back_menu_phone_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_back_menu_phone_touch_cb, event = %d\n", event);
    win_menu_phone->base.not_show = true;
    tabview_main->base.not_show = false;

    gui_tree_free(win_menu_phone);
    win_menu_phone = NULL;
}

static void switch_text_base_connect_new_phone_touch_cb(void *obj, gui_event_cb_t event)//base?
{
    gui_log("switch_text_base_connect_new_phone_touch_cb, event = %d\n", event);
    win_menu_phone->base.not_show = true;

    if (win_menu_phone_pairing == NULL)
    {
        gui_app_t *app = get_app_watch_ui();
        win_menu_phone_pairing = gui_win_create(&(app->screen), "win_phone_pairing", 0, 0, 454, 454);
        win_menu_phone_pairing->base.not_show = false;
        extern void design_win_menu_phone_pairing(void *parent);
        design_win_menu_phone_pairing(win_menu_phone_pairing);
    }
}

static void switch_switch_media_audio_off_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_switch_media_audio_off_touch_cb, event = %d\n", event);
}

static void switch_switch_media_audio_on_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_switch_media_audio_on_touch_cb, event = %d\n", event);
}

static void switch_disconnect_yes_action(void *obj)
{
    gui_log("switch_disconnect_yes_action, obj = 0x%x\n", obj);
    //to do disconnect
}

static void switch_disconnect_no_action(void *obj)
{
    gui_log("switch_disconnect_no_action, obj = 0x%x\n", obj);
    win_menu_phone->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
}
static void switch_disconnect_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_disconnect_touch_cb, event = %d\n", event);
    push_current_widget(win_menu_phone);
    gui_obj_show(win_menu_phone, false);

    gui_app_t *app = get_app_watch_ui();
    win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
    win_confirm->base.not_show = false;

    set_confirm_yes(switch_disconnect_yes_action, obj);
    set_confirm_no(switch_disconnect_no_action, obj);
    set_confirm_text(txet_disconnect_phone, 123, 131, 7);

    extern void design_win_confirm(void *parent);
    design_win_confirm(win_confirm);
}


void design_win_menu_phone(void *parent)
{
    switch_back_menu_phone = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_phone, (gui_event_cb_t)switch_back_menu_phone_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_phone, (gui_event_cb_t)switch_back_menu_phone_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_connect_new_phone = gui_switch_create(parent, 83, 193, 288, 64,
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
    text_phone_title = gui_text_create(parent, "text_phone_title", 179, 24, 128, font_size);
    gui_text_set(text_phone_title, "连接手机", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_phone_title, SIMKAI_SIZE32_BITS1_FONT_BIN);
    /*to do some changes when combining BT*/
    text_phone_device_name = gui_text_create(parent, "text_buds_device", 128, 122, 192, font_size);
    gui_text_set(text_phone_device_name, "手机设备名称", "rtk_font_mem", 0xffffffff, 18,
                 font_size);
    gui_text_type_set(text_phone_device_name, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_connect_to_new_phone = gui_text_create(parent, "text_search_buds", 147, 204, 160, font_size);
    gui_text_set(text_connect_to_new_phone, "连接新手机", "rtk_font_mem", 0xffffffff, 15,
                 font_size);
    gui_text_type_set(text_connect_to_new_phone, SIMKAI_SIZE32_BITS1_FONT_BIN);
    /*when phone is connected , to display*/
    text_phone_media_audio = gui_text_create(parent, "text_search_buds", 128, 289, 128, font_size);
    gui_text_set(text_phone_media_audio, "媒体音频", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_phone_media_audio, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_disconnect_phone = gui_text_create(parent, "text_disconnect_phone", 163, 392, 128, font_size);
    gui_text_set(text_disconnect_phone, "断开连接", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_disconnect_phone, SIMKAI_SIZE32_BITS1_FONT_BIN);

}
