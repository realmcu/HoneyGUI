#ifndef _WIN32
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
#include "gui_page.h"
#include "app_task.h"
#include "app_bond.h"
#include "bt_bond.h"


static gui_page_t *page_device_buds = NULL;
gui_switch_t *switch_back_device_buds = NULL;
gui_switch_t *switch_text_base_bonding_buds[8] = {NULL};
// gui_switch_t *switch_text_base_bonding_buds_1 = NULL;
// gui_switch_t *switch_text_base_bonding_buds_2 = NULL;
// gui_switch_t *switch_text_base_bonding_buds_3 = NULL;
gui_switch_t *switch_delete_bonding_buds = NULL;
gui_text_t *text_device_buds_title = NULL;
gui_text_t *text_bonding_buds[8] = {NULL};
// gui_text_t *text_bonding_buds_1 = NULL;
// gui_text_t *text_bonding_buds_2 = NULL;
// gui_text_t *text_bonding_buds_3 = NULL;
gui_text_t *text_delete_bonding_buds = NULL;
char *txet_delete_buds = "确认删除绑定？";
extern gui_win_t *win_buds_device;
extern gui_win_t *win_confirm;
extern gui_win_t *win_menu_buds;

static bool gui_create_connect_by_user = false;
uint8_t gui_create_connect_by_user_addr[6] = {0};
static int8_t device_count = 0;
static int8_t bond_device_index[MAX_BOND_INFO_NUM];
static int16_t gap_y = 20;

void gui_free_win_buds_device(void)
{
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (switch_text_base_bonding_buds[i] != NULL)
        {
            gui_tree_free(switch_text_base_bonding_buds[i]);
            switch_text_base_bonding_buds[i] = NULL;
        }
    }
    if (win_buds_device != NULL)
    {
        gui_tree_free(win_buds_device);
        win_buds_device = NULL;
    }
}

static void switch_text_base_bonding_buds_touch_cb(void *obj, gui_event_t event);

void gui_set_create_connect_by_user(bool flag)
{
    gui_create_connect_by_user = flag;
}

bool gui_get_create_connect_by_user(void)
{
    return gui_create_connect_by_user;
}

void gui_set_create_connect_by_user_addr(uint8_t *addr)
{
    memcpy(gui_create_connect_by_user_addr, addr, 6);
}

uint8_t *gui_get_create_connect_by_user_addr(void)
{
    return gui_create_connect_by_user_addr;
}

static void switch_back_device_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_device_buds_touch_cb, event = %d\n", event);
    // win_menu_buds->base.not_show = false;
    // win_buds_device->base.not_show = true;

    gui_app_t *app = get_app_watch_ui();
    if (win_menu_buds == NULL)
    {
        win_menu_buds = gui_win_create(&(app->screen), "win_menu_buds", 0, 0, 454, 454);
        extern void design_win_menu_buds(void *parent);
        design_win_menu_buds(win_menu_buds);
    }
    gui_free_win_buds_device();
}

static void win_buds_device_update_cb(void *p)
{
    gui_log("win_buds_device_update_cb\n");
    switch_delete_bonding_buds->base.not_show = true;
    if (gui_get_create_connect_by_user())
    {
        gui_set_create_connect_by_user(false);
        T_IO_MSG con_msg;
        con_msg.type = IO_MSG_TYPE_WRISTBNAD;
        con_msg.subtype = IO_MSG_CONNECT_BREDR_DEVICE;
        con_msg.u.buf = gui_get_create_connect_by_user_addr();
        app_send_msg_to_apptask(&con_msg);
    }
    //recreate all swtiches widget
    device_count = 0;
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (switch_text_base_bonding_buds[i] != NULL)
        {
            gui_tree_free(switch_text_base_bonding_buds[i]);
            switch_text_base_bonding_buds[i] = NULL;
        }
        bond_device_index[i] = -1;
        if (app_db.bond_device[i].exist_addr_flag &&
            app_db.bond_device[i].device_type == T_DEVICE_TYPE_EARPHONE)
        {
            bond_device_index[device_count] = i;
            device_count ++;
        }
    }

    for (uint8_t i = 0; i < device_count; i++)
    {
        switch_text_base_bonding_buds[i] = gui_switch_create(page_device_buds, 83, 111 + (64 + gap_y) * i,
                                                             288, 64,
                                                             ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
        switch_text_base_bonding_buds[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_text_base_bonding_buds[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_text_base_bonding_buds[i]->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
        switch_text_base_bonding_buds[i]->long_touch_enable = true;
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

        text_bonding_buds[i] = gui_text_create(switch_text_base_bonding_buds[i], "text_bonding_buds", 16,
                                               16, 192, FONT_H_32);
        gui_text_set(text_bonding_buds[i], app_db.bond_device[bond_device_index[i]].device_name,
                     "rtk_font_mem", APP_COLOR_WHITE,
                     app_db.bond_device[bond_device_index[i]].device_name_len,
                     FONT_H_32);
        gui_text_encoding_set(text_bonding_buds[i], UNICODE_ENCODING);
    }
}

static void img_success_touch_cb(void *obj, gui_event_t e)
{
    gui_log("img_success_touch_cb");
    if (e == GUI_EVENT_TOUCH_CLICKED)
    {
        gui_app_t *app = get_app_watch_ui();
        if (win_menu_buds == NULL)
        {
            win_menu_buds = gui_win_create(&(app->screen), "win_menu_buds", 0, 0, 454, 454);
            extern void design_win_menu_buds(void *parent);
            design_win_menu_buds(win_menu_buds);
        }
        gui_free_win_buds_device();
    }
}

static void img_fail_touch_cb(void *obj, gui_event_t e)
{
    gui_log("img_fail_touch_cb");
    if (e == GUI_EVENT_TOUCH_CLICKED)
    {
        gui_app_t *app = get_app_watch_ui();
        if (win_menu_buds == NULL)
        {
            win_menu_buds = gui_win_create(&(app->screen), "win_menu_buds", 0, 0, 454, 454);
            extern void design_win_menu_buds(void *parent);
            design_win_menu_buds(win_menu_buds);
        }
        gui_free_win_buds_device();
        //todo: action of connect buds fail
    }
}

static void img_loading_animate_callback(gui_img_t *img)
{
    gui_log("img_loading_animate_callback");
    gui_img_translate(img, img->base.w / 2, img->base.h / 2);
    gui_img_rotation(img, 360 * img->animate->progress_percent, img->base.w / 2, img->base.h / 2);
    for (uint8_t i = 0; i < device_count; i++)
    {
        if (switch_text_base_bonding_buds[i] != NULL)
        {
            gui_switch_is_off(switch_text_base_bonding_buds[i]);
            switch_text_base_bonding_buds[i]->touch_disable = true;
        }
        else
        {
            continue;
        }
    }
    //connect success
    //1. no connected buds -> connect new buds
    //2. connected buds -> disconnect -> connect new buds
    if (!gui_get_create_connect_by_user() && (gui_get_bt_status() == STATE_CONNECTED_EARPHONE ||
                                              gui_get_bt_status() == STATE_CONNECTED_TWO))
    {
        img->animate->animate = false;
        gui_img_t *img_success = gui_img_create_from_mem(win_buds_device, "img_success", ICON_SUCCESS_BIN,
                                                         163, 163, 128, 128);
        gui_obj_add_event_cb(img_success, (gui_event_cb_t)img_success_touch_cb, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
    }
    //connect animate timeout
    if (img->animate->progress_percent > 0.99f && img->animate->current_repeat_count >= 1
        && (gui_get_bt_status() != STATE_CONNECTED_EARPHONE || gui_get_bt_status() != STATE_CONNECTED_TWO))
    {
        img->animate->animate = false;
        gui_img_t *img_fail = gui_img_create_from_mem(win_buds_device, "img_fail", ICON_FAIL_BIN, 163, 163,
                                                      128, 128);
        gui_obj_add_event_cb(img_fail, (gui_event_cb_t)img_fail_touch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}

static void searching_buds_general_touch_cb(void)
{
//    gui_img_t *img_base_menu_background = gui_img_create_from_mem(win_buds_device,
//                                                                  "img_base_menu_background",
//                                                                  WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
//                                                                  454, 454);

    gui_img_t *img_loading = gui_img_create_from_mem(win_buds_device, "img_loading", ICON_LOADING_BIN,
                                                     179, 179, 96, 96);
    gui_img_set_animate(img_loading, 1000, 2, img_loading_animate_callback, img_loading);

}

static void switch_text_base_bonding_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_switch_t *this = (gui_switch_t *)obj;
    gui_log("switch_text_base_bonding_buds_touch_cb, event = %d\n", event);
    uint8_t touch_index = 0;
    bool long_touch_state = false;

    //enter delete state
    for (; touch_index < device_count; touch_index ++)
    {
        if (this == switch_text_base_bonding_buds[touch_index])
        {
            break;
        }
    }

    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            //play animate
            if (!long_touch_state)
            {
                searching_buds_general_touch_cb();
                if (!(app_db.bond_device[bond_device_index[touch_index]].used) &&
                    app_db.bond_device[bond_device_index[touch_index]].exist_addr_flag)
                {
                    for (uint8_t i = 0; i < 8; i++)
                    {
                        if (app_db.bond_device[i].used &&
                            app_db.bond_device[i].device_type == T_DEVICE_TYPE_EARPHONE)
                        {
                            gui_set_create_connect_by_user(true);
                            gui_set_create_connect_by_user_addr(app_db.bond_device[bond_device_index[touch_index]].bd_addr);
                        }
                    }
                    T_IO_MSG con_msg;
                    con_msg.type = IO_MSG_TYPE_WRISTBNAD;
                    con_msg.subtype = IO_MSG_CONNECT_BREDR_DEVICE;
                    con_msg.u.buf = app_db.bond_device[bond_device_index[touch_index]].bd_addr;
                    app_send_msg_to_apptask(&con_msg);
                }
            }
            else
            {
                //this->long_touch_state = !this->long_touch_state;
            }
            break;
        }
    case GUI_EVENT_TOUCH_LONG:
        {
            for (uint8_t i = 0; i < device_count; i++)
            {
                switch_delete_bonding_buds->base.not_show = false;
                if (switch_text_base_bonding_buds[i]->long_touch_state)
                {
                    long_touch_state = true;
                    switch_delete_bonding_buds->base.not_show = false;
                    break;
                }
            }
            break;
        }
    default:
        break;
    }
}

void device_buds_confirm_yes_action(void *obj)
{
    gui_log("device_buds_confirm_yes_action, obj = 0x%x\n", obj);
    win_buds_device->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
    //delete bondlist action

    for (uint8_t i = 0; i < device_count; i++)
    {
        if (switch_text_base_bonding_buds[i]->long_touch_state)
        {
            if ((app_db.bond_device[bond_device_index[i]].exist_addr_flag) &&
                (app_find_br_link(app_db.bond_device[bond_device_index[i]].bd_addr) == NULL))
            {
                APP_PRINT_INFO0("delete start");
                if (!bt_bond_delete(app_db.bond_device[bond_device_index[i]].bd_addr))
                {
                    APP_PRINT_INFO0("bt_bond_delete fail");
                }
                app_bt_bond_del_bond_device(app_db.bond_device[bond_device_index[i]].bd_addr);
                app_bt_bond_save_device_info_to_ftl(app_db.bond_device);
                gui_update_by_event2(win_buds_device, NULL, true);
            }
        }
    }
}

void device_buds_confirm_no_action(void *obj)
{
    gui_log("device_buds_confirm_no_action, obj = 0x%x\n", obj);
    win_buds_device->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
    //cancel delete bondlist action

}

void win_device_buds_confirm_text_display(void)
{
    gui_log("win_device_buds_confirm_text_display\n");
}

static void switch_delete_bonding_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_delete_bonding_buds_touch_cb, event = %d\n", event);
    push_current_widget(win_buds_device);
    gui_obj_show(win_buds_device, false);

    gui_app_t *app = get_app_watch_ui();
    //if (win_confirm == NULL)
    {
        win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
        extern void set_confirm_info(void *yes_action, void *no_action, void *text_display);

        set_confirm_yes(device_buds_confirm_yes_action, obj);
        set_confirm_no(device_buds_confirm_no_action, obj);
        set_confirm_text(txet_delete_buds, 123, 131, 7);

        extern void design_win_confirm(void *parent);
        design_win_confirm(win_confirm);
    }
}

void design_win_buds_device(void *parent)
{
    //add animate for update
    gui_win_t *win = (gui_win_t *)parent;
    gui_win_set_animate((gui_win_t *)win, 100, 1, win_buds_device_update_cb, NULL);
    win->animate->animate = false;

    page_device_buds = gui_page_create(parent, "page_device_buds", 0, 0, LCD_W, LCD_H);

    switch_back_device_buds = gui_switch_create(page_device_buds, 131, 24, 48, 48, ICON_BACK_BIN,
                                                ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_device_buds, (gui_event_cb_t)switch_back_device_buds_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_device_buds, (gui_event_cb_t)switch_back_device_buds_touch_cb,
                         GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    text_device_buds_title = gui_text_create(page_device_buds, "text_device_buds_title", 179, 24, 128,
                                             font_size);
    gui_text_set(text_device_buds_title, "绑定信息", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);


    switch_delete_bonding_buds = gui_switch_create(parent, 88, 373, 277, 81, ICON_DISCONNECT_BIN,
                                                   ICON_DISCONNECT_BIN);
    switch_delete_bonding_buds->off_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    switch_delete_bonding_buds->on_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    gui_obj_add_event_cb(switch_delete_bonding_buds,
                         (gui_event_cb_t)switch_delete_bonding_buds_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_delete_bonding_buds,
                         (gui_event_cb_t)switch_delete_bonding_buds_touch_cb, GUI_EVENT_2, NULL);
    switch_delete_bonding_buds->base.not_show = true;

    text_delete_bonding_buds = gui_text_create(switch_delete_bonding_buds, "text_disconnect", 75, 19,
                                               128, font_size);
    gui_text_set(text_delete_bonding_buds, "删除绑定", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);

    //get bond device index
    device_count = 0;
    for (uint8_t i = 0; i < MAX_BOND_INFO_NUM; i++)
    {
        if (switch_text_base_bonding_buds[i] != NULL)
        {
            gui_tree_free(switch_text_base_bonding_buds[i]);
            switch_text_base_bonding_buds[i] = NULL;
        }
        bond_device_index[i] = -1;
        if (app_db.bond_device[i].exist_addr_flag &&
            app_db.bond_device[i].device_type == T_DEVICE_TYPE_EARPHONE)
        {
            bond_device_index[device_count] = i;
            device_count ++;
        }
    }

    for (uint8_t i = 0; i < device_count; i++)
    {
        switch_text_base_bonding_buds[i] = gui_switch_create(page_device_buds, 83, 111 + (64 + gap_y) * i,
                                                             288, 64,
                                                             ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
        switch_text_base_bonding_buds[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_text_base_bonding_buds[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_text_base_bonding_buds[i]->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
        switch_text_base_bonding_buds[i]->long_touch_enable = true;
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(switch_text_base_bonding_buds[i],
                             (gui_event_cb_t)switch_text_base_bonding_buds_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

        text_bonding_buds[i] = gui_text_create(switch_text_base_bonding_buds[i], "text_bonding_buds", 16,
                                               16, 192, FONT_H_32);
        gui_text_set(text_bonding_buds[i], app_db.bond_device[bond_device_index[i]].device_name,
                     GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     app_db.bond_device[bond_device_index[i]].device_name_len,
                     FONT_H_32);
        gui_text_encoding_set(text_bonding_buds[i], UNICODE_ENCODING);
    }
}
#endif
