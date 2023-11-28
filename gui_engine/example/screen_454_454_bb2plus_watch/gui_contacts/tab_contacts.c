#include "app_gui_main.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_common.h"

gui_text_t *text_contacts_title = NULL;
gui_win_t *win_test_incoming_call = NULL;

static void switch_incoming_call_answer_touch_cb()
{
    gui_log("switch_incoming_call_answer_touch_cb \n");
    gui_app_t *app = get_app_watch_ui();
    gui_obj_t *current_widget = get_current_active_widget();
    push_current_widget(current_widget);
    gui_obj_show(current_widget, false);

    extern gui_win_t *win_incoming_call;
    win_incoming_call = gui_win_create(&(app->screen), "win_incoming_call", 0, 0, LCD_W, LCD_H);
    extern void design_win_incoming_call(void *parent, char *dial_num);
    design_win_incoming_call(win_incoming_call, "1234567890");
}

void design_tab_contacts(void *parent)
{
    char *string_contacts_title = "联系人";
    int font_size = 32;
    text_contacts_title = gui_text_create(parent, "text_contacts_title", 195, 24,
                                          strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_title, string_contacts_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_contacts_title), font_size);

    win_test_incoming_call = gui_win_create(parent, "win_test_incoming_call", 195, 24,
                                            strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_obj_add_event_cb(win_test_incoming_call, (gui_event_cb_t)switch_incoming_call_answer_touch_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);
}

