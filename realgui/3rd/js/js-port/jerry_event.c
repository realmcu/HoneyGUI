

#include <jerry_event.h>

static jerry_value_t emitter = 0;
static jerry_value_t _js_emitter_prototype = 0;

struct js_listener
{
    jerry_value_t func;
    struct js_listener *next;
};

struct js_event
{
    char *name;
    struct js_listener *listeners;
    struct js_event *next;
};

struct js_emitter
{
    struct js_event *events;
};

static void remove_event(struct js_emitter *emitter, struct js_event *event);

static void free_listener(void *ptr)
{
    if (ptr)
    {
        struct js_listener *listener = (struct js_listener *)ptr;
        jerry_release_value(listener->func);
        rt_free(listener);
    }
}

static void js_emitter_free_cb(void *native)
{
    struct js_emitter *emitter = (struct js_emitter *)native;
    struct js_event *_event,  *event = emitter->events;

    while (event)
    {
        _event = event;
        event = event->next;

        remove_event(emitter, _event);
    }

    rt_free(emitter);
}

static const jerry_object_native_info_t emitter_type_info =
{
    .free_cb = js_emitter_free_cb
};

static void js_event_proto_free_cb(void *native)
{
    _js_emitter_prototype = 0;
}

static const jerry_object_native_info_t event_proto_type_info =
{
    .free_cb = js_event_proto_free_cb
};

static struct js_listener *find_listener(struct js_event *event, jerry_value_t func)
{
    struct js_listener *_listener = event->listeners;

    while (_listener != NULL)
    {
        if (_listener->func == func)
        {
            break;
        }

        _listener = _listener->next;
    }

    return _listener;
}

static void append_listener(struct js_event *event, struct js_listener *listener)
{
    struct js_listener *_listener = event->listeners;

    if (event->listeners == NULL)
    {
        event->listeners = listener;
        return;
    }

    while (_listener->next != NULL)
    {
        _listener = _listener->next;
    }

    _listener->next = listener;
}

static void remove_listener(struct js_event *event, struct js_listener *listener)
{
    struct js_listener *_listener = event->listeners;

    if (event->listeners == listener || event->listeners == NULL)
    {
        event->listeners = NULL;
        return;
    }

    while (_listener->next != listener)
    {
        _listener = _listener->next;
    }

    _listener->next = listener->next;
}

static struct js_event *find_event(struct js_emitter *emitter, const char *event_name)
{
    struct js_event *event = emitter->events;

    while (event != NULL)
    {
        if (strcmp(event->name, event_name) == 0)
        {
            break;
        }

        event = event->next;
    }

    return event;
}

static void append_event(struct js_emitter *emitter, struct js_event *event)
{
    struct js_event *_event = emitter->events;

    if (emitter->events == NULL)
    {
        emitter->events = event;
        return;
    }

    while (_event->next != NULL)
    {
        _event = _event->next;
    }

    _event->next = event;
}

static void remove_event(struct js_emitter *emitter, struct js_event *event)
{
    struct js_event *_event = emitter->events;
    struct js_listener *_listener, *listener = event->listeners;

    if (emitter->events == event)
    {
        emitter->events = event->next;
    }
    else
    {
        while (_event->next != event)
        {
            _event = _event->next;
        }

        _event->next = event->next;
    }

    while (listener != NULL)
    {
        _listener = listener;
        listener = listener->next;
        free_listener(_listener);
    }

    rt_free(event->name);
    rt_free(event);
}

void js_add_event_listener(jerry_value_t obj, const char *event_name, jerry_value_t func)
{
    void *native_handle = NULL;

    jerry_get_object_native_pointer(obj, &native_handle, NULL);
    if (native_handle)
    {
        struct js_emitter *emitter = (struct js_emitter *)native_handle;
        struct js_event *event = NULL;
        struct js_listener *listener = NULL;

        event = find_event(emitter, event_name);
        if (!event)
        {
            event = (struct js_event *)rt_malloc(sizeof(struct js_event));
            if (!event)
            {
                return;
            }

            event->next = NULL;
            event->listeners = NULL;
            event->name = rt_strdup(event_name);
            append_event((struct js_emitter *)native_handle, event);
        }

        listener = (struct js_listener *)rt_malloc(sizeof(struct js_listener));
        if (!listener)
        {
            return;
        }

        listener->func = jerry_acquire_value(func);
        listener->next = NULL;

        append_listener(event, listener);
    }
}

void js_remove_event_listener(jerry_value_t obj, const char *event_name)
{
    void *native_handle = NULL;

    jerry_get_object_native_pointer(obj, &native_handle, NULL);
    if (native_handle)
    {
        struct js_emitter *emitter = (struct js_emitter *)native_handle;
        struct js_event *event = find_event(emitter, event_name);
        if (event)
        {
            struct js_listener *_listener, *listener = event->listeners;

            while (listener != NULL)
            {
                _listener = listener;
                listener = listener->next;
                free_listener(_listener);
            }

            event->listeners = NULL;
        }
    }
}

rt_bool_t js_emit_event(jerry_value_t obj, const char *event_name, const jerry_value_t argv[],
                        const jerry_length_t argc)
{
    void *native_handle = NULL;

    jerry_get_object_native_pointer(obj, &native_handle, NULL);
    if (native_handle)
    {
        struct js_emitter *emitter = (struct js_emitter *)native_handle;
        struct js_event *event = find_event(emitter, event_name);
        if (event)
        {
            struct js_listener *listener = event->listeners;

            while (listener)
            {
                jerry_value_t ret = jerry_call_function(listener->func, obj, argv, argc);
                if (jerry_value_is_error(ret))
                {
                    js_value_dump(obj);
                    rt_kprintf("event [%s] calling listener error!\n", event_name);
                }
                jerry_release_value(ret);
                listener = listener->next;
            }

            return RT_TRUE;
        }
    }

    return RT_FALSE;
}

DECLARE_HANDLER(add_listener)
{
    if (args_cnt == 2)
    {
        char *name = js_value_to_string(args[0]);
        if (name)
        {
            js_add_event_listener(this_value, name, args[1]);
        }

        rt_free(name);
    }

    return jerry_acquire_value(this_value);
}

DECLARE_HANDLER(remove_listener)
{
    if (args_cnt == 2)
    {
        char *name = js_value_to_string(args[0]);
        if (name)
        {
            void *native_handle = NULL;

            jerry_get_object_native_pointer(this_value, &native_handle, NULL);
            if (native_handle)
            {
                struct js_emitter *emitter = (struct js_emitter *)native_handle;
                struct js_event *event = find_event(emitter, name);
                if (event)
                {
                    struct js_listener *listener = find_listener(event, args[1]);
                    if (listener)
                    {
                        remove_listener(event, listener);
                        free_listener(listener);
                    }
                }
            }
        }
        rt_free(name);
    }

    return jerry_acquire_value(this_value);
}

DECLARE_HANDLER(remove_all_listeners)
{
    if (args_cnt == 1)
    {
        char *name = js_value_to_string(args[0]);
        if (name)
        {
            js_remove_event_listener(this_value, name);
        }
        rt_free(name);
    }

    return jerry_acquire_value(this_value);
}

DECLARE_HANDLER(remove_event)
{
    if (args_cnt == 1)
    {
        char *name = js_value_to_string(args[0]);
        if (name)
        {
            void *native_handle = NULL;

            jerry_get_object_native_pointer(this_value, &native_handle, NULL);
            if (native_handle)
            {
                struct js_emitter *emitter = (struct js_emitter *)native_handle;
                struct js_event *event = find_event(emitter, name);
                if (event)
                {
                    remove_event(emitter, event);
                }
            }
        }

        rt_free(name);
    }

    return jerry_acquire_value(this_value);
}

DECLARE_HANDLER(emit_event)
{
    rt_bool_t ret = RT_FALSE;

    if (args_cnt >= 1)
    {
        char *name = js_value_to_string(args[0]);
        if (name)
        {
            ret = js_emit_event(this_value, name, args + 1, args_cnt - 1);
        }

        rt_free(name);
    }

    return jerry_create_boolean(ret);
}

DECLARE_HANDLER(get_event_names)
{
    void *native_handle = NULL;

    jerry_get_object_native_pointer(this_value, &native_handle, NULL);
    if (native_handle)
    {
        struct js_emitter *emitter = (struct js_emitter *)native_handle;
        struct js_event *event = emitter->events;
        uint32_t index = 0;
        jerry_value_t ret = 0;

        while (event)
        {
            index ++;
            event = event->next;
        }

        ret = jerry_create_array(index);
        if (ret)
        {
            event = emitter->events;
            index = 0;
            while (event)
            {
                jerry_set_property_by_index(ret, index++, jerry_create_string((jerry_char_t *)event->name));
                event = event->next;
            }

            return ret;
        }
    }

    return jerry_create_undefined();
}

void js_destroy_emitter(jerry_value_t obj)
{
    void *native_handle = NULL;

    jerry_get_object_native_pointer(obj, &native_handle, NULL);
    if (native_handle)
    {
        struct js_emitter *emitter = (struct js_emitter *)native_handle;
        struct js_event *_event,  *event = emitter->events;

        while (event)
        {
            _event = event;
            event = event->next;

            remove_event(emitter, _event);
        }

        emitter->events = NULL;
    }
}

DECLARE_HANDLER(destroy)
{
    js_destroy_emitter(this_value);

    return jerry_create_undefined();
}

static void js_event_init_prototype(void)
{
    if (_js_emitter_prototype == 0)
    {
        _js_emitter_prototype = jerry_create_object();

        REGISTER_METHOD_NAME(_js_emitter_prototype, "on", add_listener);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "addListener", add_listener);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "emit", emit_event);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "removeListener", remove_listener);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "removeEvent", remove_event);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "removeAllListeners", remove_all_listeners);
        REGISTER_METHOD_NAME(_js_emitter_prototype, "eventNames", get_event_names);
        REGISTER_METHOD(_js_emitter_prototype, destroy);

        jerry_set_object_native_pointer(_js_emitter_prototype, NULL, &event_proto_type_info);
    }
}

void js_make_emitter(jerry_value_t obj, jerry_value_t prototype)
{
    jerry_value_t proto;
    struct js_emitter *emitter = NULL;

    js_event_init_prototype();
    proto = _js_emitter_prototype;
    if (jerry_value_is_object(prototype))
    {
        jerry_set_prototype(prototype, proto);
        proto = prototype;
    }
    jerry_set_prototype(obj, proto);

    emitter = rt_malloc(sizeof(struct js_emitter));
    if (emitter)
    {
        emitter->events = NULL;
        jerry_set_object_native_pointer(obj, emitter, &emitter_type_info);
    }
}

DECLARE_HANDLER(Event)
{
    if (emitter != 0)
    {
        return jerry_acquire_value(emitter);
    }

    emitter = jerry_create_object();
    js_make_emitter(emitter, jerry_create_undefined());
    return jerry_acquire_value(emitter);
}

int js_event_init(void)
{
    REGISTER_HANDLER(Event);
    return 0;
}

int js_event_deinit(void)
{
    if (emitter != 0)
    {
        js_destroy_emitter(emitter);
        jerry_release_value(emitter);
        emitter = 0;
    }

    if (_js_emitter_prototype != 0)
    {
        jerry_release_value(_js_emitter_prototype);
    }

    return 0;
}
