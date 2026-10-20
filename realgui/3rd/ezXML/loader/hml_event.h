/**
 * @file hml_event.h
 * @brief HML Event Handling
 *
 * Runtime dispatch of <events> declared on components:
 *   <events>
 *     <event type="onKeyShortPress" keyName="Home">
 *       <action type="callFunction" functionName="my_cb" />
 *     </event>
 *     <event type="onMessage" message="speed"> ... </event>
 *   </events>
 * Supported actions: switchView, callFunction, sendMessage, controlTimer.
 */

#ifndef HML_EVENT_H
#define HML_EVENT_H

#include "ezxml.h"
#include "gui_obj.h"
#include "gui_view.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GUI event code of an HML event type, or GUI_EVENT_INVALID. */
gui_event_code_t hml_parse_event_type(const char *type);
VIEW_SWITCH_STYLE hml_parse_switch_style(const char *style, VIEW_SWITCH_STYLE def);

/** Switch from the current view to target, with designer default styles. */
void hml_switch_view(const char *target, const char *out_style, const char *in_style);

/** Whether the node declares a key event (the component then takes focus). */
bool hml_has_key_events(ezxml_t node);

#ifdef __cplusplus
}
#endif

#endif // HML_EVENT_H
