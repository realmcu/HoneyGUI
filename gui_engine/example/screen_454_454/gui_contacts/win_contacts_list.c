#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "gui_tabview.h"

#define MAX_CINTACTS_NUM        3

gui_win_t *win_contacts_list = NULL;
gui_text_t *text_contacts_list_title = NULL;
gui_switch_t *switch_contacts_list_delete = NULL;
gui_grid_t *grid_contacts_list = NULL;
gui_switch_t *switch_designated_contact[MAX_CINTACTS_NUM] = {NULL};
gui_text_t *text_designated_contact[MAX_CINTACTS_NUM] = {NULL};
gui_switch_t *switch_contacts_list_back = NULL;
gui_page_t *page_contacts_list = NULL;
gui_win_t *win_add_designated_contact = NULL;
char *contacts_list_name[MAX_CINTACTS_NUM] = {"联系人1", "联系人2", "联系人3"};

static uint8_t contacts_num = 0;
extern gui_win_t *win_confirm;

char *text_contacts_list_delete_text = "确认删除联系人？";

//if current view has a "delete icon", need to set delete info before create a win_delete
static void switch_contacts_list_delete_yes_action(void *obj)
{
    gui_log("switch_contacts_list_delete_yes_action, obj = 0x%x\n", (uint32_t *)obj);
    //delete selected contacts
}
static void switch_contacts_list_delete_no_action(void *obj)
{
    gui_log("switch_contacts_list_delete_no_action, obj = 0x%x\n", (uint32_t *)obj);
    win_contacts_list->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
    //do nothing
}

static void contacts_list_back_touch_cb(void *obj, gui_event_t event)
{
    gui_log("contacts_list_back_touch_cb\n");
    gui_obj_t *object = (gui_obj_t *)obj;
    tabview_main->base.not_show = false;
//    gui_obj_t *object_return = pop_current_widget();
//     gui_obj_show(object_return, true);
    //object_return->not_show = false;


    gui_log("contacts_list_back_touch_cb object->parent->name = %s\n", object->parent->name);
    gui_tree_free(win_contacts_list);
    win_contacts_list = NULL;
    contacts_num = 0;
}

static void switch_designated_contact_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_designated_contact_touch_cb, obj = 0x%x\n", (uint32_t *)obj);

    gui_switch_t *this = (gui_switch_t *)obj;
    //for delete and contact details windows creation
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_contacts_details;
    extern gui_text_t *text_confirm;

    switch (event)
    {

    case GUI_EVENT_1:
    case GUI_EVENT_2:
        // jump to win_contact_details
        //create contact details window
        gui_log("GUI_EVENT_1 GUI_EVENT_2 obj = 0x%x\n", (uint32_t *)obj);
        if (win_contacts_details != NULL)
        {
            gui_tree_free(win_contacts_details);
            win_contacts_details = NULL;
        }
        win_contacts_details = gui_win_create(&(app->screen), "win_contacts_details", 0, 0, LCD_W, LCD_H);
        extern void design_win_contracts_details(gui_win_t *parent, gui_switch_t *selected_contacts_list);
        design_win_contracts_details(win_contacts_details, this);

        push_current_widget(win_contacts_list);
        gui_obj_show(win_contacts_list, false);
        //tabview_main->base.not_show = true;

        break;
    case GUI_EVENT_TOUCH_LONG:
        // delete
        //change color
        gui_log("GUI_EVENT_TOUCH_LONG obj = 0x%x\n", (uint32_t *)obj);
        switch_contacts_list_delete->base.not_show = !switch_contacts_list_delete->base.not_show;
        for (int j = 0; j < contacts_num; j ++)
        {
            if (this == switch_designated_contact[j])
            {
                continue;
            }
            if (switch_contacts_list_delete->base.not_show)
            {
                switch_designated_contact[j]->base.not_show = false;
            }
            else
            {
                switch_designated_contact[j]->base.not_show = true;
            }
        }

        break;

    default:
        break;
    }
}

static void switch_contacts_list_delete_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_contacts_list_delete_touch_cb\n");
    if (win_confirm != NULL)
    {
        gui_tree_free(win_confirm);
        win_confirm = NULL;
        set_confirm_yes(NULL, NULL);
        set_confirm_no(NULL, NULL);
    }
    push_current_widget(win_contacts_list);
    gui_obj_show(win_contacts_list, false);

    gui_app_t *app = get_app_watch_ui();
    win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
    win_confirm->base.not_show = false;

    set_confirm_yes(switch_contacts_list_delete_yes_action, obj);
    set_confirm_no(switch_contacts_list_delete_no_action, obj);
    set_confirm_text(text_contacts_list_delete_text, 123, 131, 7);

    extern void design_win_confirm(void *parent);
    design_win_confirm(win_confirm);
}

void design_win_contracts_list(gui_win_t *parent)
{
    //back touch
    switch_contacts_list_back = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                  ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_contacts_list_back, (gui_event_cb_t)contacts_list_back_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_contacts_list_back, (gui_event_cb_t)contacts_list_back_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    //contacts_list
    char *string_contacts_list_title = "联系人列表";
    int font_size = 32;
    text_contacts_list_title = gui_text_create(parent, "text_contacts_list_title", 179, 19,
                                               strlen(string_contacts_list_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_list_title, string_contacts_list_title, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_list_title), font_size);

    //contacts switch and text
    for (uint8_t i = 0; i < 3; i++)
    {
        switch_designated_contact[i] = gui_switch_create(parent, 83, 111 + i * 84, 288, 64,
                                                         ICON_TEXT_BASE_DARK_BIN,
                                                         ICON_TEXT_BASE_DARK_BIN);
        switch_designated_contact[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_designated_contact[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_designated_contact[i]->long_touch_enable = true;
        switch_designated_contact[i]->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
        switch_designated_contact[i]->long_touch_state_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

        //add touch event callback
        //GUI_EVENT_1 for switch is off(turn-on action)
        //GUI_EVENT_2 for switch is on(turn-off action)
        gui_obj_add_event_cb(switch_designated_contact[i],
                             (gui_event_cb_t)switch_designated_contact_touch_cb, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_designated_contact[i],
                             (gui_event_cb_t)switch_designated_contact_touch_cb, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(switch_designated_contact[i],
                             (gui_event_cb_t)switch_designated_contact_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

        text_designated_contact[i] = gui_text_create(switch_designated_contact[i],
                                                     "text_designated_contact", 71, 11,
                                                     strlen(contacts_list_name[i]) * FONT_NUM_ALPHA_W, font_size);
        gui_text_set(text_designated_contact[i], contacts_list_name[i], GUI_FONT_SRC_BMP,
                     APP_COLOR_WHITE,
                     strlen(contacts_list_name[i]), font_size);
    }

    //delete touch
    switch_contacts_list_delete = gui_switch_create(parent, 88, 373, 277, 81,
                                                    ICON_RECORD_DELETE_BIN, ICON_RECORD_DELETE_BIN);
    switch_contacts_list_delete->on_hl_pic_addr = ICON_RECORD_DELETE_BIN;
    switch_contacts_list_delete->off_hl_pic_addr = ICON_RECORD_DELETE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_contacts_list_delete,
                         (gui_event_cb_t)switch_contacts_list_delete_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_contacts_list_delete,
                         (gui_event_cb_t)switch_contacts_list_delete_touch_cb,
                         GUI_EVENT_2, NULL);
    switch_contacts_list_delete->base.not_show = true;
}