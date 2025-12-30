/**
 * @file hml_utils.c
 * @brief HML Utility Functions Implementation
 */

#include "hml_utils.h"
#include <string.h>
#include <stdlib.h>

int hml_attr_int(ezxml_t node, const char *name, int def)
{
    const char *val = ezxml_attr(node, name);
    return val ? atoi(val) : def;
}

int hml_attr_width(ezxml_t node, int def)
{
    const char *val = ezxml_attr(node, "width");
    if (!val) { val = ezxml_attr(node, "w"); }
    return val ? atoi(val) : def;
}

int hml_attr_height(ezxml_t node, int def)
{
    const char *val = ezxml_attr(node, "height");
    if (!val) { val = ezxml_attr(node, "h"); }
    return val ? atoi(val) : def;
}

const char *hml_attr_str(ezxml_t node, const char *name, const char *def)
{
    const char *val = ezxml_attr(node, name);
    return val ? val : def;
}

gui_color_t hml_attr_color(ezxml_t node, const char *name, uint32_t def)
{
    gui_color_t color;
    const char *val = ezxml_attr(node, name);
    uint32_t argb = def;

    if (val)
    {
        if (val[0] == '#')
        {
            argb = (uint32_t)strtoul(val + 1, NULL, 16);
            if (strlen(val) == 7) { argb = 0xFF000000 | argb; }
        }
        else
        {
            argb = (uint32_t)strtoul(val, NULL, 16);
        }
    }

    color.color.argb_full = argb;
    return color;
}

char *hml_resolve_asset_path(const char *src, char *buf, size_t buf_size)
{
    if (!src || !buf) { return NULL; }

    // Convert: assets/new-tool-test.png → /new-tool-test.bin
    const char *filename = strrchr(src, '/');
    if (!filename) { filename = src; }
    else { filename++; }

    // Remove extension
    const char *dot = strrchr(filename, '.');
    size_t name_len = dot ? (size_t)(dot - filename) : strlen(filename);

    snprintf(buf, buf_size, "/hml/%.*s.bin", (int)name_len, filename);
    return buf;
}
