#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
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
#include "gui_page.h"
#include "gui_rect.h"
#include "gui_img_scope.h"
#include "tp_algo.h"
#define MODE_SOURCE true
#define MODE_SINK false
#define PAGE_NAME "_page_name"
bool cfg_mode = MODE_SOURCE;
static gui_switch_t *switch_menu_buds = NULL;
static gui_switch_t *switch_menu_phone = NULL;
static gui_switch_t *switch_menu_setting = NULL;
static void page_cb(gui_win_t *win);

static void switch_menu_bluetooth_released_cb(void *obj, uint16_t event)
{
    gui_log("switch_menu_bluetooth_released_cb, event = %d\n", event);

#ifndef _WIN32
    T_AUDIO_SUPPORT audio_support = gui_get_audio_support();
    //turn on bluetooth
    switch (event)
    {
    case GUI_EVENT_1:
        gui_obj_show(switch_menu_phone, true);
        gui_obj_show(switch_menu_setting, true);
        if (audio_support >= AUDIO_SUPPORT_SOURCE)
        {
            gui_obj_show(switch_menu_buds, true);
            switch_menu_phone->base.x = 51;
        }
        else
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 177;
        }
        // send to app task
        app_bt_policy_event_handle(EVENT_BT_STARTUP, NULL);

        break;
    //turn off bluetooth
    case GUI_EVENT_2:
        gui_obj_show(switch_menu_phone, false);
        gui_obj_show(switch_menu_setting, true);
        if (audio_support >= AUDIO_SUPPORT_SOURCE)
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 51;
        }
        else
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 177;
        }
        // send to app task
        app_bt_policy_event_handle(EVENT_BT_IDLE, NULL);
        break;

    default:
        break;
    }
#endif
}

static void switch_menu_buds_released_cb(void *obj, uint16_t event)
{
    gui_log("switch_menu_buds_released_cb\n");
#ifndef _WIN32
    if (app_db.audio_play_mode != MODE_APP_A2DP_SRC)
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
        set_mode_msg.u.param = MODE_APP_A2DP_SRC;
        app_send_msg_to_apptask(&set_mode_msg);
    }
#endif
    app_watchface_switch_tabs(WATCHFACE_BUDS);
}

static void switch_menu_phone_released_cb(void *obj, uint16_t event)
{
    gui_log("switch_menu_phone_released_cb\n");
#ifndef _WIN32
    uint8_t app_bond_phone_index = app_bt_bond_check_exist_device_info(T_DEVICE_TYPE_PHONE);
    if (app_bond_phone_index != 0xff && !app_db.bond_device[app_bond_phone_index].used)
    {
        if (app_audio_cfg.support_sink && (app_db.audio_play_mode == MODE_NONE))//sink only
        {
            T_IO_MSG set_mode_msg;
            set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
            set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
            set_mode_msg.u.param = MODE_APP_A2DP_SNK;
            app_send_msg_to_apptask(&set_mode_msg);
        }
        else
        {
            T_IO_MSG con_msg;
            con_msg.type = IO_MSG_TYPE_WRISTBNAD;
            con_msg.subtype = IO_MSG_CONNECT_PHONE;
            con_msg.u.buf = app_db.bond_device[app_bond_phone_index].bd_addr;
            app_send_msg_to_apptask(&con_msg);
        }
    }
#endif
    app_watchface_switch_tabs(WATCHFACE_PHONE);
}

static void switch_menu_setting_released_cb(void *obj, uint16_t event)
{
    gui_log("switch_menu_setting_released_cb\n");
    app_watchface_switch_tabs(WATCHFACE_SETTING);
}

void design_curtain_menu(void *parent)
{
    gui_curtainview_t *ct = (gui_curtainview_t *)parent;
    gui_curtain_t *curtain_down_menu = gui_curtain_create(ct, "curtain_down_menu", 0, 0, 454, 454,
                                                          CURTAIN_DOWN, 1.0f);
    gui_img_t *img_base_menu = gui_img_create_from_mem(curtain_down_menu, "",
                                                       WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                                       454, 454);
    gui_img_set_mode(img_base_menu, IMG_BYPASS_MODE);
    gui_img_set_opacity(img_base_menu, 128);

    gui_switch_t *switch_menu_bluetooth = gui_switch_create(img_base_menu, 177, 38, 100, 100,
                                                            ICON_BLUETOOTH_OFF_BIN,
                                                            ICON_BLUETOOTH_ON_BIN);
    switch_menu_bluetooth->off_hl_pic_addr = ICON_BLUETOOTH_TOUCH_BIN;
    switch_menu_bluetooth->on_hl_pic_addr = ICON_BLUETOOTH_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_bluetooth, (gui_event_cb_t)switch_menu_bluetooth_released_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_menu_bluetooth, (gui_event_cb_t)switch_menu_bluetooth_released_cb,
                         GUI_EVENT_2, NULL);

    switch_menu_buds = gui_switch_create(img_base_menu, 300, 177, 100, 100, ICON_BUDS_OFF_BIN,
                                         ICON_BUDS_OFF_BIN);//on_bin by BT event ICON_BUDS_ON_BIN
    switch_menu_buds->off_hl_pic_addr = ICON_BUDS_TOUCH_BIN;
    switch_menu_buds->on_hl_pic_addr = ICON_BUDS_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_buds, (gui_event_cb_t)switch_menu_buds_released_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_menu_buds, (gui_event_cb_t)switch_menu_buds_released_cb, GUI_EVENT_2,
                         NULL);

    switch_menu_phone = gui_switch_create(img_base_menu, 51, 177, 100, 100,
                                          ICON_PHONE_OFF_BIN,
                                          ICON_PHONE_OFF_BIN);
    switch_menu_phone->off_hl_pic_addr = ICON_PHONE_TOUCH_BIN;
    switch_menu_phone->on_hl_pic_addr = ICON_PHONE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_phone, (gui_event_cb_t)switch_menu_phone_released_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_menu_phone, (gui_event_cb_t)switch_menu_phone_released_cb, GUI_EVENT_2,
                         NULL);

    switch_menu_setting = gui_switch_create(img_base_menu, 177, 327, 100, 100,
                                            ICON_MORE_OFF_BIN,
                                            ICON_MORE_OFF_BIN);
    switch_menu_setting->off_hl_pic_addr = ICON_MORE_TOUCH_BIN;
    switch_menu_setting->on_hl_pic_addr = ICON_MORE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_setting, (gui_event_cb_t)switch_menu_setting_released_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_menu_setting, (gui_event_cb_t)switch_menu_setting_released_cb,
                         GUI_EVENT_2, NULL);

#ifndef _WIN32
    //get bt status
    T_STATE bt_status = gui_get_bt_status();
    T_AUDIO_SUPPORT audio_support = gui_get_audio_support();
    if (bt_status == STATE_INIT)
    {
        gui_switch_is_off(switch_menu_bluetooth);
        gui_obj_show(switch_menu_phone, false);
        gui_obj_show(switch_menu_setting, true);
        //get sink\source mode
        if (audio_support >= AUDIO_SUPPORT_SOURCE)
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 51;
        }
        else
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 177;
        }
    }
    else
    {
        gui_switch_is_on(switch_menu_bluetooth);
        gui_obj_show(switch_menu_phone, true);
        gui_obj_show(switch_menu_setting, true);
        //get sink\source mode
        if (audio_support >= AUDIO_SUPPORT_SOURCE)
        {
            gui_obj_show(switch_menu_buds, true);
            switch_menu_phone->base.x = 51;
        }
        else
        {
            gui_obj_show(switch_menu_buds, false);
            switch_menu_phone->base.x = 177;
        }
    }
#endif
    gui_curtain_t *c_up  = gui_curtain_create(ct, "curtain_up_menu", 0, 0, 454, 454, CURTAIN_UP, 1.0f);
    gui_rect_create((void *)c_up, 0, 0, 454, 454, APP_COLOR_WHITE);
    gui_win_t *win = gui_win_create(c_up, 0, 0, 0, 454, 454);
    win->scope = 1;

    gui_page_t *page = gui_page_create(win, PAGE_NAME, 0, 0, 454, 454);
    gui_win_set_animate(win, 1000, -1, page_cb, win);
    gui_img_scope_t *img1 = gui_img_scope_create(page, 0, A1_BIN, 0, 0);
    gui_img_scope_create(page, 0, A2_BIN, 0, 130 * 1);
    gui_img_scope_create(page, 0, A3_BIN, 0, 130 * 2);
    gui_img_scope_create(page, 0, A4_BIN, 0, 130 * 3);
    gui_img_scope_create(page, 0, A5_BIN, 0, 130 * 4);
    gui_img_scope_create(page, 0, A6_BIN, 0, 130 * 5);

}
static void page_cb(gui_win_t *win)
{
    touch_info_t *tp = (touch_info_t *)(uintptr_t)tp_get_info();
    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;
    }
    if (hold)
    {
        if (tp->y > 454 - 100)
        {
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 1;
            gui_curtainview_t *ct = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "curtainview_menu", (void *)&ct);
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "curtain_up_menu", (void *)&c_up);
            if (ct)
            {
                GUI_BASE(ct)->gesture = 0;
            }
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 0;
            }
            gui_log(">\n");
        }
        else
        {
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 0;
            gui_curtainview_t *ct = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "curtainview_menu", (void *)&ct);
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "curtain_up_menu", (void *)&c_up);
            if (ct)
            {
                GUI_BASE(ct)->gesture = 1;
            }
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 1;
            }
            gui_log("<\n");
        }


    }

}
