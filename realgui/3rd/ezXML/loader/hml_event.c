/**
 * @file hml_event.c
 * @brief HML Event Handling Implementation
 *
 * Mirrors the designer's exported event code:
 *  - one callback per event code and object (gui_obj_add_event_cb dedupes by
 *    code), key events tell keys apart by e->indev_name;
 *  - switchView on click/long-press/swipe of an hg_view uses the view's own
 *    switch-on-event mechanism, everywhere else gui_view_switch_direct();
 *  - onMessage subscribes the component to the topic.
 */

#include "hml_event.h"
#include "hml_runtime.h"
#include "hml_json.h"
#include "hml_utils.h"
#include "gui_api.h"
#include "gui_listener.h"
#include <string.h>

/*============================================================================*
 *                              Tables
 *============================================================================*/

typedef struct
{
    const char *name;
    gui_event_code_t code;
} hml_event_map_t;

static const hml_event_map_t hml_event_map[] =
{
    { "onClick",           GUI_EVENT_TOUCH_CLICKED },
    { "onLongPress",       GUI_EVENT_TOUCH_LONG },
    { "onTouchDown",       GUI_EVENT_TOUCH_PRESSED },
    { "onTouchUp",         GUI_EVENT_TOUCH_RELEASED },
    { "onKeyShortPress",   GUI_EVENT_KB_SHORT_PRESSED },
    { "onKeyLongPress",    GUI_EVENT_KB_LONG_PRESSED },
    { "onSwipeLeft",       GUI_EVENT_TOUCH_MOVE_LEFT },
    { "onSwipeRight",      GUI_EVENT_TOUCH_MOVE_RIGHT },
    { "onSwipeUp",         GUI_EVENT_TOUCH_MOVE_UP },
    { "onSwipeDown",       GUI_EVENT_TOUCH_MOVE_DOWN },
    { "onSwipeLeftQuick",  GUI_EVENT_TOUCH_LEFT_SLIDE_QUICK },
    { "onSwipeRightQuick", GUI_EVENT_TOUCH_RIGHT_SLIDE_QUICK },
    { "onSwipeUpQuick",    GUI_EVENT_TOUCH_UP_SLIDE_QUICK },
    { "onSwipeDownQuick",  GUI_EVENT_TOUCH_DOWN_SLIDE_QUICK },
};

#define HML_STYLE(s) { #s, s }
typedef struct
{
    const char *name;
    VIEW_SWITCH_STYLE style;
} hml_style_map_t;

static const hml_style_map_t hml_style_map[] =
{
    HML_STYLE(SWITCH_INIT_STATE),
    HML_STYLE(SWITCH_IN_STILL_USE_BLUR),
    HML_STYLE(SWITCH_OUT_STILL_USE_BLUR),
    HML_STYLE(SWITCH_OUT_TO_LEFT_USE_TRANSLATION),
    HML_STYLE(SWITCH_OUT_TO_RIGHT_USE_TRANSLATION),
    HML_STYLE(SWITCH_OUT_TO_TOP_USE_TRANSLATION),
    HML_STYLE(SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION),
    HML_STYLE(SWITCH_IN_FROM_LEFT_USE_TRANSLATION),
    HML_STYLE(SWITCH_IN_FROM_RIGHT_USE_TRANSLATION),
    HML_STYLE(SWITCH_IN_FROM_TOP_USE_TRANSLATION),
    HML_STYLE(SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION),
    HML_STYLE(SWITCH_IN_FROM_LEFT_USE_REDUCTION),
    HML_STYLE(SWITCH_IN_FROM_RIGHT_USE_REDUCTION),
    HML_STYLE(SWITCH_IN_FROM_TOP_USE_REDUCTION),
    HML_STYLE(SWITCH_IN_FROM_BOTTOM_USE_REDUCTION),
    HML_STYLE(SWITCH_OUT_TO_LEFT_USE_REDUCTION),
    HML_STYLE(SWITCH_OUT_TO_RIGHT_USE_REDUCTION),
    HML_STYLE(SWITCH_OUT_TO_TOP_USE_REDUCTION),
    HML_STYLE(SWITCH_OUT_TO_BOTTOM_USE_REDUCTION),
    HML_STYLE(SWITCH_IN_FROM_LEFT_USE_CUBE),
    HML_STYLE(SWITCH_IN_FROM_RIGHT_USE_CUBE),
    HML_STYLE(SWITCH_IN_FROM_TOP_USE_CUBE),
    HML_STYLE(SWITCH_IN_FROM_BOTTOM_USE_CUBE),
    HML_STYLE(SWITCH_OUT_TO_LEFT_USE_CUBE),
    HML_STYLE(SWITCH_OUT_TO_RIGHT_USE_CUBE),
    HML_STYLE(SWITCH_OUT_TO_TOP_USE_CUBE),
    HML_STYLE(SWITCH_OUT_TO_BOTTOM_USE_CUBE),
    HML_STYLE(SWITCH_IN_FROM_LEFT_USE_ROTATE),
    HML_STYLE(SWITCH_IN_FROM_RIGHT_USE_ROTATE),
    HML_STYLE(SWITCH_IN_FROM_TOP_USE_ROTATE),
    HML_STYLE(SWITCH_IN_FROM_BOTTOM_USE_ROTATE),
    HML_STYLE(SWITCH_OUT_TO_LEFT_USE_ROTATE),
    HML_STYLE(SWITCH_OUT_TO_RIGHT_USE_ROTATE),
    HML_STYLE(SWITCH_OUT_TO_TOP_USE_ROTATE),
    HML_STYLE(SWITCH_OUT_TO_BOTTOM_USE_ROTATE),
    HML_STYLE(SWITCH_OUT_NONE_ANIMATION),
    HML_STYLE(SWITCH_OUT_ANIMATION_ZOOM),
    HML_STYLE(SWITCH_OUT_ANIMATION_FADE),
    HML_STYLE(SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT),
    HML_STYLE(SWITCH_OUT_ANIMATION_MOVE_TO_LEFT),
    HML_STYLE(SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_LEFT),
    HML_STYLE(SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_RIGHT),
    HML_STYLE(SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_LEFT),
    HML_STYLE(SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_RIGHT),
    HML_STYLE(SWITCH_IN_NONE_ANIMATION),
    HML_STYLE(SWITCH_IN_ANIMATION_ZOOM),
    HML_STYLE(SWITCH_IN_ANIMATION_FADE),
    HML_STYLE(SWITCH_IN_ANIMATION_MOVE_FADE),
    HML_STYLE(SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT),
    HML_STYLE(SWITCH_IN_ANIMATION_MOVE_FROM_LEFT),
    HML_STYLE(SWITCH_IN_ANIMATION_BOUNCE_FROM_RIGHT),
    HML_STYLE(SWITCH_IN_ANIMATION_RASTER_HORIZONTAL),
    HML_STYLE(SWITCH_IN_ANIMATION_RASTER_HORIZONTAL_REVERSE),
    HML_STYLE(SWITCH_IN_ANIMATION_RASTER_VERTICAL),
    HML_STYLE(SWITCH_IN_ANIMATION_RASTER_VERTICAL_REVERSE),
};

gui_event_code_t hml_parse_event_type(const char *type)
{
    if (!type) { return GUI_EVENT_INVALID; }
    for (size_t i = 0; i < sizeof(hml_event_map) / sizeof(hml_event_map[0]); i++)
    {
        if (strcmp(hml_event_map[i].name, type) == 0) { return hml_event_map[i].code; }
    }
    return GUI_EVENT_INVALID;
}

VIEW_SWITCH_STYLE hml_parse_switch_style(const char *style, VIEW_SWITCH_STYLE def)
{
    if (!style) { return def; }
    for (size_t i = 0; i < sizeof(hml_style_map) / sizeof(hml_style_map[0]); i++)
    {
        if (strcmp(hml_style_map[i].name, style) == 0) { return hml_style_map[i].style; }
    }
    gui_log("[HML] unknown switch style %s\n", style);
    return def;
}

void hml_switch_view(const char *target, const char *out_style, const char *in_style)
{
    if (!target || !target[0]) { return; }
    gui_view_t *cur = gui_view_get_current();
    if (!cur)
    {
        gui_log("[HML] switchView(%s): no current view\n", target);
        return;
    }
    if (!gui_view_descriptor_get(target))
    {
        gui_log("[HML] switchView: view '%s' does not exist\n", target);
        return;
    }
    gui_view_switch_direct(cur, target,
                           hml_parse_switch_style(out_style, SWITCH_OUT_TO_LEFT_USE_TRANSLATION),
                           hml_parse_switch_style(in_style, SWITCH_IN_FROM_RIGHT_USE_TRANSLATION));
}

/*============================================================================*
 *                              Actions
 *============================================================================*/

typedef struct
{
    gui_obj_t *obj;
    gui_event_t *e;          // key / touch events
    const char *topic;       // onMessage
    void *data;
    uint16_t len;
} hml_event_ctx_t;

static void hml_control_timer(const char *targets_json)
{
    hml_json_t *targets = hml_json_parse(targets_json);
    if (!targets)
    {
        gui_log("[HML] controlTimer: invalid timerTargets\n");
        return;
    }
    for (const hml_json_t *t = targets->child; t; t = t->next)
    {
        const char *id = hml_json_str(t, "componentId", NULL);
        const char *act = hml_json_str(t, "action", "start");
        gui_obj_t *obj = hml_obj_get(id);
        if (!obj)
        {
            gui_log("[HML] controlTimer: '%s' not found\n", id ? id : "?");
            continue;
        }
        if (!strcmp(act, "start")) { hml_timer_start(obj, hml_json_int(t, "timerIndex", 0)); }
        else if (!strcmp(act, "stop")) { hml_timer_stop(obj); }
    }
    hml_json_free(targets);
}

static void hml_run_actions(ezxml_t event, hml_event_ctx_t *ctx)
{
    for (ezxml_t action = ezxml_child(event, "action"); action; action = action->next)
    {
        const char *type = ezxml_attr(action, "type");
        if (!type) { continue; }

        if (!strcmp(type, "callFunction"))
        {
            hml_fn_t fn = hml_find_function(ezxml_attr(action, "functionName"));
            if (!fn)
            {
                gui_log("[HML] callFunction '%s' is not registered\n",
                        hml_attr_str(action, "functionName", ""));
                continue;
            }
            if (ctx->topic)
            {
                ((gui_listener_cb_t)fn)(ctx->obj, ctx->topic, ctx->data, ctx->len);
            }
            else
            {
                ((gui_event_cb_t)fn)(ctx->obj, ctx->e);
            }
        }
        else if (!strcmp(type, "switchView"))
        {
            hml_switch_view(ezxml_attr(action, "target"), ezxml_attr(action, "switchOutStyle"),
                            ezxml_attr(action, "switchInStyle"));
        }
        else if (!strcmp(type, "sendMessage"))
        {
            const char *msg = ezxml_attr(action, "message");
            if (msg) { gui_msg_publish(msg, ctx->topic ? ctx->data : NULL, ctx->topic ? ctx->len : 0); }
        }
        else if (!strcmp(type, "controlTimer"))
        {
            hml_control_timer(ezxml_attr(action, "timerTargets"));
        }
    }
}

/*============================================================================*
 *                              Dispatch
 *============================================================================*/

static bool hml_is_view_switch_event(const char *type)
{
    static const char *const types[] =
    {
        "onClick", "onLongPress", "onSwipeLeft", "onSwipeRight", "onSwipeUp", "onSwipeDown",
        "onSwipeLeftQuick", "onSwipeRightQuick", "onSwipeUpQuick", "onSwipeDownQuick",
    };
    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++)
    {
        if (!strcmp(types[i], type)) { return true; }
    }
    return false;
}

/* Event handled by gui_view_switch_on_event instead of a callback */
static bool hml_is_view_native(ezxml_t event, bool is_view)
{
    if (!is_view || !hml_is_view_switch_event(hml_attr_str(event, "type", ""))) { return false; }
    for (ezxml_t a = ezxml_child(event, "action"); a; a = a->next)
    {
        if (!strcmp(hml_attr_str(a, "type", ""), "switchView") && ezxml_attr(a, "target")) { return true; }
    }
    return false;
}

static void hml_event_cb(void *obj, gui_event_t *e)
{
    ezxml_t node = (ezxml_t)e->user_data;
    ezxml_t events = node ? ezxml_child(node, "events") : NULL;
    if (!events) { return; }

    hml_event_ctx_t ctx = { .obj = (gui_obj_t *)obj, .e = e };
    bool is_key = (e->code == GUI_EVENT_KB_SHORT_PRESSED || e->code == GUI_EVENT_KB_LONG_PRESSED);
    bool is_view = ((gui_obj_t *)obj)->type == VIEW;

    for (ezxml_t ev = ezxml_child(events, "event"); ev; ev = ev->next)
    {
        if (hml_parse_event_type(ezxml_attr(ev, "type")) != e->code) { continue; }
        if (hml_is_view_native(ev, is_view)) { continue; }
        if (is_key)
        {
            const char *key = ezxml_attr(ev, "keyName");
            if (!key || !e->indev_name || strcmp(key, (const char *)e->indev_name) != 0) { continue; }
        }
        hml_run_actions(ev, &ctx);
        if (is_key) { break; }  // exported code uses if / else if per key
    }
}

static void hml_message_cb(gui_obj_t *obj, const char *topic, void *data, uint16_t len)
{
    ezxml_t node = hml_runtime_node_of(obj);
    ezxml_t events = node ? ezxml_child(node, "events") : NULL;
    if (!events) { return; }

    hml_event_ctx_t ctx = { .obj = obj, .topic = topic, .data = data, .len = len };
    for (ezxml_t ev = ezxml_child(events, "event"); ev; ev = ev->next)
    {
        if (strcmp(hml_attr_str(ev, "type", ""), "onMessage") != 0) { continue; }
        if (strcmp(hml_attr_str(ev, "message", ""), topic) != 0) { continue; }
        hml_run_actions(ev, &ctx);
    }
}

bool hml_has_key_events(ezxml_t node)
{
    ezxml_t events = ezxml_child(node, "events");
    for (ezxml_t ev = events ? ezxml_child(events, "event") : NULL; ev; ev = ev->next)
    {
        const char *type = hml_attr_str(ev, "type", "");
        if ((!strcmp(type, "onKeyShortPress") || !strcmp(type, "onKeyLongPress")) &&
            ezxml_attr(ev, "keyName"))
        {
            return true;
        }
    }
    return false;
}

void hml_runtime_bind_events(gui_obj_t *obj, ezxml_t node, bool is_view)
{
    ezxml_t events = ezxml_child(node, "events");
    if (!events) { return; }

    for (ezxml_t ev = ezxml_child(events, "event"); ev; ev = ev->next)
    {
        const char *type = hml_attr_str(ev, "type", "");

        if (!strcmp(type, "onMessage"))
        {
            const char *msg = ezxml_attr(ev, "message");
            if (msg) { gui_msg_subscribe(obj, msg, hml_message_cb); }
            continue;
        }

        gui_event_code_t code = hml_parse_event_type(type);
        if (code == GUI_EVENT_INVALID)
        {
            gui_log("[HML] %s: unsupported event %s\n", hml_attr_str(node, "id", "?"), type);
            continue;
        }

        if (hml_is_view_native(ev, is_view))
        {
            for (ezxml_t a = ezxml_child(ev, "action"); a; a = a->next)
            {
                if (strcmp(hml_attr_str(a, "type", ""), "switchView") != 0) { continue; }
                if (!gui_view_descriptor_get(ezxml_attr(a, "target")))
                {
                    gui_log("[HML] switchView: view '%s' does not exist\n", hml_attr_str(a, "target", ""));
                    continue;
                }
                gui_view_switch_on_event((gui_view_t *)obj, ezxml_attr(a, "target"),
                                         hml_parse_switch_style(ezxml_attr(a, "switchOutStyle"),
                                                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION),
                                         hml_parse_switch_style(ezxml_attr(a, "switchInStyle"),
                                                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION),
                                         code);
            }
            continue;
        }

        // Deduplicated per code by gui_obj_add_event_cb; the callback scans all events
        gui_obj_add_event_cb(obj, hml_event_cb, code, node);
    }
}
