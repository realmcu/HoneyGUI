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
#include "gui_img_scope.h"
#include "gui_common.h"
#ifdef _ENABLE_RTK_SOC_WATCH_
#include "app_task.h"
#endif

#define MODE_SOURCE true
#define MODE_SINK false

extern gui_tabview_t *tabview_main;
bool cfg_mode = MODE_SOURCE;
gui_curtain_t *curtain_down_menu = NULL;
gui_img_t *img_base_menu = NULL;
gui_switch_t *switch_menu_bluetooth = NULL;
gui_switch_t *switch_menu_buds = NULL;
gui_switch_t *switch_menu_phone = NULL;
gui_switch_t *switch_menu_setting = NULL;
gui_win_t *win_menu_buds  = NULL;
gui_win_t *win_menu_phone = NULL;
gui_win_t *win_menu_setting = NULL;


static void switch_menu_bluetooth_released_cb(void *obj, gui_event_t event)
{
    gui_log("switch_menu_bluetooth_released_cb, event = %d\n", event);

#ifdef _ENABLE_RTK_SOC_WATCH_
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

static void switch_menu_buds_released_cb(void *obj, gui_event_t event)
{
    gui_log("switch_menu_buds_released_cb\n");
#ifdef _ENABLE_RTK_SOC_WATCH_
    if (app_db.audio_play_mode != MODE_APP_A2DP_SRC)
    {
        T_IO_MSG set_mode_msg;
        set_mode_msg.type = IO_MSG_TYPE_WRISTBNAD;
        set_mode_msg.subtype = IO_MSG_SET_PLAY_MODE;
        set_mode_msg.u.param = MODE_APP_A2DP_SRC;
        app_send_msg_to_apptask(&set_mode_msg);
    }
#endif
    tabview_main->base.not_show = true;
    if (win_menu_buds == NULL)
    {
        gui_app_t *app = get_app_watch_ui();
        win_menu_buds = gui_win_create(&(app->screen), "win_menu_buds", 0, 0, 454, 454);
        extern void design_win_menu_buds(void *parent);
        design_win_menu_buds(win_menu_buds);
    }
}

static void switch_menu_phone_released_cb(void *obj, gui_event_t event)
{
    gui_log("switch_menu_phone_released_cb\n");
#ifdef _ENABLE_RTK_SOC_WATCH_
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
    tabview_main->base.not_show = true;
    if (win_menu_phone == NULL)
    {
        gui_app_t *app = get_app_watch_ui();
        win_menu_phone = gui_win_create(&(app->screen), "win_menu_phone", 0, 0, 454, 454);
        extern void design_win_menu_phone(void *parent);
        design_win_menu_phone(win_menu_phone);
    }
}

static void switch_menu_setting_released_cb(void *obj, gui_event_t event)
{
    gui_log("switch_menu_setting_released_cb\n");
    tabview_main->base.not_show = true;
    if (win_menu_setting == NULL)
    {
        gui_app_t *app = get_app_watch_ui();
        win_menu_setting = gui_win_create(&(app->screen), "win_menu_setting", 0, 0, 454, 454);
        extern void design_win_menu_setting(void *parent);
        design_win_menu_setting(win_menu_setting);
    }
}

void design_curtain_menu(void *parent)
{
    gui_curtainview_t *ct = (gui_curtainview_t *)parent;
    curtain_down_menu = gui_curtain_create(ct, "curtain_down_menu", 0, 0, 454, 454, CURTAIN_DOWN, 1.0f);
    img_base_menu = gui_img_create_from_mem(curtain_down_menu, "", WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                            454, 454);
    img_base_menu->draw_img.blend_mode = IMG_BYPASS_MODE;
    img_base_menu->draw_img.opacity_value = 128;

    switch_menu_bluetooth = gui_switch_create(img_base_menu, 177, 38, 100, 100, ICON_BLUETOOTH_OFF_BIN,
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

    switch_menu_phone = gui_switch_create(img_base_menu, 51, 177, 100, 100, ICON_PHONE_OFF_BIN,
                                          ICON_PHONE_OFF_BIN);
    switch_menu_phone->off_hl_pic_addr = ICON_PHONE_TOUCH_BIN;
    switch_menu_phone->on_hl_pic_addr = ICON_PHONE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_phone, (gui_event_cb_t)switch_menu_phone_released_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_menu_phone, (gui_event_cb_t)switch_menu_phone_released_cb, GUI_EVENT_2,
                         NULL);

    switch_menu_setting = gui_switch_create(img_base_menu, 177, 327, 100, 100, ICON_MORE_OFF_BIN,
                                            ICON_MORE_OFF_BIN);
    switch_menu_setting->off_hl_pic_addr = ICON_MORE_TOUCH_BIN;
    switch_menu_setting->on_hl_pic_addr = ICON_MORE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_menu_setting, (gui_event_cb_t)switch_menu_setting_released_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_menu_setting, (gui_event_cb_t)switch_menu_setting_released_cb,
                         GUI_EVENT_2, NULL);

#ifdef _ENABLE_RTK_SOC_WATCH_
    //get bt status
    T_STATE bt_status = gui_get_bt_status();
    T_AUDIO_SUPPORT audio_support = gui_get_audio_support();
    if (bt_status == STATE_INIT)
    {
        switch_menu_bluetooth->ifon = false;
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
        switch_menu_bluetooth->ifon = true;
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
}
