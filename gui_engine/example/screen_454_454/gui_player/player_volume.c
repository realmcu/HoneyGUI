#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_switch.h"
#include "gui_progressbar.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#include "app_audio_if.h"
#include "trace.h"
#endif

//Global widget handle which need to use in the callbacks
gui_progressbar_t *progressbar_player_vol = NULL;

uint8_t gui_player_vol = 0;
float player_vol_per;

static void switch_player_vol_up_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_player_vol_up_touch_cb\n");

    //char *string_record_title;
    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
#ifndef _WIN32
            T_IO_MSG play_msg;
            play_msg.type = IO_MSG_TYPE_WRISTBNAD;
            play_msg.subtype = IO_MSG_MMI;
            play_msg.u.param = MMI_DEV_SPK_VOL_UP;
            app_send_msg_to_apptask(&play_msg);
#endif
        }
        break;

    default:
        break;
    }
}

static void switch_player_vol_down_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_player_vol_down_touch_cb\n");

    //char *string_record_title;
    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        {
#ifndef _WIN32
            T_IO_MSG play_msg;
            play_msg.type = IO_MSG_TYPE_WRISTBNAD;
            play_msg.subtype = IO_MSG_MMI;
            play_msg.u.param = MMI_DEV_SPK_VOL_DOWN;
            app_send_msg_to_apptask(&play_msg);
#endif
        }
        break;

    default:
        break;
    }
}

static void switch_player_vol_update_cb(void *obj, uint16_t event)
{
    gui_log("switch_player_vol_update_cb\n");

#ifndef _WIN32
    //char *string_record_title;
    switch (event)
    {
    case GUI_EVENT_PLAYER:
        {
            gui_player_vol = app_audio_get_volume();
            player_vol_per = (float)gui_player_vol / (float)0xf;
            gui_progressbar_set_percentage(progressbar_player_vol, player_vol_per);
        }
        break;
    default:
        break;
    }
#endif
}

void design_curtain_player_vol(void *parent)
{
    //gui_curtainview_t *curtainview_player_vol = (gui_curtainview_t *)parent;
    gui_curtain_t *curtain_down_player_vol = gui_curtain_create(parent, "curtain_down_record", 0, 0,
                                                                LCD_W, LCD_H,
                                                                CURTAIN_DOWN, 0.5f);
    gui_obj_add_event_cb(curtain_down_player_vol, (gui_event_cb_t)switch_player_vol_update_cb,
                         (gui_event_t)GUI_EVENT_PLAYER,
                         NULL);

    gui_img_create_from_mem(curtain_down_player_vol, "img_base_record_files",
                            WATCH_BASE_GREY_90ALPHA_HALF_BIN, 0, 0, 0, 0);

    gui_img_create_from_mem(curtain_down_player_vol, "img_vol_bar",
                            ICON_CALL_VOLUME_BAR_2_BIN, 117, 67, 0, 0);

    //volume bar
    //todo :add volume black base bar; add volume blue progress bar
    progressbar_player_vol = gui_progressbar_img_h_create(curtain_down_player_vol,
                                                          ICON_VOLUME_BAR_BLUE_BIN, 117, 67);
    //todo get volume from profile
    //curr_volume 0~15

    gui_player_vol = 8;//default volume
    player_vol_per = (float)gui_player_vol / (float)0xf;
    gui_progressbar_set_percentage(progressbar_player_vol, player_vol_per);

    gui_switch_t *switch_vol_down = gui_switch_create(curtain_down_player_vol, 44, 46, 48, 48,
                                                      ICON_VOLUME_DOWN_BIN,
                                                      ICON_VOLUME_DOWN_BIN);
    switch_vol_down->off_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    switch_vol_down->on_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_vol_down, (gui_event_cb_t)switch_player_vol_down_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_vol_down, (gui_event_cb_t)switch_player_vol_down_touch_cb, GUI_EVENT_2,
                         NULL);

    gui_switch_t *switch_vol_up = gui_switch_create(curtain_down_player_vol, 375, 46, 48, 48,
                                                    ICON_VOLUME_UP_BIN,
                                                    ICON_VOLUME_UP_BIN);
    switch_vol_up->off_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    switch_vol_up->on_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_vol_up, (gui_event_cb_t)switch_player_vol_up_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_vol_up, (gui_event_cb_t)switch_player_vol_up_touch_cb, GUI_EVENT_2,
                         NULL);

}
