/**
 * @file hml_loader.h
 * @brief HML Loader - Main Interface
 *
 * Typical use (resources produced by HoneyGUI Designer, UI parsed at run time):
 * @code
 *   gui_vfs_mount_posix("/", "./assets");   // or gui_vfs_mount_romfs("/", addr, 0)
 *   hml_set_asset_root("/");
 *   my_app_register_functions();            // HML_REGISTER_FUNCTION(...)
 *   hml_scan_views("/ui");                  // registers views, opens the entry view
 * @endcode
 */

#ifndef HML_LOADER_H
#define HML_LOADER_H

#include "gui_obj.h"
#include "ezxml.h"
#include "hml_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Legacy callback type for onClick / onLongPress attributes
 */
typedef void (*hml_callback_t)(gui_obj_t *obj);

/**
 * @brief Load HML from file and create its components under parent
 * @param parent Parent GUI object
 * @param path HML file path
 * @return Root GUI object, NULL on failure
 */
gui_obj_t *hml_load(gui_obj_t *parent, const char *path);

/**
 * @brief Create the components described by the children of node
 * @param parent Parent object
 * @param node XML node; must stay alive while the components exist
 */
void hml_parse_children(gui_obj_t *parent, ezxml_t node);

/**
 * @brief Register a legacy onClick / onLongPress attribute callback
 */
void hml_register_callback(const char *name, hml_callback_t func);

/**
 * @brief Find component by ID under root (tree search by widget name)
 */
gui_obj_t *hml_get_by_id(gui_obj_t *root, const char *id);

/**
 * @brief Read a whole file into a NUL-terminated heap buffer
 * @param len Optional out: length without the terminator
 */
char *hml_read_file(const char *path, size_t *len);

#ifdef __cplusplus
}
#endif

#endif // HML_LOADER_H
