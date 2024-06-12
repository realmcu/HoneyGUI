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
    gui_log("enter test \n");

    gui_msg_t msg = {.event = GUI_EVENT_EXTERN_IO_JS};
    uint8_t data[] = {0x01, 0x01, 0x05, 0x01};

    memcpy(&(msg.payload), data, sizeof(data));
    gui_extern_event_js_handler(&msg);

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
// typedef struct gui_msg
// {
//     uint16_t event;
//     gui_msg_cb cb;  // typedef void (*gui_msg_cb)(void *);
//     void *payload;
// } gui_msg_t;

typedef struct
{
    /* user field of event */
    union
    {
        struct
        {
            uint8_t extern_event_type;
            uint8_t sh_event_type;
            uint8_t data[2];
        };
        gui_msg_cb cb;    // gui_msg
    };
    union
    {
        uint8_t data_rsv[4];   // reserve
        void *payload;   // gui_msg
    };
} gui_msg_sh_t;


// smarthome event
void gui_extern_event_sh_handler(gui_msg_js_t *js_msg)
{
    gui_msg_sh_t *sh_msg = (gui_msg_sh_t *)js_msg;

    gui_log("sh_event_type: %d", sh_msg->sh_event_type);
    switch (sh_msg->sh_event_type)
    {
    case SH_EVENT_WIFI_SWITCH:
        {
            gui_sh_event_sw_handler((msg_sh_base_t *)(sh_msg->data));
            break;
        }

    default:
        {


            break;
        }
    }

}






