/**
 * @file hml_view.c
 * @brief HML View Component (hg_view, hg_window, hg_win)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_view.h"

gui_obj_t *hml_create_view(gui_obj_t *parent, ezxml_t node)
{
    // Only create view with entry="true"
    const char *entry = ezxml_attr(node, "entry");
    if (!entry || strcmp(entry, "true") != 0)
    {
        return NULL;
    }

    const char *id = hml_attr_str(node, "id", "view");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    gui_view_t *view = gui_view_create(parent, id, x, y, w, h);
    return (gui_obj_t *)view;
}
