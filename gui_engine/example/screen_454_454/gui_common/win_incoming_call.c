#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"
#ifdef _ENABLE_RTK_SOC_WATCH_
#include "app_task.h"
#include "app_mmi.h"
#endif

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

    //push win_incoming call is unnecessary
    // push_current_widget(win_incoming_call);
    // gui_obj_show(win_incoming_call, false);
    gui_obj_tree_free(win_calling);
    win_calling = NULL;

#ifdef _ENABLE_RTK_SOC_WATCH_
    //send to app task
    T_IO_MSG answer_call_msg;
    answer_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    answer_call_msg.subtype = IO_MSG_MMI;
    answer_call_msg.u.param = MMI_HF_ANSWER_CALL;
    app_send_msg_to_apptask(&answer_call_msg);
#endif
}

static void switch_incoming_call_hangup_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_incoming_call_hangup_touch_cb\n");

    gui_obj_t *object_return = pop_current_widget();
    gui_obj_show(object_return, true);
    gui_obj_tree_free(win_incoming_call);
    win_incoming_call = NULL;

#ifdef _ENABLE_RTK_SOC_WATCH_
    //send to app task
    T_IO_MSG reject_call_msg;
    reject_call_msg.type = IO_MSG_TYPE_WRISTBNAD;
    reject_call_msg.subtype = IO_MSG_MMI;
    reject_call_msg.u.param = MMI_HF_REJECT_CALL;
    app_send_msg_to_apptask(&reject_call_msg);
#endif
}

void design_win_incoming_call(void *parent, char *dial_num)
{
    char *string_incoming_call_title = "来电";
    text_incoming_call_title = gui_text_create(parent, "text_incoming_call_title", 195, 24,
                                               strlen(string_incoming_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_incoming_call_title, string_incoming_call_title, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_incoming_call_title), FONT_H_32);
    //get incoming call number
    char *string_dial_num = dial_num;
    //char *string_dial_name = "";
    gui_log("string_dial_num = %s\n", string_dial_num);
    //todo: displaying name or num depends on pbap
    text_incoming_call_num = gui_text_create(parent, "text_incoming_call_num", 0, 59,
                                             LCD_W, FONT_H_32);
    gui_text_set(text_incoming_call_num, string_dial_num, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
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

void gui_incoming_call_create(void)
{
#ifdef _ENABLE_RTK_SOC_WATCH_
    gui_log("gui_incoming_call_create\n");
    gui_app_t *app = get_app_watch_ui();
    gui_obj_t *current_widget = get_current_active_widget();
    push_current_widget(current_widget);
    gui_obj_show(current_widget, false);

    extern gui_win_t *win_incoming_call;
    win_incoming_call = gui_win_create(&(app->screen), "win_incoming_call", 0, 0, LCD_W, LCD_H);
    extern void design_win_incoming_call(void *parent, char *dial_num);
    design_win_incoming_call(win_incoming_call, get_dial_num());
#endif
}
