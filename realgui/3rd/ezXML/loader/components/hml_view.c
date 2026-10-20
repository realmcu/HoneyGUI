/**
 * @file hml_view.c
 * @brief HML View Component (hg_view)
 *
 * hg_view nodes are registered as view descriptors by hml_view_instance.c and
 * created by the view manager; this file applies their attributes when the
 * view is switched in (same order as the designer's <view>_switch_in()).
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_view.h"
#include "gui_api.h"
#include <string.h>

void hml_view_apply(gui_view_t *view, ezxml_t node)
{
    // Designer default: height / 10
    int step = hml_attr_int(node, "animateStep", hml_attr_height(node, 480) / 10);
    if (step > 0) { gui_view_set_animate_step(view, (uint16_t)step); }

    int opacity = hml_attr_int(node, "opacity", 255);
    if (opacity < 0) { opacity = 0; }
    if (opacity > 255) { opacity = 255; }
    gui_view_set_opacity(view, (uint8_t)opacity);

    if (ezxml_attr(node, "backgroundColor"))
    {
        gui_view_set_bg_color(view, hml_attr_color(node, "backgroundColor", 0xFF000000));
    }
}
