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
#include "draw_font.h"
#ifndef _WIN32
//for bt buds
#include "app_bond.h"
#include "app_gap.h"
#include "app_task.h"
#endif

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

//for bt buds
static uint8_t app_bond_earphone_index = 0xff;

char *txet_disconnect = "确认断开连接？";
char bond_bd_addr[20];
//todo:
static void win_buds_animate_cb(void *p)
{
#ifndef _WIN32
    //todo:
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
            gui_text_set(text_buds_device, bond_bd_addr, "rtk_font_mem", APP_COLOR_WHITE, 17, FONT_H_32);
        }
        else
        {
            gui_text_set(text_buds_device, app_db.bond_device[app_bond_earphone_index].device_name,
                         "rtk_font_mem", APP_COLOR_WHITE, app_db.bond_device[app_bond_earphone_index].device_name_len,
                         FONT_H_32);
            gui_text_encoding_set(text_buds_device, UNICODE_ENCODING);
        }
        app_bond_earphone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EARPHONE);
        if (app_bond_earphone_index != 0xff)
        {
            gui_obj_show(switch_disconnect, true);
            gui_obj_show(text_disconnect, true);
        }
        else
        {
            gui_obj_show(switch_disconnect, false);
            gui_obj_show(text_disconnect, false);
        }
    }
    else
    {
        gui_text_set(text_buds_device, "无耳机设备", "rtk_font_mem", APP_COLOR_WHITE, 15, FONT_H_32);
        gui_obj_show(switch_disconnect, false);
        gui_obj_show(text_disconnect, false);
    }
#endif
}

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
    // push_current_widget(win_menu_buds);
    // gui_obj_show(win_menu_buds, false);
#ifndef _WIN32
    gui_app_t *app = get_app_watch_ui();
    if (win_buds_device == NULL)
    {
        gui_log("win_buds_device create\n");
        win_buds_device = gui_win_create(&(app->screen), "win_buds_device", 0, 0, 454, 454);
        //win_buds_device->base.not_show = false;
        extern void design_win_buds_device(void *parent);
        design_win_buds_device(win_buds_device);

        gui_tree_free(win_menu_buds);
        win_menu_buds = NULL;
    }
#endif
}

static void switch_text_base_search_buds_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_text_base_search_buds_touch_cb, event = %d\n", event);
    //extern gui_win_t *win_menu_buds;
    //push_current_widget(win_menu_buds);
    //gui_obj_show(win_menu_buds, false);
#ifndef _WIN32
    gui_app_t *app = get_app_watch_ui();
    if (win_search_buds == NULL)//when to free ?
    {
        gui_log("win_buds_searching create\n");
        win_search_buds = gui_win_create(&(app->screen), "win_search_buds", 0, 0, 454, 454);
        //win_search_buds->base.not_show = false;
        extern void design_win_buds_searching(void *parent);
        design_win_buds_searching(win_search_buds);

        gui_tree_free(win_menu_buds);
        win_menu_buds = NULL;
    }
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
    gui_log("switch_disconnect_yes_action, obj = 0x%x\n", obj);

    win_menu_buds->base.not_show = false;
    gui_tree_free(win_confirm);
    win_confirm = NULL;

    gui_obj_show(switch_disconnect, false);

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

}

static void switch_disconnect_no_action(void *obj)
{
    gui_log("switch_disconnect_no_action, obj = 0x%x\n", obj);
    win_menu_buds->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
#ifndef _WIN32
    gui_update_by_event2(win_menu_buds, NULL, true);
#endif
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
    //add animate for update
    gui_win_t *win = (gui_win_t *)parent;
    gui_win_set_animate((gui_win_t *)win, 100, 1, win_buds_animate_cb, NULL);
    win->animate->animate = false;

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
    gui_text_set(text_buds_title, "连接耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    gui_text_type_set(text_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_buds_device = gui_text_create(parent, "text_buds_device", 128, 122, 192, font_size);
    gui_text_set(text_buds_device, "耳机设备名称", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 18,
                 font_size);
    gui_text_type_set(text_buds_device, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds = gui_text_create(parent, "text_search_buds", 163, 204, 128, font_size);
    gui_text_set(text_search_buds, "搜索耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    gui_text_type_set(text_search_buds, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_disconnect = gui_text_create(parent, "text_disconnect", 163, 392, 128, font_size);
    gui_text_set(text_disconnect, "断开连接", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    gui_text_type_set(text_disconnect, SIMKAI_SIZE32_BITS1_FONT_BIN);

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
            gui_text_set(text_buds_device, bond_bd_addr, "rtk_font_mem", APP_COLOR_WHITE, 17, font_size);
        }
        else
        {
            gui_text_set(text_buds_device, app_db.bond_device[app_bond_earphone_index].device_name,
                         "rtk_font_mem", APP_COLOR_WHITE, \
                         app_db.bond_device[app_bond_earphone_index].device_name_len, font_size);
            gui_text_encoding_set(text_buds_device, UNICODE_ENCODING);
        }
        app_bond_earphone_index = app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE_EARPHONE);
        if (app_bond_earphone_index != 0xff)
        {
            gui_obj_show(switch_disconnect, true);
            gui_obj_show(text_disconnect, true);
        }
        else
        {
            gui_obj_show(switch_disconnect, false);
            gui_obj_show(text_disconnect, false);
        }
    }
    else
    {
        gui_text_set(text_buds_device, "无耳机设备", "rtk_font_mem", APP_COLOR_WHITE, 15, font_size);
        gui_obj_show(switch_disconnect, false);
        gui_obj_show(text_disconnect, false);
    }
#endif
}
