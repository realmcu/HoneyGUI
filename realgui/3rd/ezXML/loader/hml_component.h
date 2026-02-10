/**
 * @file hml_component.h
 * @brief HML Component Interface
 */

#ifndef HML_COMPONENT_H
#define HML_COMPONENT_H

#include "gui_api.h"
#include "gui_obj.h"
#include "ezxml.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Component creator function type
 * @param parent Parent GUI object
 * @param node XML node
 * @return Created GUI object, NULL on failure
 */
typedef gui_obj_t *(*hml_component_creator_t)(gui_obj_t *parent, ezxml_t node);

// Component creators (implemented in components/*.c)
gui_obj_t *hml_create_view(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_text(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_button(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_canvas(gui_obj_t *parent, ezxml_t node);
gui_obj_t *hml_create_video(gui_obj_t *parent, ezxml_t node);

#ifdef __cplusplus
}
#endif

#endif // HML_COMPONENT_H
