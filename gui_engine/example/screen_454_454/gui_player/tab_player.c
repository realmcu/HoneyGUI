#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_curtainview.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "gui_scroll_text.h"

#ifndef _WIN32
#include "app_audio_if.h"
#include "app_mmi.h"
#include "app_task.h"
#include "app_hfp.h"
#include "hub_usb.h"
#include "app_playback_update_file.h"
#include "fs_if.h"
#endif


gui_text_t *text_player_title = NULL;
gui_text_t *text_song_name = NULL;
gui_switch_t *switch_song_name = NULL;
gui_switch_t *switch_play_pause = NULL;
gui_switch_t *switch_next = NULL;
gui_switch_t *switch_prev = NULL;
gui_img_t *img_slide_vol = NULL;
gui_img_t *img_spk = NULL;
gui_img_t *img_song_local = NULL;
gui_img_t *img_process = NULL;
gui_img_t *img_process_node = NULL;
gui_curtainview_t *curtainview_player_vol = NULL;
gui_scroll_text_t *scroll_text_song_name = NULL;

extern void play_next_music(void);
#ifndef _WIN32
extern T_HEAD_INFO *get_cur_play_header_info(void);
#endif

static uint8_t utf8_name[50];

void tab_player_update_cb(void *p)
{
    gui_log("tab_player_update_cb\n");

#ifndef _WIN32

    if (*(uint16_t *)MUSIC_HEADER_BIN_ADDR == 0xFFFF)
    {
        return;
    }
    T_HEAD_INFO *header_info = get_cur_play_header_info();
    unicode_to_utf8(utf8_name, 49, \
                    (uint16_t *)(MUSIC_NAME_BIN_ADDR + header_info->offset), \
                    header_info->length / 2);

    gui_scroll_text_set(scroll_text_song_name, (void *)utf8_name,
                        "rtk_font_mem", APP_COLOR_WHITE, strlen((const char *)utf8_name), FONT_H_32);

    if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
    {
        gui_switch_is_on(switch_play_pause);
    }
    else if ((app_audio_get_play_status() == APP_AUDIO_STATE_STOP) || \
             (app_audio_get_play_status() == APP_AUDIO_STATE_PAUSE))
    {
        gui_switch_is_off(switch_play_pause);
    }
#endif

}

static void switch_song_name_cb(void *obj, gui_event_t event)
{
    gui_log("switch_song_name_cb event = %d\n", event);

    //for delete and record play windows creation
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_song_lists;

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        if (win_song_lists != NULL)
        {
            gui_tree_free(win_song_lists);
            win_song_lists = NULL;
        }
        win_song_lists = gui_win_create(&(app->screen), "win_song_lists", 0, 0, LCD_W, LCD_H);
        extern void design_win_song_lists(gui_win_t *parent);
        design_win_song_lists(win_song_lists);
        push_current_widget(tabview_main);
        gui_obj_show(tabview_main, false);

        break;

    default:
        break;
    }
}

static void switch_play_pause_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_play_pause_touch_cb event = %d\n", event);

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
#ifndef _WIN32
            if (playback_db.sd_play_state < APP_AUDIO_STATE_TRY_STOPPING)
            {
                if (app_audio_get_play_status() != APP_AUDIO_STATE_STOP)
                {
                    T_IO_MSG play_msg;
                    play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                    play_msg.subtype = IO_MSG_MMI;
                    play_msg.u.param = MMI_AV_PLAY_PAUSE;
                    app_send_msg_to_apptask(&play_msg);
                }
                else if (app_db.usb_status == USB_STOPPED && app_hfp_get_call_status() == APP_HFP_CALL_IDLE &&
                         app_db.batt.allow_open.playback && app_db.transfer_status == TRANSFER_STOPPED)
                {
                    T_IO_MSG play_msg;
                    play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                    play_msg.subtype = IO_MSG_PLAY_BY_NAME;
                    play_msg.u.param = (uint32_t)(get_cur_play_header_info());
                    app_send_msg_to_apptask(&play_msg);
                }
            }
#endif
        }
        break;

    default:
        break;
    }
}

static void switch_next_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_next_touch_cb event = %d\n", event);

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
#ifndef _WIN32
            if (playback_db.sd_play_state < APP_AUDIO_STATE_TRY_STOPPING)
            {
                T_IO_MSG play_msg;
                play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                play_msg.subtype = IO_MSG_MMI;
                play_msg.u.param = MMI_AV_FWD;
                app_send_msg_to_apptask(&play_msg);
            }
#endif
        }
        break;

    default:
        break;
    }
}

static void switch_prev_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_prev_touch_cb event = %d\n", event);

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
#ifndef _WIN32
            if (playback_db.sd_play_state < APP_AUDIO_STATE_TRY_STOPPING)
            {
                T_IO_MSG play_msg;
                play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                play_msg.subtype = IO_MSG_MMI;
                play_msg.u.param = MMI_AV_BWD;
                app_send_msg_to_apptask(&play_msg);
            }
#endif
        }
        break;

    default:
        break;
    }
}

static void curtainview_set_done_cb_player_vol(gui_curtainview_t *this)
{
    gui_log("curtainview_set_done_cb_player_vol\n");
    if (this->cur_curtain == CURTAIN_DOWN)
    {
        //hide switch_record to aviod trig
        gui_log("curtainview_set_done_cb_player_vol: CURTAIN_DOWN\n");
    }
    else
    {
        gui_log("curtainview_set_done_cb_player_vol: CURTAIN_RESET\n");
    }
}

void design_tab_player(void *parent)
{
    char *string_player_title = "音乐";
    int font_size = 32;

    text_player_title = gui_text_create(parent, "text_player_title", 195, 24,
                                        strlen(string_player_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_player_title, string_player_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_player_title), font_size);


    switch_song_name = gui_switch_create(parent, 84, 110, 288, 64, ICON_TEXT_BASE_DARK_BIN,
                                         ICON_TEXT_BASE_DARK_BIN);
    switch_song_name->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_song_name->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_song_name, (gui_event_cb_t)switch_song_name_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_song_name, (gui_event_cb_t)switch_song_name_cb, GUI_EVENT_2, NULL);

    img_song_local = gui_img_create_from_mem(parent, "img_song_local",
                                             ICON_SONG_LOCAL_BIN, 330, 125, 32, 32);

    scroll_text_song_name = gui_scroll_text_create(parent, "scroll_text_song_name", 94, 122, 231,
                                                   FONT_H_32);
    gui_scroll_text_scroll_set(scroll_text_song_name, SCROLL_X, 0, 0, 5000, 0);
#ifndef _WIN32
    if (*(uint16_t *)MUSIC_HEADER_BIN_ADDR != 0xFFFF)
    {

        T_HEAD_INFO *header_info = get_cur_play_header_info();
        unicode_to_utf8(utf8_name, 49, \
                        (uint16_t *)(MUSIC_NAME_BIN_ADDR + header_info->offset), \
                        header_info->length / 2);
        gui_scroll_text_set(scroll_text_song_name, (void *)utf8_name,
                            "rtk_font_mem", APP_COLOR_WHITE, strlen((const char *)utf8_name), FONT_H_32);
    }
#endif

    switch_play_pause = gui_switch_create(parent, 197, 213, 64, 64, ICON_SONG_PLAY_BIN,
                                          ICON_SONG_PAUSE_BIN);
    switch_play_pause->off_hl_pic_addr = ICON_SONG_PLAY_TOUCH_BIN;
    switch_play_pause->on_hl_pic_addr = ICON_SONG_PAUSE_TOUCH_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_play_pause, (gui_event_cb_t)switch_play_pause_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_play_pause, (gui_event_cb_t)switch_play_pause_touch_cb, GUI_EVENT_2,
                         NULL);

    switch_next = gui_switch_create(parent, 346, 213, 56, 56, ICON_SONG_NEXT_BIN,
                                    ICON_SONG_NEXT_BIN);
    switch_next->off_hl_pic_addr = ICON_SONG_NEXT_TOUCH_BIN;
    switch_next->on_hl_pic_addr = ICON_SONG_NEXT_TOUCH_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_next, (gui_event_cb_t)switch_next_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_next, (gui_event_cb_t)switch_next_touch_cb, GUI_EVENT_2, NULL);

    switch_prev = gui_switch_create(parent, 55, 213, 56, 56, ICON_SONG_PREV_BIN,
                                    ICON_SONG_PREV_BIN);
    switch_prev->off_hl_pic_addr = ICON_SONG_PREV_TOUCH_BIN;
    switch_prev->on_hl_pic_addr = ICON_SONG_PREV_TOUCH_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_prev, (gui_event_cb_t)switch_prev_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_prev, (gui_event_cb_t)switch_prev_touch_cb, GUI_EVENT_2, NULL);

    img_process = gui_img_create_from_mem(parent, "img_process", ICON_PROCESS_BAR_BIN, 85, 321,
                                          288, 8);
    img_process_node = gui_img_create_from_mem(parent, "img_process_node", ICON_PROCESS_NODE_BIN, 211,
                                               309,
                                               32, 32);

    img_slide_vol = gui_img_create_from_mem(parent, "img_slide_vol", ICON_DISCONNECT_BIN, 88, 373,
                                            277, 81);
    img_spk = gui_img_create_from_mem(parent, "img_spk", ICON_VOLUME_BIN, 205, 386,
                                      36, 48);
    curtainview_player_vol = gui_curtainview_create(parent, "curtainview_player_vol", 0, 0, LCD_W,
                                                    LCD_H);
    curtainview_player_vol->base.cover = true;
    gui_curtainview_set_done_cb(curtainview_player_vol, curtainview_set_done_cb_player_vol);

    extern void design_curtain_player_vol(void *parent);
    design_curtain_player_vol(curtainview_player_vol);

}