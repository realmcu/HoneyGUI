/**
 * @file hml_image.c
 * @brief HML Image Component (hg_image, hg_img)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_img.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include <stdio.h>

gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "img");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    const char *src = ezxml_attr(node, "src");

    if (!src)
    {
        gui_log("hml_image: missing 'src' attribute\n");
        return NULL;
    }

    // Resolve asset path: assets/xxx.png → /xxx.bin
    char bin_path[128];
    hml_resolve_asset_path(src, bin_path, sizeof(bin_path));

    // Load from romfs
    size_t size;
    void *img_data = gui_vfs_load_file(bin_path, &size);

    if (!img_data)
    {
        gui_log("hml_image: failed to load '%s'\n", bin_path);
        return NULL;
    }

    gui_img_t *img = gui_img_create_from_mem(parent, id, img_data, x, y, 0, 0);
    return (gui_obj_t *)img;
}
