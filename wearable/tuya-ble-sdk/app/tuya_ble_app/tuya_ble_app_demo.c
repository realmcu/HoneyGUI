#include <os_msg.h>
#include <os_task.h>
#include "os_timer.h"
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_unix_time.h"
#include "tuya_ble_event.h"
#include "tuya_ble_app_demo.h"
#include "tuya_ble_log.h"
#include "tuya_ble_app_ota.h"
#include "tuya_ble_demo_version.h"
#include "custom_app_product_test.h"
#include "tuya_ble_sdk_test.h"
#ifdef __RTTHREAD__
#include "rtthread.h"
#endif
#include "tuya_ble_api.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_service_rtl8762c.h"
#include "ble_gap_msg.h"
#include "gap_msg.h"
#include "tuya_dp_data.h"
#if ( TUYA_BLE_FEATURE_WEATHER_ENABLE != 0)
#include "tuya_ble_feature_weather.h"
#endif

#if ( TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0 )
#include "tuya_ble_feature_iot_channel.h"
#if ( TUYA_BLE_FEATURE_SCENE_ENABLE != 0)
#include "tuya_ble_feature_scene.h"
#endif
#endif

#ifdef MODULE_USING_DATABASE
#include "dp_wearable.h"
#endif
#include "watch_msg.h"

#include "os_sync.h"
#include "stdint.h"
#include "trace.h"
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_CUSTOM_TASK_PRIORITY             1         //!< Task priorities
#define APP_CUSTOM_TASK_STACK_SIZE           256 * 6   //!<  Task stack size
#define MAX_NUMBER_OF_TUYA_CUSTOM_MESSAGE    0x20      //!<  GAP message queue size
/*============================================================================*
 *                              Variables
 *============================================================================*/
void *app_custom_task_handle = NULL;
void *tuya_custom_queue_handle = NULL;

tuya_ble_device_param_t device_param = {0};
static const char device_local_name[] = "TY";

static const char auth_key_test[]   =
    "11QxzWt9BZOJsGanh8mZbgxtA5lfGLYj";//"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy";
static const char device_id_test[]  = "tuyabcadf3808d6e";//"zzzzzzzzzzzzzzzz";
static const uint8_t mac_test[]     = "DC234EA457DC";
//static const char auth_key_test[]   =
//    "9maUDqnKbIOwWdAbAK7JCTM9G9gx8SBG";//"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy";
//static const char device_id_test[]  = "tuya65c3674dee93";//"zzzzzzzzzzzzzzzz";
//static const uint8_t mac_test[]     = "DC234E355E3F";
#define APP_CUSTOM_EVENT_1  1
#define APP_CUSTOM_EVENT_2  2
#define APP_CUSTOM_EVENT_3  3
#define APP_CUSTOM_EVENT_4  4
#define APP_CUSTOM_EVENT_5  5

//static uint8_t dp_data_array[255 + 3];
//static uint16_t dp_data_len = 0;

typedef struct
{
    uint8_t data[50];
} custom_data_type_t;

void custom_data_process(int32_t evt_id, void *data)
{
    custom_data_type_t *event_1_data;
    TUYA_APP_LOG_DEBUG("custom event id = %d", evt_id);
    switch (evt_id)
    {
    case APP_CUSTOM_EVENT_1:
        event_1_data = (custom_data_type_t *)data;
        TUYA_APP_LOG_HEXDUMP_DEBUG("received APP_CUSTOM_EVENT_1 data:", event_1_data->data, 50);
        break;
    case APP_CUSTOM_EVENT_2:
        break;
    case APP_CUSTOM_EVENT_3:
        break;
    case APP_CUSTOM_EVENT_4:
        break;
    case APP_CUSTOM_EVENT_5:
        break;
    default:
        break;

    }
}

custom_data_type_t custom_data;

void custom_evt_1_send_test(uint8_t data)
{
    tuya_ble_custom_evt_t event;

    for (uint8_t i = 0; i < 50; i++)
    {
        custom_data.data[i] = data;
    }
    event.evt_id = APP_CUSTOM_EVENT_1;
    event.custom_event_handler = custom_data_process;
    event.data = &custom_data;
    tuya_ble_custom_event_send(event);
}


void app_custom_task(void *p_param)
{
    tuya_ble_cb_evt_param_t event;

    os_alloc_secure_ctx(1024);
    while (true)
    {
        if (os_msg_recv(tuya_custom_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            int16_t result = 0;
            switch (event.evt)
            {
            case TUYA_BLE_CB_EVT_CONNECTE_STATUS:
                TUYA_APP_LOG_INFO("received tuya ble conncet status update event,current connect status = %d",
                                  event.connect_status);

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_DEVICE_STATE, (uint8_t *)&event.connect_status,
                                       sizeof(uint8_t));
#endif
                break;

            case TUYA_BLE_CB_EVT_DP_DATA_RECEIVED:
                {
                    TUYA_APP_LOG_DEBUG("received dp write sn = %d", event.dp_received_data.sn);
                    TUYA_APP_LOG_HEXDUMP_DEBUG("received dp write data :", event.dp_received_data.p_data,
                                               event.dp_received_data.data_len);


//                    dp_set_param(DP_NOTIFICATION_PUSH_TYPE, event.dp_received_data.data_len,
//                                 event.dp_received_data.p_data);

//                    uint8_t push[20] = {0};

//                    dp_get_param(DP_NOTIFICATION_PUSH_TYPE, event.dp_received_data.data_len, push);

//                    uint32_t res = 0;
//                    res = tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE,
//                                                push, event.dp_received_data.data_len);
//                    rt_kprintf(" res = %d\n", res);
                    dp_data_classify(&event.dp_received_data);
#if TUYA_BLE_SDK_TEST_ENABLE
                    tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_WRITE, event.dp_received_data.p_data,
                                           event.dp_received_data.data_len);
#endif
                    break;
                }

            case TUYA_BLE_CB_EVT_DP_DATA_SEND_RESPONSE:
                TUYA_APP_LOG_INFO("received dp data send response sn = %d , type = 0x%02x, flag_mode = %d , ack = %d, result code =%d",
                                  \
                                  event.dp_send_response_data.sn, \
                                  event.dp_send_response_data.type, \
                                  event.dp_send_response_data.mode, \
                                  event.dp_send_response_data.ack, \
                                  event.dp_send_response_data.status);

                if (event.dp_send_response_data.mode == DP_SEND_FOR_CLOUD_PANEL)
                {
                    if (event.dp_send_response_data.type == DP_SEND_TYPE_PASSIVE)
                    {

                    }
                    else
                    {

                    }
                }
                else if (event.dp_send_response_data.mode == DP_SEND_FOR_CLOUD)
                {

                }
                else if (event.dp_send_response_data.mode == DP_SEND_FOR_PANEL)
                {

                }
                else if (event.dp_send_response_data.mode == DP_SEND_FOR_NONE)
                {

                }
                else
                {

                }

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_dp_report_handler();
                tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_RSP, (uint8_t *)&event.dp_send_response_data.status,
                                       sizeof(uint8_t));
#endif
                break;

            case TUYA_BLE_CB_EVT_DP_DATA_WITH_TIME_SEND_RESPONSE:
                TUYA_APP_LOG_INFO("received dp data with time send response sn = %d, type = 0x%02x, flag_mode = %d , need_ack = %d, result code =%d",
                                  \
                                  event.dp_with_time_send_response_data.sn,   \
                                  event.dp_with_time_send_response_data.type, \
                                  event.dp_with_time_send_response_data.mode, \
                                  event.dp_with_time_send_response_data.ack,  \
                                  event.dp_with_time_send_response_data.status);

                if (event.dp_with_time_send_response_data.mode == DP_SEND_FOR_CLOUD_PANEL)
                {

                }
                else if (event.dp_with_time_send_response_data.mode == DP_SEND_FOR_CLOUD)
                {

                }
                else if (event.dp_with_time_send_response_data.mode == DP_SEND_FOR_PANEL)
                {

                }
                else if (event.dp_with_time_send_response_data.mode == DP_SEND_FOR_NONE)
                {

                }

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_dp_report_handler();
                tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_RSP,
                                       (uint8_t *)&event.dp_with_time_send_response_data.status, sizeof(uint8_t));
#endif
                break;

            case TUYA_BLE_CB_EVT_BULK_DATA:
                TUYA_APP_LOG_INFO("received bulk data response");
                //tuya_ble_bulk_data_demo_handler(&event.bulk_req_data);
                break;

#if (TUYA_BLE_FEATURE_WEATHER_ENABLE != 0)
            case TUYA_BLE_CB_EVT_WEATHER_DATA_REQ_RESPONSE:
                TUYA_APP_LOG_INFO("received weather data request response result code =%d",
                                  event.weather_req_response_data.status);

#if TUYA_BLE_SDK_TEST_ENABLE
                uint8_t  rsp_data[2] = {1};
                uint32_t rsp_len = 2;
                rsp_data[1] = event.weather_req_response_data.status;
                tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_WEATHER, rsp_data, rsp_len);
#endif
                break;

            case TUYA_BLE_CB_EVT_WEATHER_DATA_RECEIVED:
                {
                    TUYA_APP_LOG_DEBUG("received weather data, location = [%d] object_count = [%d]",
                                       event.weather_received_data.location, event.weather_received_data.object_count);

                    tuya_ble_wd_object_t *object;
                    uint16_t object_len = 0;

                    for (;;)
                    {
                        object = (tuya_ble_wd_object_t *)(event.weather_received_data.p_data + object_len);

                        TUYA_APP_LOG_DEBUG("weather data, n_day=[%d] key=[0x%08x] val_type=[%d] val_len=[%d]", \
                                           object->n_day, object->key_type, object->val_type, object->value_len);
                        TUYA_APP_LOG_HEXDUMP_DEBUG("vaule :", (uint8_t *)object->vaule, object->value_len);

                        // TODO .. YOUR JOBS

                        object_len += (sizeof(tuya_ble_wd_object_t) + object->value_len);
                        if (object_len >= event.weather_received_data.data_len)
                        {
                            break;
                        }
                    }

#if TUYA_BLE_SDK_TEST_ENABLE
                    tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_WEATHER, event.weather_received_data.p_data,
                                           event.weather_received_data.data_len);
#endif
                }
                break;
#endif

#if ( TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0 )
#if ( TUYA_BLE_FEATURE_SCENE_ENABLE != 0 )
            case TUYA_BLE_CB_EVT_SCENE_REQ_RESPONSE:
                TUYA_APP_LOG_INFO("received scene request response result code=[%d], err_code=[%d] sub_cmd=[%d]", \
                                  event.scene_req_response_data.status, \
                                  event.scene_req_response_data.err_code, \
                                  event.scene_req_response_data.scene_cmd);
                break;

            case TUYA_BLE_CB_EVT_SCENE_DATA_RECEIVED:
                TUYA_APP_LOG_INFO("received scene data result, status=[%d] err_code=[%d] need_update=[%d] check_code=[0x%X]",
                                  \
                                  event.scene_data_received_data.status, \
                                  event.scene_data_received_data.err_code, \
                                  event.scene_data_received_data.need_update, \
                                  event.scene_data_received_data.check_code);

                if (event.scene_data_received_data.status == 0 && event.scene_data_received_data.need_update)
                {
                    // TODO .. please storage update scene check code.

                    if (event.scene_data_received_data.data_len != 0)
                    {
                        uint8_t *iot_scene_object;
                        uint16_t scene_id_len, scene_name_len;
                        uint16_t object_len = 0;

                        for (;;)
                        {
                            iot_scene_object = (uint8_t *)(event.scene_data_received_data.p_data + object_len);

                            scene_id_len = iot_scene_object[0];
                            scene_name_len = (iot_scene_object[1 + scene_id_len] << 8) + iot_scene_object[2 + scene_id_len];

                            TUYA_APP_LOG_HEXDUMP_DEBUG("scene id :", &iot_scene_object[1], scene_id_len);
                            TUYA_APP_LOG_HEXDUMP_DEBUG("scene name unicode :", &iot_scene_object[3 + scene_id_len],
                                                       scene_name_len);

                            object_len += (3 + scene_id_len + scene_name_len);
                            if (object_len >= event.scene_data_received_data.data_len)
                            {
                                break;
                            }
                        }
                    }
                }
                break;

            case TUYA_BLE_CB_EVT_SCENE_CTRL_RESULT_RECEIVED:
                TUYA_APP_LOG_INFO("received scene ctrl result, status=[%d] err_code=[%d]", \
                                  event.scene_ctrl_received_data.status, \
                                  event.scene_ctrl_received_data.err_code);
                TUYA_APP_LOG_HEXDUMP_DEBUG("scene ctrl id :", event.scene_ctrl_received_data.p_scene_id,
                                           event.scene_ctrl_received_data.scene_id_len);
                break;
#endif
#endif

            case TUYA_BLE_CB_EVT_DP_QUERY:
                if (event.dp_query_data.data_len > 0)
                {
                    TUYA_APP_LOG_HEXDUMP_DEBUG("received TUYA_BLE_CB_EVT_DP_QUERY event, query dp id :",
                                               event.dp_query_data.p_data, event.dp_query_data.data_len);
                }
                else
                {
                    TUYA_APP_LOG_INFO("received TUYA_BLE_CB_EVT_DP_QUERY event, query all dp");
                    All_dp_data_report();
                }
                break;

            case TUYA_BLE_CB_EVT_OTA_DATA:
                TUYA_APP_LOG_INFO("received TUYA_BLE_CB_EVT_OTA_DATA event");
                //tuya_ota_proc(event.ota_data.type, event.ota_data.p_data, event.ota_data.data_len);
                break;

            case TUYA_BLE_CB_EVT_NETWORK_INFO:
                TUYA_APP_LOG_INFO("received net info : %s", event.network_data.p_data);
                tuya_ble_net_config_response(result);
                break;

            case TUYA_BLE_CB_EVT_WIFI_SSID:
                break;

            case TUYA_BLE_CB_EVT_TIME_STAMP:
                TUYA_APP_LOG_INFO("received unix timestamp : %s ,time_zone : %d",
                                  event.timestamp_data.timestamp_string, event.timestamp_data.time_zone);
                uint64_t time_stamp_ms;
                uint32_t time_stamp;

                time_stamp_ms = atoll((char *)event.timestamp_data.timestamp_string);
                time_stamp = (time_stamp_ms / 1000);
                if ((time_stamp_ms % 1000) >= 500)
                {
                    time_stamp += 1;
                }

                time_t now;
                struct tm *p_tm;
                struct tm tm_new;
                now = time_stamp + event.timestamp_data.time_zone * 36;

                p_tm = localtime(&now);
                memcpy(&tm_new, p_tm, sizeof(struct tm));

                now = mktime(&tm_new);
#ifdef __RTTHREAD__
                rt_device_t device;
                device = rt_device_find("rtc");
                rt_device_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
#else
                watch_clock_init(now);
#endif

#if TUYA_BLE_SDK_TEST_ENABLE
                //tuya_ble_bulk_data_generation(60, NULL, 0);

                tuya_ble_time_struct_data_t data;
                tuya_ble_time_noraml_data_t normal_data;

                tuya_ble_utc_sec_2_mytime(time_stamp, &data, false);
                memcpy(&normal_data, &data, sizeof(tuya_ble_time_struct_data_t));
                normal_data.time_zone = event.timestamp_data.time_zone;

                tuya_ble_sdk_test_get_time_rsp(&normal_data);
#endif
                break;

            case TUYA_BLE_CB_EVT_TIME_NORMAL:
            case TUYA_BLE_CB_EVT_APP_LOCAL_TIME_NORMAL:
                TUYA_APP_LOG_HEXDUMP_DEBUG("received normal time", (uint8_t *) & (event.time_normal_data),
                                           sizeof(tuya_ble_time_noraml_data_t));
                TUYA_APP_LOG_INFO(" timezone = [%d]", event.time_normal_data.time_zone);

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_get_time_rsp(&event.time_normal_data);
#endif
                break;

            case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH:
                TUYA_APP_LOG_HEXDUMP_DEBUG("received ble passthrough data :", event.ble_passthrough_data.p_data,
                                           event.ble_passthrough_data.data_len);
                notify_push_parse(event.ble_passthrough_data);
#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_send(TY_UARTV_CMD_PASSTHROUGH_WRITE, event.ble_passthrough_data.p_data,
                                       event.ble_passthrough_data.data_len);
#endif
                break;

            case TUYA_BLE_CB_EVT_UPDATE_LOGIN_KEY_VID:
                TUYA_APP_LOG_HEXDUMP_DEBUG("binding success -> login key :",
                                           event.device_login_key_vid_data.login_key, event.device_login_key_vid_data.login_key_len);
                TUYA_APP_LOG_HEXDUMP_DEBUG(" -> beacon key :", event.device_login_key_vid_data.beacon_key,
                                           BEACON_KEY_LEN);
                TUYA_APP_LOG_HEXDUMP_DEBUG(" -> vid :", event.device_login_key_vid_data.vid,
                                           event.device_login_key_vid_data.vid_len);
                break;

            case TUYA_BLE_CB_EVT_UNBOUND:
                TUYA_APP_LOG_INFO("received unbound req\n");

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_unbind_mode_rsp(0);
#endif
                break;

            case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND:
                TUYA_APP_LOG_INFO("received anomaly unbound req\n");

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_unbind_mode_rsp(1);
#endif
                break;

            case TUYA_BLE_CB_EVT_DEVICE_RESET:
                TUYA_APP_LOG_INFO("received device reset req\n");

#if TUYA_BLE_SDK_TEST_ENABLE
                tuya_ble_sdk_test_unbind_mode_rsp(2);
#endif
                break;

            case TUYA_BLE_CB_EVT_UNBIND_RESET_RESPONSE:
                if (event.reset_response_data.type == RESET_TYPE_UNBIND)
                {
                    if (event.reset_response_data.status == 0)
                    {
                        TUYA_APP_LOG_INFO("device unbind succeed.");

#if TUYA_BLE_SDK_TEST_ENABLE
                        uint8_t device_reset_mode = 3;
                        tuya_ble_sdk_test_send(TY_UARTV_CMD_UNBIND_MODE, &device_reset_mode, 1);
#endif
                    }
                    else
                    {
                        TUYA_APP_LOG_INFO("device unbind failed.");
                    }
                }
                else if (event.reset_response_data.type == RESET_TYPE_FACTORY_RESET)
                {
                    if (event.reset_response_data.status == 0)
                    {
                        TUYA_APP_LOG_INFO("device factory reset succeed.");

#if TUYA_BLE_SDK_TEST_ENABLE
                        uint8_t device_reset_mode = 4;
                        tuya_ble_sdk_test_send(TY_UARTV_CMD_UNBIND_MODE, &device_reset_mode, 1);
#endif
                    }
                    else
                    {
                        TUYA_APP_LOG_INFO("device factory reset failed.");
                    }
                }
                break;

            default:
                TUYA_APP_LOG_WARNING("app_tuya_cb_queue msg: unknown event type 0x%04x", event.evt);
                break;
            }

            tuya_ble_event_response(&event);
        }
    }
}



int tuya_ble_app_init(void)
{
    TUYA_APP_LOG_INFO("app version : "TY_APP_VER_STR);
    os_msg_queue_create(&tuya_custom_queue_handle, "tuya custom Q", MAX_NUMBER_OF_TUYA_CUSTOM_MESSAGE,
                        sizeof(tuya_ble_cb_evt_param_t));
    os_task_create(&app_custom_task_handle, "app_custom", app_custom_task, 0,
                   APP_CUSTOM_TASK_STACK_SIZE, APP_CUSTOM_TASK_PRIORITY);

    device_param.p_type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    device_param.product_id_len = strlen(APP_PRODUCT_ID);
    memcpy(device_param.product_id, APP_PRODUCT_ID, device_param.product_id_len);
    device_param.firmware_version = TY_APP_VER_NUM;
    device_param.hardware_version = TY_HARD_VER_NUM;
    device_param.adv_local_name_len = strlen(device_local_name);
    memcpy(device_param.adv_local_name, device_local_name, device_param.adv_local_name_len);

    // Noticed!!! if use the license stored by the SDK initialized to 0, Otherwise 1.
    device_param.use_ext_license_key = 1;
    device_param.device_id_len = 16;
    if (device_param.use_ext_license_key == 1)
    {
        memcpy(device_param.auth_key, (void *)auth_key_test, AUTH_KEY_LEN);
        memcpy(device_param.device_id, (void *)device_id_test, DEVICE_ID_LEN);
        memcpy(device_param.mac_addr_string, mac_test, 12);
        device_param.mac_addr.addr_type = TUYA_BLE_ADDRESS_TYPE_RANDOM;
    }

    tuya_ble_sdk_init(&device_param);
    tuya_ble_callback_queue_register(tuya_custom_queue_handle);

    tuya_ble_device_delay_ms(200);
    //tuya_ota_init();

#if TUYA_BLE_SDK_TEST_ENABLE
    tuya_ble_sdk_test_init();
#endif

    TUYA_APP_LOG_INFO("tuya ble demo version : "TUYA_BLE_DEMO_VERSION_STR);
    TUYA_APP_LOG_INFO("app version : "TY_APP_VER_STR);
    return 0;

}



static T_SERVER_ID tuya_srv_id;

static T_APP_RESULT app_tuya_callback(T_SERVER_ID service_id, void *p_data)
{
    if (service_id == tuya_srv_id)
    {
        T_KNS_CALLBACK_DATA *p_kns_cb_data = (T_KNS_CALLBACK_DATA *)p_data;
        switch (p_kns_cb_data->msg_type)
        {
        case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
            {
                switch (p_kns_cb_data->msg_data.notification_indification_index)
                {
                case KNS_NOTIFY_ENABLE:
                    {
                        APP_PRINT_INFO0("KNS_NOTIFY_ENABLE");
                    }
                    break;

                case KNS_NOTIFY_DISABLE:
                    {
                        APP_PRINT_INFO0("KNS_NOTIFY_DISABLE");
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
            {
                APP_PRINT_INFO1("gatt received data len = %d", p_kns_cb_data->msg_data.write_value.data_len);
                //   gTimeParaValue = p_kns_cb_data->msg_data.write_value;
                /*
                event.hdr.event_id = TUYA_BLE_DATA_REV_EVT;
                event.ble_rev_event.len = p_kns_cb_data->msg_data.write_value.data_len;
                memcpy(event.ble_rev_event.data,p_kns_cb_data->msg_data.write_value.data,p_kns_cb_data->msg_data.write_value.data_len);
                if(tuya_event_send(&event)!=0)
                {
                    APP_PRINT_ERROR1("tuya_event_send ble data error,data len = %d ", p_kns_cb_data->msg_data.write_value.data_len);
                }
                */
                tuya_ble_gatt_receive_data(p_kns_cb_data->msg_data.write_value.data,
                                           p_kns_cb_data->msg_data.write_value.data_len);
                // ble_value_notify(p_kns_cb_data->msg_data.write_value.data,p_kns_cb_data->msg_data.write_value.data_len);
            }
            break;

        default:
            break;
        }
    }
    return APP_RESULT_SUCCESS;
}

static T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */
static T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */
static void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO3("app_handle_dev_state_evt: init state %d, adv state %d, cause 0x%x",
                    new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            APP_PRINT_INFO0("GAP stack ready for tuya");
            tuya_ble_app_init();
        }
    }

    gap_dev_state = new_state;
}

static void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                      uint16_t disc_cause)
{
    APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                    conn_id, gap_conn_state, new_state, disc_cause);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            tuya_ble_disconnected_handler();
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            tuya_ble_connected_handler();
        }
        break;

    default:
        break;
    }
    gap_conn_state = new_state;
}


static void app_tuya_le_msg_cb(T_IO_MSG *msg)
{
    T_LE_GAP_MSG gap_msg;
    if (msg->type == IO_MSG_TYPE_BT_STATUS)
    {
        memcpy(&gap_msg, &msg->u.param, sizeof(msg->u.param));
        switch (msg->subtype)
        {
        case GAP_MSG_LE_DEV_STATE_CHANGE:
            app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                     gap_msg.msg_data.gap_dev_state_change.cause);
            break;
        case GAP_MSG_LE_CONN_STATE_CHANGE:
            {
                app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                          (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                          gap_msg.msg_data.gap_conn_state_change.disc_cause);
            }
            break;
        default:
            break;
        }
    }
}

static void app_tuya_app_msg_cb(T_IO_MSG *msg)
{
    T_LE_GAP_MSG gap_msg;
    if (msg->type == IO_MSG_TYPE_WRISTBNAD)
    {
        if (IO_MSG_TUYA_BLE_EVENT == msg->subtype)
        {
            extern void tuya_ble_event_handler_port(void);
            tuya_ble_event_handler_port();
        }
    }
}

T_SERVER_ID get_tuya_srv_id(void)
{
    return tuya_srv_id;
}



void app_ble_tuya_init(void)
{

    tuya_srv_id  = kns_add_service(app_tuya_callback);
    le_msg_handler_cback_register(app_tuya_le_msg_cb);
    app_msg_handler_cback_register(app_tuya_app_msg_cb);
    TUYA_BLE_PRINTF("tuya_srv_id = %d \n", tuya_srv_id);
}












void tuya_test(uint8_t argc, char **argv)
{
    uint8_t dp_test[7];
    dp_test[0] = 1; //DP refs
    dp_test[1] = DT_VALUE;
    dp_test[2] = 0x00; //len high
    dp_test[3] = 0x03; //len low

    uint32_t steps = atoi(argv[1]);

//    dp_set_param(DP_STEP_TOTAL_TYPE, sizeof(uint32_t), &steps);

//    dp_get_param(DP_STEP_TOTAL_TYPE, sizeof(uint32_t), &steps);


    dp_test[4] = 0x00;
    dp_test[5] = (steps >> 8) & 0xff;
    dp_test[6] = steps & 0xff;

    uint32_t res = 0;
    res = tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE,
                                dp_test, 7);
    TUYA_BLE_PRINTF(" res = %d\n", res);
}
#ifdef __RTTHREAD__
MSH_CMD_EXPORT(tuya_test, lcd display test);
#endif
