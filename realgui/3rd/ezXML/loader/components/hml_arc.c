/**
 * @file hml_arc.c
 * @brief HML Arc Component (hg_arc)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_arc.h"
#include "gui_api.h"
#include <string.h>
#include <stdlib.h>

gui_obj_t *hml_create_arc(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "arc");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int radius = hml_attr_int(node, "radius", 50);
    float start_angle = (float)hml_attr_int(node, "startAngle", 0);
    float end_angle = (float)hml_attr_int(node, "endAngle", 90);
    float stroke_width = (float)hml_attr_int(node, "strokeWidth", 2);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);

    gui_arc_t *arc = gui_arc_create(parent, id, x, y, radius,
                                    start_angle, end_angle, stroke_width, color);

    // Opacity support
    int opacity = hml_attr_int(node, "opacity", 255);
    if (opacity != 255)
    {
        gui_arc_set_opacity(arc, (uint8_t)opacity);
    }

    // Gradient support
    const char *use_grad = hml_attr_str(node, "useGradient", "false");
    if (strcmp(use_grad, "true") == 0)
    {
        // Get gradient end angle (default to end_angle if not specified)
        int grad_end = hml_attr_int(node, "gradientEndAngle", (int)end_angle);
        gui_arc_set_angular_gradient(arc, start_angle, (float)grad_end);

        // Parse gradient stops (simple format: [{"position":0,"color":"#FF0000"},{"position":1,"color":"#0000FF"}])
        const char *stops = ezxml_attr(node, "gradientStops");
        if (stops)
        {
            const char *ptr = stops;
            while ((ptr = strstr(ptr, "\"position\":")) != NULL)
            {
                ptr += 11; // Skip "position":
                float position = (float)atof(ptr);

                // Find color field
                const char *color_ptr = strstr(ptr, "\"color\":");
                if (color_ptr)
                {
                    color_ptr += 8; // Skip "color":
                    // Skip whitespace and quotes
                    while (*color_ptr == ' ' || *color_ptr == '\"') { color_ptr++; }

                    // Parse color (format: #RRGGBB or #AARRGGBB)
                    if (*color_ptr == '#')
                    {
                        color_ptr++;
                        uint32_t argb = (uint32_t)strtoul(color_ptr, NULL, 16);
                        // If 6 chars (RGB), add full opacity
                        if (strlen(color_ptr) >= 6 && (color_ptr[6] == '\"' || color_ptr[6] == ',' || color_ptr[6] == '}'))
                        {
                            argb = 0xFF000000 | argb;
                        }
                        gui_color_t color;
                        color.color.argb_full = argb;
                        gui_arc_add_gradient_stop(arc, position, color);
                    }
                }
            }
        }
    }

    return (gui_obj_t *)arc;
}
