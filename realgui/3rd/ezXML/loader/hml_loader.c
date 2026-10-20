/**
 * @file hml_loader.c
 * @brief HML Loader Implementation
 */

#include "hml_loader.h"
#include "hml_view_instance.h"
#include "hml_component.h"
#include "hml_runtime.h"
#include "hml_event.h"
#include "hml_utils.h"
#include "ezxml.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include "gui_view.h"
#include "gui_obj_focus.h"
#include <string.h>
#include <stdlib.h>

#define HML_MAX_CALLBACKS   64
#define HML_MAX_CHILDREN    128

/*============================================================================*
 *                              Legacy callback registry
 *============================================================================*/

typedef struct
{
    const char *name;
    hml_callback_t func;
} hml_callback_entry_t;

static hml_callback_entry_t hml_callbacks[HML_MAX_CALLBACKS];
static int hml_callback_count = 0;

void hml_register_callback(const char *name, hml_callback_t func)
{
    if (hml_callback_count < HML_MAX_CALLBACKS && name && func)
    {
        hml_callbacks[hml_callback_count].name = name;
        hml_callbacks[hml_callback_count].func = func;
        hml_callback_count++;
    }
}

static hml_callback_t hml_find_callback(const char *name)
{
    if (!name) { return NULL; }
    for (int i = 0; i < hml_callback_count; i++)
    {
        if (strcmp(hml_callbacks[i].name, name) == 0)
        {
            return hml_callbacks[i].func;
        }
    }
    return NULL;
}

/* onClick="name" / onLongPress="name" attributes (older HML) */
static void hml_bind_attr_events(gui_obj_t *obj, ezxml_t node)
{
    const char *on_click = ezxml_attr(node, "onClick");
    hml_callback_t func = hml_find_callback(on_click);
    if (func)
    {
        gui_obj_add_event_cb(obj, (gui_event_cb_t)func, GUI_EVENT_TOUCH_CLICKED, NULL);
    }

    const char *on_long_press = ezxml_attr(node, "onLongPress");
    func = hml_find_callback(on_long_press);
    if (func)
    {
        gui_obj_add_event_cb(obj, (gui_event_cb_t)func, GUI_EVENT_TOUCH_LONG, NULL);
    }
}

/*============================================================================*
 *                              Parser
 *============================================================================*/

typedef struct
{
    const char *tag;
    hml_component_creator_t create;
} hml_tag_entry_t;

static const hml_tag_entry_t hml_tags[] =
{
    { "hg_window",     hml_create_window },
    { "hg_win",        hml_create_window },
    { "hg_image",      hml_create_image },
    { "hg_img",        hml_create_image },
    { "hg_text",       hml_create_text },
    { "hg_label",      hml_create_label },
    { "hg_canvas",     hml_create_canvas },
    { "hg_rect",       hml_create_canvas },
    { "hg_arc",        hml_create_arc },
    { "hg_circle",     hml_create_circle },
    { "hg_list",       hml_create_list },
    { "hg_button",     hml_create_button },
    { "hg_video",      hml_create_video },
    { "hg_streaming",  hml_create_streaming },
    { "hg_time_label", hml_create_time_label },
};

/* Nodes that carry data for their parent, not components */
static bool hml_is_data_node(const char *tag)
{
    return !strcmp(tag, "events") || !strcmp(tag, "event") || !strcmp(tag, "action") ||
           !strcmp(tag, "meta") || !strcmp(tag, "hg_list_item");
}

static hml_component_creator_t hml_find_creator(const char *tag)
{
    for (size_t i = 0; i < sizeof(hml_tags) / sizeof(hml_tags[0]); i++)
    {
        if (strcmp(hml_tags[i].tag, tag) == 0) { return hml_tags[i].create; }
    }
    return NULL;
}

/*
 * Common steps after a component is created, in the order of the designer's
 * exported code: properties, timers, events, focus for key events.
 */
void hml_component_finish(gui_obj_t *obj, ezxml_t node, bool is_view)
{
    hml_runtime_add(obj, node);

    if (!is_view && !hml_attr_bool(node, "visible", true))
    {
        gui_obj_hidden(obj, true);
    }

    hml_runtime_bind_timers(obj, node, is_view);
    hml_runtime_bind_events(obj, node, is_view);
    hml_bind_attr_events(obj, node);

    if (hml_has_key_events(node))
    {
        gui_obj_focus_set(obj);
    }
}

static gui_obj_t *hml_create_component(gui_obj_t *parent, ezxml_t node)
{
    hml_component_creator_t create = hml_find_creator(node->name);
    if (!create) { return NULL; }

    gui_obj_t *obj = create(parent, node);
    if (!obj)
    {
        gui_log("[HML] failed to create <%s id=%s>\n", node->name, hml_attr_str(node, "id", "?"));
        return NULL;
    }
    hml_component_finish(obj, node, false);
    return obj;
}

void hml_parse_children(gui_obj_t *parent, ezxml_t node)
{
    // Children are created in ascending zIndex, stable for equal values,
    // so later (higher) ones are drawn on top, as in the exported code
    ezxml_t kids[HML_MAX_CHILDREN];
    int n = 0;
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        if (hml_is_data_node(child->name)) { continue; }
        if (n >= HML_MAX_CHILDREN)
        {
            gui_log("[HML] too many children under %s\n", hml_attr_str(node, "id", node->name));
            break;
        }
        kids[n++] = child;
    }
    for (int i = 1; i < n; i++)
    {
        ezxml_t k = kids[i];
        int z = hml_attr_int(k, "zIndex", 0);
        int j = i - 1;
        while (j >= 0 && hml_attr_int(kids[j], "zIndex", 0) > z)
        {
            kids[j + 1] = kids[j];
            j--;
        }
        kids[j + 1] = k;
    }

    for (int i = 0; i < n; i++)
    {
        ezxml_t child = kids[i];
        gui_obj_t *obj = hml_create_component(parent, child);
        if (obj)
        {
            // A list builds its items lazily through its note_design callback
            if (strcmp(child->name, "hg_list") != 0)
            {
                hml_parse_children(obj, child);
            }
        }
        else if (!hml_find_creator(child->name))
        {
            gui_log("[HML] Skip <%s>\n", child->name);
        }
    }
}

/*============================================================================*
 *                              Public API
 *============================================================================*/

char *hml_read_file(const char *path, size_t *len)
{
    if (!path) { return NULL; }

    gui_vfs_file_t *f = gui_vfs_open(path, GUI_VFS_READ);
    if (!f) { return NULL; }

    gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
    int size = gui_vfs_tell(f);
    gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);
    if (size <= 0)
    {
        gui_vfs_close(f);
        return NULL;
    }

    char *buf = gui_malloc((size_t)size + 1);
    if (buf)
    {
        int got = gui_vfs_read(f, buf, (size_t)size);
        buf[got > 0 ? got : 0] = '\0';
        if (len) { *len = got > 0 ? (size_t)got : 0; }
    }
    gui_vfs_close(f);
    return buf;
}

gui_obj_t *hml_load(gui_obj_t *parent, const char *path)
{
    size_t size = 0;
    char *content = hml_read_file(path, &size);
    if (!content)
    {
        gui_log("hml_load: failed to load '%s'\n", path ? path : "(null)");
        return NULL;
    }

    // The parsed tree is kept for the lifetime of the program: widgets keep
    // pointers to attribute strings (text, ids) and events read it lazily
    ezxml_t root = ezxml_parse_str(content, size);
    if (!root)
    {
        gui_free(content);
        return NULL;
    }

    gui_obj_t *result = hml_create_component(parent, root);
    if (result)
    {
        hml_parse_children(result, root);
    }
    else
    {
        hml_parse_children(parent, root);
        result = parent;
    }
    return result;
}

gui_obj_t *hml_get_by_id(gui_obj_t *root, const char *id)
{
    if (!root || !id) { return NULL; }

    if (root->name && strcmp(root->name, id) == 0)
    {
        return root;
    }

    struct gui_node_list *node = NULL;
    gui_list_for_each(node, &root->child_list)
    {
        gui_obj_t *child = gui_list_entry(node, gui_obj_t, brother_list);
        gui_obj_t *found = hml_get_by_id(child, id);
        if (found) { return found; }
    }

    return NULL;
}
