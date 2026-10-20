/**
 * @file hml_runtime.c
 * @brief HML runtime: function registry, object table, events and timers
 */

#include "hml_runtime.h"
#include "hml_event.h"
#include "hml_json.h"
#include "hml_utils.h"
#include "gui_api.h"
#include "gui_img.h"
#include "gui_obj_focus.h"
#include "gui_listener.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define HML_MAX_FUNCTIONS   256
#define HML_MAX_BINDINGS    256

/*============================================================================*
 *                              Function registry
 *============================================================================*/

typedef struct
{
    const char *name;
    hml_fn_t fn;
} hml_fn_entry_t;

static hml_fn_entry_t hml_fns[HML_MAX_FUNCTIONS];
static int hml_fn_count = 0;

void hml_register_function(const char *name, hml_fn_t fn)
{
    if (!name || !fn) { return; }
    for (int i = 0; i < hml_fn_count; i++)
    {
        if (strcmp(hml_fns[i].name, name) == 0)
        {
            hml_fns[i].fn = fn;
            return;
        }
    }
    if (hml_fn_count >= HML_MAX_FUNCTIONS)
    {
        gui_log("[HML] function table full, drop %s\n", name);
        return;
    }
    hml_fns[hml_fn_count].name = name;
    hml_fns[hml_fn_count].fn = fn;
    hml_fn_count++;
}

hml_fn_t hml_find_function(const char *name)
{
    if (!name || !name[0]) { return NULL; }
    for (int i = 0; i < hml_fn_count; i++)
    {
        if (strcmp(hml_fns[i].name, name) == 0) { return hml_fns[i].fn; }
    }
    return NULL;
}

static hml_fn_t hml_require_function(const char *name)
{
    hml_fn_t fn = hml_find_function(name);
    if (!fn && name && name[0])
    {
        gui_log("[HML] function '%s' is not registered\n", name);
    }
    return fn;
}

/*============================================================================*
 *                              Object table
 *============================================================================*/

/* Parsed "timers" attribute of one component */
typedef struct
{
    hml_json_t *json;          // root array (owned)
    const hml_json_t *active;  // timer config currently driving the preset clock
    uint32_t start_ms;
    uint32_t prev_elapsed_ms;
    bool started;
    const hml_json_t *seq_action;  // last imageSequence action applied
    int seq_index;                 // and its frame, to skip reloading the same file
} hml_timer_state_t;

/* One runtime-created component */
typedef struct hml_obj_entry
{
    struct hml_obj_entry *next;
    gui_obj_t *obj;
    const char *id;            // points into the resident XML tree
    const char *tag;
    gui_obj_t *sentinel;       // child whose destroy callback unregisters the entry
    void (*list_cb)(gui_obj_t *, T_OBJ_CB_TYPE);  // saved list obj_cb, when chained
    hml_timer_state_t timers;
    ezxml_t node;
} hml_obj_entry_t;

typedef struct
{
    const char *id;
    void **slot;
} hml_binding_t;

static hml_obj_entry_t *hml_obj_head = NULL;
static hml_binding_t hml_bindings[HML_MAX_BINDINGS];
static int hml_binding_count = 0;

static void hml_update_bindings(const char *id, gui_obj_t *obj)
{
    for (int i = 0; i < hml_binding_count; i++)
    {
        if (strcmp(hml_bindings[i].id, id) == 0)
        {
            *hml_bindings[i].slot = obj;
        }
    }
}

static hml_obj_entry_t *hml_entry_of(gui_obj_t *obj)
{
    for (hml_obj_entry_t *e = hml_obj_head; e; e = e->next)
    {
        if (e->obj == obj) { return e; }
    }
    return NULL;
}

static hml_obj_entry_t *hml_entry_of_id(const char *id)
{
    // Newest first: the list is prepended, so the first hit is the latest object
    for (hml_obj_entry_t *e = hml_obj_head; e; e = e->next)
    {
        if (e->id && strcmp(e->id, id) == 0) { return e; }
    }
    return NULL;
}

static void hml_entry_free(hml_obj_entry_t *e)
{
    if (e->timers.json) { hml_json_free(e->timers.json); }
    gui_free(e);
}

/* Unlink an entry; refresh bindings to an older live object with the same id */
static void hml_entry_remove(hml_obj_entry_t *target)
{
    hml_obj_entry_t **pp = &hml_obj_head;
    while (*pp)
    {
        if (*pp == target)
        {
            *pp = target->next;
            break;
        }
        pp = &(*pp)->next;
    }
    if (target->id)
    {
        hml_obj_entry_t *older = hml_entry_of_id(target->id);
        hml_update_bindings(target->id, older ? older->obj : NULL);
    }
    hml_entry_free(target);
}

/* Destroy hook of the sentinel: the owner is being freed together with it */
static void hml_sentinel_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (cb_type != OBJ_DESTROY) { return; }
    hml_obj_entry_t *e = (hml_obj_entry_t *)obj->user_data;
    // Guard: the entry may already be gone, and freeing it must not be repeated
    for (hml_obj_entry_t *it = hml_obj_head; it; it = it->next)
    {
        if (it == e)
        {
            hml_entry_remove(e);
            return;
        }
    }
}

/*
 * List destroy hook: the sentinel lives in the list's parent, so an entry only
 * went away when that parent did. gui_obj_tree_free(list) with the parent still
 * alive left the entry, and the id mapping, pointing at freed memory. Chain the
 * list's own callback and drop the entry when the list goes away on its own.
 */
static void hml_list_destroy_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    hml_obj_entry_t *e = NULL;
    for (hml_obj_entry_t *it = hml_obj_head; it; it = it->next)
    {
        if (it->obj == obj) { e = it; break; }
    }

    // Run the list's own callback first: it frees the notes, and their sentinel
    // callbacks drop their entries while the notes are still valid.
    if (e && e->list_cb) { e->list_cb(obj, cb_type); }
    if (cb_type != OBJ_DESTROY || !e) { return; }

    // The sentinel is a sibling of the list, so it may fire after this one when
    // the parent is freed. Detach it before the entry memory goes away.
    if (e->sentinel) { e->sentinel->user_data = NULL; }
    hml_entry_remove(e);
}

void hml_runtime_add(gui_obj_t *obj, ezxml_t node)
{
    if (!obj || !node) { return; }

    hml_obj_entry_t *e = gui_malloc(sizeof(hml_obj_entry_t));
    if (!e) { return; }
    memset(e, 0, sizeof(hml_obj_entry_t));
    e->obj = obj;
    e->node = node;
    e->tag = node->name;
    e->id = ezxml_attr(node, "id");

    // Zero-sized hidden child: freed with its parent, so its destroy callback
    // tells us when the component goes away (views, list notes, user frees).
    // gui_list treats every child as a note, so a list's sentinel lives in the
    // list's parent instead. That alone misses "gui_obj_tree_free(list)" while
    // the parent stays alive, so the list's own destroy callback is chained as
    // well and drops the entry when the list goes away on its own.
    bool is_list = (obj->type == LIST);
    gui_obj_t *host = is_list ? obj->parent : obj;
    gui_obj_t *s = host ? gui_obj_create(host, "_hml", 0, 0, 0, 0) : NULL;
    if (s)
    {
        s->hidden = true;
        s->obj_cb = hml_sentinel_cb;
        s->has_destroy_cb = true;
        s->user_data = e;
        e->sentinel = s;
    }
    if (is_list && obj->obj_cb && obj->obj_cb != hml_list_destroy_cb)
    {
        e->list_cb = obj->obj_cb;
        obj->obj_cb = hml_list_destroy_cb;
        obj->has_destroy_cb = true;
    }

    e->next = hml_obj_head;
    hml_obj_head = e;

    if (e->id) { hml_update_bindings(e->id, obj); }
}

gui_obj_t *hml_obj_get(const char *id)
{
    if (!id) { return NULL; }
    hml_obj_entry_t *e = hml_entry_of_id(id);
    return e ? e->obj : NULL;
}

const char *hml_obj_id(gui_obj_t *obj)
{
    hml_obj_entry_t *e = hml_entry_of(obj);
    return e ? e->id : NULL;
}

ezxml_t hml_runtime_node_of(gui_obj_t *obj)
{
    hml_obj_entry_t *e = hml_entry_of(obj);
    return e ? e->node : NULL;
}

void hml_bind(const char *id, void *slot)
{
    if (!id || !slot) { return; }
    if (hml_binding_count >= HML_MAX_BINDINGS)
    {
        gui_log("[HML] binding table full, drop %s\n", id);
        return;
    }
    hml_bindings[hml_binding_count].id = id;
    hml_bindings[hml_binding_count].slot = (void **)slot;
    hml_binding_count++;
    *(void **)slot = hml_obj_get(id);
}

void hml_runtime_reset(void)
{
    while (hml_obj_head)
    {
        hml_obj_entry_t *next = hml_obj_head->next;
        if (hml_obj_head->sentinel) { hml_obj_head->sentinel->user_data = NULL; }
        // Give the list its own callback back: the entry is going away
        if (hml_obj_head->list_cb && hml_obj_head->obj &&
            hml_obj_head->obj->obj_cb == hml_list_destroy_cb)
        {
            hml_obj_head->obj->obj_cb = hml_obj_head->list_cb;
            hml_obj_head->obj->has_destroy_cb = true;
        }
        hml_entry_free(hml_obj_head);
        hml_obj_head = next;
    }
    for (int i = 0; i < hml_binding_count; i++) { *hml_bindings[i].slot = NULL; }
}

/*============================================================================*
 *                              Preset animation engine
 *
 * Interprets a designer "preset" timer at run time with the same semantics
 * as the exported C (CallbackFileGenerator.emitPresetTimerCallback):
 *  - the timeline is sampled from real elapsed time (gui_ms_get), so dropped
 *    callbacks only lower the visual sampling density;
 *  - boundary actions (visibility, changeImage, switchView, switchTimer,
 *    setFocus, colour without a start value) fire once when their segment
 *    is entered; the others are interpolated inside their segment;
 *  - stopOnComplete=true clamps to the end and stops, false loops against
 *    the original time origin.
 *============================================================================*/

static void hml_apply_switch_timer(gui_obj_t *obj, const hml_json_t *a);

static bool hml_is_boundary_action(const hml_json_t *a)
{
    const char *type = hml_json_str(a, "type", "");
    if (!strcmp(type, "visibility") || !strcmp(type, "changeImage") || !strcmp(type, "switchView") ||
        !strcmp(type, "switchTimer") || !strcmp(type, "setFocus"))
    {
        return true;
    }
    if (!strcmp(type, "fgColor")) { return !hml_json_has(a, "fgColorFrom"); }
    if (!strcmp(type, "bgColor")) { return !hml_json_has(a, "bgColorFrom"); }
    return false;
}

static int hml_timer_segments(const hml_json_t *timer, const hml_json_t **segs)
{
    const hml_json_t *s = hml_json_get(timer, "segments");
    if (s && s->type == HML_JSON_ARRAY && hml_json_size(s) > 0)
    {
        *segs = s;
        return hml_json_size(s);
    }
    *segs = NULL;
    return 0;
}

static uint32_t hml_seg_duration(const hml_json_t *seg)
{
    int d = hml_json_int(seg, "duration", 0);
    return d > 0 ? (uint32_t)d : 0;
}

static uint32_t hml_lerp_argb(uint32_t from, uint32_t to, float t)
{
    uint32_t out = 0;
    for (int sh = 0; sh <= 24; sh += 8)
    {
        int a = (int)((from >> sh) & 0xFF), b = (int)((to >> sh) & 0xFF);
        out |= (uint32_t)(uint8_t)(a + (b - a) * t) << sh;
    }
    return out;
}

static uint32_t hml_json_argb(const hml_json_t *a, const char *key, uint32_t def)
{
    return hml_parse_argb(hml_json_str(a, key, NULL), def);
}

/* Tags the loader builds as gui_img, i.e. the only components whose storage
 * really is gui_img_t. */
static bool hml_tag_is_image(const char *tag)
{
    return tag && (!strcmp(tag, "hg_image") || !strcmp(tag, "hg_img"));
}

/*
 * A gui_img is IMAGE_FROM_MEM whatever its source: gui_img_ctor() sets that
 * type for gui_img_create_from_fs() as well, so filesystem images already
 * qualify. The problem with the bare type test is the opposite one -- video,
 * streaming, gif and glass also report IMAGE_FROM_MEM, and casting one of them
 * to gui_img_t points the image fields at foreign members (a stream's `img`
 * pointer sits where gui_img_t has `degrees`). Classify by the component that
 * created the object instead.
 */
static bool hml_is_image(gui_obj_t *obj)
{
    if (obj->type != IMAGE_FROM_MEM) { return false; }
    hml_obj_entry_t *e = hml_entry_of(obj);
    return e ? hml_tag_is_image(e->tag) : false;
}

static void hml_set_image(gui_obj_t *obj, const char *src)
{
    if (!hml_is_image(obj) || !src) { return; }
    char path[160];
    hml_resolve_asset_path(src, path, sizeof(path));
    gui_img_set_src((gui_img_t *)obj, (const uint8_t *)path, IMG_SRC_FILESYS);
    gui_img_refresh_size((gui_img_t *)obj);
}

/* Apply one action; t is the progress in [0, 1] inside its segment */
static void hml_apply_action(gui_obj_t *obj, hml_timer_state_t *st, const hml_json_t *a, float t)
{
    const char *type = hml_json_str(a, "type", "");

    if (!strcmp(type, "visibility"))
    {
        gui_obj_hidden(obj, !hml_json_bool(a, "visible", true));
    }
    else if (!strcmp(type, "changeImage"))
    {
        hml_set_image(obj, hml_json_str(a, "imagePath", NULL));
    }
    else if (!strcmp(type, "imageSequence"))
    {
        const hml_json_t *seq = hml_json_get(a, "imageSequence");
        int n = hml_json_size(seq);
        if (n <= 0) { return; }
        int index = (int)((float)(n - 1) * t);
        if (index >= n) { index = n - 1; }
        if (index < 0) { index = 0; }
        if (st->seq_action == a && st->seq_index == index) { return; }
        st->seq_action = a;
        st->seq_index = index;
        const hml_json_t *item = hml_json_at(seq, index);
        if (item && item->type == HML_JSON_STRING) { hml_set_image(obj, item->str); }
    }
    else if (!strcmp(type, "switchView"))
    {
        hml_switch_view(hml_json_str(a, "target", NULL), hml_json_str(a, "switchOutStyle", NULL),
                        hml_json_str(a, "switchInStyle", NULL));
    }
    else if (!strcmp(type, "switchTimer"))
    {
        hml_apply_switch_timer(obj, a);
    }
    else if (!strcmp(type, "position"))
    {
        float fx = hml_json_float(a, "fromX", 0), fy = hml_json_float(a, "fromY", 0);
        float tx = hml_json_float(a, "toX", 0), ty = hml_json_float(a, "toY", 0);
        gui_obj_move(obj, (int)(fx + (tx - fx) * t), (int)(fy + (ty - fy) * t));
    }
    else if (!strcmp(type, "size"))
    {
        float fw = hml_json_float(a, "fromW", 0), fh = hml_json_float(a, "fromH", 0);
        float tw = hml_json_float(a, "toW", 0), th = hml_json_float(a, "toH", 0);
        obj->w = (int16_t)(fw + (tw - fw) * t);
        obj->h = (int16_t)(fh + (th - fh) * t);
    }
    else if (!strcmp(type, "opacity"))
    {
        float from = hml_json_float(a, "from", 255), to = hml_json_float(a, "to", 255);
        int v = (int)(from + (to - from) * t);
        if (v < 0) { v = 0; }
        if (v > 255) { v = 255; }
        if (hml_is_image(obj)) { gui_img_set_opacity((gui_img_t *)obj, (unsigned char)v); }
        else { obj->opacity_value = (uint8_t)v; }
    }
    else if (!strcmp(type, "rotation"))
    {
        if (!hml_is_image(obj)) { return; }
        float from = hml_json_float(a, "angleOrigin", 0), to = hml_json_float(a, "angleTarget", 0);
        gui_img_rotation((gui_img_t *)obj, from + (to - from) * t);
    }
    else if (!strcmp(type, "scale"))
    {
        if (!hml_is_image(obj)) { return; }
        float fx = hml_json_float(a, "zoomXOrigin", 1), tx = hml_json_float(a, "zoomXTarget", 1);
        float fy = hml_json_float(a, "zoomYOrigin", 1), ty = hml_json_float(a, "zoomYTarget", 1);
        gui_img_scale((gui_img_t *)obj, fx + (tx - fx) * t, fy + (ty - fy) * t);
    }
    else if (!strcmp(type, "fgColor") || !strcmp(type, "bgColor"))
    {
        if (!hml_is_image(obj)) { return; }
        bool fg = (type[0] == 'f');
        uint32_t to = hml_json_argb(a, fg ? "fgColorTo" : "bgColorTo", 0xFFFFFFFF);
        const char *from_key = fg ? "fgColorFrom" : "bgColorFrom";
        uint32_t c = hml_json_has(a, from_key) ? hml_lerp_argb(hml_json_argb(a, from_key, to), to, t) : to;
        if (fg) { gui_img_a8_recolor((gui_img_t *)obj, c); }
        else { gui_img_a8_fix_bg((gui_img_t *)obj, c); }
    }
    else if (!strcmp(type, "setFocus"))
    {
        gui_obj_focus_set(obj);
    }
}

/* Boundary crossed within (prev, now]? Same rule as the exported helper */
static bool hml_boundary_crossed(uint32_t at, uint32_t prev, uint32_t now, bool wrapped,
                                 bool skipped)
{
    if (skipped) { return true; }
    if (wrapped) { return (at > prev) || (at <= now); }
    return (at > prev) && (at <= now);
}

/* One sample of the active preset timeline */
static void hml_preset_step(gui_obj_t *obj, hml_timer_state_t *st)
{
    const hml_json_t *timer = st->active;
    const hml_json_t *segs = NULL;
    int seg_count = hml_timer_segments(timer, &segs);
    bool stop_on_complete = hml_json_bool(timer, "stopOnComplete", true);

    uint32_t total = 0;
    for (int i = 0; i < seg_count; i++) { total += hml_seg_duration(hml_json_at(segs, i)); }

    uint32_t now_ms = gui_ms_get();
    bool first_sample = false;
    if (!st->started)
    {
        st->started = true;
        st->start_ms = now_ms;
        st->prev_elapsed_ms = 0;
        first_sample = true;
    }

    // Degenerate timeline: apply every action at its end value and stop
    if (total == 0)
    {
        gui_obj_stop_timer(obj);
        for (int i = 0; i < seg_count; i++)
        {
            const hml_json_t *acts = hml_json_get(hml_json_at(segs, i), "actions");
            for (const hml_json_t *a = acts ? acts->child : NULL; a; a = a->next)
            {
                hml_apply_action(obj, st, a, 1.0f);
                if (st->active != timer) { return; }   // switchTimer replaced us
            }
        }
        return;
    }

    uint32_t elapsed = now_ms - st->start_ms;
    uint32_t prev_elapsed = st->prev_elapsed_ms;
    st->prev_elapsed_ms = elapsed;

    bool finished = false;
    uint32_t timeline, prev_timeline;
    bool wrapped = false, skipped = false;
    if (stop_on_complete)
    {
        finished = elapsed >= total;
        timeline = finished ? total : elapsed;
        prev_timeline = prev_elapsed > total ? total : prev_elapsed;
    }
    else
    {
        timeline = elapsed % total;
        prev_timeline = prev_elapsed % total;
        skipped = (elapsed - prev_elapsed) >= total;
        wrapped = skipped || (timeline < prev_timeline);
    }

    // Segment entry actions: once per entry, not once per frame
    uint32_t start = 0;
    for (int i = 0; i < seg_count; i++)
    {
        const hml_json_t *seg = hml_json_at(segs, i);
        uint32_t dur = hml_seg_duration(seg);
        bool crossed = (start == 0) ? (first_sample ||
                                       hml_boundary_crossed(0, prev_timeline, timeline, wrapped, skipped))
                       : hml_boundary_crossed(start, prev_timeline, timeline, wrapped, skipped);
        if (crossed)
        {
            const hml_json_t *acts = hml_json_get(seg, "actions");
            for (const hml_json_t *a = acts ? acts->child : NULL; a; a = a->next)
            {
                // A 0 ms segment applies everything it carries at its end value
                if (dur == 0 || hml_is_boundary_action(a))
                {
                    hml_apply_action(obj, st, a, 1.0f);
                    if (st->active != timer) { return; }
                }
            }
        }
        start += dur;
    }

    // Sampled state of the segment the timeline is currently inside
    start = 0;
    const hml_json_t *cur = NULL;
    uint32_t cur_start = 0;
    for (int i = 0; i < seg_count; i++)
    {
        const hml_json_t *seg = hml_json_at(segs, i);
        uint32_t dur = hml_seg_duration(seg);
        if (dur == 0) { continue; }
        cur = seg;
        cur_start = start;
        if (timeline < start + dur) { break; }
        start += dur;
    }
    if (cur)
    {
        float t = (float)(timeline - cur_start) / (float)hml_seg_duration(cur);
        if (t > 1.0f) { t = 1.0f; }
        const hml_json_t *acts = hml_json_get(cur, "actions");
        for (const hml_json_t *a = acts ? acts->child : NULL; a; a = a->next)
        {
            if (!hml_is_boundary_action(a)) { hml_apply_action(obj, st, a, t); }
        }
    }

    if (finished) { gui_obj_stop_timer(obj); }
}

static void hml_preset_timer_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    hml_obj_entry_t *e = hml_entry_of(obj);
    if (!e || !e->timers.active)
    {
        gui_obj_stop_timer(obj);
        return;
    }
    hml_preset_step(obj, &e->timers);
}

/*============================================================================*
 *                              Timer control
 *============================================================================*/

static const hml_json_t *hml_timers_of(hml_obj_entry_t *e)
{
    if (!e) { return NULL; }
    if (!e->timers.json)
    {
        const char *raw = ezxml_attr(e->node, "timers");
        if (!raw || !raw[0]) { return NULL; }
        e->timers.json = hml_json_parse(raw);
        if (!e->timers.json)
        {
            gui_log("[HML] %s: invalid timers JSON\n", e->id ? e->id : "?");
            return NULL;
        }
    }
    return e->timers.json;
}

static bool hml_is_preset(const hml_json_t *timer)
{
    const hml_json_t *segs = NULL;
    return !strcmp(hml_json_str(timer, "mode", ""), "preset") && hml_timer_segments(timer, &segs) > 0;
}

/*
 * Create the object timer for one timer config, like the exported code:
 *   gui_obj_create_timer(obj, interval, reload, cb);
 *   if (start) gui_obj_start_timer(obj);
 * A freshly created timer fires on the next pass; gui_obj_start_timer()
 * delays the first call by one interval. Presets always replay from 0.
 */
static bool hml_timer_arm(hml_obj_entry_t *e, const hml_json_t *timer, bool start)
{
    gui_obj_t *obj = e->obj;
    int interval = hml_json_int(timer, "interval", 1000);
    if (interval < 0) { interval = 0; }

    if (hml_is_preset(timer))
    {
        e->timers.active = timer;
        e->timers.started = false;
        e->timers.seq_action = NULL;
        gui_obj_create_timer(obj, (uint32_t)interval, true, hml_preset_timer_cb);
    }
    else
    {
        hml_fn_t fn = hml_require_function(hml_json_str(timer, "callback", NULL));
        if (!fn) { return false; }
        e->timers.active = NULL;
        gui_obj_create_timer(obj, (uint32_t)interval, hml_json_bool(timer, "reload", true),
                             (void (*)(void *))fn);
    }
    if (start) { gui_obj_start_timer(obj); }
    return true;
}

bool hml_timer_start(gui_obj_t *obj, int index)
{
    hml_obj_entry_t *e = hml_entry_of(obj);
    const hml_json_t *timer = hml_json_at(hml_timers_of(e), index);
    if (!timer) { return false; }
    return hml_timer_arm(e, timer, true);
}

bool hml_timer_start_by_id(const char *id, int index)
{
    gui_obj_t *obj = hml_obj_get(id);
    return obj ? hml_timer_start(obj, index) : false;
}

void hml_timer_stop(gui_obj_t *obj)
{
    if (obj && obj->timer) { gui_obj_stop_timer(obj); }
}

void hml_preset_reset(gui_obj_t *obj)
{
    hml_obj_entry_t *e = hml_entry_of(obj);
    if (e) { e->timers.started = false; }
}

static const hml_json_t *hml_timer_by_id(const hml_json_t *timers, const char *tid)
{
    for (const hml_json_t *t = timers ? timers->child : NULL; t; t = t->next)
    {
        if (!strcmp(hml_json_str(t, "id", ""), tid)) { return t; }
    }
    return NULL;
}

/* "switchTimer" preset action: start/stop timers of the same component */
static void hml_apply_switch_timer(gui_obj_t *obj, const hml_json_t *a)
{
    hml_obj_entry_t *e = hml_entry_of(obj);
    const hml_json_t *timers = hml_timers_of(e);
    const hml_json_t *targets = hml_json_get(a, "timerTargets");

    if (!targets || !targets->child)
    {
        const hml_json_t *timer = hml_timer_by_id(timers, hml_json_str(a, "timerId", ""));
        if (timer) { hml_timer_arm(e, timer, !hml_json_bool(timer, "runImmediately", false)); }
        return;
    }
    for (const hml_json_t *t = targets->child; t; t = t->next)
    {
        const char *act = hml_json_str(t, "action", "start");
        const hml_json_t *timer = hml_timer_by_id(timers, hml_json_str(t, "timerId", ""));
        if (!timer) { continue; }
        if (!strcmp(act, "start"))
        {
            hml_timer_arm(e, timer, !hml_json_bool(timer, "runImmediately", false));
        }
        else if (!strcmp(act, "stop"))
        {
            hml_timer_stop(obj);
        }
    }
}

void hml_runtime_bind_timers(gui_obj_t *obj, ezxml_t node, bool is_view)
{
    GUI_UNUSED(node);
    GUI_UNUSED(is_view);
    hml_obj_entry_t *e = hml_entry_of(obj);
    const hml_json_t *timers = hml_timers_of(e);
    // The exported code binds every enabled timer in order; an object has a
    // single timer slot, so the last enabled one wins, as in the C build
    for (const hml_json_t *timer = timers ? timers->child : NULL; timer; timer = timer->next)
    {
        if (!hml_json_bool(timer, "enabled", false)) { continue; }
        hml_timer_arm(e, timer, !hml_json_bool(timer, "runImmediately", false));
    }
}
