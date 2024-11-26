#ifndef JERRY_WLAN_H__
#define JERRY_WLAN_H__

#include <rtthread.h>

#ifdef RT_USING_WIFI

#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jerry_util.h>
#include <jerry_event.h>
#include <jerry_callbacks.h>
#include <jerry_buffer.h>
#include <wlan_mgnt.h>

struct wifi_ssid
{
    char *ssid;
    int index;
};

struct wifi_info
{
    jerry_value_t this_value;
    struct js_callback *event_callback;
    struct rt_wlan_scan_result wifi_list;
    struct wifi_ssid *gb_ssid;
    char *ssid;
    char *password;
    char *bssid;
};

struct event_callback_info
{
    char *event_name;
    jerry_value_t js_return;
    jerry_value_t js_target;
};

struct close_callback_info
{
    jerry_value_t js_target;
};

#endif

#endif
