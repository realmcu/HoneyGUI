#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_obj.h"
#include "gui_switch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_curtainview.h"


gui_switch_t *switch_record = NULL;
gui_img_t *img_slide_record = NULL;
gui_curtainview_t *curtainview_record = NULL;

gui_text_t *text_record_title = NULL;
gui_text_t *text_record_tips = NULL;

char *string_record_time = "00: 00: 00";
uint8_t record_time_display[11];
uint32_t record_time_ms_init;
uint32_t record_time_ms;
uint8_t record_time_s;
uint8_t record_time_min;
uint8_t record_time_hour;

static char *update_record_time(void)
{
    record_time_ms = gui_ms_get() - record_time_ms_init;
    record_time_s = record_time_ms / 1000;
    record_time_min = record_time_s / 60;
    record_time_hour = record_time_min / 60;
    record_time_display[0] = record_time_hour % 60 / 10 + 0x30;
    record_time_display[1] = record_time_hour % 60 % 10 + 0x30;
    record_time_display[2] = ':';
    record_time_display[3] = ' ';
    record_time_display[4] = record_time_min % 60 / 10 + 0x30;
    record_time_display[5] = record_time_min % 60 % 10 + 0x30;
    record_time_display[6] = ':';
    record_time_display[7] = ' ';
    record_time_display[8] = record_time_s % 60 / 10 + 0x30;
    record_time_display[9] = record_time_s % 60 % 10 + 0x30;
    record_time_display[10] = '\0';

    return (char *)record_time_display;
}
void reset_record_time(void)
{
    record_time_ms_init = gui_ms_get();
    record_time_ms = 0;
    record_time_s = 0;
    record_time_min = 0;
    record_time_hour = 0;
    record_time_display[0] = '0';
    record_time_display[1] = '0';
    record_time_display[2] = ':';
    record_time_display[3] = ' ';
    record_time_display[4] = '0';
    record_time_display[5] = '0';
    record_time_display[6] = ':';
    record_time_display[7] = ' ';
    record_time_display[8] = '0';
    record_time_display[9] = '0';
    record_time_display[10] = '\0';
}

static void switch_record_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_record_touch_cb\n");

    char *string_record_title;
    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
        gui_log("switch_record_touch_cb GUI_EVENT_2\n");
        string_record_title = "录音";
        text_record_title->base.w = strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_title, 195, 24);
        gui_text_set(text_record_title, string_record_title, "rtk_font_mem", 0xffffffff,
                     strlen(string_record_title), FONT_H_32);

        char *string_record_tips = "轻触录音";
        text_record_tips->base.w = strlen(string_record_tips) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_tips, 163, 314);
        gui_text_set(text_record_tips, string_record_tips, "rtk_font_mem", 0xffffffff,
                     strlen(string_record_tips), FONT_H_32);
        //gui_obj_set_update_att(text_record_tips, 250, false, NULL);
        reset_record_time();

        //todo: add record stop action
        break;


    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        gui_log("switch_record_touch_cb GUI_EVENT_1\n");
        string_record_title = "正在录音";
        text_record_title->base.w = strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W;
        gui_text_move(text_record_title, 165, 24);
        gui_text_set(text_record_title, string_record_title, "rtk_font_mem", 0xffffffff,
                     strlen(string_record_title), FONT_H_32);

        string_record_time = (char *)record_time_display;
        reset_record_time();

        text_record_tips->base.w = strlen(string_record_time) * FONT_NUM_ALPHA_W;
        gui_text_move(text_record_tips, 142, 314);
        gui_text_set(text_record_tips, string_record_time, "rtk_font_mem", 0xffffffff,
                     strlen(string_record_time), FONT_H_32);
        //gui_obj_set_update_att(text_record_tips, 250, true, NULL);


        //todo: add record start action
        break;

    default:
        break;
    }
}

static void curtainview_set_done_cb_record(gui_curtainview_t *this)
{
    gui_log("curtainview_set_done_cb_record\n");
    if (this->cur_curtain == CURTAIN_DOWN)
    {
        //hide switch_record to aviod trig
        gui_log("curtainview_set_done_cb_record: CURTAIN_DOWN\n");
        gui_obj_show(switch_record, false);
        //switch_record->base.not_show = true;
    }
    else
    {
        gui_log("curtainview_set_done_cb_record: CURTAIN_RESET\n");
        gui_obj_show(switch_record, true);
        //switch_record->base.not_show = false;
    }
}

static void text_record_tips_update_cb(void *obj)
{
    gui_log("text_record_tips_update_cb\n");
    //todo: update record time
    string_record_time = update_record_time();

}

void design_tab_record(void *parent)
{
    switch_record = gui_switch_create(parent, 131, 78, 192, 192, ICON_RECORD_START_BIN,
                                      ICON_RECORD_STOP_BIN);
    //add touch event callback
    //GUI_EVENT_1 for switch is off(turn-on action)
    //GUI_EVENT_2 for switch is on(turn-off action)
    gui_obj_add_event_cb(switch_record, (gui_event_cb_t)switch_record_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record, (gui_event_cb_t)switch_record_touch_cb, GUI_EVENT_2, NULL);

    char *string_record_title = "录音";
    text_record_title = gui_text_create(parent, "text_record_title", 195, 24,
                                        strlen(string_record_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_record_title, string_record_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_record_title), FONT_H_32);

    char *string_record_tips = "轻触录音";
    text_record_tips = gui_text_create(parent, "text_record_tips", 163, 314,
                                       strlen(string_record_tips) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_record_tips, string_record_tips, "rtk_font_mem", 0xffffffff,
                 strlen(string_record_tips), FONT_H_32);
    //add update callback, interval 0 for continous update, 0xffffffff for one-time update.
    //gui_obj_add_update_cb(text_record_tips, (gui_update_cb_t)text_record_tips_update_cb, 0xFFFFFFFF,
    //                      false, NULL);

    img_slide_record = gui_img_create_from_mem(parent, "img_slide_record", ICON_UP_SLIDE_BIN, 88, 373,
                                               277, 81);

    curtainview_record = gui_curtainview_create(parent, "curtainview_record", 0, 0, LCD_W, LCD_H);
    curtainview_record->base.cover = true;
    gui_curtainview_set_done_cb(curtainview_record, curtainview_set_done_cb_record);

    extern void design_curtain_record_files(void *parent);
    design_curtain_record_files(curtainview_record);
}