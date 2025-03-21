#include "js_user.h"
#include "js_extern_io.h"

extern void gui_extern_event_sh_handler(gui_msg_js_t *js_msg);
extern void gui_extern_event_timer_handler(gui_msg_js_t *js_msg);
extern void gui_extern_event_wifi_handler(gui_msg_js_t *js_msg);
extern void gui_extern_event_key_handler(gui_msg_js_t *js_msg);


void gui_send_msg_to_js(uint8_t event, uint8_t subevent, void *data)
{
    gui_msg_js_t msg = {.gui_event = GUI_EVENT_USER_DEFINE, .gui_cb = gui_extern_event_js_handler};
    msg.js_event = event;
    msg.js_subevent = subevent;
    msg.pdata = data;

    gui_send_msg_to_server(&msg);
}


void gui_extern_event_js_handler(gui_msg_t *msg)
{
    gui_msg_js_t *js_msg = (gui_msg_js_t *) msg;

    switch (js_msg->js_event)
    {
    case EXTERN_EVENT_SMARTHOME:
        {
            gui_extern_event_sh_handler(js_msg);
            break;
        }
    case EXTERN_EVENT_TIMER:
        {
            gui_extern_event_timer_handler(js_msg);
            break;
        }
    case EXTERN_EVENT_WIFI:
        {
            gui_extern_event_wifi_handler(js_msg);
            break;
        }
    case EXTERN_EVENT_KEY:
        {
            gui_extern_event_key_handler(js_msg);
            break;
        }
    // case EXTERN_EVENT_XXX:


    default:
        {


            break;
        }
    }

}

