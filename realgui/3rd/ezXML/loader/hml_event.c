/**
 * @file hml_event.c
 * @brief HML Event Handling Implementation
 */

#include "hml_event.h"
#include "hml_loader.h"
#include "gui_api.h"
#include <string.h>

gui_event_code_t hml_parse_event_type(const char *type)
{
    if (!type) { return GUI_EVENT_TOUCH_CLICKED; }

    if (strcmp(type, "onSwipeLeft") == 0) { return GUI_EVENT_TOUCH_MOVE_LEFT; }
    if (strcmp(type, "onSwipeRight") == 0) { return GUI_EVENT_TOUCH_MOVE_RIGHT; }
    if (strcmp(type, "onSwipeUp") == 0) { return GUI_EVENT_TOUCH_MOVE_UP; }
    if (strcmp(type, "onSwipeDown") == 0) { return GUI_EVENT_TOUCH_MOVE_DOWN; }
    if (strcmp(type, "onClick") == 0) { return GUI_EVENT_TOUCH_CLICKED; }
    if (strcmp(type, "onLongPress") == 0) { return GUI_EVENT_TOUCH_LONG; }

    return GUI_EVENT_TOUCH_CLICKED;
}

VIEW_SWITCH_STYLE hml_parse_switch_style(const char *style)
{
    if (!style) { return SWITCH_OUT_TO_LEFT_USE_TRANSLATION; }

    // SWITCH_OUT styles
    if (strcmp(style, "SWITCH_OUT_TO_LEFT_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_LEFT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_RIGHT_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_RIGHT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_TOP_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_TOP_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_STILL_USE_BLUR") == 0) { return SWITCH_OUT_STILL_USE_BLUR; }
    if (strcmp(style, "SWITCH_OUT_ANIMATION_FADE") == 0) { return SWITCH_OUT_ANIMATION_FADE; }
    if (strcmp(style, "SWITCH_INIT_STATE") == 0) { return SWITCH_INIT_STATE; }

    // SWITCH_IN styles
    if (strcmp(style, "SWITCH_IN_FROM_LEFT_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_LEFT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_RIGHT_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_RIGHT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_TOP_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_TOP_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_ANIMATION_FADE") == 0) { return SWITCH_IN_ANIMATION_FADE; }

    return SWITCH_OUT_TO_LEFT_USE_TRANSLATION;
}

void hml_bind_view_events(gui_view_t *view, ezxml_t node)
{
    ezxml_t events = ezxml_child(node, "events");
    if (!events) { return; }

    for (ezxml_t event = ezxml_child(events, "event"); event; event = event->next)
    {
        const char *type = ezxml_attr(event, "type");
        if (!type) { continue; }

        ezxml_t action = ezxml_child(event, "action");
        if (!action) { continue; }

        const char *action_type = ezxml_attr(action, "type");
        if (!action_type || strcmp(action_type, "switchView") != 0) { continue; }

        const char *target = ezxml_attr(action, "target");
        const char *switch_out = ezxml_attr(action, "switchOutStyle");
        const char *switch_in = ezxml_attr(action, "switchInStyle");

        if (!target) { continue; }

        gui_event_code_t evt = hml_parse_event_type(type);
        VIEW_SWITCH_STYLE out_style = hml_parse_switch_style(switch_out);
        VIEW_SWITCH_STYLE in_style = hml_parse_switch_style(switch_in);

        gui_view_switch_on_event(view, target, out_style, in_style, evt);
    }
}

// Context for switchView callback
typedef struct
{
    const char *target;
    VIEW_SWITCH_STYLE out_style;
    VIEW_SWITCH_STYLE in_style;
} hml_switch_view_ctx_t;

static void hml_switch_view_callback(gui_obj_t *obj)
{
    (void)obj;  // Unused for now
    gui_log("[HML] switchView callback triggered\n");
}

void hml_bind_obj_events(gui_obj_t *obj, ezxml_t node)
{
    (void)obj;  // Will be used when full implementation is done

    ezxml_t events = ezxml_child(node, "events");
    if (!events) { return; }

    for (ezxml_t event = ezxml_child(events, "event"); event; event = event->next)
    {
        const char *type = ezxml_attr(event, "type");
        if (!type) { continue; }

        ezxml_t action = ezxml_child(event, "action");
        if (!action) { continue; }

        const char *action_type = ezxml_attr(action, "type");
        if (!action_type) { continue; }

        if (strcmp(action_type, "switchView") == 0)
        {
            const char *target = ezxml_attr(action, "target");
            if (!target) { continue; }

            // For now, log the event binding
            gui_log("[HML] Bind %s -> switchView(%s)\n", type, target);

            // TODO: Implement proper context passing for switchView
            // This requires extending the event system or using a different approach
        }
    }
}
