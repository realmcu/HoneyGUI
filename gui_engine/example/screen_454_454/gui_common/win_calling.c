#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"
#include "gui_progressbar.h"
#include "gui_interface.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#include "app_hfp.h"
#include "app_audio_if.h"
#endif

#define GUI_CALLING_NUM_MAX 24

//win calling
gui_win_t *win_calling = NULL;
gui_text_t *text_calling_num = NULL;
gui_img_t *img_calling = NULL;
gui_text_t *text_talk_time = NULL;
gui_switch_t *switch_calling_volume = NULL;
gui_switch_t *switch_calling_hangup = NULL;
gui_switch_t *switch_calling_mute = NULL;

char talk_time[11];
char *string_talk_time = talk_time;
uint32_t talk_time_ms_init;
uint32_t talk_time_ms;
uint8_t talk_time_s;
uint8_t talk_time_min;
uint8_t talk_time_hour;

//win volume
gui_win_t *win_calling_volume = NULL;
gui_img_t *img_calling_volume_base = NULL;
gui_switch_t *switch_calling_volume_up = NULL;
gui_switch_t *switch_calling_volume_down = NULL;
gui_progressbar_t *progressbar_calling_volume = NULL;
float volume_percentage = 0.1f;
// sync with gain level 0~15
float volume_step = 0.0666f;
uint32_t volume_show_state = false;
uint8_t gui_curr_volume = 0;

void design_win_calling_volume(void *parent);

static void reset_talk_time(void)
{
    gui_log("talk_time_reset\n");
    talk_time_ms_init = gui_ms_get();
    talk_time_ms = 0;
    talk_time_s = 0;
    talk_time_min = 0;
    talk_time_hour = 0;
    talk_time[0] = '0';
    talk_time[1] = '0';
    talk_time[2] = ':';
    talk_time[3] = ' ';
    talk_time[4] = '0';
    talk_time[5] = '0';
    talk_time[6] = ':';
    talk_time[7] = ' ';
    talk_time[8] = '0';
    talk_time[9] = '0';
    talk_time[10] = '\0';
}

static void update_talk_time(void)
{
    //gui_log("talk_time_update\n");
    talk_time_ms = gui_ms_get() - talk_time_ms_init;
    talk_time_s = talk_time_ms / 1000;
    talk_time_min = talk_time_s / 60;
    talk_time_hour = talk_time_min / 60;
    talk_time[0] = talk_time_hour % 60 / 10 + 0x30;
    talk_time[1] = talk_time_hour % 60 % 10 + 0x30;
    talk_time[2] = ':';
    talk_time[3] = ' ';
    talk_time[4] = talk_time_min % 60 / 10 + 0x30;
    talk_time[5] = talk_time_min % 60 % 10 + 0x30;
    talk_time[6] = ':';
    talk_time[7] = ' ';
    talk_time[8] = talk_time_s % 60 / 10 + 0x30;
    talk_time[9] = talk_time_s % 60 % 10 + 0x30;
    talk_time[10] = '\0';
}

static void win_calling_volume_animate_cb(void *data)
{
    uint32_t show_state = *(uint32_t *)data;
    gui_log("win_calling_volume_animate_cb show_state = %d\n", show_state);
    float offset_y = 0.0f;
    if (show_state)
    {
        offset_y = LCD_H * (1.0f - win_calling_volume->animate->progress_percent);
        if (win_calling_volume->animate->progress_percent > 0.99f)
        {
            offset_y = 0.0f;
            gui_obj_show(switch_calling_volume, false);
            gui_obj_show(switch_calling_hangup, false);
            gui_obj_show(switch_calling_mute, false);
        }
    }
    else
    {
        offset_y = LCD_H * win_calling_volume->animate->progress_percent;
        if (win_calling_volume->animate->progress_percent > 0.99f)
        {
            offset_y = LCD_H;
        }
    }
    win_calling_volume->base.y = (int16_t)offset_y;
    gui_log("win_calling_animate_cb win_calling_volume->base.y = %d, progress_percent = %f\n",
            win_calling_volume->base.y, win_calling_volume->animate->progress_percent);
}

static void win_calling_update_cb(void *p)
{
    gui_log("win_calling_update_cb\n");
#ifndef _WIN32
    //update volume
    uint8_t temp_gui_curr_volume = app_audio_get_volume();
    if (temp_gui_curr_volume != gui_curr_volume)
    {
        //todo: display volume
        volume_show_state = true;
        gui_win_set_animate(win_calling_volume, 100, 1, win_calling_volume_animate_cb, &volume_show_state);
    }

    //update hangup by remote
    if (app_hfp_get_call_status() == APP_HFP_CALL_IDLE)
    {
        win_calling->base.not_show = true;
        T_IO_MSG gui_free_msg;
        gui_free_msg.type = IO_MSG_TYPE_WRISTBNAD;
        gui_free_msg.subtype = IO_MSG_GUI_FREE;
        gui_free_msg.u.param = (uint32_t)win_calling;
        app_send_msg_to_apptask(&gui_free_msg);
    }
#endif
}

static void gui_text_talk_time_animate_cb(void *obj)
{
    //gui_log("gui_text_talk_time_animate_cb\n");
    gui_text_t *this = (gui_text_t *)obj;
    //update every 7 frame(about 210 ms)
    if (this->animate->current_frame % 7 == 0)
    {
        //gui_log("gui_text_talk_time_animate_cb update talk time\n");
        update_talk_time();
        gui_text_set(text_talk_time, string_talk_time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(string_talk_time), FONT_H_32);
    }
}

static void switch_calling_volume_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_calling_volume_touch_cb\n");
    //todo: show volume bar
    win_calling_volume->base.not_show = false;
    volume_show_state = true;
    gui_win_set_animate(win_calling_volume, 100, 1, win_calling_volume_animate_cb, &volume_show_state);
}

static void switch_calling_hangup_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_calling_hangup_touch_cb\n");
    extern gui_win_t *win_incoming_call;
    //reset talk time
    gui_win_calling_destruct();

#ifndef _WIN32
    //send msg to app task
    T_IO_MSG end_call_msg;
    end_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    end_call_msg.subtype = IO_MSG_MMI;
    end_call_msg.u.param = MMI_HF_REJECT_CALL;
    app_send_msg_to_apptask(&end_call_msg);
#endif
}

static void switch_calling_mute_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_calling_mute_touch_cb\n");
#ifndef _WIN32
    T_IO_MSG mic_mute_msg;
#endif
    switch (event)
    {
    // switch is off(touch to do some turn-on action)
    case GUI_EVENT_1:
        gui_log("switch_calling_mute_touch_cb mic mute on\n");
#ifndef _WIN32
        //send msg to app task
        mic_mute_msg.type = IO_MSG_TYPE_WRISTBNAD;
        mic_mute_msg.subtype = IO_MSG_MMI;
        mic_mute_msg.u.param = MMI_DEV_MIC_MUTE;
        app_send_msg_to_apptask(&mic_mute_msg);
#endif
        break;
    // switch is on(touch to do some turn-off action)
    case GUI_EVENT_2:
        gui_log("switch_calling_mute_touch_cb mic mute off\n");
#ifndef _WIN32
        mic_mute_msg.type = IO_MSG_TYPE_WRISTBNAD;
        mic_mute_msg.subtype = IO_MSG_MMI;
        mic_mute_msg.u.param = MMI_DEV_MIC_UNMUTE;
        app_send_msg_to_apptask(&mic_mute_msg);
#endif
        break;
    default:
        break;
    }
}

void design_win_calling(void *parent, char *dial_num)
{
    //add animate for update
    gui_win_t *win = (gui_win_t *)parent;
    gui_win_set_animate((gui_win_t *)win, 100, 1, win_calling_update_cb, NULL);
    win->animate->animate = false;

    char *string_dial_num = dial_num;
    //char *string_dial_name = "";
    gui_log("string_dial_num = %s\n", string_dial_num);
    //todo: displaying name or num depends on pbap
    text_calling_num = gui_text_create(parent, "text_calling_num", 0, 59,
                                       LCD_W, FONT_H_32);
    gui_text_set(text_calling_num, string_dial_num, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_dial_num), FONT_H_32);
    gui_text_mode_set(text_calling_num, CENTER);

    img_calling = gui_img_create_from_mem(parent, "img_calling", ICON_CALLER_BIN, 165, 111, 124, 124);

    text_talk_time = gui_text_create(parent, "text_talk_time", 0, 259, LCD_W, FONT_H_32);
    gui_text_set(text_talk_time, string_talk_time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_talk_time), FONT_H_32);
    gui_text_mode_set(text_talk_time, CENTER);
    //set animate for updating display string of text_record_tips
    gui_text_set_animate(text_talk_time, 1000, -1, gui_text_talk_time_animate_cb, text_talk_time);
    reset_talk_time();

    switch_calling_volume = gui_switch_create(parent, 60, 297, 80, 80, ICON_VOLUME_CALL_BIN,
                                              ICON_VOLUME_CALL_BIN);
    switch_calling_volume->off_hl_pic_addr = ICON_VOLUME_CALL_TOUCH_BIN;
    switch_calling_volume->on_hl_pic_addr = ICON_VOLUME_CALL_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_volume, (gui_event_cb_t)switch_calling_volume_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_volume, (gui_event_cb_t)switch_calling_volume_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_calling_hangup = gui_switch_create(parent, 187, 339, 80, 80, ICON_HANGUP_BIN,
                                              ICON_HANGUP_BIN);
    switch_calling_hangup->off_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    switch_calling_hangup->on_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_hangup, (gui_event_cb_t)switch_calling_hangup_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_hangup, (gui_event_cb_t)switch_calling_hangup_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_calling_mute = gui_switch_create(parent, 314, 297, 80, 80, ICON_MIC_MUTE_OFF_BIN,
                                            ICON_MIC_MUTE_ON_BIN);
    switch_calling_mute->off_hl_pic_addr = ICON_MIC_MUTE_OFF_TOUCH_BIN;
    switch_calling_mute->on_hl_pic_addr = ICON_MIC_MUTE_ON_TOUCH_BIN;
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_calling_mute, (gui_event_cb_t)switch_calling_mute_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_calling_mute, (gui_event_cb_t)switch_calling_mute_touch_cb, GUI_EVENT_2,
                         NULL);
    //volume bar
    win_calling_volume = gui_win_create(win_calling, "win_calling_volume", 0, LCD_H, LCD_W, LCD_H);
    design_win_calling_volume(win_calling_volume);
    win_calling_volume->base.not_show = true;

}

//window volume
static void switch_calling_volume_up_touch_cb(void *parent, gui_event_t event)
{
    volume_percentage = volume_percentage + volume_step;

    gui_log("switch_calling_volume_up_touch_cb volume_percentage = %f\n", volume_percentage);
    if (volume_percentage >= 1.0f)
    {
        volume_percentage = 1.0f;
    }
    gui_progressbar_set_percentage(progressbar_calling_volume, volume_percentage);

#ifndef _WIN32
    T_IO_MSG calling_volume_down_msg;
    calling_volume_down_msg.type = IO_MSG_TYPE_WRISTBNAD;
    calling_volume_down_msg.subtype = IO_MSG_MMI;
    calling_volume_down_msg.u.param = MMI_DEV_SPK_VOL_UP;
    app_send_msg_to_apptask(&calling_volume_down_msg);
#endif
}

static void switch_calling_volume_down_touch_cb(void *parent, gui_event_t event)
{
    volume_percentage = volume_percentage - volume_step;

    gui_log("switch_calling_volume_down_touch_cb volume_percentage = %f\n", volume_percentage);
    if (volume_percentage < 0.01f)
    {
        volume_percentage = 0.001f;
    }
    gui_progressbar_set_percentage(progressbar_calling_volume, volume_percentage);

#ifndef _WIN32
    T_IO_MSG calling_volume_down_msg;
    calling_volume_down_msg.type = IO_MSG_TYPE_WRISTBNAD;
    calling_volume_down_msg.subtype = IO_MSG_MMI;
    calling_volume_down_msg.u.param = MMI_DEV_SPK_VOL_DOWN;
    app_send_msg_to_apptask(&calling_volume_down_msg);
#endif
}

static void win_calling_volume_touch_cb(void *parent, gui_event_t event)
{
    gui_log("win_calling_volume_touch_cb\n");
    switch (event)
    {
    case GUI_EVENT_3:
        volume_show_state = false;
        gui_win_set_animate(parent, 100, 1, win_calling_volume_animate_cb, &volume_show_state);
        gui_obj_show(switch_calling_volume, true);
        gui_obj_show(switch_calling_hangup, true);
        gui_obj_show(switch_calling_mute, true);
        break;
    default:
        break;
    }
}

void design_win_calling_volume(void *parent)
{
    //window calling volume animate to show smoothly
    volume_show_state = false;
    gui_win_set_animate(parent, 100, 1, win_calling_volume_animate_cb, &volume_show_state);
    //add slide down touch event
    gui_obj_add_event_cb(parent, win_calling_volume_touch_cb, GUI_EVENT_3, NULL);
    //grey base
    img_calling_volume_base = gui_img_create_from_mem(parent, "img_calling_volume_base",
                                                      WATCH_BASE_GREY_90ALPHA_HALF_BIN, 0, LCD_H / 2, LCD_W, LCD_H / 2);

    //volume button
    switch_calling_volume_up = gui_switch_create(img_calling_volume_base, 373, 42, 48, 48,
                                                 ICON_VOLUME_UP_BIN, ICON_VOLUME_UP_BIN);
    switch_calling_volume_up->off_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    switch_calling_volume_up->on_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_volume_up, (gui_event_cb_t)switch_calling_volume_up_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_volume_up, (gui_event_cb_t)switch_calling_volume_up_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_calling_volume_down = gui_switch_create(img_calling_volume_base, 37, 42, 48, 48,
                                                   ICON_VOLUME_DOWN_BIN, ICON_VOLUME_DOWN_BIN);
    switch_calling_volume_down->off_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    switch_calling_volume_down->on_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_volume_down,
                         (gui_event_cb_t)switch_calling_volume_down_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_volume_down,
                         (gui_event_cb_t)switch_calling_volume_down_touch_cb, GUI_EVENT_2, NULL);
    //volume bar
    //todo :add volume black base bar; add volume blue progress bar
    progressbar_calling_volume = gui_progressbar_img_h_create(img_calling_volume_base,
                                                              ICON_CALL_VOLUME_BAR_2_BIN, 107, 62);
    //todo get volume from profile
    //curr_volume 0~15
#ifndef _WIN32
    gui_curr_volume = app_audio_get_volume();
    volume_percentage = (float)(gui_curr_volume / 0xf);
    gui_progressbar_set_percentage(progressbar_calling_volume, volume_percentage);
#endif
}

void gui_win_calling_destruct(void)
{
    reset_talk_time();
    //back to previous view
    gui_obj_t *object_return = pop_current_widget();
    // if (object_return == (gui_obj_t *)win_incoming_call)
    // {
    //     //pop again, if previous view is incoming call
    //     object_return = pop_current_widget();
    //     gui_tree_free(win_incoming_call);
    //     win_incoming_call = NULL;
    // }
    gui_obj_show(object_return, true);
    gui_tree_free(win_calling);
    win_calling = NULL;
    win_calling_volume = NULL;

    gui_fb_change();
}
