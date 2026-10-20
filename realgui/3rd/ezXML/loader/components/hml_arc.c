/**
 * @file hml_arc.c
 * @brief HML Arc Component (hg_arc)
 *
 * Attribute mapping follows the designer's ArcGenerator: x/y/width/height is
 * the bounding box, gui_arc_create() takes the center.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_json.h"
#include "gui_arc.h"
#include "gui_api.h"
#include <string.h>

gui_obj_t *hml_create_arc(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "arc");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 80);
    int h = hml_attr_height(node, 80);
    int radius = hml_attr_int(node, "radius", 40);
    float start_angle = (float)hml_attr_int(node, "startAngle", 0);
    float end_angle = (float)hml_attr_int(node, "endAngle", 270);
    float stroke = (float)hml_attr_int(node, "strokeWidth", 8);

    uint32_t argb = hml_parse_argb(ezxml_attr(node, "color"), 0xFFFFFFFF);
    int opacity = hml_attr_int(node, "opacity", 255);
    if (opacity >= 0 && opacity < 255)
    {
        argb = (argb & 0x00FFFFFFu) | ((uint32_t)opacity << 24);
    }
    gui_color_t color;
    color.color.argb_full = argb;

    gui_arc_t *arc = gui_arc_create(parent, id, x + w / 2, y + h / 2, radius,
                                    start_angle, end_angle, stroke, color);
    if (!arc) { return NULL; }

    if (hml_attr_bool(node, "useGradient", false))
    {
        hml_json_t *stops = hml_json_parse(ezxml_attr(node, "gradientStops"));
        if (hml_json_size(stops) >= 2)
        {
            gui_arc_set_angular_gradient(arc, (float)hml_attr_int(node, "gradientStartAngle", (int)start_angle),
                                         (float)hml_attr_int(node, "gradientEndAngle", 360));
            for (const hml_json_t *s = stops->child; s; s = s->next)
            {
                gui_color_t c;
                c.color.argb_full = hml_parse_argb(hml_json_str(s, "color", NULL), 0xFFFFFFFF);
                gui_arc_add_gradient_stop(arc, hml_json_float(s, "position", 0.0f), c);
            }
        }
        hml_json_free(stops);
    }

    return (gui_obj_t *)arc;
}
