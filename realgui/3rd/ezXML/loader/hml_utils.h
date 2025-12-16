/**
 * @file hml_utils.h
 * @brief HML Utility Functions
 */

#ifndef HML_UTILS_H
#define HML_UTILS_H

#include "ezxml.h"
#include "guidef.h"

#ifdef __cplusplus
extern "C" {
#endif

// Attribute readers
int hml_attr_int(ezxml_t node, const char *name, int def);
int hml_attr_width(ezxml_t node, int def);
int hml_attr_height(ezxml_t node, int def);
const char *hml_attr_str(ezxml_t node, const char *name, const char *def);
gui_color_t hml_attr_color(ezxml_t node, const char *name, uint32_t def);

// Path resolver
char *hml_resolve_asset_path(const char *src, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif // HML_UTILS_H
