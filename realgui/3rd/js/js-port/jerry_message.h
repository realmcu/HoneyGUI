
#ifndef JERRY_MESSAGE_H__
#define JERRY_MESSAGE_H__

#include <rtthread.h>
#include <jerry_util.h>
#include <jerry_callbacks.h>
#include <jerry_event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*js_message_send_func)(const char *name, rt_uint8_t *data, rt_uint32_t size);

void js_message_send_func_init(js_message_send_func func);
rt_bool_t js_message_send(const char *name, rt_uint8_t *data, rt_uint32_t size);
int js_message_init(void);

#ifdef __cplusplus
}
#endif

#endif
