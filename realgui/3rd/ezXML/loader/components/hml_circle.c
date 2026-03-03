/**
 * @file hml_circle.c
 * @brief HML Circle Component (hg_circle)
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_circle.h"
#include "gui_api.h"
#include <string.h>
#include <stdlib.h>

gui_obj_t *hml_create_circle(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "circle");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int radius = hml_attr_int(node, "radius", 50);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);

    gui_circle_t *circle = gui_circle_create(parent, id, x, y, radius, color);

    // Opacity support
    int opacity = hml_attr_int(node, "opacity", 255);
    if (opacity != 255)
    {
        gui_circle_set_opacity(circle, (uint8_t)opacity);
    }

    // Gradient support
    const char *grad_type = hml_attr_str(node, "gradientType", "none");
    if (strcmp(grad_type, "radial") == 0)
    {
        gui_circle_set_radial_gradient(circle);

        // Parse gradient stops (simple format: [{"position":0,"color":"#FFFFFF"},{"position":1,"color":"#0000FF"}])
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
                        gui_circle_add_gradient_stop(circle, position, color);
                    }
                }
            }
        }
    }

    return (gui_obj_t *)circle;
}
