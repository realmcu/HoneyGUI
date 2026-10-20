/**
 * @file hml_view_instance.h
 * @brief HML View Instance Header
 */

#ifndef __HML_VIEW_INSTANCE_H__
#define __HML_VIEW_INSTANCE_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Scan a directory for .hml/.xml files and register their hg_view nodes
 *
 * Each file is parsed once and kept resident; a view's components are created
 * from it every time the view is switched in. The first view with
 * entry="true" is created on the root.
 * @param dir_path Directory path to scan
 * @return Number of views registered, -1 on error
 */
int hml_scan_views(const char *dir_path);

/**
 * @brief Name of the entry view found by hml_scan_views(), or NULL
 */
const char *hml_entry_view(void);

/**
 * @brief Hot reload: re-read the scanned directory and rebuild the current view
 *
 * Files whose content changed are parsed again; views get the new layout the
 * next time they are created, and the current view is rebuilt right away.
 * Only the HML is reloaded: new images / fonts must already be converted,
 * C business code needs a rebuild.
 * @param force Reload every file even if unchanged
 * @return Number of files reloaded
 */
int hml_reload(bool force);

/**
 * @brief Poll the scanned directory and hot reload on change (simulator use)
 * @param enable Start or stop watching
 * @param interval_ms Polling period, 0 for the default (1000 ms)
 */
void hml_watch(bool enable, uint32_t interval_ms);
bool hml_watch_enabled(void);

/**
 * @brief Cleanup all HML resources (call on app exit)
 *
 * Views must already be destroyed; registered descriptors stay registered.
 */
void hml_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* __HML_VIEW_INSTANCE_H__ */
