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
#include "draw_font.h"
//for bt
#include "app_bond.h"
#include "app_gap.h"
#include "app_task.h"


gui_switch_t *switch_back_search_buds = NULL;
gui_switch_t *switch_text_base_search_buds_1 = NULL;
gui_switch_t *switch_text_base_search_buds_2 = NULL;
//gui_switch_t *switch_text_base_search_buds_3 = NULL;
gui_text_t *text_search_buds_title = NULL;
gui_text_t *text_search_buds_1 = NULL;
gui_text_t *text_search_buds_2 = NULL;
//gui_text_t *text_search_buds_3 = NULL;
extern gui_win_t *win_search_buds;
extern gui_win_t *win_menu_buds;
char bd_addr[20];
T_APP_BOND_DEVICE *temp_search_device[TEMP_MAX_SEARCH] = {NULL};

static void win_search_buds_update_cb(void *p)
{
    gui_log("win_search_buds_update_cb\n");

    //update found devices
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        temp_search_device[i] = app_bt_bond_get_temp_cache_device_by_index(i);
    }
    if (temp_search_device[0] != NULL)
    {
        if (temp_search_device[0]->device_name_len == 0)
        {
            sprintf(bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    temp_search_device[0]->bd_addr[5],
                    temp_search_device[0]->bd_addr[4],
                    temp_search_device[0]->bd_addr[3],
                    temp_search_device[0]->bd_addr[2],
                    temp_search_device[0]->bd_addr[1],
                    temp_search_device[0]->bd_addr[0]);
            gui_text_set(text_search_buds_1, bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, FONT_H_32);
        }
        else
        {
            gui_text_set(text_search_buds_1, &temp_search_device[0]->device_name, GUI_FONT_SRC_BMP,
                         APP_COLOR_WHITE,
                         temp_search_device[0]->device_name_len,
                         FONT_H_32);
            gui_text_encoding_set(text_search_buds_1, UNICODE_ENCODING);
        }
    }
    if (temp_search_device[1] != NULL)
    {
        if (temp_search_device[1]->device_name_len == 0)
        {
            sprintf(bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    temp_search_device[1]->bd_addr[5],
                    temp_search_device[1]->bd_addr[4],
                    temp_search_device[1]->bd_addr[3],
                    temp_search_device[1]->bd_addr[2],
                    temp_search_device[1]->bd_addr[1],
                    temp_search_device[1]->bd_addr[0]);
            gui_text_set(text_search_buds_2, bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, FONT_H_32);
        }
        else
        {
            gui_text_set(text_search_buds_2, &temp_search_device[1]->device_name, GUI_FONT_SRC_BMP,
                         APP_COLOR_WHITE,
                         temp_search_device[1]->device_name_len,
                         FONT_H_32);
            gui_text_encoding_set(text_search_buds_2, UNICODE_ENCODING);
        }
    }
    //the connected buds has already disconn
    //send connect msg to app task again
    if (gui_get_create_connect_by_user())
    {
        gui_set_create_connect_by_user(false);
        T_IO_MSG con_msg;
        con_msg.type = IO_MSG_TYPE_WRISTBNAD;
        con_msg.subtype = IO_MSG_CONNECT_BREDR_DEVICE;
        con_msg.u.buf = gui_get_create_connect_by_user_addr();
        app_send_msg_to_apptask(&con_msg);
    }
}

static void switch_back_buds_searching_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_buds_searching_touch_cb, event = %d\n", event);
    //win_menu_buds->base.not_show = false;
    //win_search_buds->base.not_show = true;

    gui_app_t *app = get_app_watch_ui();
    if (win_menu_buds == NULL)
    {
        win_menu_buds = gui_win_create(&(app->screen), "win_menu_buds", 0, 0, 454, 454);
        extern void design_win_menu_buds(void *parent);
        design_win_menu_buds(win_menu_buds);
    }
    gui_tree_free(win_search_buds);
    win_search_buds = NULL;
    temp_search_device[0] = NULL;
    temp_search_device[1] = NULL;

#if (F_APP_AUTO_SUPPORT == 0)
    if (get_search_status() == SEARCH_START)
#endif
    {
        T_IO_MSG dis_inquiry_msg;
        dis_inquiry_msg.type = IO_MSG_TYPE_WRISTBNAD;
        dis_inquiry_msg.subtype = IO_MSG_INQUIRY_STOP;
        app_send_msg_to_apptask(&dis_inquiry_msg);
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
        gui_tree_free(win_search_buds);
        win_search_buds = NULL;
        temp_search_device[0] = NULL;
        temp_search_device[1] = NULL;
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
        gui_tree_free(win_search_buds);
        win_search_buds = NULL;
        temp_search_device[0] = NULL;
        temp_search_device[1] = NULL;
        //todo: action of connect buds fail
    }
}

static void img_loading_animate_callback(gui_img_t *img)
{
    gui_log("img_loading_animate_callback");
    gui_img_translate(img, img->base.w / 2, img->base.h / 2);
    gui_img_rotation(img, 360 * img->animate->progress_percent, img->base.w / 2, img->base.h / 2);
    //connect success
    //1. no connected buds -> connect new buds
    //2. connected buds -> disconnect -> connect new buds
    if (!gui_get_create_connect_by_user() && (gui_get_bt_status() == STATE_CONNECTED_EARPHONE ||
                                              gui_get_bt_status() == STATE_CONNECTED_TWO))
    {
        img->animate->animate = false;
        gui_img_t *img_success = gui_img_create_from_mem(win_search_buds, "img_success", ICON_SUCCESS_BIN,
                                                         163, 163, 128, 128);
        gui_obj_add_event_cb(img_success, (gui_event_cb_t)img_success_touch_cb, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
    }
    //connect animate timeout
    if (img->animate->progress_percent > 0.99f && img->animate->current_repeat_count >= 1
        && (gui_get_bt_status() != STATE_CONNECTED_EARPHONE || gui_get_bt_status() != STATE_CONNECTED_TWO))
    {
        img->animate->animate = false;
        gui_img_t *img_fail = gui_img_create_from_mem(win_search_buds, "img_fail", ICON_FAIL_BIN, 163, 163,
                                                      128, 128);
        gui_obj_add_event_cb(img_fail, (gui_event_cb_t)img_fail_touch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        // bool test_flag = true;//only for test
        // img->animate->dur = 0;
        // if (test_flag)
        // {
        //     gui_img_t *img_success = gui_img_create_from_mem(win_search_buds, "img_success", ICON_SUCCESS_BIN,
        //                                                      163, 163,
        //                                                      128, 128);
        //     gui_img_set_animate(img_success, 500, 0, img_success_animate_callback, img_success);
        // }
        // else
        // {
        //     gui_img_t *img_fail = gui_img_create_from_mem(win_search_buds, "img_fail", ICON_FAIL_BIN, 163, 163,
        //                                                   128, 128);
        // }
    }
}

static void searching_buds_general_touch_cb(void)
{
    gui_img_t *img_base_menu_background = gui_img_create_from_mem(win_search_buds,
                                                                  "img_base_menu_background",
                                                                  WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                                                  454, 454);
    gui_img_t *img_back_search_buds = gui_img_create_from_mem(win_search_buds, "img_back_search_buds",
                                                              ICON_BACK_BIN, 131, 24, 48, 48);
    gui_img_t *img_search_buds_1 = gui_img_create_from_mem(win_search_buds, "img_search_buds_1",
                                                           ICON_TEXT_BASE_DARK_BIN, 83, 111, 288, 64);
    img_search_buds_1->draw_img.opacity_value = 128;
    gui_img_t *img_search_buds_2 = gui_img_create_from_mem(win_search_buds, "img_search_buds_2",
                                                           ICON_TEXT_BASE_DARK_BIN, 83, 195, 288, 64);
    img_search_buds_2->draw_img.opacity_value = 128;
    // gui_img_t *img_search_buds_3 = gui_img_create_from_mem(win_search_buds, "img_search_buds_3",
    //                                                        ICON_TEXT_BASE_DARK_BIN, 83, 275, 288, 64);
    // img_search_buds_3->draw_img.opacity_value = 128;

    gui_obj_show(switch_back_search_buds, false);
    gui_obj_show(switch_text_base_search_buds_1, false);
    gui_obj_show(switch_text_base_search_buds_2, false);
    // gui_obj_show(switch_text_base_search_buds_3, false);
// todo
    int16_t font_size = 32;
    text_search_buds_title = gui_text_create(win_search_buds, "text_search_buds_title", 179, 24, 128,
                                             font_size);
    gui_text_set(text_search_buds_title, "搜索耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_type_set(text_search_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_1 = gui_text_create(win_search_buds, "text_bonding_buds_1", 138, 122, 178,
                                         font_size);
    gui_text_set(text_search_buds_1, "搜索到设备1", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 16,
                 font_size);
    gui_text_type_set(text_search_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_2 = gui_text_create(win_search_buds, "text_search_buds_2", 138, 204, 178,
                                         font_size);
    gui_text_set(text_search_buds_2, "搜索到设备2", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 16,
                 font_size);
    gui_text_type_set(text_search_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    // text_search_buds_3 = gui_text_create(win_search_buds, "text_search_buds_3", 138, 286, 178,
    //                                      font_size);
    // gui_text_set(text_search_buds_3, "搜索到设备3", GUI_FONT_SRC_BMP, 0xffffffff, 16, font_size);
    // gui_text_type_set(text_search_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);

    gui_img_t *img_loading = gui_img_create_from_mem(win_search_buds, "img_loading", ICON_LOADING_BIN,
                                                     179, 179, 96, 96);
    gui_img_set_animate(img_loading, 1000, 2, img_loading_animate_callback, img_loading);
}

static void switch_text_base_searching_buds_1_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_searching_buds_1_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            //play animate
            searching_buds_general_touch_cb();
            //send to app task
            temp_search_device[0] = app_bt_bond_get_temp_cache_device_by_index(0);
            if (temp_search_device[0])
            {
                //check if found device was already saved in bond list?
                if (app_bt_bond_check_active_device_info_by_addr(temp_search_device[0]->bd_addr))
                {
                    APP_PRINT_INFO0("connect active device error");
                }
                else
                {
                    app_bt_bond_temp_cache_save_to_search();
#if (F_APP_AUTO_SUPPORT == 0)
                    if (get_search_status() == SEARCH_START)
#endif
                    {
                        T_IO_MSG dis_inquiry_msg;
                        dis_inquiry_msg.type = IO_MSG_TYPE_WRISTBNAD;
                        dis_inquiry_msg.subtype = IO_MSG_INQUIRY_STOP;
                        app_send_msg_to_apptask(&dis_inquiry_msg);
                    }
                    //
                    for (uint8_t i = 0; i < 8; i++)
                    {
                        if (app_db.bond_device[i].used &&
                            app_db.bond_device[i].device_type == T_DEVICE_TYPE_EARPHONE)
                        {
                            gui_set_create_connect_by_user(true);
                            gui_set_create_connect_by_user_addr(temp_search_device[0]->bd_addr);
                        }
                    }
                    T_IO_MSG con_msg;
                    con_msg.type = IO_MSG_TYPE_WRISTBNAD;
                    con_msg.subtype = IO_MSG_CONNECT_BREDR_DEVICE;
                    con_msg.u.buf = temp_search_device[0]->bd_addr;
                    app_send_msg_to_apptask(&con_msg);
                }
            }
            else
            {
                APP_PRINT_INFO0("get temp_search1 error");
            }
            break;
        }
    default:
        break;
    }
}

static void switch_text_base_searching_buds_2_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_searching_buds_2_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            //play animate
            searching_buds_general_touch_cb();
            //send to app task
            temp_search_device[1] = app_bt_bond_get_temp_cache_device_by_index(1);
            if (temp_search_device[1])
            {
                //check if found device was already saved in bond list?
                if (app_bt_bond_check_active_device_info_by_addr(temp_search_device[1]->bd_addr))
                {
                    APP_PRINT_INFO0("connect active device error");
                }
                else
                {
                    app_bt_bond_temp_cache_save_to_search();
#if (F_APP_AUTO_SUPPORT == 0)
                    if (get_search_status() == SEARCH_START)
#endif
                    {
                        //search_status = 0;
                        T_IO_MSG dis_inquiry_msg;
                        dis_inquiry_msg.type = IO_MSG_TYPE_WRISTBNAD;
                        dis_inquiry_msg.subtype = IO_MSG_INQUIRY_STOP;
                        app_send_msg_to_apptask(&dis_inquiry_msg);
                    }
                    //
                    for (uint8_t i = 0; i < 8; i++)
                    {
                        if (app_db.bond_device[i].used &&
                            app_db.bond_device[i].device_type == T_DEVICE_TYPE_EARPHONE)

                        {
                            gui_set_create_connect_by_user(true);
                            gui_set_create_connect_by_user_addr(temp_search_device[1]->bd_addr);
                        }
                    }
                    T_IO_MSG con_msg;
                    con_msg.type = IO_MSG_TYPE_WRISTBNAD;
                    con_msg.subtype = IO_MSG_CONNECT_BREDR_DEVICE;
                    con_msg.u.buf = temp_search_device[1]->bd_addr;
                    app_send_msg_to_apptask(&con_msg);
                }
            }
            else
            {
                APP_PRINT_INFO0("get temp_search1 error");
            }
            break;
        }
    default:
        break;
    }
}

// static void switch_text_base_searching_buds_3_touch_cb(void *obj, gui_event_t event)
// {
//     gui_log("switch_text_base_searching_buds_3_touch_cb, event = %d\n", event);
//     switch (event)
//     {
//     case GUI_EVENT_1: //widget switch is on
//     case GUI_EVENT_2: //widget switch is off
//         {
//             searching_buds_general_touch_cb();
//             break;
//         }

//     default:
//         break;
//     }
// }

void design_win_buds_searching(void *parent)
{
    //add animate for update
    gui_win_t *win = (gui_win_t *)parent;
    gui_win_set_animate((gui_win_t *)win, 100, 1, win_search_buds_update_cb, NULL);
    win->animate->animate = false;

    switch_back_search_buds = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_search_buds, (gui_event_cb_t)switch_back_buds_searching_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_search_buds, (gui_event_cb_t)switch_back_buds_searching_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_search_buds_1 = gui_switch_create(parent, 83, 111, 288, 64,
                                                       ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds_1->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds_1->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

    gui_obj_add_event_cb(switch_text_base_search_buds_1,
                         (gui_event_cb_t)switch_text_base_searching_buds_1_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds_1,
                         (gui_event_cb_t)switch_text_base_searching_buds_1_touch_cb, GUI_EVENT_2, NULL);

    switch_text_base_search_buds_2 = gui_switch_create(parent, 83, 195, 288, 64,
                                                       ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds_2->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds_2->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

    gui_obj_add_event_cb(switch_text_base_search_buds_2,
                         (gui_event_cb_t)switch_text_base_searching_buds_2_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds_2,
                         (gui_event_cb_t)switch_text_base_searching_buds_2_touch_cb, GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    text_search_buds_title = gui_text_create(parent, "text_search_buds_title", 179, 24, 128, font_size);
    gui_text_set(text_search_buds_title, "搜索耳机", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_type_set(text_search_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_1 = gui_text_create(parent, "text_bonding_buds_1", 138, 122, 178, font_size);
    gui_text_set(text_search_buds_1, "搜索中...", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    gui_text_type_set(text_search_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_2 = gui_text_create(parent, "text_search_buds_2", 138, 204, 178, font_size);
    gui_text_set(text_search_buds_2, "搜索中...", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 12, font_size);
    gui_text_type_set(text_search_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    //check if found devices
    for (uint8_t i = 0; i < TEMP_MAX_SEARCH; i++)
    {
        temp_search_device[i] = app_bt_bond_get_temp_cache_device_by_index(i);
    }
    if (temp_search_device[0] != NULL)
    {
        if (temp_search_device[0]->device_name_len == 0)
        {
            sprintf(bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    temp_search_device[0]->bd_addr[5],
                    temp_search_device[0]->bd_addr[4],
                    temp_search_device[0]->bd_addr[3],
                    temp_search_device[0]->bd_addr[2],
                    temp_search_device[0]->bd_addr[1],
                    temp_search_device[0]->bd_addr[0]);
            gui_text_set(text_search_buds_1, bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, font_size);
        }
        else
        {
            gui_text_set(text_search_buds_1, &temp_search_device[0]->device_name, GUI_FONT_SRC_BMP,
                         APP_COLOR_WHITE,
                         temp_search_device[0]->device_name_len,
                         FONT_H_32);
            gui_text_encoding_set(text_search_buds_1, UNICODE_ENCODING);
        }
    }
    if (temp_search_device[1] != NULL)
    {
        if (temp_search_device[1]->device_name_len == 0)
        {
            sprintf(bd_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    temp_search_device[1]->bd_addr[5],
                    temp_search_device[1]->bd_addr[4],
                    temp_search_device[1]->bd_addr[3],
                    temp_search_device[1]->bd_addr[2],
                    temp_search_device[1]->bd_addr[1],
                    temp_search_device[1]->bd_addr[0]);
            gui_text_set(text_search_buds_2, bd_addr, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 17, font_size);
        }
        else
        {
            gui_text_set(text_search_buds_2, &temp_search_device[1]->device_name, GUI_FONT_SRC_BMP,
                         APP_COLOR_WHITE,
                         temp_search_device[1]->device_name_len,
                         FONT_H_32);
            gui_text_encoding_set(text_search_buds_2, UNICODE_ENCODING);
        }
    }
}
#endif
