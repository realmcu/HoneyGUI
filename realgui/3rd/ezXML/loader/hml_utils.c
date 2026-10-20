/**
 * @file hml_utils.c
 * @brief HML Utility Functions Implementation
 */

#include "hml_utils.h"
#include "gui_api.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static char hml_asset_root[32] = "/hml";

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

bool hml_attr_bool(ezxml_t node, const char *name, bool def)
{
    const char *val = ezxml_attr(node, name);
    if (!val) { return def; }
    return strcmp(val, "true") == 0 || strcmp(val, "1") == 0;
}

uint32_t hml_parse_argb(const char *val, uint32_t def)
{
    if (!val || !val[0]) { return def; }

    const char *hex = val;
    if (hex[0] == '#') { hex++; }
    else if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) { hex += 2; }

    size_t len = strlen(hex);
    uint32_t v = (uint32_t)strtoul(hex, NULL, 16);
    if (len == 6) { return 0xFF000000u | v; }
    if (len == 3)
    {
        uint32_t r = (v >> 8) & 0xF, g = (v >> 4) & 0xF, b = v & 0xF;
        return 0xFF000000u | ((r * 0x11u) << 16) | ((g * 0x11u) << 8) | (b * 0x11u);
    }
    return v;
}

gui_color_t hml_attr_color(ezxml_t node, const char *name, uint32_t def)
{
    gui_color_t color;
    color.color.argb_full = hml_parse_argb(ezxml_attr(node, name), def);
    return color;
}

void hml_set_asset_root(const char *root)
{
    if (!root) { root = ""; }
    // "/" means "mounted at the VFS root": paths then start directly with '/'
    if (strcmp(root, "/") == 0) { root = ""; }
    strncpy(hml_asset_root, root, sizeof(hml_asset_root) - 1);
    hml_asset_root[sizeof(hml_asset_root) - 1] = '\0';
}

const char *hml_get_asset_root(void)
{
    return hml_asset_root;
}

static bool hml_is_image_ext(const char *ext)
{
    static const char *const exts[] = { ".png", ".jpg", ".jpeg", ".bmp", ".tif", ".tiff", ".webp" };
    for (size_t i = 0; i < sizeof(exts) / sizeof(exts[0]); i++)
    {
        const char *a = ext, *b = exts[i];
        while (*a && *b && ((*a | 0x20) == *b || *a == *b)) { a++; b++; }
        if (!*a && !*b) { return true; }
    }
    return false;
}

char *hml_resolve_asset_path_ext(const char *src, const char *new_ext, char *buf, size_t buf_size)
{
    if (!src || !buf || buf_size == 0) { return NULL; }

    // Strip "assets/" and any leading slashes, keep the sub directories
    const char *rel = src;
    if (strncmp(rel, "assets/", 7) == 0) { rel += 7; }
    while (*rel == '/') { rel++; }

    const char *slash = strrchr(rel, '/');
    const char *dot = strrchr(rel, '.');
    if (dot && slash && dot < slash) { dot = NULL; }

    size_t stem_len = dot ? (size_t)(dot - rel) : strlen(rel);
    const char *ext = dot ? dot : "";
    if (new_ext) { ext = new_ext; }
    else if (dot && hml_is_image_ext(dot)) { ext = ".bin"; }

    snprintf(buf, buf_size, "%s/%.*s%s", hml_asset_root, (int)stem_len, rel, ext);
    return buf;
}

char *hml_resolve_asset_path(const char *src, char *buf, size_t buf_size)
{
    return hml_resolve_asset_path_ext(src, NULL, buf, buf_size);
}

char *hml_resolve_font_path(ezxml_t node, char *buf, size_t buf_size)
{
    const char *font_file = ezxml_attr(node, "fontFile");
    if (!font_file || !font_file[0] || !buf || buf_size == 0) { return NULL; }

    const char *base = strrchr(font_file, '/');
    base = base ? base + 1 : font_file;
    const char *dot = strrchr(base, '.');
    size_t base_len = dot ? (size_t)(dot - base) : strlen(base);

    // Directory part, without leading "assets/" and slashes
    const char *dir = font_file;
    if (strncmp(dir, "assets/", 7) == 0) { dir += 7; }
    while (*dir == '/') { dir++; }
    size_t dir_len = (dir < base) ? (size_t)(base - dir) : 0;   // keeps trailing '/'

    char name[96];
    size_t n = 0;
    for (size_t i = 0; i < base_len && n + 1 < sizeof(name); i++)
    {
        // Same as the font converter: '-' becomes '_'
        name[n++] = (base[i] == '-') ? '_' : base[i];
    }
    name[n] = '\0';

    const char *type = hml_attr_str(node, "fontType", "bitmap");
    if (strcmp(type, "vector") == 0)
    {
        snprintf(buf, buf_size, "%s/%.*s%s_vector.bin", hml_asset_root, (int)dir_len, dir, name);
    }
    else
    {
        snprintf(buf, buf_size, "%s/%.*s%s_size%d_bits%d_bitmap.bin", hml_asset_root, (int)dir_len, dir,
                 name, hml_attr_int(node, "fontSize", 16), hml_attr_int(node, "renderMode", 4));
    }
    return buf;
}

/*============================================================================*
 *                              String pool
 *============================================================================*/

typedef struct hml_str_node
{
    struct hml_str_node *next;
    char str[1];
} hml_str_node_t;

static hml_str_node_t *hml_str_head = NULL;

const char *hml_strpool_intern(const char *s)
{
    if (!s) { return NULL; }
    for (hml_str_node_t *n = hml_str_head; n; n = n->next)
    {
        if (strcmp(n->str, s) == 0) { return n->str; }
    }
    size_t len = strlen(s);
    hml_str_node_t *n = gui_malloc(sizeof(hml_str_node_t) + len);
    if (!n) { return NULL; }
    memcpy(n->str, s, len + 1);
    n->next = hml_str_head;
    hml_str_head = n;
    return n->str;
}

void hml_strpool_clear(void)
{
    while (hml_str_head)
    {
        hml_str_node_t *next = hml_str_head->next;
        gui_free(hml_str_head);
        hml_str_head = next;
    }
}
