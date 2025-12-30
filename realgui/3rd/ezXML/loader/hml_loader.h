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

/**
 * @brief Callback function type
 */
typedef void (*hml_callback_t)(gui_obj_t *obj);

/**
 * @brief Scan directory and register all hg_view descriptors
 * @param dir_path Directory path to scan (e.g., "/hml/ui")
 * @return Number of views registered, -1 on error
 */
int hml_scan_views(const char *dir_path);

/**
 * @brief Load HML from file
 * @param parent Parent GUI object
 * @param path HML file path in romfs
 * @return Root GUI object, NULL on failure
 */
gui_obj_t *hml_load(gui_obj_t *parent, const char *path);

/**
 * @brief Load HML from memory
 * @param parent Parent GUI object
 * @param hml_str HML string
 * @param len String length
 * @return Root GUI object, NULL on failure
 */
gui_obj_t *hml_load_mem(gui_obj_t *parent, char *hml_str, size_t len);

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
