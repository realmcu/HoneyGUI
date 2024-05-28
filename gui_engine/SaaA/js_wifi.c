#include "js_user.h"
#include "js_extern_io.h"


// #ifdef __WIN32
// typedef struct
// {
//    uint8_t ssid_index;
//    char password[65];
// } ssid_info_t;
// #endif



#if defined ENABLE_WIFI_NIC
#include "app_msg.h"
#include "wifi_nic_msg.h"
#if (defined RTL87x2G)

static ssid_info_t ssid_info;
static T_IO_MSG wifi_msg_tab[] =
{
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_ON},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_OFF},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_SCAN},
    {.type = WIFI_MSG_TYPE_MANAGE, .subtype = WIFI_MSG_MANAGE_CONNECT, .u.buf = &ssid_info},
};

//extern bool app_send_msg_to_apptask(T_IO_MSG *p_msg);

#endif

#else

#endif


typedef enum
{
    WIFI_CMD_TURN_ON  = 0,
    WIFI_CMD_TURN_OFF = 1,
    WIFI_CMD_SCAN     = 2,
    WIFI_CMD_CONNECT  = 3,

} wifi_cmd_t;



static void gui_send_msg_to_app_task(wifi_cmd_t cmd)
{
#if defined ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    send_msg_to_wifi_server(&wifi_msg_tab[cmd]);
#endif
#endif
}
static void js_wifi_set_ssid_info(uint8_t idx, char *passwd)
{
#if defined ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    ssid_info.ssid_index = idx;
    memset(ssid_info.password, 0, sizeof(ssid_info.password));
    strcpy(ssid_info.password, passwd);
#endif
#endif
}
/*============================================================================*
 *                           Wi-Fi Manage Functions
 *============================================================================*/
//#define MAX_AP_SIZE (10)

//typedef struct
//{
//    char ssid[33];
//    uint8_t ssid_len;
//    uint8_t security;
//    int8_t rssi;
//} ap_info_t;

//typedef struct
//{
//    ap_info_t ap_info[MAX_AP_SIZE];
//    uint8_t ap_count;
//} ap_array_t;


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
    gui_send_msg_to_app_task(WIFI_CMD_CONNECT);
    return jerry_create_undefined();
}

/*============================================================================*
 *                           Wi-Fi Speed Test Functions
 *============================================================================*/

DECLARE_HANDLER(startSpeed)
{
    gui_log("enter startSpeed\n");
#ifdef ENABLE_WIFI_NIC
    extern void iperf_start(char *ip, char *port);
    iperf_start(NULL, NULL);
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(stopSpeed)
{
    gui_log("enter stopSpeed\n");
#ifdef ENABLE_WIFI_NIC
    extern void iperf_stop(void);
    iperf_stop();
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(readSpeed)
{
//    gui_log("enter readSpeed\n");
    static uint32_t speed = 10;
#ifdef ENABLE_WIFI_NIC
    extern uint32_t iperf_get_current_speed(void);
    speed = iperf_get_current_speed();
#else
    speed++;
    speed = speed > 2000 ?  10 : speed;
#endif
    return jerry_create_number((double)speed);
}

void wifi_init()
{

    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t wifi = jerry_create_object();
    js_set_property(global_obj, "wifi", wifi);
    REGISTER_METHOD(wifi, turnOn);
    REGISTER_METHOD(wifi, turnOff);
    REGISTER_METHOD(wifi, scan);
    REGISTER_METHOD(wifi, connect);
    REGISTER_METHOD(wifi, startSpeed);
    REGISTER_METHOD(wifi, stopSpeed);
    REGISTER_METHOD(wifi, readSpeed);
    jerry_release_value(global_obj);
}

/*============================================================================*
 *                           Event Handler Functions
 *============================================================================*/

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
            uint8_t extern_event_type; // EXTERN_EVENT_WIFI
            uint8_t sub_event_type;
            uint8_t data[2];
        };
        gui_msg_cb cb;    // gui_msg
    };
    union
    {
        uint8_t data_rsv[4];   // reserve
        void *param;
        void *payload;   // gui_msg
    };
} gui_msg_wifi_t;

// // turnOn
// gui_msg_t msg = {.event = 0x18, .cb = 0x0003, .payload=0x00/errflg};
// // turnOff
// gui_msg_t msg = {.event = 0x18, .cb = 0x0103, .payload=0x00};
// // scan
// gui_msg_t msg = {.event = 0x18, .cb = 0x0203, .payload=&ap_array};
// // connect
// gui_msg_t msg = {.event = 0x18, .cb = 0x0303, .payload=0x00/errflg};



static void gui_wifi_manage_on_handler(gui_msg_wifi_t *msg)
{
    gui_log("wifi on param: 0x%x\n", msg->param);
}
static void gui_wifi_manage_off_handler(gui_msg_wifi_t *msg)
{
    gui_log("wifi off param: 0x%x\n", msg->param);
}
static void gui_wifi_manage_scan_handler(gui_msg_wifi_t *msg)
{
    gui_log("wifi scan param: 0x%x\n", msg->param);
#if defined ENABLE_WIFI_NIC
#if (defined RTL87x2G)
    ap_array_t *array = (ap_array_t *)msg->param;
    char *ssid = "86box_demo";

    for (uint8_t i = 0; i < array->ap_count; i++)
    {
        gui_log("AP %d: [%s](%d), %d, %d\n", i, array->ap_info[i].ssid, array->ap_info[i].ssid_len,
                array->ap_info[i].security, array->ap_info[i].rssi);
        if (strcmp(array->ap_info[i].ssid, ssid) == 0)
        {
            char *pw = "123123123";
            js_wifi_set_ssid_info(i, pw);
        }
    }
#endif
#endif
}
static void gui_wifi_manage_connect_handler(gui_msg_wifi_t *msg)
{
    gui_log("wifi connect param: 0x%x\n", msg->param);
}

// wifi event
void gui_extern_event_wifi_handler(gui_msg_js_t *js_msg)
{
    gui_msg_wifi_t *wifi_msg = (gui_msg_wifi_t *)js_msg;

    gui_log("sub_event_type: %d", wifi_msg->sub_event_type);
    switch (wifi_msg->sub_event_type)
    {
    case WIFI_EVENT_MANAGE_ON:
        {
            gui_wifi_manage_on_handler(wifi_msg);
        }
        break;
    case WIFI_EVENT_MANAGE_OFF:
        {
            gui_wifi_manage_off_handler(wifi_msg);
        }
        break;
    case WIFI_EVENT_MANAGE_SCAN:
        {
            gui_wifi_manage_scan_handler(wifi_msg);
        }
        break;
    case WIFI_EVENT_MANAGE_CONNECT:
        {
            gui_wifi_manage_connect_handler(wifi_msg);
        }
        break;
    default:
        {


            break;
        }
    }

}


