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

#define MAX_CINTACTS_NUM        3

gui_switch_t *switch_designated_contact[MAX_CINTACTS_NUM] = {NULL};
gui_text_t *text_designated_contact[MAX_CINTACTS_NUM] = {NULL};
char *contacts_list_name[MAX_CINTACTS_NUM] = {"联系人1", "联系人2", "联系人3"};

static void contacts_list_back_touch_cb(void *obj, uint16_t event)
{
    gui_log("contacts_list_back_touch_cb\n");
    app_contacts_switch_tabs(CONTACTS_MAIN);
}

static void switch_designated_contact_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_designated_contact_touch_cb, obj = 0x%x\n", obj);
    switch (event)
    {

    case GUI_EVENT_1:
    case GUI_EVENT_2:

        app_contacts_switch_tabs(CONTACTS_DETAILS);
        break;

    default:
        break;
    }
}

void design_tab_contacts_list(void *parent)
{
    //back touch
    gui_switch_t *switch_contacts_list_back = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN,
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
    gui_text_t *text_contacts_list_title = gui_text_create(parent, "text_contacts_list_title", 179, 19,
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
}
