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

gui_switch_t *switch_back_menu_buds = NULL;
gui_switch_t *switch_text_base_buds_device = NULL;
gui_switch_t *switch_text_base_search_buds = NULL;
gui_switch_t *switch_disconnect = NULL;
gui_text_t *text_buds_title = NULL;
gui_text_t *text_buds_device = NULL;
gui_text_t *text_search_buds = NULL;
gui_text_t *text_disconnect = NULL;
gui_win_t *win_buds_device = NULL;
gui_win_t *win_search_buds = NULL;

extern gui_tabview_t *tabview_main;
extern gui_win_t *win_menu_buds;
extern gui_win_t *win_confirm;

char *txet_disconnect = "确认断开连接？";

static void switch_back_menu_buds_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_back_menu_buds_touch_cb, event = %d\n", event);
    win_menu_buds->base.not_show = true;
    tabview_main->base.not_show = false;

    gui_tree_free(win_menu_buds);
    win_menu_buds = NULL;
}

static void switch_text_base_buds_device_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_text_base_buds_device_touch_cb, event = %d\n", event);
    push_current_widget(win_menu_buds);
    gui_obj_show(win_menu_buds, false);

    gui_app_t *app = get_app_watch_ui();
    //if (win_buds_device == NULL)
    {
        gui_log("win_buds_device create\n");
        win_buds_device = gui_win_create(&(app->screen), "win_buds_device", 0, 0, 454, 454);
        win_buds_device->base.not_show = false;
        extern void design_win_buds_device(void *parent);
        design_win_buds_device(win_buds_device);
    }

}

static void switch_text_base_search_buds_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_text_base_search_buds_touch_cb, event = %d\n", event);
    extern gui_win_t *win_menu_buds;
    push_current_widget(win_menu_buds);
    gui_obj_show(win_menu_buds, false);
    gui_app_t *app = get_app_watch_ui();
    //if (win_search_buds == NULL)//when to free ?
    {
        gui_log("win_buds_searching create\n");
        win_search_buds = gui_win_create(&(app->screen), "win_search_buds", 0, 0, 454, 454);
        win_search_buds->base.not_show = false;
        extern void design_win_buds_searching(void *parent);
        design_win_buds_searching(win_search_buds);
    }
}

static void switch_disconnect_yes_action(void *obj)
{
    gui_log("switch_disconnect_yes_action, obj = 0x%x\n", obj);
}

static void switch_disconnect_no_action(void *obj)
{
    gui_log("switch_disconnect_no_action, obj = 0x%x\n", obj);
    win_menu_buds->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
}

static void switch_disconnect_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_disconnect_touch_cb, event = %d\n", event);
    push_current_widget(win_menu_buds);
    gui_obj_show(win_menu_buds, false);

    gui_app_t *app = get_app_watch_ui();
    win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
    win_confirm->base.not_show = false;

    set_confirm_yes(switch_disconnect_yes_action, obj);
    set_confirm_no(switch_disconnect_no_action, obj);
    set_confirm_text(txet_disconnect, 123, 131, 7);

    extern void design_win_confirm(void *parent);
    design_win_confirm(win_confirm);
}

void design_win_menu_buds(void *parent)
{
    switch_back_menu_buds = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_menu_buds, (gui_event_cb_t)switch_back_menu_buds_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_menu_buds, (gui_event_cb_t)switch_back_menu_buds_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_buds_device = gui_switch_create(parent, 83, 111, 288, 64, ICON_TEXT_BASE_DARK_BIN,
                                                     ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_buds_device->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_buds_device->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_buds_device,
                         (gui_event_cb_t)switch_text_base_buds_device_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    switch_text_base_search_buds = gui_switch_create(parent, 83, 193, 288, 64, ICON_TEXT_BASE_DARK_BIN,
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
    text_buds_title = gui_text_create(parent, "text_buds_title", 179, 24, 128, font_size);
    gui_text_set(text_buds_title, "连接耳机", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_buds_device = gui_text_create(parent, "text_buds_device", 128, 122, 192, font_size);
    gui_text_set(text_buds_device, "耳机设备名称", "rtk_font_mem", 0xffffffff, 18, font_size);
    gui_text_type_set(text_buds_device, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds = gui_text_create(parent, "text_search_buds", 163, 204, 128, font_size);
    gui_text_set(text_search_buds, "搜索耳机", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_search_buds, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_disconnect = gui_text_create(parent, "text_disconnect", 163, 392, 128, font_size);
    gui_text_set(text_disconnect, "断开连接", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_disconnect, SIMKAI_SIZE32_BITS1_FONT_BIN);

}
