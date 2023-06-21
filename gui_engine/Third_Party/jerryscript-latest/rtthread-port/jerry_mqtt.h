#ifndef JERRY_MQTT_H__
#define JERRY_MQTT_H__

#include <rtthread.h>

#ifdef PKG_USING_PAHOMQTT

#include <jerry_util.h>
#include <jerry_event.h>
#include <jerry_callbacks.h>
#include <jerry_buffer.h>
#include "paho_mqtt.h"

#define MQTT_BUFFER_SIZE 1024

struct mqtt_callback_info
{
    jerry_value_t this_value;
    jerry_value_t js_func;
    int return_count;
    jerry_value_t *return_value;
    char *event_name;
} typedef mqtt_cbinfo_t;

struct mqtt_client_info
{
    jerry_value_t this_value;
    int subCount; //the count of topics subscribed
    MQTTClient *client;

    struct js_callback *event_callback;
    struct js_callback *fun_callback;
    struct js_callback *close_callback;

    struct topic_callback_list
    {
        char *topic;
        jerry_value_t js_func;
    } callbackHandler[MAX_MESSAGE_HANDLERS];

    rt_sem_t sem;
} typedef mqtt_info_t;


#endif

#endif
