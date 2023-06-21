
#ifndef JERRY_EVENT_H__
#define JERRY_EVENT_H__

#include <rtthread.h>
#include <jerry_util.h>

#ifdef __cplusplus
extern "C" {
#endif

void js_add_event_listener(jerry_value_t obj, const char *event_name, jerry_value_t func);
void js_remove_event_listener(jerry_value_t obj, const char *event_name);
rt_bool_t js_emit_event(jerry_value_t obj, const char *event_name, const jerry_value_t argv[],
                        const jerry_length_t argc);
void js_destroy_emitter(jerry_value_t obj);
void js_make_emitter(jerry_value_t obj, jerry_value_t prototype);
int js_event_init(void);
int js_event_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
