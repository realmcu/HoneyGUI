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
#include "gui_img_scope.h"
#include "gui_scroll_text.h"

gui_win_t *win_contacts_details = NULL;
gui_switch_t *switch_contacts_details_back = NULL;
gui_text_t *text_contacts_details_title = NULL;
gui_text_t *text_contacts_details_name_title = NULL;
gui_text_t *text_contacts_details_phone_title = NULL;
gui_text_t *text_contacts_details_name = NULL;
gui_text_t *text_contacts_details_phone = NULL;
gui_switch_t *switch_contacts_details_call = NULL;
gui_img_t *img_contacts_details_name = NULL;
gui_img_t *img_contacts_details_phone = NULL;

char *test_dail_num = "12345678910";

static void contacts_details_back_touch_cb(void *obj, gui_event_t event)
{
    gui_log("contacts_details_back_touch_cb\n");
    gui_obj_t *object = (gui_obj_t *)obj;

    gui_obj_t *object_return = pop_current_widget();
//    gui_obj_t *object_return = design_win_contracts_list();
    gui_obj_show(object_return, true);
    //object_return->not_show = false;

    gui_log("contacts_details_back_touch_cb object->parent->name = %s\n", object->parent->name);
    gui_tree_free(win_contacts_details);
    win_contacts_details = NULL;
}

static void switch_contacts_details_call_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_contacts_details_call_touch_cb, obj = 0x%x\n", obj);

    //gui_switch_t *this = (gui_switch_t *)obj;
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_calling;

    switch (event)
    {

    case GUI_EVENT_1:
    case GUI_EVENT_2:
        // jump to win_calling
        gui_log("GUI_EVENT_1 GUI_EVENT_2 obj = 0x%x\n", obj);
        extern gui_win_t *win_calling;
        //gui_app_t *app = get_app_watch_ui();

        win_calling = gui_win_create(&(app->screen), "win_calling", 0, 0, LCD_H, LCD_W);
        extern void design_win_calling(void *parent, char *dial_num);
        design_win_calling(win_calling, test_dail_num);

        push_current_widget(win_contacts_details);
        gui_obj_show(win_contacts_details, false);
        //tabview_main->base.not_show = true;

        break;

    default:
        break;
    }
}

static void text_contacts_details_name_animate_cb(void *obj)
{
    gui_log("text_contacts_details_name_animate_cb\n");

    gui_switch_t *file = (gui_switch_t *)obj;
    gui_text_t *file_name = NULL;

    gui_list_t *node = NULL;
    gui_list_for_each(node, &file->base.child_list)
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->type == TEXTBOX)
        {
            file_name = (gui_text_t *)find_obj;
            break;
        }
    }

    if (file_name == NULL)
    {
        return;
    }

    char *string_title = (char *)file_name->content;
    gui_text_set(text_contacts_details_name, string_title, "rtk_font_mem", APP_COLOR_WHITE,
                 strlen(string_title),
                 FONT_H_32);
}

void design_win_contracts_details(gui_win_t *parent, gui_switch_t *selected_contacts_list)
{
    //back touch
    switch_contacts_details_back = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
                                                     ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_contacts_details_back, (gui_event_cb_t)contacts_details_back_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_contacts_details_back, (gui_event_cb_t)contacts_details_back_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    //contacts_details
    char *string_contacts_details_title = "联系人详情";
    int font_size = 32;
    text_contacts_details_title = gui_text_create(parent, "text_contacts_details_title", 179, 24,
                                                  strlen(string_contacts_details_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_title, string_contacts_details_title, "rtk_font_mem",
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_title), font_size);

    //name/phone
    char *string_contacts_details_name_title = "姓名";
    text_contacts_details_name_title = gui_text_create(parent, "text_contacts_details_name", 83, 94,
                                                       strlen(string_contacts_details_name_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_name_title, string_contacts_details_name_title, "rtk_font_mem",
                 APP_COLOR_WHITE, strlen(string_contacts_details_name_title), font_size);
    img_contacts_details_name = gui_img_create_from_mem(parent, "img_contacts_details_name",
                                                        ICON_TEXT_BASE_DARK_BIN, 83, 137,
                                                        288, 64);
    char *string_contacts_details_name = "联系人0";
    text_contacts_details_name = gui_text_create(parent, "text_contacts_details_name", 154, 148, 160,
                                                 font_size);
    gui_text_set(text_contacts_details_name, string_contacts_details_name, "rtk_font_mem",
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_name), font_size);
    gui_text_set_animate(text_contacts_details_name, 1000, 2, text_contacts_details_name_animate_cb,
                         selected_contacts_list);

    char *string_contacts_details_phone_title = "电话";
    text_contacts_details_phone_title = gui_text_create(parent, "text_contacts_details_phone_title", 83,
                                                        201,
                                                        strlen(string_contacts_details_phone_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_phone_title, string_contacts_details_phone_title, "rtk_font_mem",
                 APP_COLOR_WHITE, strlen(string_contacts_details_phone_title), font_size);
    img_contacts_details_phone = gui_img_create_from_mem(parent, "img_contacts_details_phone",
                                                         ICON_TEXT_BASE_DARK_BIN, 83, 244,
                                                         288, 64);
    char *string_contacts_details_phone = "12345678910";
    text_contacts_details_phone = gui_text_create(parent, "text_contacts_details_phone", 154, 255, 200,
                                                  font_size);
    gui_text_set(text_contacts_details_phone, string_contacts_details_phone, "rtk_font_mem",
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_phone), font_size);

    //call phone
    switch_contacts_details_call = gui_switch_create(parent, 88, 373, 277, 81,
                                                     ICON_CALL_CONTACTS_BIN, ICON_CALL_CONTACTS_BIN);
    switch_contacts_details_call->on_hl_pic_addr = ICON_CALL_CONTACTS_TOUCH_BIN;
    switch_contacts_details_call->off_hl_pic_addr = ICON_CALL_CONTACTS_TOUCH_BIN;
    gui_obj_add_event_cb(switch_contacts_details_call,
                         (gui_event_cb_t)switch_contacts_details_call_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_contacts_details_call,
                         (gui_event_cb_t)switch_contacts_details_call_touch_cb,
                         GUI_EVENT_2, NULL);

}