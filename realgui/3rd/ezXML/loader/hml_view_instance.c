/**
 * @file hml_view_instance.c
 * @brief HML View Instance Implementation
 *
 * Every hg_view of every scanned file becomes a view descriptor. Its
 * on_switch_in creates the view's components from the resident XML tree, the
 * same way the designer's generated <view>_switch_in() does.
 */

#include "hml_view_instance.h"
#include "hml_loader.h"
#include "hml_component.h"
#include "hml_runtime.h"
#include "hml_event.h"
#include "hml_utils.h"
#include "ezxml.h"
#include "gui_vfs.h"
#include "gui_api.h"
#include "gui_view.h"
#include "gui_fb.h"
#include "gui_message.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*============================================================================*
 *                              View Data Storage
 *============================================================================*/

/*
 * One allocation per view: the view manager keeps a pointer to desc, so an
 * instance must never move. Registered descriptors cannot be unregistered,
 * so instances live for the whole program (see hml_cleanup).
 */
typedef struct hml_view_instance
{
    struct hml_view_instance *next;
    gui_view_descriptor_t desc;   // registered with the view manager
    gui_view_t *view;             // desc.pView points here
    char *name;                   // owned copy, desc.name points here
    ezxml_t node;                 // hg_view node inside a resident document
} hml_view_instance_t;

/* One parsed HML file, kept resident */
typedef struct hml_doc
{
    struct hml_doc *next;
    ezxml_t root;
    char *text;                   // parse buffer, referenced by the tree
    char *path;                   // file it was read from
    uint32_t hash;                // content hash, for change detection
    bool stale;                   // replaced by a reload, kept for live objects
} hml_doc_t;

static hml_view_instance_t *hml_views = NULL;
static hml_doc_t *hml_docs = NULL;
static const char *hml_entry_view_name = NULL;
static char hml_scan_dir[128];

static uint32_t hml_hash(const char *s, size_t len)
{
    uint32_t h = 2166136261u;   // FNV-1a
    for (size_t i = 0; i < len; i++)
    {
        h = (h ^ (uint8_t)s[i]) * 16777619u;
    }
    return h;
}

static hml_view_instance_t *hml_find_view_instance(const char *name)
{
    for (hml_view_instance_t *v = hml_views; v; v = v->next)
    {
        if (strcmp(v->name, name) == 0) { return v; }
    }
    return NULL;
}

/*============================================================================*
 *                              View Switch Callbacks
 *============================================================================*/

static void hml_view_on_switch_in(gui_view_t *view)
{
    if (!view || !view->descriptor) { return; }

    hml_view_instance_t *inst = hml_find_view_instance(view->descriptor->name);
    if (!inst || !inst->node)
    {
        gui_log("[HML] switch_in: no HML for view %s\n", view->descriptor->name);
        return;
    }

    hml_view_apply(view, inst->node);
    // View timers, events and focus are bound before the children, as exported
    hml_component_finish((gui_obj_t *)view, inst->node, true);
    hml_parse_children((gui_obj_t *)view, inst->node);
}

static void hml_view_on_switch_out(gui_view_t *view)
{
    // Component entries are released by their destroy sentinels
    GUI_UNUSED(view);
}

/*============================================================================*
 *                              View Scanner
 *============================================================================*/

static bool hml_register_view(ezxml_t node)
{
    const char *id = ezxml_attr(node, "id");
    if (!id || !id[0]) { return false; }

    hml_view_instance_t *inst = hml_find_view_instance(id);
    if (inst)
    {
        // Same id scanned again: point the existing descriptor at the new node
        inst->node = node;
    }
    else
    {
        if (gui_view_descriptor_get(id))
        {
            gui_log("[HML] View %s already registered by C code, HML one ignored\n", id);
            return false;
        }
        size_t len = strlen(id);
        inst = gui_malloc(sizeof(hml_view_instance_t));
        char *name = gui_malloc(len + 1);
        if (!inst || !name)
        {
            gui_log("[HML] Out of memory, drop view %s\n", id);
            if (inst) { gui_free(inst); }
            if (name) { gui_free(name); }
            return false;
        }
        memset(inst, 0, sizeof(hml_view_instance_t));
        memcpy(name, id, len + 1);
        inst->name = name;
        inst->node = node;
        inst->desc.name = inst->name;
        inst->desc.pView = &inst->view;
        inst->desc.on_switch_in = hml_view_on_switch_in;
        inst->desc.on_switch_out = hml_view_on_switch_out;
        inst->desc.keep = hml_attr_bool(node, "residentMemory", false);
        gui_view_descriptor_register(&inst->desc);
        inst->next = hml_views;
        hml_views = inst;
    }

    bool is_entry = hml_attr_bool(node, "entry", false);
    gui_log("[HML] Register view: %s%s\n", id, is_entry ? " (entry)" : "");
    if (is_entry && !hml_entry_view_name)
    {
        hml_entry_view_name = inst->name;
    }
    return true;
}

static void hml_scan_node_for_views(ezxml_t node, int *count)
{
    for (ezxml_t child = node->child; child; child = child->ordered)
    {
        if (strcmp(child->name, "hg_view") == 0)
        {
            if (hml_register_view(child)) { (*count)++; }
            continue;   // views do not nest
        }
        hml_scan_node_for_views(child, count);
    }
}

static hml_doc_t *hml_find_doc(const char *path)
{
    for (hml_doc_t *d = hml_docs; d; d = d->next)
    {
        if (!d->stale && strcmp(d->path, path) == 0) { return d; }
    }
    return NULL;
}

/* Parse one file into a new resident document and register its views */
static int hml_load_file(const char *path, char *content, size_t size, uint32_t hash, int *count)
{
    hml_doc_t *doc = gui_malloc(sizeof(hml_doc_t));
    char *path_copy = doc ? gui_malloc(strlen(path) + 1) : NULL;
    // ezxml keeps pointers into content; the document stays resident
    ezxml_t root = path_copy ? ezxml_parse_str(content, size) : NULL;
    if (!root)
    {
        gui_log("[HML] Failed to parse %s\n", path);
        if (path_copy) { gui_free(path_copy); }
        if (doc) { gui_free(doc); }
        gui_free(content);
        return -1;
    }
    if (ezxml_error(root)[0])
    {
        gui_log("[HML] %s: %s\n", path, ezxml_error(root));
    }
    strcpy(path_copy, path);
    memset(doc, 0, sizeof(hml_doc_t));
    doc->root = root;
    doc->text = content;
    doc->path = path_copy;
    doc->hash = hash;
    doc->next = hml_docs;
    hml_docs = doc;
    hml_scan_node_for_views(root, count);
    return 0;
}

static int hml_scan_file(const char *path, int *count)
{
    size_t size = 0;
    char *content = hml_read_file(path, &size);
    if (!content)
    {
        gui_log("[HML] Failed to read %s\n", path);
        return -1;
    }
    // Hash before parsing: ezxml modifies the buffer in place
    return hml_load_file(path, content, size, hml_hash(content, size), count);
}

/*
 * Visit every .hml / .xml file of a directory.
 * cb returns the number of views it registered (or 0).
 */
static int hml_for_each_file(const char *dir_path, int (*cb)(const char *path, void *arg),
                             void *arg)
{
    gui_vfs_dir_t *dir = gui_vfs_opendir(dir_path);
    if (!dir)
    {
        gui_log("[HML] Failed to open dir: %s\n", dir_path);
        return -1;
    }

    int total = 0;
    gui_vfs_stat_t stat;
    char path[256];
    while (gui_vfs_readdir(dir, &stat) == 0)
    {
        if (stat.name[0] == '.') { continue; }
        if (stat.type != GUI_VFS_TYPE_FILE) { continue; }

        const char *ext = strrchr(stat.name, '.');
        if (ext && (strcmp(ext, ".xml") == 0 || strcmp(ext, ".hml") == 0))
        {
            snprintf(path, sizeof(path), "%s/%s", dir_path, stat.name);
            total += cb(path, arg);
        }
    }
    gui_vfs_closedir(dir);
    return total;
}

static int hml_scan_cb(const char *path, void *arg)
{
    GUI_UNUSED(arg);
    int count = 0;
    gui_log("[HML] Scan file: %s\n", path);
    hml_scan_file(path, &count);
    return count;
}

int hml_scan_views(const char *dir_path)
{
    if (!dir_path) { return -1; }

    strncpy(hml_scan_dir, dir_path, sizeof(hml_scan_dir) - 1);
    hml_scan_dir[sizeof(hml_scan_dir) - 1] = '\0';

    int count = hml_for_each_file(dir_path, hml_scan_cb, NULL);
    if (count < 0) { return -1; }
    gui_log("[HML] Total views registered: %d\n", count);

    if (hml_entry_view_name)
    {
        gui_log("[HML] Creating entry view: %s\n", hml_entry_view_name);
        gui_view_create(gui_obj_get_root(), hml_entry_view_name, 0, 0, 0, 0);
    }
    else if (count > 0)
    {
        gui_log("[HML] No view has entry=\"true\"\n");
    }

    return count;
}

const char *hml_entry_view(void)
{
    return hml_entry_view_name;
}

/*============================================================================*
 *                              Hot reload
 *============================================================================*/

typedef struct
{
    bool force;
    int changed;
} hml_reload_ctx_t;

/* Re-read one file; reload it when its content changed (or always if forced) */
static int hml_reload_cb(const char *path, void *arg)
{
    hml_reload_ctx_t *ctx = (hml_reload_ctx_t *)arg;
    size_t size = 0;
    char *content = hml_read_file(path, &size);
    if (!content) { return 0; }

    uint32_t hash = hml_hash(content, size);
    hml_doc_t *old = hml_find_doc(path);
    if (old && old->hash == hash && !ctx->force)
    {
        gui_free(content);
        return 0;
    }

    // Old tree stays allocated: hidden resident views may still use it
    if (old) { old->stale = true; }
    int count = 0;
    if (hml_load_file(path, content, size, hash, &count) == 0)
    {
        gui_log("[HML] Reloaded %s\n", path);
        ctx->changed++;
    }
    else if (old)
    {
        old->stale = false;   // keep the last good version
    }
    return count;
}

/* Runs on the GUI server, outside the frame walk */
static void hml_rebuild_current(void *msg)
{
    GUI_UNUSED(msg);
    gui_view_t *cur = gui_view_get_current();
    if (!cur || !cur->descriptor) { return; }
    if (gui_view_get_next())
    {
        // Switching: the next view is built from the new HML anyway
        return;
    }

    // Only views that come from HML can be rebuilt
    hml_view_instance_t *inst = hml_find_view_instance(cur->descriptor->name);
    if (!inst || &inst->desc != cur->descriptor) { return; }

    gui_obj_t *parent = GUI_BASE(cur)->parent;
    const char *name = inst->name;
    gui_obj_tree_free(cur);   // on_switch_out, clears the current view
    gui_view_create(parent, name, 0, 0, 0, 0);
    gui_fb_change();
}

int hml_reload(bool force)
{
    if (!hml_scan_dir[0]) { return 0; }

    hml_reload_ctx_t ctx = { .force = force, .changed = 0 };
    hml_for_each_file(hml_scan_dir, hml_reload_cb, &ctx);
    if (ctx.changed == 0) { return 0; }

    gui_msg_t msg = { .event = GUI_EVENT_USER_DEFINE, .cb = hml_rebuild_current, .payload = NULL };
    gui_send_msg_to_server(&msg);
    return ctx.changed;
}

/*============================================================================*
 *                              Watch (polling)
 *============================================================================*/

static gui_obj_t *hml_watch_obj = NULL;

static void hml_watch_cb(void *obj)
{
    GUI_UNUSED(obj);
    hml_reload(false);
}

void hml_watch(bool enable, uint32_t interval_ms)
{
    if (!hml_watch_obj)
    {
        if (!enable) { return; }
        // Hidden holder under the root: its timer still runs, nothing is drawn
        hml_watch_obj = gui_obj_create(gui_obj_get_root(), "_hml_watch", 0, 0, 0, 0);
        if (!hml_watch_obj) { return; }
        hml_watch_obj->hidden = true;
    }
    if (enable)
    {
        gui_obj_create_timer(hml_watch_obj, interval_ms ? interval_ms : 1000, true, hml_watch_cb);
        gui_obj_start_timer(hml_watch_obj);
        gui_log("[HML] Watching %s every %u ms\n", hml_scan_dir,
                (unsigned)(interval_ms ? interval_ms : 1000));
    }
    else if (hml_watch_obj->timer)
    {
        gui_obj_stop_timer(hml_watch_obj);
        gui_log("[HML] Watch off\n");
    }
}

bool hml_watch_enabled(void)
{
    return hml_watch_obj && hml_watch_obj->timer && hml_watch_obj->timer->expire_time != 0xFFFFFFFFu;
}

void hml_cleanup(void)
{
    hml_runtime_reset();
    // Descriptors stay registered; they only lose their HML until rescanned
    for (hml_view_instance_t *v = hml_views; v; v = v->next)
    {
        v->node = NULL;
    }
    while (hml_docs)
    {
        hml_doc_t *next = hml_docs->next;
        ezxml_free(hml_docs->root);   // does not own a parse_str buffer
        gui_free(hml_docs->text);
        gui_free(hml_docs->path);
        gui_free(hml_docs);
        hml_docs = next;
    }
    hml_entry_view_name = NULL;
    hml_strpool_clear();
}
