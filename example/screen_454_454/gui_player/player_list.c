#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_switch.h"
#include "gui_scroll_text.h"
#include "gui_pagelistview.h"
#include "gui_pagelist.h"
#include "player_mgr.h"
#ifndef _WIN32
#include "fs_if.h"
#include "app_task.h"
#include "gui_interface.h"
#endif

#define MAX_SHOW_FILE_LIST_NUM      4
#define MAX_NAME_LEN                50

//Global widget handle which need to use in the callbacks
gui_switch_t *switch_song_files[MAX_SHOW_FILE_LIST_NUM] = {NULL};
gui_scroll_text_t *scroll_text_pagelist_song[MAX_SHOW_FILE_LIST_NUM] = {NULL};

static int8_t show_num = MAX_SHOW_FILE_LIST_NUM;
static int16_t real_num;
static int16_t list_gap_y = 20;
static int16_t slide_index = 0;
static int16_t play_index = 0;

#ifndef _WIN32
T_HEAD_INFO  *header_info = (T_HEAD_INFO *)(MUSIC_HEADER_BIN_ADDR +
                                            FS_HEADER_INFO_START);
//delete songs by ble/spp transfer,header bin total num is not updated
uint16_t  *song_count = (uint16_t *)MUSIC_HEADER_BIN_ADDR;
#endif


#ifndef _WIN32
uint16_t get_song_count(void)
{
    return *song_count;
}
#endif

#ifndef _WIN32
T_HEAD_INFO *get_cur_play_header_info(void)
{
    return (header_info + play_index);
}
#endif
void play_next_music(void)
{
#ifndef _WIN32
    play_index++;

    if (play_index >= *song_count)
    {
        play_index = 0;
    }

    APP_PRINT_INFO2("play next:play_index = %d, song_count = %d",
                    play_index, (*song_count));

    T_IO_MSG play_msg;
    play_msg.type = IO_MSG_TYPE_WRISTBNAD;
    play_msg.subtype = IO_MSG_PLAY_BY_NAME;
    play_msg.u.param = (uint32_t)(header_info + play_index);
    app_send_msg_to_apptask(&play_msg);
#endif
}

void play_prev_music(void)
{
#ifndef _WIN32
    play_index--;

    if (play_index <= 0)
    {
        play_index = 0;
    }
    APP_PRINT_INFO2("play prev:play_num = %d, song_count = %d,",
                    play_index, (*song_count));

    T_IO_MSG play_msg;
    play_msg.type = IO_MSG_TYPE_WRISTBNAD;
    play_msg.subtype = IO_MSG_PLAY_BY_NAME;
    play_msg.u.param = (uint32_t)(header_info + play_index);
    app_send_msg_to_apptask(&play_msg);
#endif
}

void play_select_music(uint16_t index)
{
#ifndef _WIN32
    play_index = index;

    if (play_index <= 0)
    {
        play_index = 0;
    }
    if (play_index >= *song_count)
    {
        play_index = 0;
    }
    APP_PRINT_INFO2("play select:play_num = %d, song_count = %d,",
                    play_index, (*song_count));

    T_IO_MSG play_msg;
    play_msg.type = IO_MSG_TYPE_WRISTBNAD;
    play_msg.subtype = IO_MSG_PLAY_BY_NAME;
    play_msg.u.param = (uint32_t)(header_info + play_index);
    app_send_msg_to_apptask(&play_msg);
#endif
}

static void switch_song_back_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_song_back_cb event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_TOUCH_RELEASED:
        app_player_switch_tabs(PLAYER_MAIN);
        break;

    default:
        break;
    }
}

static void switch_song_cb(void *obj, uint16_t event, void *param)
{
    gui_log("switch_song_1_cb event = %d\n", event);
    gui_switch_t *this = (gui_switch_t *)obj;

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
            uint8_t i;
            for (i = 0; i < MAX_SHOW_FILE_LIST_NUM; i++)
            {
                if (switch_song_files[i] == this)
                {
                    break;
                }
            }
            int16_t m = slide_index / MAX_SHOW_FILE_LIST_NUM;
            int16_t n = slide_index % MAX_SHOW_FILE_LIST_NUM;
            int16_t index_to_play;
            if (i < n)
            {
                index_to_play = (m + 1) * 4 + i;
            }
            else
            {
                index_to_play = m * 4 + i;
            }
            play_select_music(index_to_play);

            app_player_switch_tabs(PLAYER_MAIN);
        }
        break;
    default:
        break;
    }
}

static void pagelist_test_update_list_first_cb(gui_pagelist_t *this, gui_switch_t *list_first)
{
    gui_log("pagelist_test_update_list_first_cb\n");

#ifndef _WIN32
    slide_index++;
    if (slide_index > 0x7FFF  - MAX_SHOW_FILE_LIST_NUM)
    {
        slide_index = 0x7FFF  - MAX_SHOW_FILE_LIST_NUM;
    }

    if (get_song_count() != 0xFFFF)
    {
        uint16_t *name_buf = (uint16_t *)(MUSIC_NAME_BIN_ADDR + header_info[MAX_SHOW_FILE_LIST_NUM - 1 +
                                                                                                   slide_index].offset);
        gui_scroll_text_set(scroll_text_pagelist_song[(slide_index - 1) % 4], name_buf,
                            GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                            header_info[MAX_SHOW_FILE_LIST_NUM - 1 + slide_index].length - 1,
                            FONT_H_32);
    }
#endif
}

static void pagelist_test_update_list_last_cb(gui_pagelist_t *obj, gui_switch_t *list_last)
{
    gui_log("pagelist_test_update_list_last_cb\n");

#ifndef _WIN32
    slide_index--;
    if (slide_index < 0)
    {
        slide_index = 0;
    }

    if (get_song_count() != 0xFFFF)
    {
        uint16_t *name_buf = (uint16_t *)(MUSIC_NAME_BIN_ADDR + header_info[slide_index].offset);
        gui_scroll_text_set(scroll_text_pagelist_song[slide_index % 4], name_buf,
                            GUI_FONT_SRC_BMP, APP_COLOR_WHITE, header_info[slide_index].length - 1, FONT_H_32);
    }
#endif
}

static void tab_player_list_update_cb(void *obj, uint16_t event, void *param)
{
    gui_log("tab_player_list_update_cb  event = 0x%x\n", event);
    app_player_switch_tabs(PLAYER_MAIN);
    app_player_switch_tabs(PLAYER_LIST);//update player list
}

void design_tab_song_lists(void *parent)
{
    char *string_song_list_title = "播放列表";
    gui_obj_add_event_cb(parent, (gui_event_cb_t)tab_player_list_update_cb,
                         (gui_event_t)GUI_EVENT_PLAYER,
                         NULL);
#ifndef _WIN32
    if (get_song_count() != 0xFFFF)
    {
        real_num = get_song_count();
    }
#endif
    //mask should be the brother of pagelistview
    //mask image rgb should be larger than 00 00 00
    gui_pagelistview_t *pagelistview_song = gui_pagelistview_create(parent, "pagelistview", 0, 0, LCD_W,
                                                                    LCD_H);
    gui_img_t *img_top_mask = gui_img_create_from_mem(parent, "top_mask", WATCH_BASE_MASK_BLACK_BIN, 0,
                                                      0,
                                                      LCD_W, 100);
    gui_img_t *img_bottom_mask = gui_img_create_from_mem(parent, "bottom_mask",
                                                         WATCH_BASE_MASK_BLACK_BIN,
                                                         0, 354, LCD_W, 100);

    gui_pagelistview_add_top_mask(pagelistview_song, img_top_mask);
    gui_pagelistview_add_bottom_mask(pagelistview_song, img_bottom_mask);

    gui_pagelist_t *pagelist_song = gui_pagelist_create(pagelistview_song, "pagelist", 0, 0, LCD_W,
                                                        LCD_H);

#ifndef _WIN32
    for (int8_t i = 0; i < MAX_SHOW_FILE_LIST_NUM; i++)
    {
        if (i >= get_song_count())
        {
            break;
        }
        switch_song_files[i] = gui_switch_create(pagelist_song, 83, 111 + i * (list_gap_y + 64), 288, 64,
                                                 ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
        switch_song_files[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_song_files[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        //add touch event callback
        //GUI_EVENT_1 for switch is off(turn-on action)
        //GUI_EVENT_2 for switch is on(turn-off action)
        gui_obj_add_event_cb(switch_song_files[i], (gui_event_cb_t)switch_song_cb, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_song_files[i], (gui_event_cb_t)switch_song_cb, GUI_EVENT_2, NULL);

        scroll_text_pagelist_song[i] = gui_scroll_text_create(switch_song_files[i],
                                                              "scroll_text_record_files", 10, 12, 268, FONT_H_32);
        gui_scroll_text_encoding_set(scroll_text_pagelist_song[i], UNICODE_ENCODING);

        uint16_t *name_buf = (uint16_t *)(MUSIC_NAME_BIN_ADDR + header_info[i].offset);

        gui_scroll_text_set(scroll_text_pagelist_song[i], name_buf,
                            GUI_FONT_SRC_BMP, APP_COLOR_WHITE, header_info[i].length - 1, FONT_H_32);
        gui_scroll_text_scroll_set(scroll_text_pagelist_song[i], SCROLL_X, 0, 0, 5000, 0);
    }
#endif

    gui_pagelist_set_att(pagelist_song, real_num, show_num, list_gap_y, switch_song_files[0],
                         switch_song_files[MAX_SHOW_FILE_LIST_NUM - 1]);
    //pagelist_test->show_border_bottom = 354;
    //pagelist_test->show_border_top = 100;
    gui_pagelist_add_list_update_cb(pagelist_song,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_first_cb,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_last_cb);

    gui_img_create_from_mem(parent, "img_back", ICON_BACK_BIN, 131, 24, 48, 48);
    gui_text_t *text_song_list_title = gui_text_create(parent, "text_song_list_title", 177, 30,
                                                       strlen(string_song_list_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_song_list_title, string_song_list_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_song_list_title), FONT_H_32);

    gui_win_t *win_song_lists_back = gui_win_create(parent, "win_song_lists_back", 131, 24, 176, 48);
    gui_obj_add_event_cb(win_song_lists_back, (gui_event_cb_t)switch_song_back_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);

    slide_index = 0;

}

