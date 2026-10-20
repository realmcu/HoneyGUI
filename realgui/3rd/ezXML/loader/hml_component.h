/**
 * @file hml_component.h
 * @brief HML Component creators
 *
 * A creator builds the widget for one HML tag and applies its own attributes.
 * Generic steps (object table, visibility, timers, events, focus) are done by
 * hml_component_finish(), called by the loader after the creator returns.
 */

#ifndef HML_COMPONENT_H
#define HML_COMPONENT_H

#include "gui_api.h"
#include "gui_obj.h"
#include "gui_view.h"
#include "ezxml.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef gui_obj_t *(*hml_component_creator_t)(gui_obj_t *parent, ezxml_t node);

/** Generic post-creation steps; is_view selects hg_view rules. */
void hml_component_finish(gui_obj_t *obj, ezxml_t node, bool is_view);

// Component creators (implemented in components/*.c)
gui_obj_t *hml_create_window(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_text(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_label(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_button(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_canvas(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_arc(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_circle(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_list(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_video(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_streaming(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_time_label(gui_obj_t *parent, ezxml_t node);

/** Apply the attributes of an hg_view to a view created by the view manager. */
void hml_view_apply(gui_view_t *view, ezxml_t node);

#ifdef __cplusplus
}
#endif

#endif // HML_COMPONENT_H
