/**
 * @file hml_view_instance.c
 * @brief HML View Instance Implementation
 */

#include "hml_view_instance.h"
#include "hml_component.h"
#include "ezxml.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include "gui_view.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*============================================================================*
 *                              Forward Declarations
 *============================================================================*/
void hml_parse_children(gui_obj_t *parent, ezxml_t node);

/*============================================================================*
 *                              View Data Storage
 *============================================================================*/

typedef struct
{
    const char *view_name;
    char *xml_content;
    size_t xml_len;
    bool is_entry;  // Track if this is an entry view
} hml_view_instance_t;

#define HML_MAX_VIEWS 32
static hml_view_instance_t hml_view_instance_map[HML_MAX_VIEWS];
static int hml_view_instance_count = 0;
static const char *hml_entry_view_name = NULL;  // First entry view name

static hml_view_instance_t *hml_find_view_instance(const char *name)
{
    for (int i = 0; i < hml_view_instance_count; i++)
    {
        if (strcmp(hml_view_instance_map[i].view_name, name) == 0)
        {
            return &hml_view_instance_map[i];
        }
    }
    return NULL;
}

/*============================================================================*
 *                              View Switch Callback
 *============================================================================*/

// Helper function to recursively find hg_view by id
static ezxml_t hml_find_view_by_id(ezxml_t node, const char *target_id)
{
    if (!node) { return NULL; }

    // Check current node
    if (strcmp(node->name, "hg_view") == 0)
    {
        const char *id = ezxml_attr(node, "id");
        if (id && strcmp(id, target_id) == 0)
        {
            return node;
        }
    }

    // Search children recursively
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        ezxml_t found = hml_find_view_by_id(child, target_id);
        if (found) { return found; }
    }

    return NULL;
}

static void hml_view_on_switch_in(gui_view_t *view)
{
    gui_log("[HML] switch_in callback: view=%p\n", view);

    if (!view || !view->descriptor)
    {
        gui_log("[HML] switch_in: view or descriptor is NULL\n");
        return;
    }

    gui_log("[HML] switch_in: view name=%s\n", view->descriptor->name);

    hml_view_instance_t *data = hml_find_view_instance(view->descriptor->name);
    if (!data)
    {
        gui_log("[HML] switch_in: view data not found\n");
        return;
    }
    if (!data->xml_content)
    {
        gui_log("[HML] switch_in: xml_content is NULL\n");
        return;
    }

    gui_log("[HML] switch_in: parsing XML (len=%zu)\n", data->xml_len);

    // Copy XML content because ezxml_parse_str modifies the string
    char *xml_copy = gui_malloc(data->xml_len);
    if (!xml_copy)
    {
        gui_log("[HML] switch_in: failed to allocate memory\n");
        return;
    }
    memcpy(xml_copy, data->xml_content, data->xml_len);

    // Debug: print first 100 chars
    gui_log("[HML] switch_in: XML content start: %.100s\n", xml_copy);

    // Parse XML and create children
    ezxml_t root = ezxml_parse_str(xml_copy, data->xml_len);
    if (!root)
    {
        gui_log("[HML] switch_in: failed to parse XML\n");
        gui_free(xml_copy);
        return;
    }

    gui_log("[HML] switch_in: root node name=%s\n", root->name ? root->name : "NULL");

    // Find the view node by id - search recursively
    // XML structure: <hml> -> <view> -> <hg_view>
    ezxml_t view_node = hml_find_view_by_id(root, view->descriptor->name);

    if (!view_node)
    {
        gui_log("[HML] switch_in: view node '%s' not found\n", view->descriptor->name);
        ezxml_free(root);
        gui_free(xml_copy);
        return;
    }

    gui_log("[HML] switch_in: found view node id=%s\n", view->descriptor->name);

    // Parse view events
    ezxml_t events = ezxml_child(view_node, "events");
    if (events)
    {
        gui_log("[HML] switch_in: parsing events\n");
        for (ezxml_t event = ezxml_child(events, "event"); event; event = event->next)
        {
            const char *type = ezxml_attr(event, "type");
            if (!type) { continue; }

            ezxml_t action = ezxml_child(event, "action");
            if (!action) { continue; }

            const char *action_type = ezxml_attr(action, "type");
            if (!action_type || strcmp(action_type, "switchView") != 0) { continue; }

            const char *target = ezxml_attr(action, "target");
            const char *switch_out_str = ezxml_attr(action, "switchOutStyle");
            const char *switch_in_str = ezxml_attr(action, "switchInStyle");

            if (!target) { continue; }

            gui_log("[HML] switch_in: bind event %s -> %s\n", type, target);

            // Parse event type and styles (simplified)
            gui_event_code_t evt = GUI_EVENT_TOUCH_MOVE_LEFT;
            if (strcmp(type, "onSwipeRight") == 0) { evt = GUI_EVENT_TOUCH_MOVE_RIGHT; }
            else if (strcmp(type, "onSwipeUp") == 0) { evt = GUI_EVENT_TOUCH_MOVE_UP; }
            else if (strcmp(type, "onSwipeDown") == 0) { evt = GUI_EVENT_TOUCH_MOVE_DOWN; }

            VIEW_SWITCH_STYLE out_style = SWITCH_OUT_TO_LEFT_USE_TRANSLATION;
            VIEW_SWITCH_STYLE in_style = SWITCH_IN_FROM_RIGHT_USE_TRANSLATION;

            (void)switch_out_str;
            (void)switch_in_str;

            gui_view_switch_on_event(view, target, out_style, in_style, evt);
        }
    }

    // Create children
    gui_log("[HML] switch_in: creating children\n");
    hml_parse_children((gui_obj_t *)view, view_node);
    gui_log("[HML] switch_in: done\n");

    ezxml_free(root);
    gui_free(xml_copy);
}

/*============================================================================*
 *                              View Scanner
 *============================================================================*/

static void hml_scan_node_for_views(ezxml_t node, int *count, char *xml_content, size_t xml_len)
{
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        if (strcmp(child->name, "hg_view") == 0)
        {
            const char *id = ezxml_attr(child, "id");
            const char *entry = ezxml_attr(child, "entry");
            bool is_entry = (entry && strcmp(entry, "true") == 0);

            if (id)
            {
                size_t len = strlen(id) + 1;
                char *name_copy = gui_malloc(len);
                gui_view_descriptor_t *desc = gui_malloc(sizeof(gui_view_descriptor_t));
                gui_view_t **pView = gui_malloc(sizeof(gui_view_t *));

                if (desc && name_copy && pView && hml_view_instance_count < HML_MAX_VIEWS)
                {
                    memcpy(name_copy, id, len);
                    *pView = NULL;

                    // Store XML content in global map
                    hml_view_instance_map[hml_view_instance_count].view_name = name_copy;
                    hml_view_instance_map[hml_view_instance_count].xml_content = xml_content;
                    hml_view_instance_map[hml_view_instance_count].xml_len = xml_len;
                    hml_view_instance_map[hml_view_instance_count].is_entry = is_entry;
                    hml_view_instance_count++;

                    memset(desc, 0, sizeof(gui_view_descriptor_t));
                    desc->name = name_copy;
                    desc->pView = pView;
                    desc->on_switch_in = hml_view_on_switch_in;
                    gui_view_descriptor_register(desc);
                    gui_log("[HML] Register view: %s%s\n", id, is_entry ? " (entry)" : "");

                    // Remember the first entry view
                    if (is_entry && !hml_entry_view_name)
                    {
                        hml_entry_view_name = name_copy;
                    }

                    (*count)++;
                }
            }
        }
        hml_scan_node_for_views(child, count, xml_content, xml_len);
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

    // Allocate two copies: one for parsing, one for storage
    char *content_parse = (char *)gui_malloc(size);
    char *content_store = (char *)gui_malloc(size);
    if (!content_parse || !content_store)
    {
        if (content_parse) { gui_free(content_parse); }
        if (content_store) { gui_free(content_store); }
        gui_vfs_close(f);
        return -1;
    }

    gui_vfs_read(f, content_parse, size);
    gui_vfs_close(f);

    // Copy for storage
    memcpy(content_store, content_parse, size);

    ezxml_t root = ezxml_parse_str(content_parse, size);
    if (root)
    {
        hml_scan_node_for_views(root, count, content_store, size);
        ezxml_free(root);
    }
    else
    {
        gui_free(content_store);
    }

    gui_free(content_parse);
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

    // Auto-create the first entry view to trigger on_switch_in
    if (hml_entry_view_name)
    {
        gui_log("[HML] Creating entry view: %s\n", hml_entry_view_name);
        gui_view_create(gui_obj_get_root(), hml_entry_view_name, 0, 0, 0, 0);
    }

    return count;
}
