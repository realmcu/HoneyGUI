/**
 * @file hml_loader.c
 * @brief HML Loader Implementation
 */

#include "hml_loader.h"
#include "hml_view_instance.h"
#include "hml_component.h"
#include "hml_utils.h"
#include "ezxml.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include "gui_view.h"
#include <string.h>
#include <stdlib.h>

#define HML_MAX_CALLBACKS 64


/*============================================================================*
 *                              Callback Registry
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

/*============================================================================*
 *                              Event Binding
 *============================================================================*/

static void hml_bind_events(gui_obj_t *obj, ezxml_t node)
{
    const char *on_click = ezxml_attr(node, "onClick");
    if (on_click)
    {
        hml_callback_t func = hml_find_callback(on_click);
        if (func)
        {
            gui_obj_add_event_cb(obj, (gui_event_cb_t)func, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }

    const char *on_long_press = ezxml_attr(node, "onLongPress");
    if (on_long_press)
    {
        hml_callback_t func = hml_find_callback(on_long_press);
        if (func)
        {
            gui_obj_add_event_cb(obj, (gui_event_cb_t)func, GUI_EVENT_TOUCH_LONG, NULL);
        }
    }
}

/*============================================================================*
 *                              Parser
 *============================================================================*/

static gui_obj_t *hml_create_component(gui_obj_t *parent, ezxml_t node)
{
    const char *tag = node->name;

    if (strcmp(tag, "hg_view") == 0 || strcmp(tag, "hg_window") == 0 || strcmp(tag, "hg_win") == 0)
    {
        return hml_create_view(parent, node);
    }
    else if (strcmp(tag, "hg_image") == 0)
    {
        return hml_create_image(parent, node);
    }
    else if (strcmp(tag, "hg_text") == 0)
    {
        return hml_create_text(parent, node);
    }
    else if (strcmp(tag, "hg_canvas") == 0 || strcmp(tag, "hg_rect") == 0)
    {
        return hml_create_canvas(parent, node);
    }
    else if (strcmp(tag, "hg_button") == 0)
    {
        return hml_create_button(parent, node);
    }
    else if (strcmp(tag, "hg_video") == 0)
    {
        return hml_create_video(parent, node);
    }

    return NULL;
}

void hml_parse_children(gui_obj_t *parent, ezxml_t node)
{
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        gui_obj_t *obj = hml_create_component(parent, child);
        if (obj)
        {
            gui_log("[HML] Create <%s>\n", child->name);
            hml_bind_events(obj, child);
            hml_parse_children(obj, child);
        }
        else
        {
            gui_log("[HML] Skip <%s>\n", child->name);
            hml_parse_children(parent, child);
        }
    }
}

/*============================================================================*
 *                              Public API
 *============================================================================*/

gui_obj_t *hml_load(gui_obj_t *parent, const char *path)
{
    if (!path) { return NULL; }

    gui_vfs_file_t *f = gui_vfs_open(path, GUI_VFS_READ);
    if (!f)
    {
        gui_log("hml_load: failed to load '%s'\n", path);
        return NULL;
    }

    gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
    int size = gui_vfs_tell(f);
    gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);

    if (size <= 0) { gui_vfs_close(f); return NULL; }

    char *content = (char *)gui_malloc(size);
    if (!content) { gui_vfs_close(f); return NULL; }

    gui_vfs_read(f, content, size);
    gui_vfs_close(f);

    // Parse XML
    ezxml_t root = ezxml_parse_str(content, size);
    gui_free(content);

    if (!root) { return NULL; }

    // Create root component
    gui_obj_t *result = hml_create_component(parent, root);
    if (result)
    {
        hml_bind_events(result, root);
        hml_parse_children(result, root);
    }
    else
    {
        hml_parse_children(parent, root);
        result = parent;
    }

    ezxml_free(root);
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
