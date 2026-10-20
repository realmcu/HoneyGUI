/**
 * @file hml_runtime.h
 * @brief HML runtime: function registry, object table, events and timers
 *
 * The designer can export a project as C code, or the HML can be parsed on the
 * target at run time. This runtime gives the parsed UI the same behaviour as
 * the exported C code:
 *  - every created component is recorded by its HML id, so business C code can
 *    reach it with hml_obj_get() / hml_bind() instead of generated globals;
 *  - <events> are dispatched at run time (keys, touch, messages);
 *  - "timers" are started by index, preset animations are interpreted from the
 *    JSON timeline, custom ones call a registered C function;
 *  - callFunction / custom timer callbacks are resolved by name through
 *    hml_register_function().
 */

#ifndef HML_RUNTIME_H
#define HML_RUNTIME_H

#include "gui_obj.h"
#include "gui_view.h"
#include "ezxml.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Function registry
 *============================================================================*/

/** Generic function pointer; cast back to the real signature before calling. */
typedef void (*hml_fn_t)(void);

/**
 * Register a C function under the name used in HML (functionName, callback,
 * handler, transporter, ...). Signatures expected by the runtime:
 *  - key / touch event action:  void fn(void *obj, gui_event_t *e)
 *  - onMessage action/handler:  void fn(gui_obj_t *obj, const char *topic, void *data, uint16_t len)
 *  - custom timer callback:     void fn(void *obj)
 *  - stream transporter:        stp_transport_t *fn(void)
 */
void hml_register_function(const char *name, hml_fn_t fn);
hml_fn_t hml_find_function(const char *name);

/** Register a function under its own C name. */
#define HML_REGISTER_FUNCTION(fn) hml_register_function(#fn, (hml_fn_t)(fn))

/*============================================================================*
 *                              Object table
 *============================================================================*/

/** Latest live object created for an HML id, or NULL. */
gui_obj_t *hml_obj_get(const char *id);

/**
 * Keep a C handle in sync with an HML id: *slot is set whenever a component
 * with this id is created and cleared when its view is destroyed.
 * slot must point to a pointer variable (gui_img_t *, gui_text_t *, ...).
 */
void hml_bind(const char *id, void *slot);

/** HML id of a runtime-created object, or NULL. */
const char *hml_obj_id(gui_obj_t *obj);

/*============================================================================*
 *                              Timers
 *============================================================================*/

/**
 * Start timer #index of the object's "timers" attribute. A preset animation is
 * replayed from the beginning. Returns false if there is no such timer.
 */
bool hml_timer_start(gui_obj_t *obj, int index);
bool hml_timer_start_by_id(const char *id, int index);
void hml_timer_stop(gui_obj_t *obj);
/** Rewind the preset animation clock of an object (next tick restarts it). */
void hml_preset_reset(gui_obj_t *obj);

/*============================================================================*
 *                              Loader internals
 *============================================================================*/

/** Record a freshly created component. */
void hml_runtime_add(gui_obj_t *obj, ezxml_t node);
/** HML node of a runtime-created object, or NULL. */
ezxml_t hml_runtime_node_of(gui_obj_t *obj);
/** Bind the component's enabled timers, as the exported code does at creation. */
void hml_runtime_bind_timers(gui_obj_t *obj, ezxml_t node, bool is_view);
/** Bind <events> of a component (keys, touch, messages, view switching). */
void hml_runtime_bind_events(gui_obj_t *obj, ezxml_t node, bool is_view);
/** Forget everything (unload). */
void hml_runtime_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* HML_RUNTIME_H */
