/**
 * @file hml_image.c
 * @brief HML Image Component (hg_image, hg_img)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_img.h"
#include <stdio.h>
#include <string.h>

gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "img");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    const char *src = ezxml_attr(node, "src");

    if (!src)
    {
        gui_log("[HML] Image: Missing 'src' attribute\n");
        return NULL;
    }

    // Resolve asset path: assets/xxx.png → /hml/xxx.bin
    char bin_path[128];
    hml_resolve_asset_path(src, bin_path, sizeof(bin_path));

    // Allocate persistent memory for path string
    size_t path_len = strlen(bin_path) + 1;
    char *path_copy = gui_malloc(path_len);
    if (!path_copy)
    {
        gui_log("[HML] Image: Failed to allocate memory for path\n");
        return NULL;
    }
    memcpy(path_copy, bin_path, path_len);

    // Create image from file path (let gui_img handle file loading)
    gui_img_t *img = gui_img_create_from_fs(parent, id, (void *)path_copy, x, y, 0, 0);

    return (gui_obj_t *)img;
}
