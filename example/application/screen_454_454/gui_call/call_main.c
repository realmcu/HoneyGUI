#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "call_mgr.h"

static void gui_call_status_update_cb(void *obj, uint16_t event)
{
    gui_log("gui_call_status_update_cb event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_CALL_INCOMING:
        {
            app_call_switch_tabs(CALL_INCOMING);
        }
        break;
    case GUI_EVENT_CALL_OUTGOING:
        {
            app_call_switch_tabs(CALL_OUTGOING);
        }
        break;
    default:
        break;
    }
}

static void switch_call_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_call_touch_cb event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        app_call_switch_tabs(CALL_DIAL);
        break;
    default:
        break;
    }
}

void design_tab_call_main(void *parent)
{
    char *string_call_title = "电话";
    gui_text_t *text_call_title = gui_text_create(parent, "text_call_title", 195, 24,
                                                  strlen(string_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_call_title, string_call_title, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_call_title), FONT_H_32);
    gui_switch_t *switch_call = gui_switch_create(parent, 129, 129, 196, 196, ICON_CALL_BIN,
                                                  ICON_CALL_BIN);
    switch_call->on_hl_pic_addr = ICON_CALL_TOUCH_BIN;
    switch_call->off_hl_pic_addr = ICON_CALL_TOUCH_BIN;
    //add touch event callback
    gui_obj_add_event_cb(switch_call, (gui_event_cb_t)switch_call_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_call, (gui_event_cb_t)switch_call_touch_cb, GUI_EVENT_2, NULL);

    // add event to update incomming/outgoing/active/end call gui
    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_INCOMING,
                         NULL);
    gui_obj_add_event_cb(parent, (gui_event_cb_t)gui_call_status_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_OUTGOING,
                         NULL);
}
