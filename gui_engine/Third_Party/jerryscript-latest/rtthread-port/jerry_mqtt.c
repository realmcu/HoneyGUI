#include "jerry_mqtt.h"

#ifdef PKG_USING_PAHOMQTT

#include <rtdbg.h>
#define PIPE_BUFSZ    512
static bool hasClient = false;

extern int MQTT_CMD(MQTTClient *c, const char *cmd);

void mqtt_event_callback(const void *args, uint32_t size)
{
    mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)args;
    if (cb_info->return_value != RT_NULL)
    {
        js_emit_event(cb_info->this_value, cb_info->event_name, cb_info->return_value,
                      cb_info->return_count);
    }
    else
    {
        js_emit_event(cb_info->this_value, cb_info->event_name, RT_NULL, 0);
    }

    if (cb_info->return_value != RT_NULL)
    {
        for (int i = 0 ; i < cb_info->return_count ; i++)
        {
            jerry_release_value(cb_info->return_value[i]);
        }
        free(cb_info->return_value);
    }
    free(cb_info->event_name);
    free(cb_info);
}

void mqtt_free_callback(const void *args, uint32_t size)
{

}

void mqtt_func_callback(const void *args, uint32_t size)
{
    mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)args;
    jerry_value_t ret = jerry_call_function(cb_info->js_func, cb_info->this_value, RT_NULL, 0);
    jerry_release_value(ret);
    free(cb_info);
}

void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    mqtt_info_t *mqtt_info = (mqtt_info_t *)(c->user_data);
    struct js_callback *event_callback = mqtt_info->event_callback;

    char *topicName = (char *)malloc((msg_data->topicName->lenstring.len + 1) * sizeof(char));
    memset(topicName, 0, msg_data->topicName->lenstring.len + 1);
    memcpy(topicName, msg_data->topicName->lenstring.data, msg_data->topicName->lenstring.len);

    js_buffer_t *js_buffer;
    jerry_value_t js_payload = jerry_buffer_create(msg_data->message->payloadlen, &js_buffer);
    if (js_buffer)
    {
        rt_memcpy(js_buffer->buffer, msg_data->message->payload, msg_data->message->payloadlen);
    }

    /*emit message event*/
    mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
    memset(cb_info, 0, sizeof(mqtt_cbinfo_t));

    cb_info->this_value  = mqtt_info->this_value;
    cb_info->return_value = (jerry_value_t *)malloc(sizeof(jerry_value_t) * 2);
    cb_info->return_value[0] = jerry_create_string((const jerry_char_t *)(topicName));
    cb_info->return_value[1] = js_payload;
    cb_info->return_count = 2;
    cb_info->js_func = RT_NULL;
    cb_info->event_name = rt_strdup("message");

    js_send_callback(event_callback, cb_info, sizeof(mqtt_cbinfo_t));
    free(cb_info);

    /*emit topic's publish callback*/
    for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
    {
        if (strcmp(mqtt_info->callbackHandler[i].topic, topicName) == 0)
        {
            mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
            memset(cb_info, 0, sizeof(mqtt_cbinfo_t));

            cb_info->this_value  = mqtt_info->this_value;
            cb_info->return_value = RT_NULL;
            cb_info->return_count = 0;
            cb_info->js_func = mqtt_info->callbackHandler[i].js_func;

            js_send_callback(mqtt_info->fun_callback, cb_info, sizeof(mqtt_cbinfo_t));
            free(cb_info);
            break;
        }
    }
    free(topicName);

    return;
}

void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
    //to do
}

void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");

    mqtt_info_t *mqtt_info = (mqtt_info_t *)(c->user_data);
    struct js_callback *event_callback = mqtt_info->event_callback;

    mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
    memset(cb_info, 0, sizeof(mqtt_cbinfo_t));
    cb_info->this_value  = mqtt_info->this_value;
    cb_info->return_value = RT_NULL;
    cb_info->js_func = RT_NULL;
    cb_info->event_name = rt_strdup("connect");

    js_send_callback(event_callback, cb_info, sizeof(mqtt_cbinfo_t));
    free(cb_info);
}

void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");

    mqtt_info_t *mqtt_info = (mqtt_info_t *)(c->user_data);
    struct js_callback *event_callback = mqtt_info->event_callback;

    mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
    memset(cb_info, 0, sizeof(mqtt_cbinfo_t));
    cb_info->this_value  = mqtt_info->this_value;
    cb_info->return_value = RT_NULL;
    cb_info->event_name = rt_strdup("offline");

    js_send_callback(event_callback, cb_info, sizeof(mqtt_cbinfo_t));
    free(cb_info);
}

void mqtt_client_free_callback(void *native_p)
{
    mqtt_info_t *mqtt_info = (mqtt_info_t *)native_p;

    if (!mqtt_info)
    {
        return;
    }

    if (mqtt_info->client->isconnected == 1)
    {
        MQTT_CMD(mqtt_info->client, "DISCONNECT");
    }

    if (mqtt_info->sem)
    {
        rt_sem_delete(mqtt_info->sem);
        mqtt_info->sem = RT_NULL;
        js_destroy_emitter(mqtt_info->this_value);
    }


    if (mqtt_info->close_callback)
    {
        js_remove_callback(mqtt_info->close_callback);
    }

    if (mqtt_info->event_callback)
    {
        js_remove_callback(mqtt_info->event_callback);
    }

    if (mqtt_info->fun_callback)
    {
        js_remove_callback(mqtt_info->fun_callback);
    }

    if (mqtt_info->client)
    {
        if (mqtt_info->client->uri)
        {
            free((void *)mqtt_info->client->uri);
        }
        if (mqtt_info->client->buf)
        {
            free(mqtt_info->client->buf);
        }
        if (mqtt_info->client->readbuf)
        {
            free(mqtt_info->client->readbuf);
        }
        for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
        {
            if (mqtt_info->client->messageHandlers[i].topicFilter)
            {
                free(mqtt_info->client->messageHandlers[i].topicFilter);
            }
        }
        free(mqtt_info->client);
    }

    for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
    {
        if (mqtt_info->callbackHandler[i].topic)
        {
            free(mqtt_info->callbackHandler[i].topic);
            mqtt_info->callbackHandler[i].topic = RT_NULL;
            mqtt_info->callbackHandler[i].js_func = RT_NULL;
        }
    }

    free(mqtt_info);

    hasClient = false;
}

const static jerry_object_native_info_t mqtt_client_info =
{
    mqtt_client_free_callback
};

void get_mqtt_info(void **info, jerry_value_t js_target)
{
    jerry_value_t js_info = js_get_property(js_target, "info");
    jerry_get_object_native_pointer(js_info, info, NULL);
    jerry_release_value(js_info);
}

DECLARE_HANDLER(connect)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    paho_mqtt_start(mqtt_info->client);

    return jerry_create_undefined();
}

DECLARE_HANDLER(publish)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    char *topic;
    unsigned char *send_str = NULL;
    int length = 0;
    int qos = 1;
    bool dup = false;
    bool retain = false;
    jerry_value_t js_pub_callback = RT_NULL;

    switch (args_cnt)
    {
    case 4:
        if (jerry_value_is_function(args[3]))
        {
            js_pub_callback = args[3];
        }
        else
        {
            LOG_E("the 4rd parameter is not function");
            goto _exit;
        }
    case 3:
        if (jerry_value_is_function(args[2]))
        {
            js_pub_callback = args[2];
        }
        else if (jerry_value_is_object(args[2]))
        {
            if (jerry_value_is_object(js_get_property(args[2], "qos")))
            {
                qos = jerry_get_number_value(js_get_property(args[2], "qos"));

                if (qos < 0)
                {
                    qos = 0;
                }
                else if (qos > 2)
                {
                    qos = 2;
                }
            }

            if (jerry_value_is_boolean(js_get_property(args[2], "dup")))
            {
                dup = jerry_value_to_boolean(js_get_property(args[2], "dup"));
            }

            if (jerry_value_is_boolean(js_get_property(args[2], "retain")))
            {
                retain = jerry_value_to_boolean(js_get_property(args[2], "retain"));
            }
        }
        else
        {
            LOG_E("the 3nd parameter is not object or func");
            goto _exit;
        }
    case 2:
        if (jerry_value_is_string(args[1]))
        {
            send_str = (unsigned char *)js_value_to_string(args[1]);
            length = strlen((const char *)send_str);
        }
        else if (jerry_value_is_object(args[1]))
        {
            js_buffer_t *js_buffer = jerry_buffer_find(args[1]);
            if (js_buffer)
            {
                send_str = js_buffer->buffer;
                length = js_buffer->bufsize;
            }
        }
        else
        {
            LOG_E("the 1st parameter is not string or buffer");
            goto _exit;
        }
    case 1 :
        if (jerry_value_is_string(args[0]))
        {
            topic = js_value_to_string(args[0]);
        }
        else
        {
            LOG_E("the 1st parameter is not string");
            goto _exit;
        }
        break;
    default:
        LOG_E("the count of parameters is wrong");
        goto _exit;
        break;
    }
    rt_sem_take(mqtt_info->sem, RT_WAITING_FOREVER);
    MQTTMessage message;

    message.qos = qos;
    message.retained = retain;
    message.payload = (void *)send_str;
    message.payloadlen = length;

    MQTTPublish(mqtt_info->client, topic, &message);

    if (js_pub_callback != RT_NULL)
    {
        for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
        {
            if (mqtt_info->callbackHandler[i].topic == RT_NULL)
            {
                mqtt_info->callbackHandler[i].topic = topic;
                mqtt_info->callbackHandler[i].js_func = js_pub_callback;
                break;
            }
        }
    }
    rt_sem_release(mqtt_info->sem);
    free(send_str);
    free(topic);
_exit:
    return jerry_create_undefined();
}

DECLARE_HANDLER(subscribe)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    if (mqtt_info->subCount == MAX_MESSAGE_HANDLERS)
    {
        LOG_E("the count of topics is max");
        goto _exit;
    }

    int qos = 0;
    char *topic = RT_NULL;
    jerry_value_t js_sub_callback = 0;
    switch (args_cnt)
    {
    case 3:
        if (jerry_value_is_function(args[2]))
        {
            js_sub_callback = args[2];
        }
        else
        {
            LOG_E("the 3rd parameter is not function");
            goto _exit;
        }
    case 2:
        if (jerry_value_is_object(args[1]) && jerry_value_is_number(js_get_property(args[1], "qos")))
        {
            qos = jerry_get_number_value(js_get_property(args[1], "qos"));

            if (qos < 0)
            {
                qos = 0;
            }
            else if (qos > 2)
            {
                qos = 2;
            }
        }
        else if (jerry_value_is_function(args[1]))
        {
            js_sub_callback = args[1];
        }
        else
        {
            LOG_E("the 2nd parameter is not number or func");
            goto _exit;
        }
    case 1 :
        if (jerry_value_is_string(args[0]))
        {
            topic = js_value_to_string(args[0]);
        }
        else
        {
            LOG_E("the 1st parameter is not string");
            goto _exit;
        }
        break;
    default:
        LOG_E("the count of parameters is wrong");
        goto _exit;
        break;
    }
    rt_sem_take(mqtt_info->sem, RT_WAITING_FOREVER);
    for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
    {
        if (rt_strcmp(topic, mqtt_info->client->messageHandlers[i].topicFilter) == 0)
        {
            free(topic);
            rt_sem_release(mqtt_info->sem);
            goto _exit;
            break;
        }
    }

    int index;
    for (index = 0; index < MAX_MESSAGE_HANDLERS ; index++)
    {
        if (mqtt_info->client->messageHandlers[index].topicFilter == RT_NULL)
        {
            mqtt_info->client->messageHandlers[index].topicFilter = topic;
            mqtt_info->client->messageHandlers[index].callback = mqtt_sub_callback;
            mqtt_info->client->messageHandlers[index].qos = qos;


            mqtt_info->subCount++;

            if (mqtt_info->client->isconnected == 1)
            {
                MQTT_CMD(mqtt_info->client, "RECONNECT");
            }

            /*emit the callback*/
            if (js_sub_callback)
            {
                mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
                memset(cb_info, 0, sizeof(mqtt_cbinfo_t));
                cb_info->this_value  = this_value;
                cb_info->return_value = RT_NULL;
                cb_info->js_func = js_sub_callback;
                cb_info->event_name = RT_NULL;

                js_send_callback(mqtt_info->fun_callback, cb_info, sizeof(mqtt_cbinfo_t));
                free(cb_info);
            }
            break;
        }
    }
    rt_sem_release(mqtt_info->sem);


_exit:
    return jerry_create_undefined();
}

DECLARE_HANDLER(unsubscribe)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    jerry_value_t js_unsub_callback = RT_NULL;
    char *topic;
    switch (args_cnt)
    {
    case 2:
        if (jerry_value_is_function(args[1]))
        {
            js_unsub_callback = args[1];
        }
        else
        {
            goto _exit;
        }
    case 1 :
        if (jerry_value_is_string(args[0]))
        {
            topic = js_value_to_string(args[0]);
        }
        else
        {
            goto _exit;
        }
        break;
    default:
        goto _exit;
        break;
    }
    rt_sem_take(mqtt_info->sem, RT_WAITING_FOREVER);
    for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
    {
        if (strcmp(topic, mqtt_info->client->messageHandlers[i].topicFilter) == 0)
        {
            /*free data*/
            free(mqtt_info->client->messageHandlers[i].topicFilter);
            mqtt_info->client->messageHandlers[i].topicFilter = RT_NULL;

            /*restart mqtt*/
            mqtt_info->subCount--;
            if (mqtt_info->client->isconnected == 1)
            {
                MQTT_CMD(mqtt_info->client, "RECONNECT");
            }

            /*emit the callback*/
            if (js_unsub_callback)
            {
                mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
                memset(cb_info, 0, sizeof(mqtt_cbinfo_t));
                cb_info->this_value  = this_value;
                cb_info->return_value = RT_NULL;
                cb_info->js_func = js_unsub_callback;
                cb_info->event_name = RT_NULL;

                js_send_callback(mqtt_info->fun_callback, cb_info, sizeof(mqtt_cbinfo_t));
                free(cb_info);
            }

            for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
            {
                if (strcmp(mqtt_info->callbackHandler[i].topic, topic) == 0)
                {
                    free(mqtt_info->callbackHandler[i].topic);
                    mqtt_info->callbackHandler[i].topic = RT_NULL;
                    mqtt_info->callbackHandler[i].js_func = RT_NULL;
                }
            }
            break;
        }
    }
    free(topic);
    rt_sem_release(mqtt_info->sem);
_exit:
    return jerry_create_undefined();
}

DECLARE_HANDLER(end)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    jerry_value_t js_end_callback = RT_NULL;
    bool force;
    switch (args_cnt)
    {
    case 2:
        if (jerry_value_is_function(args[1]))
        {
            js_end_callback = args[1];
        }
        else
        {
            goto _exit;
        }
    case 1 :
        if (jerry_value_is_boolean(args[0]))
        {
            force = jerry_value_to_boolean(args[0]);
        }
        else if (jerry_value_is_function(args[0]))
        {
            js_end_callback = args[0];
        }
        else
        {
            goto _exit;
        }
        break;
    default:
        goto _exit;
        break;
    }

    // if(mqtt_info->client->isconnected == 1)
    {
        MQTT_CMD(mqtt_info->client, "DISCONNECT");

        /*emit the callback*/
        if (js_end_callback)
        {
            mqtt_cbinfo_t *cb_info = (mqtt_cbinfo_t *)malloc(sizeof(mqtt_cbinfo_t));
            memset(cb_info, 0, sizeof(mqtt_cbinfo_t));
            cb_info->this_value  = this_value;
            cb_info->return_value = RT_NULL;
            cb_info->js_func = js_end_callback;
            cb_info->event_name = RT_NULL;

            js_send_callback(mqtt_info->fun_callback, cb_info, sizeof(mqtt_cbinfo_t));
            free(cb_info);
        }
    }
_exit:
    return jerry_create_undefined();
}

DECLARE_HANDLER(reconnect)
{
    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    if (mqtt_info->client->isconnected == 1)
    {
        MQTT_CMD(mqtt_info->client, "RECONNECT");
    }
    else
    {
        paho_mqtt_start(mqtt_info->client);
    }

    return jerry_create_undefined();
}

static jerry_value_t mqtt_create_client(int arg_cnt, const jerry_value_t *args)
{
    jerry_value_t js_client = jerry_create_object();
    mqtt_info_t *client_info = RT_NULL;

    if (hasClient == true)
    {
        goto _exit;
    }

    client_info = (mqtt_info_t *)malloc(sizeof(mqtt_info_t));
    memset(client_info, 0, sizeof(mqtt_info_t));

    if (!client_info)
    {
        goto _exit;
    }

    client_info->client = (MQTTClient *)malloc(sizeof(MQTTClient));
    if (!client_info->client)
    {
        goto _exit;
    }

    /*initialize MQTTClient* client */
    client_info->client->isconnected = 0;
    client_info->client->uri = js_value_to_string(args[0]);
    client_info->client->toClose = 0;
    client_info->client->user_data = (void *)client_info;
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    memcpy(&(client_info->client->condata), &condata, sizeof(condata));

    static char cid[20] = { 0 };
    rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
    client_info->client->condata.clientID.cstring = cid;
    client_info->client->condata.keepAliveInterval = 60;
    client_info->client->condata.cleansession = 1;
    client_info->client->reconnectPeriod = 1000; //1s
    client_info->client->connectTimeout = 30 * 1000; //30s
    client_info->client->condata.username.cstring = RT_NULL;
    client_info->client->condata.password.cstring = RT_NULL;

    for (int i = 0 ; i < MAX_MESSAGE_HANDLERS ; i++)
    {
        client_info->client->messageHandlers[i].callback = RT_NULL;
        client_info->client->messageHandlers[i].qos = QOS0;
        client_info->client->messageHandlers[i].topicFilter = RT_NULL;
    }
    /* config MQTT will param. */
    client_info->client->condata.willFlag = 0;

    /*set the options of client*/
    if (arg_cnt == 2)
    {
        jerry_value_t js_keepalive = js_get_property(args[1], "keepalive");
        if (jerry_value_is_number(js_keepalive))
        {
            client_info->client->condata.keepAliveInterval = jerry_get_number_value(js_keepalive);
        }
        jerry_release_value(js_keepalive);

        jerry_value_t js_clientId = js_get_property(args[1], "clientId");
        if (jerry_value_is_string(js_clientId))
        {
            char *cid = js_value_to_string(js_clientId);
            client_info->client->condata.clientID.cstring = cid;
            free(cid);
        }
        jerry_release_value(js_clientId);

        jerry_value_t js_clean = js_get_property(args[1], "clean");
        if (jerry_value_is_boolean(js_clean))
        {
            client_info->client->condata.cleansession = jerry_value_to_boolean(js_clean);
        }
        jerry_release_value(js_clean);

        jerry_value_t js_reconnectPeriod = js_get_property(args[1], "reconnectPeriod");
        if (jerry_value_is_number(js_reconnectPeriod))
        {
            client_info->client->reconnectPeriod = jerry_get_number_value(js_reconnectPeriod);
        }
        jerry_release_value(js_reconnectPeriod);

        jerry_value_t js_connectTimeout = js_get_property(args[1], "connectTimeout");
        if (jerry_value_is_number(js_connectTimeout))
        {
            client_info->client->connectTimeout = jerry_get_number_value(js_connectTimeout);
        }
        jerry_release_value(js_connectTimeout);

        jerry_value_t js_username = js_get_property(args[1], "username");
        if (jerry_value_is_string(js_username))
        {
            char *username = js_value_to_string(js_username);
            client_info->client->condata.username.cstring = username;
            free(username);
        }
        jerry_release_value(js_username);

        jerry_value_t js_password = js_get_property(args[1], "password");
        if (jerry_value_is_string(js_password))
        {
            char *password = js_value_to_string(js_password);
            client_info->client->condata.password.cstring = password;
            free(password);
        }
        jerry_release_value(js_password);

        jerry_value_t js_will =  js_get_property(args[1], "will");
        if (jerry_value_is_object(js_will))
        {
            client_info->client->condata.willFlag = 1;
            client_info->client->condata.will.qos = 0;
            client_info->client->condata.will.retained = 0;

            jerry_value_t js_will_topic = js_get_property(js_will, "topic");
            if (jerry_value_is_string(js_will_topic))
            {
                char *will_topic = js_value_to_string(js_will_topic);
                client_info->client->condata.will.topicName.cstring = will_topic;
                free(will_topic);
                jerry_release_value(js_will_topic);
            }
            else
            {
                jerry_release_value(js_will_topic);
                goto _exit;
            }

            jerry_value_t js_will_message = js_get_property(js_will, "payload");
            if (jerry_value_is_string(js_will_message))
            {
                char *will_message = js_value_to_string(js_will_message);
                client_info->client->condata.will.topicName.cstring = will_message;
                free(will_message);
                jerry_release_value(js_will_topic);
            }
            else
            {
                jerry_release_value(js_will_topic);
                goto _exit;
            }

            jerry_value_t js_will_qos = js_get_property(js_will, "qos");
            if (jerry_value_is_number(js_will_message))
            {
                int qos = jerry_get_number_value(js_will_qos);
                if (qos < 0)
                {
                    qos = 0;
                }
                else if (qos > 2)
                {
                    qos = 2;
                }
                client_info->client->condata.will.qos = qos;
            }
            jerry_release_value(js_will_qos);

            jerry_value_t js_will_retain = js_get_property(js_will, "retain");
            if (jerry_value_is_number(js_will_retain))
            {
                client_info->client->condata.will.retained = jerry_value_to_boolean(js_will_retain);
            }
            jerry_release_value(js_will_retain);
        }
    }

    /* malloc buffer. */
    client_info->client->buf_size = client_info->client->readbuf_size = 1024;
    client_info->client->buf = malloc(client_info->client->buf_size);
    client_info->client->readbuf = malloc(client_info->client->readbuf_size);
    if (!(client_info->client->buf && client_info->client->readbuf))
    {
        goto _exit;
    }

    /*create sem*/
    client_info->sem = rt_sem_create("mqtt_msghandler_semt", 1, RT_IPC_FLAG_FIFO);

    js_make_emitter(js_client, jerry_create_undefined());

    /*add js event callback*/
    client_info->fun_callback = js_add_callback(mqtt_func_callback);
    client_info->event_callback = js_add_callback(mqtt_event_callback);
    client_info->close_callback = js_add_callback(mqtt_free_callback);

    /* set client's event callback function */
    client_info->client->connect_callback = mqtt_connect_callback;
    client_info->client->online_callback = mqtt_online_callback;
    client_info->client->offline_callback = mqtt_offline_callback;

    /*create js object*/
    jerry_value_t js_info = jerry_create_object();
    client_info->this_value = js_client;
    js_set_property(js_client, "info", js_info);
    jerry_set_object_native_pointer(js_info, client_info, &mqtt_client_info);  // set native_pointer
    jerry_release_value(js_info);
    /*set method*/
    REGISTER_METHOD_NAME(js_client, "connect", connect);
    REGISTER_METHOD_NAME(js_client, "publish", publish);
    REGISTER_METHOD_NAME(js_client, "subscribe", subscribe);
    REGISTER_METHOD_NAME(js_client, "unsubscribe", unsubscribe);
    REGISTER_METHOD_NAME(js_client, "end", end);
    REGISTER_METHOD_NAME(js_client, "reconnect", reconnect);

    hasClient = true;

    return (js_client);
_exit:
    jerry_release_value(js_client);
    if (client_info)
    {
        if (client_info->client)
        {
            free(client_info->client);
        }

        free(client_info);
    }
    return jerry_create_undefined();
}

DECLARE_HANDLER(Client)
{
    if (args_cnt > 2 || args_cnt == 0)
    {
        return jerry_create_undefined();
    }

    switch (args_cnt)
    {
    case 2:
        if (!jerry_value_is_object(args[1]))
        {
            return jerry_create_undefined();
        }
    case 1:
        if (!jerry_value_is_string(args[0]))
        {
            return jerry_create_undefined();
        }
    default:
        break;
    }

    jerry_value_t js_client = mqtt_create_client(args_cnt, args);
    return js_client;
}

DECLARE_HANDLER(client_connect)
{
    if (args_cnt > 2 || args_cnt == 0)
    {
        return jerry_create_undefined();
    }

    switch (args_cnt)
    {
    case 2:
        if (!jerry_value_is_object(args[1]))
        {
            return jerry_create_undefined();
        }
    case 1:
        if (!jerry_value_is_string(args[0]))
        {
            return jerry_create_undefined();
        }
    default:
        break;
    }

    jerry_value_t js_client =  mqtt_create_client(args_cnt, args);

    mqtt_info_t *mqtt_info = RT_NULL;
    get_mqtt_info((void **)&mqtt_info, this_value);

    paho_mqtt_start(mqtt_info->client);

    return js_client;
}

static jerry_value_t jerry_mqtt_init()
{
    jerry_value_t js_mqtt = jerry_create_object();

    REGISTER_METHOD_NAME(js_mqtt, "Client", Client);
    REGISTER_METHOD_NAME(js_mqtt, "connect", client_connect);

    return js_mqtt;
}

JS_MODULE(mqtt, jerry_mqtt_init)

#endif
