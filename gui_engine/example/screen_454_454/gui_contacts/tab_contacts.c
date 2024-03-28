#include "app_gui_main.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_curtainview.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "gui_grid.h"
#include "gui_tab.h"

gui_text_t *text_contacts_title = NULL;
gui_img_t *img_touch_contacts = NULL;
gui_switch_t *switch_touch_contacts = NULL;
gui_curtainview_t *curtainview_contacts_list = NULL;
gui_grid_t *grid_contacts = NULL;
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

static void switch_contacts_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_contacts_touch_cb, obj = 0x%x\n", obj);

    //gui_switch_t *this = (gui_switch_t *)obj;
    //for contracts list window creation
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_contacts_list;

    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        // jump to switch_contacts_touch_cb
        //create contracts list window
        gui_log("GUI_EVENT_TOUCH_PRESSED obj = 0x%x\n", obj);
        if (win_contacts_list != NULL)
        {
            gui_tree_free(win_contacts_list);
            win_contacts_list = NULL;
        }
        win_contacts_list = gui_win_create(&(app->screen), "win_contacts_list", 0, 0, LCD_W, LCD_H);
        extern void design_win_contracts_list(gui_win_t *parent);
        design_win_contracts_list(win_contacts_list);

        //  push_current_widget(tabview_main);
        //  gui_obj_show(tabview_main, false);
        tabview_main->base.not_show = true;

        break;

    default:
        break;

    }
}

void design_tab_contacts(void *parent)
{
    char *string_contacts_title = "联系人";
    int font_size = 32;
    text_contacts_title = gui_text_create(parent, "text_contacts_title", 179, 24,
                                          strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_title, string_contacts_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_contacts_title), font_size);

    switch_touch_contacts = gui_switch_create(parent, 129, 129, 196, 196,
                                              ICON_CONTACTCS_BIN, ICON_CONTACTCS_TOUCH_BIN);
    switch_touch_contacts->on_hl_pic_addr = ICON_CONTACTCS_BIN;
    switch_touch_contacts->off_hl_pic_addr = ICON_CONTACTCS_TOUCH_BIN;
    gui_obj_add_event_cb(switch_touch_contacts,
                         (gui_event_cb_t)switch_contacts_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_touch_contacts,
                         (gui_event_cb_t)switch_contacts_touch_cb,
                         GUI_EVENT_2, NULL);

    win_test_incoming_call = gui_win_create(parent, "win_test_incoming_call", 195, 24,
                                            strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_obj_add_event_cb(win_test_incoming_call, (gui_event_cb_t)switch_incoming_call_answer_touch_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);

}


