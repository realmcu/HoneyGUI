#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "app_gui_main.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "watchface_mgr.h"
#ifndef _WIN32
#include "app_task.h"
#endif

char *txet_sw_reset = "确认重启？";
char *txet_factory_data_reset = "确认恢复出厂？";

static void switch_back_menu_setting_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_back_menu_setting_touch_cb, event = %d\n", event);
    app_watchface_switch_tabs(WATCHFACE_MAIN);
}

static void switch_sw_reset_yes_action(void *obj)
{
    gui_log("switch_sw_reset_yes_action\n");
#ifndef _WIN32
    chip_reset(RESET_ALL);
#endif
}
static void switch_sw_reset_no_action(void *obj)
{
    gui_log("switch_sw_reset_no_action\n");
    app_watchface_switch_tabs(WATCHFACE_SETTING);
}

static void switch_text_base_sw_reset_touch_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_text_base_sw_reset_touch_cb, event = %d\n", event);

    set_confirm_yes(switch_sw_reset_yes_action, obj);
    set_confirm_no(switch_sw_reset_no_action, obj);
    set_confirm_text(txet_sw_reset, 157, 131, 5);

    app_watchface_switch_tabs(WATCHFACE_CONFIRM);
}

static void switch_factory_data_reset_yes_action(void *obj)
{
    gui_log("switch_factory_data_reset_yes_action\n");

#ifndef _WIN32
    T_IO_MSG factory_reset_msg;
    factory_reset_msg.type = IO_MSG_TYPE_WRISTBNAD;
    factory_reset_msg.subtype = IO_MSG_FACTORY_RESET;
    app_send_msg_to_apptask(&factory_reset_msg);
#endif
    app_watchface_switch_tabs(WATCHFACE_MAIN);
}
static void switch_factory_data_reset_no_action(void *obj)
{
    gui_log("switch_factory_data_reset_no_action\n");
    app_watchface_switch_tabs(WATCHFACE_SETTING);
}

static void switch_text_base_factory_data_reset_touch_cb(void *obj, uint16_t event,
                                                         void *param)
{
    gui_log("switch_text_base_factory_data_reset_touch_cb, event = %d\n", event);

    set_confirm_yes(switch_factory_data_reset_yes_action, obj);
    set_confirm_no(switch_factory_data_reset_no_action, obj);
    set_confirm_text(txet_factory_data_reset, 123, 131, 7);

    app_watchface_switch_tabs(WATCHFACE_CONFIRM);
}

void design_tab_setting(void *parent)
{
    gui_switch_t *switch_back_menu_setting = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                               ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_setting, (gui_event_cb_t)switch_back_menu_setting_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_setting, (gui_event_cb_t)switch_back_menu_setting_touch_cb,
                         GUI_EVENT_2, NULL);

    gui_switch_t *switch_sw_reset = gui_switch_create(parent, 83, 195, 288, 64, ICON_TEXT_BASE_DARK_BIN,
                                                      ICON_TEXT_BASE_DARK_BIN);
    switch_sw_reset->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_sw_reset->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_sw_reset,
                         (gui_event_cb_t)switch_text_base_sw_reset_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_sw_reset,
                         (gui_event_cb_t)switch_text_base_sw_reset_touch_cb, GUI_EVENT_2, NULL);

    gui_switch_t *switch_factory_data_reset_ = gui_switch_create(parent, 83, 275, 288, 64,
                                                                 ICON_TEXT_BASE_DARK_BIN,
                                                                 ICON_TEXT_BASE_DARK_BIN);
    switch_factory_data_reset_->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_factory_data_reset_->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_factory_data_reset_,
                         (gui_event_cb_t)switch_text_base_factory_data_reset_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_factory_data_reset_,
                         (gui_event_cb_t)switch_text_base_factory_data_reset_touch_cb, GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    gui_text_t *text_setting_title = gui_text_create(parent, "text_setting_title", 196, 24, 64,
                                                     font_size);
    gui_text_set(text_setting_title, "设置", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 6, font_size);

    gui_text_t *text_sw_reset = gui_text_create(parent, "text_sw_reset", 188, 204, 78, font_size);
    gui_text_set(text_sw_reset, "Reset", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 5, font_size);

    gui_text_t *text_factory_data_reset = gui_text_create(parent, "text_factory_data_reset", 131, 286,
                                                          192,
                                                          font_size);
    gui_text_set(text_factory_data_reset, "恢复出厂设置", GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 18,
                 font_size);

}
