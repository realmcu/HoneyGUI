/**
 * @file hml_view.c
 * @brief HML View Component (hg_view, hg_window, hg_win)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_win.h"

gui_obj_t *hml_create_view(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "view");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    gui_win_t *win = gui_win_create(parent, id, x, y, w, h);
    return (gui_obj_t *)win;
}
