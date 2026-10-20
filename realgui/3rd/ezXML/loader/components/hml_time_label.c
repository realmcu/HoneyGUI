/**
 * @file hml_time_label.c
 * @brief HML Time Label Component (hg_time_label)
 *
 * Same as hg_label (font, alignment, color: see hml_label.c) with the text
 * replaced by the current time, refreshed by an object timer, as the
 * designer's TimeLabelGenerator does. The text buffer is owned by a hidden
 * child object and freed with it.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_text.h"
#include "gui_api.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
    char format[24];
    char text[32];
} hml_time_holder_t;

/* The holder hangs on a hidden child, so it is freed with the label */
static void hml_time_holder_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (cb_type == OBJ_DESTROY && obj->user_data)
    {
        gui_free(obj->user_data);
        obj->user_data = NULL;
    }
}

static hml_time_holder_t *hml_time_holder(gui_obj_t *text)
{
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &text->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->obj_cb == hml_time_holder_cb) { return (hml_time_holder_t *)o->user_data; }
    }
    return NULL;
}

static void hml_time_format(const char *fmt, char *out, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t)
    {
        snprintf(out, size, "--:--");
        return;
    }
    int hh = t->tm_hour % 24, mm = t->tm_min % 60, ss = t->tm_sec % 60;
    int year = (t->tm_year + 1900) % 10000, mon = (t->tm_mon % 12) + 1, day = t->tm_mday % 32;

    if (!strcmp(fmt, "HH")) { snprintf(out, size, "%02d", hh); }
    else if (!strcmp(fmt, "mm")) { snprintf(out, size, "%02d", mm); }
    else if (!strcmp(fmt, "HH:mm") || !strcmp(fmt, "HH:mm-split")) { snprintf(out, size, "%02d:%02d", hh, mm); }
    else if (!strcmp(fmt, "YYYY-MM-DD")) { snprintf(out, size, "%04d-%02d-%02d", year, mon, day); }
    else if (!strcmp(fmt, "YYYY-MM-DD HH:mm:ss"))
    {
        snprintf(out, size, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hh, mm, ss);
    }
    else if (!strcmp(fmt, "MM-DD HH:mm")) { snprintf(out, size, "%02d-%02d %02d:%02d", mon, day, hh, mm); }
    else { snprintf(out, size, "%02d:%02d:%02d", hh, mm, ss); }   // HH:mm:ss (default)
}

static void hml_time_update(void *p)
{
    gui_text_t *txt = (gui_text_t *)p;
    hml_time_holder_t *h = hml_time_holder((gui_obj_t *)txt);
    if (!h) { return; }
    hml_time_format(h->format, h->text, sizeof(h->text));
    gui_text_content_set(txt, h->text, (uint16_t)strlen(h->text));
}

/* Designer refresh periods: 500 ms with seconds, 30 s for minutes, 60 s for dates */
static uint32_t hml_time_interval(const char *fmt)
{
    if (strstr(fmt, "ss")) { return 500; }
    if (!strcmp(fmt, "YYYY-MM-DD")) { return 60000; }
    return 30000;
}

gui_obj_t *hml_create_time_label(gui_obj_t *parent, ezxml_t node)
{
    // Font, color, alignment, spacing exactly as hg_label
    gui_obj_t *obj = hml_create_label(parent, node);
    if (!obj) { return NULL; }

    hml_time_holder_t *h = gui_malloc(sizeof(hml_time_holder_t));
    if (!h) { return obj; }
    memset(h, 0, sizeof(hml_time_holder_t));
    strncpy(h->format, hml_attr_str(node, "timeFormat", "HH:mm:ss"), sizeof(h->format) - 1);

    gui_obj_t *holder = gui_obj_create(obj, "_hml_time", 0, 0, 0, 0);
    if (!holder)
    {
        gui_free(h);
        return obj;
    }
    holder->hidden = true;
    holder->user_data = h;
    holder->obj_cb = hml_time_holder_cb;
    holder->has_destroy_cb = true;

    hml_time_update(obj);
    gui_obj_create_timer(obj, hml_time_interval(h->format), true, hml_time_update);
    gui_obj_start_timer(obj);

    return obj;
}
