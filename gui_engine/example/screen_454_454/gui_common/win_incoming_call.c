#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"

gui_win_t *win_incoming_call = NULL;
gui_text_t *text_incoming_call_title = NULL;
gui_text_t *text_incoming_call_num = NULL;
gui_img_t *img_incoming_call = NULL;
gui_switch_t *switch_incoming_call_answer = NULL;
gui_switch_t *switch_incoming_call_hangup = NULL;

static void switch_incoming_call_answer_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_incoming_call_answer_touch_cb\n");
    gui_obj_t *this = (gui_obj_t *)obj;
    char *dial_num = (char *)this->event_dsc->user_data;
    //create window calling
    extern gui_win_t *win_calling;
    gui_app_t *app = get_app_watch_ui();

    win_calling = gui_win_create(&(app->screen), "win_calling", 0, 0, LCD_H, LCD_W);

    extern void design_win_calling(void *obj, char *dial_num);
    design_win_calling(win_calling, dial_num);

    push_current_widget(win_incoming_call);
    gui_obj_show(win_incoming_call, false);
}

static void switch_incoming_call_hangup_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_incoming_call_hangup_touch_cb\n");

    gui_obj_t *object_return = pop_current_widget();
    gui_obj_show(object_return, true);
    gui_tree_free(win_incoming_call);
    win_incoming_call = NULL;
}

void design_win_incoming_call(void *parent, char *dial_num)
{
    char *string_incoming_call_title = "来电";
    text_incoming_call_title = gui_text_create(parent, "text_incoming_call_title", 195, 24,
                                               strlen(string_incoming_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_incoming_call_title, string_incoming_call_title, "rtk_font_mem", APP_COLOR_WHITE,
                 strlen(string_incoming_call_title), FONT_H_32);
    //incoming call number
    char *string_dial_num = dial_num;
    //char *string_dial_name = "";
    gui_log("string_dial_num = %s\n", string_dial_num);
    //todo: displaying name or num depends on pbap
    text_incoming_call_num = gui_text_create(parent, "text_incoming_call_num", 0, 59,
                                             LCD_W, FONT_H_32);
    gui_text_set(text_incoming_call_num, string_dial_num, "rtk_font_mem", APP_COLOR_WHITE,
                 strlen(string_dial_num), FONT_H_32);
    gui_text_mode_set(text_incoming_call_num, CENTER);
    //incoming call image
    img_incoming_call = gui_img_create_from_mem(parent, "img_incoming_call", ICON_CALLER_BIN, 165, 191,
                                                124, 124);
    //incoming call answer
    switch_incoming_call_answer = gui_switch_create(parent, 285, 285, 80, 80, ICON_ANSWER_BIN,
                                                    ICON_ANSWER_BIN);
    switch_incoming_call_answer->off_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    switch_incoming_call_answer->on_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    gui_obj_add_event_cb(switch_incoming_call_answer,
                         (gui_event_cb_t)switch_incoming_call_answer_touch_cb, GUI_EVENT_1, string_dial_num);
    gui_obj_add_event_cb(switch_incoming_call_answer,
                         (gui_event_cb_t)switch_incoming_call_answer_touch_cb, GUI_EVENT_2, string_dial_num);
    //incoming call hangup
    switch_incoming_call_hangup = gui_switch_create(parent, 89, 285, 80, 80, ICON_HANGUP_BIN,
                                                    ICON_HANGUP_BIN);
    switch_incoming_call_hangup->off_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    switch_incoming_call_hangup->on_hl_pic_addr = ICON_HANGUP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_incoming_call_hangup,
                         (gui_event_cb_t)switch_incoming_call_hangup_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_incoming_call_hangup,
                         (gui_event_cb_t)switch_incoming_call_hangup_touch_cb, GUI_EVENT_2, NULL);
}

