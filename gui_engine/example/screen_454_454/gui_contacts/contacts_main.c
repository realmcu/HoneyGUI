#include "app_gui_main.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_curtainview.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_common.h"
#include "gui_grid.h"
#include "gui_tab.h"
#include "contacts_mgr.h"

static void switch_contacts_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_contacts_touch_cb, obj = 0x%x\n", obj);
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        app_contacts_switch_tabs(CONTACTS_LIST);
        break;

    default:
        break;
    }
}

void design_tab_contacts_main(void *parent)
{
    char *string_contacts_title = "联系人";
    int font_size = 32;
    gui_text_t *text_contacts_title = gui_text_create(parent, "text_contacts_title", 179, 24,
                                                      strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_title, string_contacts_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_contacts_title), font_size);

    gui_switch_t *switch_touch_contacts = gui_switch_create(parent, 129, 129, 196, 196,
                                                            ICON_CONTACTCS_BIN, ICON_CONTACTCS_TOUCH_BIN);
    switch_touch_contacts->on_hl_pic_addr = ICON_CONTACTCS_BIN;
    switch_touch_contacts->off_hl_pic_addr = ICON_CONTACTCS_TOUCH_BIN;
    gui_obj_add_event_cb(switch_touch_contacts,
                         (gui_event_cb_t)switch_contacts_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_touch_contacts,
                         (gui_event_cb_t)switch_contacts_touch_cb,
                         GUI_EVENT_2, NULL);
}


