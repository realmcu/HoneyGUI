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
#include "contacts_mgr.h"

gui_text_t *text_contacts_details_name = NULL;
gui_text_t *text_contacts_details_phone = NULL;

char *test_dail_num = "12345678910";

static void contacts_details_back_touch_cb(void *obj, uint16_t event)
{
    gui_log("contacts_details_back_touch_cb\n");
    app_contacts_switch_tabs(CONTACTS_LIST);
}

static void switch_contacts_details_call_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_contacts_details_call_touch_cb, obj = 0x%x\n", obj);
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        gui_log("GUI_EVENT_1 GUI_EVENT_2 obj = 0x%x\n", obj);
//Add the call gui jump
        break;

    default:
        break;
    }
}

static void text_contacts_details_name_animate_cb(void *obj)
{
    gui_log("text_contacts_details_name_animate_cb\n");
//Add the display of the name
}

void design_tab_contacts_details(void *parent)
{
    //back touch
    gui_switch_t *switch_contacts_details_back = gui_switch_create(parent, 131, 24, 48, 48,
                                                                   ICON_BACK_BIN,
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
    gui_text_t *text_contacts_details_title = gui_text_create(parent, "text_contacts_details_title",
                                                              179, 24,
                                                              strlen(string_contacts_details_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_title, string_contacts_details_title, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_title), font_size);

    //name/phone
    char *string_contacts_details_name_title = "姓名";
    gui_text_t *text_contacts_details_name_title = gui_text_create(parent, "text_contacts_details_name",
                                                                   83, 94,
                                                                   strlen(string_contacts_details_name_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_name_title, string_contacts_details_name_title,
                 GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_name_title), font_size);
    gui_img_create_from_mem(parent, "img_contacts_details_name",
                            ICON_TEXT_BASE_DARK_BIN, 83, 137,
                            288, 64);
    char *string_contacts_details_name = "联系人0";
    text_contacts_details_name = gui_text_create(parent, "text_contacts_details_name", 154, 148, 160,
                                                 font_size);
    gui_text_set(text_contacts_details_name, string_contacts_details_name, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_name), font_size);
    gui_text_set_animate(text_contacts_details_name, 1000, 2, text_contacts_details_name_animate_cb,
                         text_contacts_details_name);

    char *string_contacts_details_phone_title = "电话";
    gui_text_t *text_contacts_details_phone_title = gui_text_create(parent,
                                                                    "text_contacts_details_phone_title", 83,
                                                                    201,
                                                                    strlen(string_contacts_details_phone_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_details_phone_title, string_contacts_details_phone_title,
                 GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_phone_title), font_size);
    gui_img_create_from_mem(parent, "img_contacts_details_phone",
                            ICON_TEXT_BASE_DARK_BIN, 83, 244,
                            288, 64);
    char *string_contacts_details_phone = "12345678910";
    text_contacts_details_phone = gui_text_create(parent, "text_contacts_details_phone", 154, 255, 200,
                                                  font_size);
    gui_text_set(text_contacts_details_phone, string_contacts_details_phone, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_contacts_details_phone), font_size);

    //call phone
    gui_switch_t *switch_contacts_details_call = gui_switch_create(parent, 88, 373, 277, 81,
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
