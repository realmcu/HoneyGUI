/**
 * @file hml_text.c
 * @brief HML Text Component (hg_text)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_text.h"
#include <string.h>

gui_obj_t *hml_create_text(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "text");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 30);
    const char *text = hml_attr_str(node, "text", "");
    int font_size = hml_attr_int(node, "fontSize", 16);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);

    gui_text_t *txt = gui_text_create(parent, id, x, y, w, h);
    gui_text_set(txt, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);

    return (gui_obj_t *)txt;
}
