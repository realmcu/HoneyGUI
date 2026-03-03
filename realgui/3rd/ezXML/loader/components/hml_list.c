/**
 * @file hml_list.c
 * @brief HML List Component (hg_list, hg_list_item)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_loader.h"
#include "gui_list.h"
#include "gui_win.h"
#include "gui_api.h"
#include <string.h>

// Forward declaration
void hml_parse_children(gui_obj_t *parent, ezxml_t node);

gui_obj_t *hml_create_list(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "list");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    // Parse list-specific attributes
    int item_height = hml_attr_int(node, "itemHeight", 50);
    int space = hml_attr_int(node, "space", 0);
    const char *direction = hml_attr_str(node, "direction", "VERTICAL");
    const char *create_bar = hml_attr_str(node, "createBar", "false");

    LIST_DIR dir = (strcmp(direction, "HORIZONTAL") == 0) ? HORIZONTAL : VERTICAL;
    bool bar = (strcmp(create_bar, "true") == 0);

    // Create list without callback (children will be added manually)
    gui_list_t *list = gui_list_create(parent, id, x, y, w, h,
                                       item_height, space, dir,
                                       NULL, NULL, bar);

    // Parse style
    const char *style_str = hml_attr_str(node, "style", "LIST_CLASSIC");
    LIST_STYLE style = LIST_CLASSIC;
    if (strcmp(style_str, "LIST_CARD") == 0) { style = LIST_CARD; }
    else if (strcmp(style_str, "LIST_CIRCLE") == 0) { style = LIST_CIRCLE; }
    else if (strcmp(style_str, "LIST_ZOOM") == 0) { style = LIST_ZOOM; }
    else if (strcmp(style_str, "LIST_FADE") == 0) { style = LIST_FADE; }
    else if (strcmp(style_str, "LIST_FAN") == 0) { style = LIST_FAN; }
    else if (strcmp(style_str, "LIST_HELIX") == 0) { style = LIST_HELIX; }
    else if (strcmp(style_str, "LIST_CURL") == 0) { style = LIST_CURL; }
    gui_list_set_style(list, style);

    // Parse optional attributes
    const char *auto_align = hml_attr_str(node, "autoAlign", "false");
    if (strcmp(auto_align, "true") == 0)
    {
        gui_list_set_auto_align(list, true);
    }

    const char *inertia = hml_attr_str(node, "inertia", "true");
    if (strcmp(inertia, "false") == 0)
    {
        gui_list_set_inertia(list, false);
    }

    const char *loop = hml_attr_str(node, "loop", "false");
    if (strcmp(loop, "true") == 0)
    {
        list->loop = 1;
    }

    return (gui_obj_t *)list;
}

gui_obj_t *hml_create_list_item(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "list_item");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    // Create a window as list item container
    gui_win_t *item = gui_win_create(parent, id, x, y, w, h);

    return (gui_obj_t *)item;
}
