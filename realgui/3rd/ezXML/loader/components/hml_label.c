/**
 * @file hml_label.c
 * @brief HML Label Component (hg_label)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_text.h"
#include "gui_api.h"
#include <string.h>

gui_obj_t *hml_create_label(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "label");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 30);
    const char *text = hml_attr_str(node, "text", "");
    int font_size = hml_attr_int(node, "fontSize", 16);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);

    // Parse text alignment
    const char *h_align = hml_attr_str(node, "hAlign", "LEFT");
    const char *v_align = hml_attr_str(node, "vAlign", "TOP");

    TEXT_MODE mode = LEFT;
    if (strcmp(h_align, "CENTER") == 0)
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_CENTER; }
        else { mode = CENTER; }
    }
    else if (strcmp(h_align, "RIGHT") == 0)
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_RIGHT; }
        else { mode = RIGHT; }
    }
    else // LEFT
    {
        if (strcmp(v_align, "MIDDLE") == 0) { mode = MID_LEFT; }
        else { mode = LEFT; }
    }

    gui_text_t *txt = gui_text_create(parent, id, x, y, w, h);
    gui_text_set(txt, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);
    gui_text_mode_set(txt, mode);

    return (gui_obj_t *)txt;
}
