/**
 * @file hml_list.c
 * @brief HML List Component (hg_list with hg_list_item children)
 *
 * gui_list creates and frees its notes while scrolling. As in the designer's
 * ListGenerator, the note of index i is filled with the children of the i-th
 * hg_list_item; here they are read from the resident XML node.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "../hml_loader.h"
#include "../hml_runtime.h"
#include "gui_list.h"
#include "gui_api.h"
#include <string.h>

static ezxml_t hml_list_item_at(ezxml_t list_node, int index)
{
    for (ezxml_t c = list_node->child; c; c = c->ordered)
    {
        if (strcmp(c->name, "hg_list_item") != 0) { continue; }
        if (index-- == 0) { return c; }
    }
    return NULL;
}

static int hml_list_item_count(ezxml_t list_node)
{
    int n = 0;
    for (ezxml_t c = list_node->child; c; c = c->ordered)
    {
        if (strcmp(c->name, "hg_list_item") == 0) { n++; }
    }
    return n;
}

static void hml_list_note_design(gui_obj_t *obj, void *param)
{
    ezxml_t list_node = (ezxml_t)param;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    gui_list_t *list = (gui_list_t *)obj->parent;

    int index = note->index;
    int note_num = (int)list->note_num;
    if (note_num > 0)
    {
        // Loop mode produces negative indexes
        index %= note_num;
        if (index < 0) { index += note_num; }
    }

    ezxml_t item = hml_list_item_at(list_node, index);
    if (!item) { return; }

    hml_parse_children(obj, item);
    // A list item can carry events / timers of its own, bound on the note
    hml_component_finish(obj, item, false);
}

static LIST_STYLE hml_list_style(const char *s)
{
    static const struct
    {
        const char *name;
        LIST_STYLE style;
    } map[] =
    {
        { "LIST_CLASSIC", LIST_CLASSIC }, { "LIST_CIRCLE", LIST_CIRCLE }, { "LIST_ZOOM", LIST_ZOOM },
        { "LIST_CARD", LIST_CARD },       { "LIST_FADE", LIST_FADE },     { "LIST_FAN", LIST_FAN },
        { "LIST_HELIX", LIST_HELIX },     { "LIST_CURL", LIST_CURL },
    };
    for (size_t i = 0; s && i < sizeof(map) / sizeof(map[0]); i++)
    {
        if (!strcmp(map[i].name, s)) { return map[i].style; }
    }
    return LIST_CLASSIC;
}

gui_obj_t *hml_create_list(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "list");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 100);

    bool vertical = strcmp(hml_attr_str(node, "direction", "VERTICAL"), "HORIZONTAL") != 0;
    int note_len = vertical ? hml_attr_int(node, "itemHeight", 100) : hml_attr_int(node, "itemWidth",
                                                                                   100);
    int space = hml_attr_int(node, "space", 10);
    bool loop = hml_attr_bool(node, "loop", false);
    LIST_STYLE style = hml_list_style(ezxml_attr(node, "style"));
    int note_num = hml_attr_int(node, "noteNum", hml_list_item_count(node));

    gui_list_t *list = gui_list_create(parent, id, x, y, w, h, (uint16_t)note_len, (uint8_t)space,
                                       vertical ? VERTICAL : HORIZONTAL,
                                       hml_list_note_design, (void *)node,
                                       hml_attr_bool(node, "createBar", false));
    if (!list) { return NULL; }

    // Style and count right after creation, as the designer does
    gui_list_set_style(list, style);
    gui_list_set_note_num(list, (uint16_t)note_num);

    if (hml_attr_bool(node, "autoAlign", true)) { gui_list_set_auto_align(list, true); }
    if (!hml_attr_bool(node, "inertia", true)) { gui_list_set_inertia(list, false); }
    if (loop) { gui_list_enable_loop(list, true); }
    int offset = hml_attr_int(node, "offset", 0);
    if (offset != 0) { gui_list_set_offset(list, (int16_t)offset); }
    int out_scope = hml_attr_int(node, "outScope", 0);
    if (out_scope != 0 && !loop && style != LIST_CARD) { gui_list_set_out_scope(list, (int16_t)out_scope); }
    if (style == LIST_CARD)
    {
        gui_list_set_card_stack_location(list, (int16_t)hml_attr_int(node, "cardStackLocation", 0));
    }
    if (style == LIST_CIRCLE)
    {
        gui_list_set_circle_radius(list, (uint16_t)hml_attr_int(node, "circleRadius", vertical ? w : h));
    }
    if (hml_attr_bool(node, "enableAreaDisplay", false)) { gui_list_enable_area_display(list, true); }
    if (hml_attr_bool(node, "keepNoteAlive", false)) { gui_list_keep_note_alive(list, true); }

    return (gui_obj_t *)list;
}
