/**
 * @file hml_canvas.c
 * @brief HML Rect Component (hg_rect, hg_canvas)
 *
 * Attribute mapping follows the designer's RectGenerator.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_json.h"
#include "gui_rect.h"
#include <string.h>

static gui_rect_gradient_dir_t hml_rect_gradient_dir(const char *dir)
{
    if (dir && !strcmp(dir, "vertical")) { return RECT_GRADIENT_VERTICAL; }
    if (dir && !strcmp(dir, "diagonal_tl_br")) { return RECT_GRADIENT_DIAGONAL_TL_BR; }
    if (dir && !strcmp(dir, "diagonal_tr_bl")) { return RECT_GRADIENT_DIAGONAL_TR_BL; }
    return RECT_GRADIENT_HORIZONTAL;
}

gui_obj_t *hml_create_canvas(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "rect");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    int radius = hml_attr_int(node, "borderRadius", 0);
    int max_radius = (w < h ? w : h) / 2;
    if (radius > max_radius) { radius = max_radius; }

    // fillColor (designer), color (older HML)
    const char *fill = ezxml_attr(node, "fillColor");
    if (!fill) { fill = ezxml_attr(node, "color"); }
    uint32_t argb = hml_parse_argb(fill, 0xFFFFFFFF);
    int opacity = hml_attr_int(node, "opacity", 255);
    if (opacity >= 0 && opacity < 255)
    {
        argb = (argb & 0x00FFFFFFu) | ((uint32_t)opacity << 24);
    }
    gui_color_t color;
    color.color.argb_full = argb;

    gui_rounded_rect_t *rect = gui_rect_create(parent, id, x, y, w, h, radius, color);
    if (!rect) { return NULL; }

    if (hml_attr_bool(node, "useGradient", false))
    {
        hml_json_t *stops = hml_json_parse(ezxml_attr(node, "gradientStops"));
        if (hml_json_size(stops) >= 2)
        {
            gui_rect_set_linear_gradient(rect, hml_rect_gradient_dir(ezxml_attr(node, "gradientDirection")));
            for (const hml_json_t *s = stops->child; s; s = s->next)
            {
                gui_color_t c;
                c.color.argb_full = hml_parse_argb(hml_json_str(s, "color", NULL), 0xFFFFFFFF);
                gui_rect_add_gradient_stop(rect, hml_json_float(s, "position", 0.0f), c);
            }
        }
        hml_json_free(stops);
    }

    return (gui_obj_t *)rect;
}
