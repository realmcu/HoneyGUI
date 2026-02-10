/**
 * @file hml_view.c
 * @brief HML View Component (hg_view, hg_window, hg_win)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_view.h"
#include "gui_api.h"
#include <string.h>

// Event type mapping
static gui_event_code_t hml_parse_event_type(const char *type)
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

// Switch style mapping
static VIEW_SWITCH_STYLE hml_parse_switch_style(const char *style)
{
    if (!style) { return SWITCH_OUT_TO_LEFT_USE_TRANSLATION; }

    if (strcmp(style, "SWITCH_OUT_TO_LEFT_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_LEFT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_RIGHT_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_RIGHT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_TOP_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_TOP_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION") == 0) { return SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION; }

    if (strcmp(style, "SWITCH_IN_FROM_LEFT_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_LEFT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_RIGHT_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_RIGHT_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_TOP_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_TOP_USE_TRANSLATION; }
    if (strcmp(style, "SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION") == 0) { return SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION; }

    return SWITCH_OUT_TO_LEFT_USE_TRANSLATION;
}

// Parse events node
static void hml_parse_view_events(gui_view_t *view, ezxml_t node)
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

gui_obj_t *hml_create_view(gui_obj_t *parent, ezxml_t node)
{
    // Only create view with entry="true"
    const char *entry = ezxml_attr(node, "entry");
    if (!entry || strcmp(entry, "true") != 0)
    {
        return NULL;
    }

    const char *id = hml_attr_str(node, "id", "view");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    gui_view_t *view = gui_view_create(parent, id, x, y, w, h);

    // Parse and bind view switch events
    hml_parse_view_events(view, node);

    return (gui_obj_t *)view;
}
