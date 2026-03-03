/**
 * @file hml_event.h
 * @brief HML Event Handling
 */

#ifndef HML_EVENT_H
#define HML_EVENT_H

#include "ezxml.h"
#include "gui_obj.h"
#include "gui_view.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse event type string to GUI event code
 * @param type Event type string (e.g., "onClick", "onSwipeLeft")
 * @return GUI event code
 */
gui_event_code_t hml_parse_event_type(const char *type);

/**
 * @brief Parse switch style string
 * @param style Style string (e.g., "SWITCH_OUT_TO_LEFT_USE_TRANSLATION")
 * @return VIEW_SWITCH_STYLE enum value
 */
VIEW_SWITCH_STYLE hml_parse_switch_style(const char *style);

/**
 * @brief Parse and bind events for a view
 * @param view View object
 * @param node XML node containing <events> child
 */
void hml_bind_view_events(gui_view_t *view, ezxml_t node);

/**
 * @brief Parse and bind events for a regular object (image, button, etc.)
 * @param obj GUI object
 * @param node XML node containing <events> child
 */
void hml_bind_obj_events(gui_obj_t *obj, ezxml_t node);

#ifdef __cplusplus
}
#endif

#endif // HML_EVENT_H
