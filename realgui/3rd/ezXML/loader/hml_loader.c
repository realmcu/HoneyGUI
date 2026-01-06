/**
 * @file hml_loader.c
 * @brief HML Loader Implementation
 */

#include "hml_loader.h"
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
 *                              View Scan
 *============================================================================*/

static void hml_view_on_switch_in(gui_view_t *view)
{
    (void)view;
}

static void hml_scan_node_for_views(ezxml_t node, int *count)
{
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        if (strcmp(child->name, "hg_view") == 0)
        {
            const char *id = ezxml_attr(child, "id");
            if (id)
            {
                size_t len = strlen(id) + 1;
                char *name_copy = gui_malloc(len);
                gui_view_descriptor_t *desc = gui_malloc(sizeof(gui_view_descriptor_t));
                gui_view_t **pView = gui_malloc(sizeof(gui_view_t *));
                if (desc && name_copy && pView)
                {
                    memcpy(name_copy, id, len);
                    *pView = NULL;
                    memset(desc, 0, sizeof(gui_view_descriptor_t));
                    desc->name = name_copy;
                    desc->pView = pView;
                    desc->on_switch_in = hml_view_on_switch_in;
                    gui_view_descriptor_register(desc);
                    gui_log("[HML] Register view: %s\n", id);
                    (*count)++;
                }
            }
        }
        hml_scan_node_for_views(child, count);
    }
}

static int hml_scan_file(const char *path, int *count)
{
    gui_vfs_file_t *f = gui_vfs_open(path, GUI_VFS_READ);
    if (!f) { return -1; }

    gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
    int size = gui_vfs_tell(f);
    gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);

    if (size <= 0) { gui_vfs_close(f); return -1; }

    char *content = (char *)gui_malloc(size);
    if (!content) { gui_vfs_close(f); return -1; }

    gui_vfs_read(f, content, size);
    gui_vfs_close(f);

    ezxml_t root = ezxml_parse_str(content, size);
    if (root)
    {
        hml_scan_node_for_views(root, count);
        ezxml_free(root);
    }

    gui_free(content);
    return 0;
}

int hml_scan_views(const char *dir_path)
{
    if (!dir_path) { return -1; }

    void *dir = gui_vfs_opendir(dir_path);
    if (!dir)
    {
        gui_log("[HML] Failed to open dir: %s\n", dir_path);
        return -1;
    }

    int count = 0;
    gui_vfs_stat_t stat;
    char path[512];

    while (gui_vfs_readdir(dir, &stat) == 0)
    {
        if (stat.name[0] == '.') { continue; }

        snprintf(path, sizeof(path), "%s/%s", dir_path, stat.name);

        if (stat.type == GUI_VFS_TYPE_FILE)
        {
            const char *ext = strrchr(stat.name, '.');
            if (ext && (strcmp(ext, ".xml") == 0 || strcmp(ext, ".hml") == 0))
            {
                gui_log("[HML] Scan file: %s\n", path);
                hml_scan_file(path, &count);
            }
        }
    }

    gui_vfs_closedir(dir);
    gui_log("[HML] Total views registered: %d\n", count);
    return count;
}

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
 *                              Component Registry
 *============================================================================*/

const hml_component_entry_t *hml_get_component_registry(void)
{
    static const hml_component_entry_t registry[] =
    {
        {"hg_view",     hml_create_view},
        {"hg_window",   hml_create_view},
        {"hg_win",      hml_create_view},
        {"hg_image",    hml_create_image},
        {"hg_img",      hml_create_image},
        {"hg_text",     hml_create_text},
        {"hg_canvas",   hml_create_canvas},
        {"hg_rect",     hml_create_canvas},
        {"hg_button",   hml_create_button},
        {"hg_video",    hml_create_video},
        {NULL, NULL}
    };
    return registry;
}

/*============================================================================*
 *                              Parser
 *============================================================================*/

static void hml_parse_children(gui_obj_t *parent, ezxml_t node)
{
    const hml_component_entry_t *registry = hml_get_component_registry();

    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        bool matched = false;
        for (int i = 0; registry[i].tag; i++)
        {
            if (strcmp(child->name, registry[i].tag) == 0)
            {
                gui_log("[HML] Create <%s>\n", child->name);
                gui_obj_t *obj = registry[i].creator(parent, child);
                if (obj)
                {
                    hml_bind_events(obj, child);
                    hml_parse_children(obj, child);
                }
                matched = true;
                break;
            }
        }
        // Skip wrapper nodes (hml, meta, view, etc.), parse their children
        if (!matched)
        {
            gui_log("[HML] Skip <%s>\n", child->name);
            hml_parse_children(parent, child);
        }
    }
}

/*============================================================================*
 *                              Public API
 *============================================================================*/

gui_obj_t *hml_load_mem(gui_obj_t *parent, char *hml_str, size_t len)
{
    if (!hml_str || len == 0) { return NULL; }

    ezxml_t root = ezxml_parse_str(hml_str, len);
    if (!root) { return NULL; }

    const hml_component_entry_t *registry = hml_get_component_registry();
    gui_obj_t *result = NULL;

    // Try to create root component
    for (int i = 0; registry[i].tag; i++)
    {
        if (strcmp(root->name, registry[i].tag) == 0)
        {
            result = registry[i].creator(parent, root);
            if (result)
            {
                hml_bind_events(result, root);
                hml_parse_children(result, root);
            }
            break;
        }
    }

    // If root is not a component, parse children directly
    if (!result)
    {
        hml_parse_children(parent, root);
        result = parent;
    }

    ezxml_free(root);
    return result;
}

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

    gui_obj_t *result = hml_load_mem(parent, content, size);
    gui_free(content);

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
