

#include <jerry_message.h>
#include <jerry_buffer.h>

static jerry_value_t js_message_obj = 0;
static struct js_callback *js_message_cb = NULL;
static js_message_send_func js_msg_send_func = NULL;

struct js_message
{
    char *name;
    rt_uint8_t *data;
    rt_uint32_t size;
};

void js_message_send_func_init(js_message_send_func func)
{
    js_msg_send_func = func;
}

rt_bool_t js_message_send(const char *name, rt_uint8_t *data, rt_uint32_t size)
{
    rt_bool_t ret = RT_FALSE;

    if (js_message_cb)
    {
        struct js_message msg;

        msg.name = rt_strdup(name);
        msg.size = size;
        msg.data = rt_malloc(msg.size);
        if (msg.data)
        {
            rt_memcpy(msg.data, data, size);
            ret = js_send_callback(js_message_cb, &msg, sizeof(struct js_message));
        }

        if (!ret)
        {
            rt_free(msg.data);
            rt_free(msg.name);
        }
    }

    return ret;
}

static void js_callback_message(const void *args, uint32_t size)
{
    struct js_message *msg = (struct js_message *)args;
    if (msg)
    {
        js_buffer_t *buffer;
        jerry_value_t buf_obj = jerry_buffer_create(msg->size, &buffer);
        if (buffer)
        {
            rt_memcpy(buffer->buffer, msg->data, msg->size);
            js_emit_event(js_message_obj, msg->name, &buf_obj, 1);
        }
        /* release value */
        jerry_release_value(buf_obj);
        rt_free(msg->name);
        rt_free(msg->data);
        rt_free(msg);
    }
}

static void js_message_info_free(void *native)
{
    js_message_obj = 0;
    js_remove_callback(js_message_cb);
    js_message_cb = NULL;
}

static const jerry_object_native_info_t js_message_info =
{
    .free_cb = js_message_info_free
};

DECLARE_HANDLER(send)
{
    bool result = false;

    if (js_msg_send_func && args_cnt == 2 && jerry_value_is_string(args[0]))
    {
        if (jerry_value_is_string(args[1]))
        {
            char *name = js_value_to_string(args[0]);
            if (name)
            {
                char *str = js_value_to_string(args[1]);
                js_msg_send_func(name, (rt_uint8_t *)str, rt_strlen(str));
                result = true;
                rt_free(str);
                rt_free(name);
            }
        }
        else
        {
            js_buffer_t *buffer = jerry_buffer_find(args[1]);
            if (buffer)
            {
                char *name = js_value_to_string(args[0]);
                if (name)
                {
                    js_msg_send_func(name, buffer->buffer, buffer->bufsize);
                    result = true;
                    rt_free(name);
                }
            }
        }
    }

    return jerry_create_boolean(result);
}

DECLARE_HANDLER(Message)
{
    jerry_value_t info = 0;

    if (js_message_obj != 0)
    {
        return jerry_create_undefined();
    }

    js_message_obj = jerry_create_object();
    if (jerry_value_is_error(js_message_obj))
    {
        jerry_release_value(js_message_obj);
        return jerry_create_undefined();
    }

    info = jerry_create_object();
    if (jerry_value_is_error(js_message_obj))
    {
        jerry_release_value(js_message_obj);
        jerry_release_value(info);
        return jerry_create_undefined();
    }

    js_make_emitter(js_message_obj, jerry_create_undefined());

    REGISTER_METHOD(js_message_obj, send);

    jerry_set_object_native_pointer(info, NULL, &js_message_info);
    js_set_property(js_message_obj, "info", info);
    jerry_release_value(info);

    js_message_cb = js_add_callback(js_callback_message);
    if (js_message_cb)
    {
        return js_message_obj;
    }
    else
    {
        jerry_release_value(js_message_obj);
        return jerry_create_undefined();
    }
}

int js_message_init(void)
{
    REGISTER_HANDLER(Message);

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
int js_msg_test(int argc, char **argv)
{
    if (argc > 2)
    {
        js_message_send(argv[1], (rt_uint8_t *)argv[2], rt_strlen(argv[2]));
    }

    return 0;
}
MSH_CMD_EXPORT(js_msg_test, send msg to JS by test);
#endif
