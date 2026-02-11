/**
 * @file hml_image.c
 * @brief HML Image Component (hg_image, hg_img)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_img.h"
#include "gui_api.h"
#include <stdio.h>
#include <string.h>

gui_obj_t *hml_create_image(gui_obj_t *parent, ezxml_t node)
{
    const char *id_name = hml_attr_str(node, "id", "img");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    const char *src = ezxml_attr(node, "src");

    if (!src)
    {
        gui_log("[HML] Image: Missing 'src' attribute\n");
        return NULL;
    }
    gui_log("creating image: id='%s', x=%d, y=%d, src='%s'\n", id_name, x, y, src);

    // Resolve asset path: assets/xxx.png → /hml/xxx.bin
    char bin_path[128];
    hml_resolve_asset_path(src, bin_path, sizeof(bin_path));

    gui_img_t *img = gui_img_create_from_fs(parent, "hml_image", (void *)bin_path, x, y, 0, 0);

    return (gui_obj_t *)img;
}
