#include "js_user.h"
#include "js_extern_io.h"

#define MAX_NUM_JS_KEY  (12)

typedef enum
{
    JS_KEY_IS_IDLE       = 0,
    JS_KEY_IS_PRESSING   = 1,

} JS_KEY_INFO;

typedef struct
{
    jerry_value_t cb;
    jerry_value_t param;
} js_key_cb_t;

typedef struct
{
    uint8_t id;
    uint8_t state;
    js_key_cb_t press_cb;
    js_key_cb_t click_cb;
    js_key_cb_t longPress_cb;
    js_key_cb_t release_cb;

} js_key_data_t;

static uint8_t key_num;
static js_key_data_t *key_data[MAX_NUM_JS_KEY];

#if 1
#include "gui_server.h"
// test event handle process
DECLARE_HANDLER(test)
{
    gui_log("enter test \n");

    uint8_t id = 1;
    uint32_t data = id;

    gui_send_msg_to_js(EXTERN_EVENT_KEY, KEY_EVENT_CLICK, (void *)data);
    return jerry_create_undefined();
}
#endif



DECLARE_HANDLER(onPress)
{
    gui_log("enter onPress \n");
    uint8_t id = 0;
    jerry_value_t val_1;
    val_1 = js_get_property(this_value, "id");
    id = jerry_get_number_value(val_1);

    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_key_data_t *pkey_data = key_data[id];
        pkey_data->press_cb.cb = args[0];
        if (args_cnt >= 2)
        {
            pkey_data->press_cb.param = args[1];
        }
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onClick)
{
    gui_log("enter onClick \n");
    uint8_t id = 0;
    jerry_value_t val_1;
    val_1 = js_get_property(this_value, "id");
    id = jerry_get_number_value(val_1);
    gui_log("id %d\n", id);

    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_key_data_t *pkey_data = key_data[id];
        pkey_data->click_cb.cb = args[0];
        gui_log("cb 0x%x\n", pkey_data->click_cb.cb);
        if (args_cnt >= 2)
        {
            pkey_data->click_cb.param = args[1];
        }
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onLongPress)
{
    gui_log("enter onLongPress \n");
    uint8_t id = 0;
    jerry_value_t val_1;
    val_1 = js_get_property(this_value, "id");
    id = jerry_get_number_value(val_1);

    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_key_data_t *pkey_data = key_data[id];
        pkey_data->longPress_cb.cb = args[0];
        if (args_cnt >= 2)
        {
            pkey_data->longPress_cb.param = args[1];
        }
    }

    return jerry_create_undefined();
}
DECLARE_HANDLER(onRelease)
{
    gui_log("enter onRelease \n");
    uint8_t id = 0;
    jerry_value_t val_1;
    val_1 = js_get_property(this_value, "id");
    id = jerry_get_number_value(val_1);

    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_key_data_t *pkey_data = key_data[id];
        pkey_data->release_cb.cb = args[0];
        if (args_cnt >= 2)
        {
            pkey_data->release_cb.param = args[1];
        }
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(state)
{
    // gui_log("enter state \n");
    uint8_t id = 0;
    jerry_value_t val_1;
    val_1 = js_get_property(this_value, "id");
    id = jerry_get_number_value(val_1);

    //
    js_key_data_t *pkey_data = key_data[id];
    double state = pkey_data->state;

    return jerry_create_number(state);
}

// extern module abstract
DECLARE_HANDLER(KEY)
{
    gui_log("enter KEY\n");
    int id = -1;
    if (args_cnt >= 1)
    {
        if (jerry_value_is_number(args[0]))
        {
            id = jerry_get_number_value(args[0]);

        }
    }
    jerry_value_t id_js = jerry_create_number(id);
    js_set_property(this_value, "id", id_js);

    REGISTER_METHOD(this_value, state);
    REGISTER_METHOD(this_value, onPress);
    REGISTER_METHOD(this_value, onClick);
    REGISTER_METHOD(this_value, onLongPress);
    REGISTER_METHOD(this_value, onRelease);
    REGISTER_METHOD(this_value, test);

    key_num++;
    if (key_num <= MAX_NUM_JS_KEY)
    {
        js_key_data_t *pkey_data = (js_key_data_t *)gui_malloc(sizeof(js_key_data_t));
        GUI_ASSERT(pkey_data != NULL);
        memset(pkey_data, 0, sizeof(js_key_data_t));
        pkey_data->id = id;
        key_data[key_num - 1] = pkey_data;
    }
    else
    {
        gui_log("js key not created\n");
    }
    return jerry_create_undefined();
}

void js_key_init(void)
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, KEY);
}

/*============================================================================*
 *                           Event Handler Functions
 *============================================================================*/

// user defined data struct, depend on application


// back to js script field, enable complex action, not bond with widget
void gui_key_event_press_handler(uint32_t id)
{
    js_key_data_t *pkey_data = key_data[id];

    if (!(pkey_data->press_cb.cb))
    {
        gui_log("key cb is NULL");
        return ;
    }
    gui_log("id: %d", id);

    // change state
    pkey_data->state = JS_KEY_IS_PRESSING;

    // call func with param
    jerry_value_t res = jerry_call_function((const jerry_value_t)(pkey_data->press_cb.cb),
                                            jerry_create_undefined(), &(pkey_data->press_cb.param), 1);

    jerry_release_value(res);
}
void gui_key_event_click_handler(uint32_t id)
{
    js_key_data_t *pkey_data = key_data[id];

    gui_log("id: %d, cb 0x%x", id, pkey_data->click_cb.cb);
    if (!(pkey_data->click_cb.cb))
    {
        gui_log("key cb is NULL");
        return ;
    }
    gui_log("id: %d", id);

    // change state
    pkey_data->state = JS_KEY_IS_IDLE;

    // call func with param
    jerry_value_t res = jerry_call_function((const jerry_value_t)(pkey_data->click_cb.cb),
                                            jerry_create_undefined(), &(pkey_data->click_cb.param), 1);

    jerry_release_value(res);
}
void gui_key_event_longPress_handler(uint32_t id)
{
    js_key_data_t *pkey_data = key_data[id];

    if (!(pkey_data->longPress_cb.cb))
    {
        gui_log("key cb is NULL");
        return ;
    }
    gui_log("id: %d", id);

    // change state
    pkey_data->state = JS_KEY_IS_PRESSING;

    // call func with param
    jerry_value_t res = jerry_call_function((const jerry_value_t)(pkey_data->longPress_cb.cb),
                                            jerry_create_undefined(), &(pkey_data->longPress_cb.param), 1);

    jerry_release_value(res);
}
void gui_key_event_release_handler(uint32_t id)
{
    js_key_data_t *pkey_data = key_data[id];

    if (!(pkey_data->release_cb.cb))
    {
        gui_log("key cb is NULL");
        return ;
    }
    gui_log("id: %d", id);

    // change state
    pkey_data->state = JS_KEY_IS_IDLE;

    // call func with param
    jerry_value_t res = jerry_call_function((const jerry_value_t)(pkey_data->release_cb.cb),
                                            jerry_create_undefined(), &(pkey_data->release_cb.param), 1);

    jerry_release_value(res);
}


// key event
void gui_extern_event_key_handler(gui_msg_js_t *js_msg)
{
    gui_log("sub_event_type: %d", js_msg->js_subevent);
    switch (js_msg->js_subevent)
    {
    case KEY_EVENT_PRESS:
        {
            gui_key_event_press_handler((js_msg->data32));
        }
        break;
    case KEY_EVENT_CLICK:
        {
            gui_key_event_click_handler((js_msg->data32));
        }
        break;
    case KEY_EVENT_LONG_PRESS:
        {
            gui_key_event_longPress_handler((js_msg->data32));
        }
        break;
    case KEY_EVENT_RELEASE:
        {
            gui_key_event_release_handler((js_msg->data32));
        }
        break;
    default:
        {



        }
        break;
    }

}





