#include "js_user.h"
#include "js_extern_io.h"


typedef struct
{
    jerry_value_t cb;
    uint32_t param;
} js_sh_sw_cb_t;
static js_sh_sw_cb_t js_sh_switch_cb;




#if 1
#include "gui_server.h"
// test event handle process
DECLARE_HANDLER(test)
{
    double val = jerry_get_number_value(args[0]);
    gui_log("enter test arg: %d\n", val);

    // gui_msg_t msg = {.event = GUI_EVENT_EXTERN_IO_JS};
    // uint8_t data[] = {0x01, 0x01, 0x05, 0x01};

    // memcpy(&(msg.payload), data, sizeof(data));
    // gui_extern_event_js_handler(&msg);

    uint8_t id = 5;     // device ID (count from 0)
    uint8_t state = val;  // device Status (0:off, 1:on, 2:offline, 3:online)
    uint32_t data = 0;
    uint8_t *pdata = &data;

    pdata[0] = id;
    pdata[1] = state;

    gui_send_msg_to_js(EXTERN_EVENT_SMARTHOME, SH_EVENT_IOT_SWITCH, (void *)data);

    return jerry_create_undefined();
}
#endif



DECLARE_HANDLER(OnSyncSW)
{
    gui_log("enter OnSyncSW \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register switch cb
        js_sh_switch_cb.cb = args[0];
    }

    return jerry_create_undefined();
}

// extern module abstract
DECLARE_HANDLER(SMTHM)
{
    gui_log("enter SMTHM\n");
    REGISTER_METHOD(this_value, OnSyncSW);
    REGISTER_METHOD(this_value, test);
    return jerry_create_undefined();
}

void js_smarthome_init(void)
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, SMTHM);

}

/*============================================================================*
 *                           Event Handler Functions
 *============================================================================*/

// user defined data struct, depend on application
typedef struct
{
    /* widget event */
    // uint8_t type;    // widget type
    uint8_t index;   // widget index
    uint8_t act;     // widget action
    uint8_t data[2];
} msg_sh_base_t;


// back to js script field, enable complex action, not bond with widget
void gui_sh_event_sw_handler(msg_sh_base_t *msg_sw)
{
    double idx = msg_sw->index;
    double val = msg_sw->act;

    if (!js_sh_switch_cb.cb)
    {
        gui_log("SH sw cb is NULL");
        return ;
    }
    gui_log("index: %d, act: %d", msg_sw->index, msg_sw->act);

    // pass param as object
    jerry_value_t sw = jerry_create_object();
    js_set_property(sw, "idx", jerry_create_number(idx));
    js_set_property(sw, "val", jerry_create_number(val));

    // call func with param
    jerry_value_t res = jerry_call_function((const jerry_value_t)(js_sh_switch_cb.cb),
                                            jerry_create_undefined(), &sw, 1);

    jerry_release_value(res);
}



// same size and mem alignment as gui_msg_js_t
#ifdef  __CC_ARM
#pragma anon_unions
#endif


// smarthome event
void gui_extern_event_sh_handler(gui_msg_js_t *js_msg)
{
    gui_log("sh_event_type: %d", js_msg->js_subevent);
    switch (js_msg->js_subevent)
    {
    case SH_EVENT_IOT_SWITCH:
        {
            gui_sh_event_sw_handler((msg_sh_base_t *)(js_msg->data));
            break;
        }

    default:
        {


            break;
        }
    }

}






