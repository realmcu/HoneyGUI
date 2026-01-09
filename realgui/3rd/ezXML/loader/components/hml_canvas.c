/**
 * @file hml_canvas.c
 * @brief HML Canvas Component (hg_canvas, hg_rect)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_rect.h"

gui_obj_t *hml_create_canvas(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "rect");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);

    gui_rounded_rect_t *rect = gui_rect_create(parent, id, x, y, w, h, 0, color);
    return (gui_obj_t *)rect;
}
