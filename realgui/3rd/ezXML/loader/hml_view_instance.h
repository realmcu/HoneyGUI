/**
 * @file hml_view_instance.h
 * @brief HML View Instance Header
 */

#ifndef __HML_VIEW_INSTANCE_H__
#define __HML_VIEW_INSTANCE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Scan directory for HML files and register views
 * @param dir_path Directory path to scan
 * @return Number of views registered, -1 on error
 */
int hml_scan_views(const char *dir_path);

#ifdef __cplusplus
}
#endif

#endif /* __HML_VIEW_INSTANCE_H__ */
