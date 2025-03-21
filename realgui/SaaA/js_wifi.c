#include "js_user.h"
#include "js_extern_io.h"

#if defined ENABLE_WIFI_NIC

#if (defined RTL87x2G)
#include "app_msg.h"
#include "wifi_nic_msg.h"

static ssid_info_t ssid_info;
static T_IO_MSG wifi_msg_tab[] =
{
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_ON},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_OFF},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_SCAN},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_CONNECT, .u.buf = &ssid_info},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_EVENT_MANAGE_DISCONNECT},
};
#endif

#elif (defined __WIN32)
#define JS_DBG_WIFI
typedef struct
{
    uint8_t ssid_index;
    char password[65];
} ssid_info_t;
static ssid_info_t ssid_info;

#endif


typedef enum
{
    WIFI_CMD_TURN_ON        = 0,
    WIFI_CMD_TURN_OFF       = 1,
    WIFI_CMD_SCAN           = 2,
    WIFI_CMD_CONNECT        = 3,
    WIFI_CMD_DISCONNECT     = 4,

} wifi_cmd_t;


typedef struct
{
    jerry_value_t on_cb;
    jerry_value_t off_cb;
    jerry_value_t scan_cb;
    jerry_value_t connect_cb;
    jerry_value_t disconnect_cb;

    void *ap_array;

    uint8_t state;
    uint8_t ap_count;
#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    ssid_info_t ssid_info;
#endif
} js_wifi_data_t;

js_wifi_data_t js_wifi_data;


static void gui_send_msg_to_app_task(wifi_cmd_t cmd)
{
#if defined ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    send_msg_to_wifi_server(&wifi_msg_tab[cmd]);
#endif
#elif (defined __WIN32)
    gui_log("send msg -> wifi: cmd %d\n", cmd);
#endif
}

/*============================================================================*
 *                           Wi-Fi Manage Functions
 *============================================================================*/

#ifdef __WIN32
#define MAX_AP_SIZE (10)
typedef struct
{
    char ssid[33];
    uint8_t ssid_len;
    uint8_t security;  // 0: no passwd
    int16_t rssi;
} ap_info_t;

typedef struct
{
    ap_info_t ap_info[MAX_AP_SIZE];
    uint8_t ap_count;
} ap_array_t;

ap_array_t ap_array =
{
    .ap_info[0] = {.ssid = {"ssid_00"}, .ssid_len = 7, .rssi = -80, .security = 0},
    .ap_info[1] = {.ssid = {"ssid_001"}, .ssid_len = 8, .rssi = -60, .security = 1},
    .ap_info[2] = {.ssid = {"ssid_002"}, .ssid_len = 8, .rssi = -50, .security = 0},
    .ap_info[3] = {.ssid = {"ssid_003"}, .ssid_len = 8, .rssi = -90, .security = 1},
    .ap_count = 4

};
#endif



DECLARE_HANDLER(test)
{
#ifdef JS_DBG_WIFI
    uint32_t param = (uint32_t)jerry_get_number_value(args[0]);

    uint8_t sub_event = 0;
    void *data32 = 0;

    gui_log("enter test %d\n", param);
    switch (param)
    {
    case 0:
        sub_event = WIFI_EVENT_MANAGE_ON;
        data32 = (void *)0x00;
        break;
    case 1:
        sub_event = WIFI_EVENT_MANAGE_ON;
        data32 = (void *)0x01;
        break;
    case 2:
        sub_event = WIFI_EVENT_MANAGE_OFF;
        data32 = (void *)0x00;
        break;
    case 3:
        sub_event = WIFI_EVENT_MANAGE_SCAN;
        data32 = (void *)&ap_array;
        break;
    case 4:
        sub_event = WIFI_EVENT_MANAGE_CONNECT;
        data32 = (void *)0x00;
        break;
    case 5:
        sub_event = WIFI_EVENT_MANAGE_CONNECT;
        data32 = (void *)0x01;
        break;
    }

    gui_send_msg_to_js(EXTERN_EVENT_WIFI, sub_event, (void *)data32);
#endif
    return jerry_create_undefined();
}


DECLARE_HANDLER(turnOn)
{
    gui_log("wifi turnOn\n");
    gui_send_msg_to_app_task(WIFI_CMD_TURN_ON);
    return jerry_create_undefined();
}
DECLARE_HANDLER(turnOff)
{
    gui_log("wifi turnOff\n");
    gui_send_msg_to_app_task(WIFI_CMD_TURN_OFF);
    return jerry_create_undefined();
}
DECLARE_HANDLER(scan)
{
    gui_log("wifi scan\n");
    gui_send_msg_to_app_task(WIFI_CMD_SCAN);
    return jerry_create_undefined();
}
DECLARE_HANDLER(connect)
{
    // gui_log("wifi connect, %d %s\n", ssid_info.ssid_index, ssid_info.password);
    gui_log("wifi connect\n");

#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    ssid_info = js_wifi_data.ssid_info;
#endif
    gui_send_msg_to_app_task(WIFI_CMD_CONNECT);
    return jerry_create_undefined();
}
DECLARE_HANDLER(disconnect)
{
    gui_log("wifi disconnect\n");
    gui_send_msg_to_app_task(WIFI_CMD_DISCONNECT);
    return jerry_create_undefined();
}

DECLARE_HANDLER(setConIndex)
{
    gui_log("wifi setConIndex\n");

#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    if (args_cnt == 1 && jerry_value_is_number(args[0]))
    {
        int i = jerry_get_number_value(args[0]);
        if (i >= 0 && i < js_wifi_data.ap_count)
        {
            js_wifi_data.ssid_info.ssid_index = i;
            js_wifi_data.ssid_info.password[0] = '\0';
        }
    }
#endif
    return jerry_create_undefined();
}

DECLARE_HANDLER(initPasswd)
{
    gui_log("wifi initPasswd\n");
#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    js_wifi_data.ssid_info.password[0] = '\0';
    gui_kb_set_pswd(js_wifi_data.ssid_info.password, 0);
#endif
    return jerry_create_undefined();
}

DECLARE_HANDLER(setConPasswd)
{
    char *pswd = gui_kb_get_pswd();
    gui_log("wifi setConPasswd %s\n", pswd);

#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    memcpy(js_wifi_data.ssid_info.password, pswd, strlen(pswd) + 1);
#endif
    return jerry_create_undefined();
}

DECLARE_HANDLER(setPasswdVisible)
{
    gui_log("wifi setPasswdVisible\n");

#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
    if (args_cnt == 2 && jerry_value_is_string(args[0]) && jerry_value_is_number(args[1]))
    {
        jerry_value_t global_obj = jerry_get_global_object();
        jerry_value_t app_property = js_get_property(global_obj, "app");
        gui_app_t *app = NULL;
        gui_obj_t *widget = NULL;
        char *a = js_value_to_string(args[0]);

        jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
        jerry_release_value(global_obj);
        jerry_release_value(app_property);

        gui_obj_tree_get_widget_by_name(&app->screen, a, &widget);
        gui_free(a);

        gui_text_t *txtbox = (gui_text_t *)widget;
        int visible = jerry_get_number_value(args[1]);
        if (txtbox)
        {
            if (visible)
            {
                memcpy(txtbox->content, js_wifi_data.ssid_info.password, txtbox->len);
            }
            else
            {
                memset(txtbox->content, '*', txtbox->len);
            }
            gui_text_content_set(txtbox, txtbox->content, txtbox->len);
        }
    }
#endif

    return jerry_create_undefined();
}


DECLARE_HANDLER(onOnDone)
{
    gui_log("enter onOnDone \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_wifi_data.on_cb = args[0];
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onOffDone)
{
    gui_log("enter onOffDone \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_wifi_data.off_cb = args[0];
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onScanDone)
{
    gui_log("enter onScanDone \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_wifi_data.scan_cb = args[0];
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onConnectDone)
{
    gui_log("enter onConnectDone \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_wifi_data.connect_cb = args[0];
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onDisconnect)
{
    gui_log("enter onConnectDone \n");
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        // register cb
        js_wifi_data.disconnect_cb = args[0];
    }

    return jerry_create_undefined();
}
/*============================================================================*
 *                           Wi-Fi Speed Test Functions
 *============================================================================*/

DECLARE_HANDLER(startSpeed)
{
    gui_log("enter startSpeed\n");
#ifdef ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    extern void iperf_start(char *ip, char *port);
    iperf_start(NULL, NULL);
#endif
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(stopSpeed)
{
    gui_log("enter stopSpeed\n");
#ifdef ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    extern void iperf_stop(void);
    iperf_stop();
#endif
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(readSpeed)
{
//    gui_log("enter readSpeed\n");
    static uint32_t speed = 10;
#ifdef ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    extern uint32_t iperf_get_current_speed(void);
    speed = iperf_get_current_speed();
#endif
#else
    speed++;
    speed = speed > 2000 ?  10 : speed;
#endif
    return jerry_create_number((double)speed);
}

void js_wifi_init()
{

    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t wifi = jerry_create_object();
    js_set_property(global_obj, "wifi", wifi);
    REGISTER_METHOD(wifi, turnOn);
    REGISTER_METHOD(wifi, turnOff);
    REGISTER_METHOD(wifi, scan);
    REGISTER_METHOD(wifi, connect);
    REGISTER_METHOD(wifi, disconnect);

    REGISTER_METHOD(wifi, setConIndex);
    REGISTER_METHOD(wifi, initPasswd);
    REGISTER_METHOD(wifi, setConPasswd);
    REGISTER_METHOD(wifi, setPasswdVisible);

    REGISTER_METHOD(wifi, onOnDone);
    REGISTER_METHOD(wifi, onOffDone);
    REGISTER_METHOD(wifi, onScanDone);
    REGISTER_METHOD(wifi, onConnectDone);

    REGISTER_METHOD(wifi, startSpeed);
    REGISTER_METHOD(wifi, stopSpeed);
    REGISTER_METHOD(wifi, readSpeed);
    REGISTER_METHOD(wifi, test);

    jerry_release_value(global_obj);
}

/*============================================================================*
 *                           Event Handler Functions
 *============================================================================*/

static void gui_wifi_manage_on_handler(uint32_t data)
{
    // gui_log("wifi on param: 0x%x\n", msg->param);
    if (js_wifi_data.on_cb)
    {
        // call func with param
        double a = (uint32_t)(data);
        jerry_value_t param = jerry_create_number(a);
        jerry_value_t res = jerry_call_function((const jerry_value_t)(js_wifi_data.on_cb),
                                                jerry_create_undefined(), &param, 1);

        jerry_release_value(res);
    }
}
static void gui_wifi_manage_off_handler(uint32_t data)
{
    // gui_log("wifi off param: 0x%x\n", msg->param);
    if (js_wifi_data.off_cb)
    {
        // call func with param
        double a = (uint32_t)(data);
        jerry_value_t param = jerry_create_number(a);
        jerry_value_t res = jerry_call_function((const jerry_value_t)(js_wifi_data.off_cb),
                                                jerry_create_undefined(), &param, 1);

        jerry_release_value(res);
    }
}
static void gui_wifi_manage_scan_handler(uint32_t data)
{
    // gui_log("wifi scan param: 0x%x\n", *((int *)msg->param));
#if (defined ENABLE_WIFI_NIC) || (defined __WIN32)
#if (defined RTL87x2G) || (defined __WIN32)
    if (js_wifi_data.scan_cb)
    {
        ap_array_t *array = (ap_array_t *)data;
        js_wifi_data.ap_count = array->ap_count;
        js_wifi_data.ap_array = array;

        jerry_value_t ap_info = jerry_create_object();

        js_set_property(ap_info, "cnt", jerry_create_number(array->ap_count));

        jerry_value_t ssid_array = jerry_create_array(array->ap_count);
        jerry_value_t rssi_array = jerry_create_array(array->ap_count);
        jerry_value_t security_array = jerry_create_array(array->ap_count);
        for (uint8_t i = 0; i < array->ap_count; i++)
        {
            jerry_set_property_by_index(ssid_array, i, jerry_create_string(array->ap_info[i].ssid));
            jerry_set_property_by_index(rssi_array, i, jerry_create_number(array->ap_info[i].rssi));
            jerry_set_property_by_index(security_array, i, jerry_create_number(array->ap_info[i].security));
        }
        js_set_property(ap_info, "ssid", ssid_array);
        js_set_property(ap_info, "rssi", rssi_array);
        js_set_property(ap_info, "scrt", security_array);

        // call func with param
        double a = (uint32_t)(data);
        jerry_value_t param = jerry_create_number(a);
        jerry_value_t res = jerry_call_function((const jerry_value_t)(js_wifi_data.scan_cb),
                                                jerry_create_undefined(), &ap_info, 1);

        jerry_release_value(res);
    }

#endif
#endif
}
static void gui_wifi_manage_connect_handler(uint32_t data)
{
    // gui_log("wifi connect param: 0x%x\n", msg->param);
    if (js_wifi_data.connect_cb)
    {
        // call func with param
        double a = (uint32_t)(data);
        jerry_value_t param = jerry_create_number(a);
        jerry_value_t res = jerry_call_function((const jerry_value_t)(js_wifi_data.connect_cb),
                                                jerry_create_undefined(), &param, 1);

        jerry_release_value(res);
    }
}

static void gui_wifi_manage_disconnect_handler(uint32_t data)
{
    // gui_log("wifi disconnect param: 0x%x\n", msg->param);
    if (js_wifi_data.disconnect_cb)
    {
        // call func with param
        double a = (uint32_t)(data);
        jerry_value_t param = jerry_create_number(a);
        jerry_value_t res = jerry_call_function((const jerry_value_t)(js_wifi_data.disconnect_cb),
                                                jerry_create_undefined(), &param, 1);

        jerry_release_value(res);
    }
}

// wifi event
void gui_extern_event_wifi_handler(gui_msg_js_t *js_msg)
{
    gui_log("sub_event_type: %d\n", js_msg->js_subevent);
    switch (js_msg->js_subevent)
    {
    case WIFI_EVENT_MANAGE_ON:
        {
            gui_wifi_manage_on_handler(js_msg->data32);
        }
        break;
    case WIFI_EVENT_MANAGE_OFF:
        {
            gui_wifi_manage_off_handler(js_msg->data32);
        }
        break;
    case WIFI_EVENT_MANAGE_SCAN:
        {
            gui_wifi_manage_scan_handler(js_msg->data32);
        }
        break;
    case WIFI_EVENT_MANAGE_CONNECT:
        {
            gui_wifi_manage_connect_handler(js_msg->data32);
        }
        break;
    case WIFI_EVENT_MANAGE_DISCONNECT:
        {
            gui_wifi_manage_disconnect_handler(js_msg->data32);
        }
        break;
    default:
        {


            break;
        }
    }

}


