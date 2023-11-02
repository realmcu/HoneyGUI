

#ifndef JERRY_CALLBACKS_H__
#define JERRY_CALLBACKS_H__

#include <rtthread.h>
#include <jerry_util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*js_callback_func)(const void *args, uint32_t size);
typedef rt_bool_t(*js_mq_func)(void *args);

struct js_callback
{
    js_callback_func function;
    struct js_callback *next;
};

struct js_mq_callback
{
    struct js_callback *callback;
    void *args;
    uint32_t size;
};

struct js_callback *js_add_callback(js_callback_func callback);
void js_remove_callback(struct js_callback *callback);
void js_remove_all_callbacks(void);
void js_call_callback(struct js_callback *callback, const void *data, uint32_t size);
rt_bool_t js_send_callback(struct js_callback *callback, const void *args, uint32_t size);
void js_mq_func_init(js_mq_func signal);
void js_mq_func_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
