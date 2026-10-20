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
bool hml_attr_bool(ezxml_t node, const char *name, bool def);

/** Parse "#RRGGBB", "#AARRGGBB", "0xAARRGGBB" or "AARRGGBB" into ARGB8888. */
uint32_t hml_parse_argb(const char *val, uint32_t def);

/**
 * Resolve a designer asset path to the converted file in the mounted resource
 * tree, following the designer code generator:
 *   assets/resource/map/map_00.png -> <root>/resource/map/map_00.bin
 * Images get ".bin"; other files keep their extension unless new_ext is given.
 */
char *hml_resolve_asset_path(const char *src, char *buf, size_t buf_size);
char *hml_resolve_asset_path_ext(const char *src, const char *new_ext, char *buf, size_t buf_size);

/**
 * Resolve the converted font file used by a label, following the designer:
 *   fontFile="/HarmonyOS_Sans_Bold.ttf", fontSize=30, bitmap
 *   -> <root>/HarmonyOS_Sans_Bold_size30_bits4_bitmap.bin
 *   vector -> <root>/HarmonyOS_Sans_Bold_vector.bin
 */
char *hml_resolve_font_path(ezxml_t node, char *buf, size_t buf_size);

/** Mount prefix under which the HML resources live ("" when mounted at "/"). */
void hml_set_asset_root(const char *root);
const char *hml_get_asset_root(void);

/**
 * Return a pooled copy of a string that stays valid until hml_strpool_clear().
 * Widgets such as gui_text keep the pointer they are given (font path), so
 * strings passed to them must outlive the stack buffer they were built in.
 */
const char *hml_strpool_intern(const char *s);
void hml_strpool_clear(void);

#ifdef __cplusplus
}
#endif

#endif // HML_UTILS_H
