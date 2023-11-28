#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_curtainview.h"
#include "gui_app.h"
#include "gui_win.h"

gui_text_t *text_call_title = NULL;
gui_switch_t *switch_call = NULL;
gui_curtainview_t *curtainview_call = NULL;
extern gui_win_t *win_dial;

static void switch_call_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_call_touch_cb\n");
    push_current_widget(tabview_main);
    gui_app_t *app = NULL;
    app = get_app_watch_ui();
    win_dial = gui_win_create(app, "win_dial", 0, 0, LCD_W, LCD_H);
    extern void design_win_dial(void *parent);
    design_win_dial(win_dial);

    gui_obj_show(tabview_main, false);
    //jump to dial ui
}

void design_tab_call(void *parent)
{
    char *string_call_title = "电话";
    text_call_title = gui_text_create(parent, "text_call_title", 195, 24,
                                      strlen(string_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_call_title, string_call_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_call_title), FONT_H_32);

    switch_call = gui_switch_create(parent, 129, 129, 196, 196, ICON_CALL_BIN, ICON_CALL_BIN);
    switch_call->on_hl_pic_addr = ICON_CALL_TOUCH_BIN;
    switch_call->off_hl_pic_addr = ICON_CALL_TOUCH_BIN;
    //add touch event callback
    gui_obj_add_event_cb(switch_call, (gui_event_cb_t)switch_call_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_call, (gui_event_cb_t)switch_call_touch_cb, GUI_EVENT_2, NULL);

    curtainview_call = gui_curtainview_create(parent, "curtainview_call", 0, 0, LCD_W, LCD_H);
    //design_curtainview_call(curtainview_call);
}
