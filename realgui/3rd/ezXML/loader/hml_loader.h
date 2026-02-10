/**
 * @file hml_loader.h
 * @brief HML Loader - Main Interface
 */

#ifndef HML_LOADER_H
#define HML_LOADER_H

#include "gui_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ezxml;
typedef struct ezxml *ezxml_t;

/**
 * @brief Callback function type
 */
typedef void (*hml_callback_t)(gui_obj_t *obj);

/**
 * @brief Load HML from file
 * @param parent Parent GUI object
 * @param path HML file path in romfs
 * @return Root GUI object, NULL on failure
 */
gui_obj_t *hml_load(gui_obj_t *parent, const char *path);

/**
 * @brief Parse children nodes (exported for hml_view_instance.c)
 * @param parent Parent object
 * @param node XML node
 */
void hml_parse_children(gui_obj_t *parent, ezxml_t node);

/**
 * @brief Register callback function
 * @param name Function name
 * @param func Callback function
 */
void hml_register_callback(const char *name, hml_callback_t func);

/**
 * @brief Find component by ID
 * @param root Root GUI object
 * @param id Component ID
 * @return GUI object, NULL if not found
 */
gui_obj_t *hml_get_by_id(gui_obj_t *root, const char *id);

#ifdef __cplusplus
}
#endif

#endif // HML_LOADER_H
