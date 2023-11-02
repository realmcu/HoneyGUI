#include "jerry_wlan.h"


#ifdef RT_USING_WIFI

#include <rtgui/gb2312.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME    "JS WLAN"
#define DBG_LEVEL           DBG_LOG
#include <rtdbg.h>

static void get_wifi_info(void **info, jerry_value_t js_target)
{
    jerry_value_t js_info = js_get_property(js_target, "info");
    if (js_info)
    {
        jerry_get_object_native_pointer(js_info, info, NULL);
    }
    jerry_release_value(js_info);
}

static void js_event_callback_func_wifi(const void *args, uint32_t size)
{
    struct event_callback_info *cb_info = (struct event_callback_info *)args;

    if (cb_info->js_return != RT_NULL)
    {
        js_emit_event(cb_info->js_target, cb_info->event_name, &cb_info->js_return, 1);
        jerry_release_value(cb_info->js_return);
    }
    else
    {
        js_emit_event(cb_info->js_target, cb_info->event_name, NULL, 0);
    }

    free(cb_info->event_name);
    free(cb_info);
}

static int connect_wifi(struct wifi_info *info, char *ssid, char *password, char *bssid)
{
    int ret = -2;

    if (info && ssid)
    {
        if (info->wifi_list.num)
        {
            int i = 0;
            char buffer[32];

            for (; i < info->wifi_list.num; i ++)
            {
                rt_sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                           info->wifi_list.info[i].bssid[0],
                           info->wifi_list.info[i].bssid[1],
                           info->wifi_list.info[i].bssid[2],
                           info->wifi_list.info[i].bssid[3],
                           info->wifi_list.info[i].bssid[4],
                           info->wifi_list.info[i].bssid[5]
                          );

                if (bssid)
                {
                    if (rt_strcmp((const char *)bssid, (const char *)buffer) != 0)
                    {
                        continue;
                    }
                }

                if (rt_strcmp((const char *)ssid, (const char *)info->wifi_list.info[i].ssid.val) == 0)
                {
                    ret = rt_wlan_connect_adv(&(info->wifi_list.info[i]), password);
                    break;
                }
                else if (info->gb_ssid[i].ssid &&
                         rt_strcmp((const char *)ssid, (const char *)info->gb_ssid[i].ssid) == 0)
                {
                    ret = rt_wlan_connect_adv(&(info->wifi_list.info[info->gb_ssid[i].index]), password);
                    break;
                }
            }

            if (ret != RT_EOK)
            {
                ret = -1;
            }

            if (i >= info->wifi_list.num)
            {
                LOG_W("The scanned wifi is not included %s", ssid);
                ret = -2;
            }
        }
    }

    return ret;
}

static void scanEvent_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    jerry_value_t js_wifi = (jerry_value_t)(parameter);
    struct wifi_info *info = RT_NULL;

    get_wifi_info((void **)&info, js_wifi);
    if (info)
    {
        int i;
        jerry_value_t js_return = 0;
        struct rt_wlan_scan_result *temp_result = (struct rt_wlan_scan_result *)buff->data;

        if (temp_result->num > 0)
        {
            if (info->wifi_list.info)
            {
                free(info->wifi_list.info);
            }

            if (info->gb_ssid)
            {
                for (i = 0; i < info->wifi_list.num; i++)
                {
                    if (info->gb_ssid[i].ssid)
                    {
                        rt_free(info->gb_ssid[i].ssid);
                    }
                }
                free(info->gb_ssid);
            }

            info->wifi_list.num = temp_result->num;
            info->wifi_list.info = (struct rt_wlan_info *)malloc(sizeof(struct rt_wlan_info) *
                                                                 info->wifi_list.num);
            info->gb_ssid = (struct wifi_ssid *)malloc(sizeof(struct wifi_ssid) * info->wifi_list.num);
            if (info->wifi_list.info)
            {
                memcpy(info->wifi_list.info, temp_result->info, sizeof(struct rt_wlan_info) * info->wifi_list.num);

                if (info->gb_ssid)
                {
                    int j = 0;
                    memset(info->gb_ssid, 0x00, sizeof(struct wifi_ssid) * info->wifi_list.num);

                    for (i = 0; i < info->wifi_list.num; i++)
                    {
                        if (!is_utf8_string((void *)info->wifi_list.info[i].ssid.val,
                                            strlen((const char *)info->wifi_list.info[i].ssid.val)))
                        {
                            info->gb_ssid[j].index = i;
                            Gb2312ToUtf8((char *)info->wifi_list.info[i].ssid.val,
                                         strlen((const char *)info->wifi_list.info[i].ssid.val), &info->gb_ssid[j].ssid);
                            j ++;
                        }
                    }
                }

                if (info->ssid)
                {
                    if (connect_wifi(info, info->ssid, info->password, info->bssid) == -2)
                    {
                        struct event_callback_info cb_info;

                        cb_info.event_name = strdup("ConnectEvent");
                        cb_info.js_target = js_wifi;
                        cb_info.js_return = jerry_create_null();

                        if (!js_send_callback(info->event_callback, &cb_info, sizeof(struct event_callback_info)))
                        {
                            jerry_release_value(js_return);
                        }
                    }

                    free(info->ssid);
                    free(info->password);
                    free(info->bssid);
                    info->ssid = NULL;
                    info->password = NULL;
                    info->bssid = NULL;

                    return;
                }
                else
                {
                    char buffer[32];
                    jerry_value_t js_ssid, js_strength, js_bssid, js_secure, js_ret;

                    js_return = jerry_create_array(info->wifi_list.num);

                    for (i = 0; i < info->wifi_list.num; i++)
                    {
                        struct rt_wlan_info *wifi_info = &(info->wifi_list.info[i]);
                        jerry_value_t js_wifi_info = jerry_create_object();

                        js_ssid = js_string_to_value((const char *)wifi_info->ssid.val);
                        js_set_property(js_wifi_info, "ssid", js_ssid);
                        jerry_release_value(js_ssid);

                        js_strength = jerry_create_number(wifi_info->rssi);
                        js_set_property(js_wifi_info, "strength", js_strength);
                        jerry_release_value(js_strength);

                        rt_sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                                   wifi_info->bssid[0],
                                   wifi_info->bssid[1],
                                   wifi_info->bssid[2],
                                   wifi_info->bssid[3],
                                   wifi_info->bssid[4],
                                   wifi_info->bssid[5]);

                        js_bssid = jerry_create_string((const jerry_char_t *)buffer);
                        js_set_property(js_wifi_info, "bssid", js_bssid);
                        jerry_release_value(js_bssid);

                        js_secure = jerry_create_boolean(wifi_info->security);
                        js_set_property(js_wifi_info, "secure", js_secure);
                        jerry_release_value(js_secure);

                        js_ret = jerry_set_property_by_index(js_return, i, js_wifi_info);
                        jerry_release_value(js_ret);

                        jerry_release_value(js_wifi_info);
                    }
                }
            }
        }

        {
            struct event_callback_info cb_info;

            cb_info.event_name = strdup("ScanEvent");
            cb_info.js_target = js_wifi;
            cb_info.js_return = js_return;

            if (!js_send_callback(info->event_callback, &cb_info, sizeof(struct event_callback_info)))
            {
                jerry_release_value(js_return);
            }
        }
    }
}

static void connectEvent_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    jerry_value_t js_wifi = (jerry_value_t)parameter;
    struct wifi_info *info = RT_NULL;

    get_wifi_info((void **)&info, js_wifi);
    if (info)
    {
        jerry_value_t js_return = 0;
        struct rt_wlan_info *wifi_info = (struct rt_wlan_info *)(buff->data);

        if (event == RT_WLAN_EVT_STA_CONNECTED)
        {
            char buffer[32];
            jerry_value_t js_ssid, js_strength, js_bssid, js_secure;
            js_return = jerry_create_object();

            js_ssid = js_string_to_value((const char *)wifi_info->ssid.val);
            js_set_property(js_return, "ssid", js_ssid);
            jerry_release_value(js_ssid);

            js_strength = jerry_create_number(wifi_info->rssi);
            js_set_property(js_return, "strength", js_strength);
            jerry_release_value(js_strength);

            rt_sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                       wifi_info->bssid[0],
                       wifi_info->bssid[1],
                       wifi_info->bssid[2],
                       wifi_info->bssid[3],
                       wifi_info->bssid[4],
                       wifi_info->bssid[5]);

            js_bssid = jerry_create_string((const jerry_char_t *)buffer);
            js_set_property(js_return, "bssid", js_bssid);
            jerry_release_value(js_bssid);

            js_secure = jerry_create_boolean(wifi_info->security);
            js_set_property(js_return, "secure", js_secure);
            jerry_release_value(js_secure);
        }
        else
        {
            js_return = jerry_create_null();
        }

        {
            struct event_callback_info cb_info;

            cb_info.event_name = strdup("ConnectEvent");
            cb_info.js_target = js_wifi;
            cb_info.js_return = js_return;

            if (!js_send_callback(info->event_callback, &cb_info, sizeof(struct event_callback_info)))
            {
                jerry_release_value(js_return);
            }
        }
    }
}

static void networkEvent_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    jerry_value_t js_wifi = (jerry_value_t)parameter;
    struct wifi_info *info = RT_NULL;

    get_wifi_info((void **)&info, js_wifi);
    if (info)
    {
        jerry_value_t js_return = 0;
        struct event_callback_info cb_info;

        if (RT_WLAN_EVT_READY == event)
        {
            js_return = jerry_create_boolean(true);
        }
        else
        {
            js_return = jerry_create_boolean(false);
        }

        cb_info.event_name = strdup("NetworkEvent");
        cb_info.js_target = js_wifi;
        cb_info.js_return = js_return;

        if (!js_send_callback(info->event_callback, &cb_info, sizeof(struct event_callback_info)))
        {
            jerry_release_value(js_return);
        }
    }
}

void wifi_free_callback(void *native_p)
{
    struct wifi_info *info = (struct wifi_info *)native_p;
    if (info)
    {
        if (info->wifi_list.info)
        {
            free(info->wifi_list.info);
        }

        if (info->event_callback)
        {
            js_remove_callback(info->event_callback);
        }

        if (info)
        {
            free(info);
        }
    }

    rt_wlan_unregister_event_handler(RT_WLAN_EVT_SCAN_DONE);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_CONNECTED);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_READY);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_DISCONNECTED);
}

const static jerry_object_native_info_t wifi_free_info =
{
    wifi_free_callback
};

void register_handler(jerry_value_t js_wifi)
{
    rt_wlan_register_event_handler(RT_WLAN_EVT_SCAN_DONE, scanEvent_handler, (void *)js_wifi);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED, connectEvent_handler, (void *)js_wifi);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL, connectEvent_handler,
                                   (void *)js_wifi);
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, networkEvent_handler, (void *)js_wifi);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_DISCONNECTED, networkEvent_handler, (void *)js_wifi);
}

DECLARE_HANDLER(openWifi)
{
    if (args_cnt >= 1)
    {
        char *name = RT_NULL;

        if (jerry_value_is_string(args[0]))
        {
            name = js_value_to_string(args[0]);
        }

        if (name)
        {
            rt_err_t ret = rt_wlan_set_mode(name, RT_WLAN_STATION);
            if (ret == RT_EOK)
            {
                return jerry_create_boolean(true);
            }
        }
    }

    return jerry_create_boolean(false);
}

DECLARE_HANDLER(scanWifi)
{
    rt_err_t ret = rt_wlan_scan();
    if (ret == RT_EOK)
    {
        return jerry_create_boolean(true);
    }

    return jerry_create_boolean(false);
}

DECLARE_HANDLER(connectWifi)
{
    if (args_cnt >= 1 && jerry_value_is_object(args[0]))
    {
        char *ssid = RT_NULL;
        char *bssid = RT_NULL;
        char *password = RT_NULL;
        struct wifi_info *info = RT_NULL;

        get_wifi_info((void **)&info, this_value);
        if (info)
        {
            jerry_value_t js_ssid = js_get_property(args[0], "ssid");
            jerry_value_t js_bssid = js_get_property(args[0], "bssid");
            jerry_value_t js_password = js_get_property(args[0], "password");

            if (jerry_value_is_string(js_ssid))
            {
                ssid = js_value_to_string(js_ssid);
            }
            jerry_release_value(js_ssid);

            if (jerry_value_is_string(js_bssid))
            {
                bssid = js_value_to_string(js_bssid);
            }
            jerry_release_value(js_bssid);

            if (jerry_value_is_string(js_password))
            {
                password = js_value_to_string(js_password);
            }
            jerry_release_value(js_password);

            if (ssid)
            {
                rt_err_t ret = -1;

                ret = connect_wifi(info, ssid, password, bssid);
                if (ret == RT_EOK)
                {
                    free(ssid);
                    free(password);
                    free(bssid);
                    return jerry_create_boolean(true);
                }

                if (ret == -2)
                {
                    ret = rt_wlan_scan();
                    if (ret == RT_EOK)
                    {
                        free(info->ssid);
                        free(info->password);
                        free(info->bssid);
                        info->ssid = ssid;
                        info->password = password;
                        info->bssid = bssid;

                        return jerry_create_boolean(true);
                    }
                }
            }

            free(ssid);
            free(password);
            free(bssid);
        }
    }

    return jerry_create_boolean(false);
}

DECLARE_HANDLER(disconnectWifi)
{
    rt_err_t ret = rt_wlan_disconnect();
    if (ret == RT_EOK)
    {
        return jerry_create_boolean(true);
    }

    return jerry_create_boolean(false);
}

DECLARE_HANDLER(onScanEvent)
{
    if (args_cnt == 1 && jerry_value_is_function(args[0]))
    {
        js_add_event_listener(this_value, "ScanEvent", args[0]);
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onConnectEvent)
{
    if (args_cnt == 1 && jerry_value_is_function(args[0]))
    {
        js_add_event_listener(this_value, "ConnectEvent", args[0]);
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(onNetworkEvent)
{
    if (args_cnt == 1 && jerry_value_is_function(args[0]))
    {
        js_add_event_listener(this_value, "NetworkEvent", args[0]);
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(getConnectedWifi)
{
    struct rt_wlan_info info;

    if (rt_wlan_is_connected())
    {
        rt_err_t ret = rt_wlan_get_info(&info);
        if (ret == RT_EOK)
        {
            jerry_value_t js_ssid, js_strength, js_bssid, js_secure;
            jerry_value_t js_wifi_info = jerry_create_object();
            if (js_wifi_info)
            {
                char bssid[32];

                js_ssid = js_string_to_value((const char *)info.ssid.val);
                js_set_property(js_wifi_info, "ssid", js_ssid);
                jerry_release_value(js_ssid);

                js_strength = jerry_create_number(info.rssi);
                js_set_property(js_wifi_info, "strength", js_strength);
                jerry_release_value(js_strength);

                rt_sprintf(bssid, "%02x:%02x:%02x:%02x:%02x:%02x",
                           info.bssid[0],
                           info.bssid[1],
                           info.bssid[2],
                           info.bssid[3],
                           info.bssid[4],
                           info.bssid[5]);

                js_bssid = jerry_create_string((const jerry_char_t *)bssid);
                js_set_property(js_wifi_info, "bssid", js_bssid);
                jerry_release_value(js_bssid);

                js_secure = jerry_create_boolean(info.security);
                js_set_property(js_wifi_info, "secure", js_secure);
                jerry_release_value(js_secure);

                return js_wifi_info;
            }
        }
    }

    return jerry_create_null();
}

DECLARE_HANDLER(destroy)
{
    struct wifi_info *info = RT_NULL;
    jerry_value_t js_info = js_get_property(this_value, "info");

    if (js_info)
    {
        jerry_get_object_native_pointer(js_info, (void **)&info, NULL);
        jerry_set_object_native_pointer(js_info, NULL, NULL);
    }
    jerry_release_value(js_info);

    rt_wlan_unregister_event_handler(RT_WLAN_EVT_SCAN_DONE);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_CONNECTED);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_READY);
    rt_wlan_unregister_event_handler(RT_WLAN_EVT_STA_DISCONNECTED);

    js_destroy_emitter(this_value);

    if (info)
    {
        if (info->wifi_list.info)
        {
            free(info->wifi_list.info);
        }

        if (info->event_callback)
        {
            js_remove_callback(info->event_callback);
        }

        if (info)
        {
            free(info);
        }
    }

    return jerry_create_undefined();
}

DECLARE_HANDLER(createWifi)
{
    struct wifi_info *wifi_info = (struct wifi_info *)malloc(sizeof(struct wifi_info));

    if (wifi_info)
    {
        jerry_value_t js_wifi = jerry_create_object();
        jerry_value_t js_info = jerry_create_object();

        if (js_wifi && js_info)
        {
            memset(wifi_info, 0x00, sizeof(struct wifi_info));
            js_make_emitter(js_wifi, jerry_create_undefined());

            wifi_info->this_value = js_wifi;
            wifi_info->event_callback = js_add_callback(js_event_callback_func_wifi);

            js_set_property(js_wifi, "info", js_info);
            jerry_set_object_native_pointer(js_info, wifi_info, &wifi_free_info);
            jerry_release_value(js_info);

            REGISTER_METHOD_NAME(js_wifi, "open", openWifi);
            REGISTER_METHOD_NAME(js_wifi, "scan", scanWifi);
            REGISTER_METHOD_NAME(js_wifi, "connect", connectWifi);
            REGISTER_METHOD_NAME(js_wifi, "disconnect", disconnectWifi);
            REGISTER_METHOD_NAME(js_wifi, "getConnected", getConnectedWifi);

            REGISTER_METHOD_NAME(js_wifi, "onScanEvent", onScanEvent);
            REGISTER_METHOD_NAME(js_wifi, "onConnectEvent", onConnectEvent);
            REGISTER_METHOD_NAME(js_wifi, "onNetworkEvent", onNetworkEvent);
            REGISTER_METHOD(js_wifi, destroy);

            register_handler(js_wifi);

            return js_wifi;
        }
        else
        {
            jerry_release_value(js_wifi);
            jerry_release_value(js_info);
        }

        free(wifi_info);
    }

    return jerry_create_undefined();
}

int jerry_wifi_init(jerry_value_t obj)
{
    REGISTER_METHOD(obj, createWifi);

    return 0;
}

#endif
