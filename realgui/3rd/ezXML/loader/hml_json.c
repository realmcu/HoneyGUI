/**
 * @file hml_json.c
 * @brief Minimal JSON reader for HML attribute values
 */

#include "hml_json.h"
#include "gui_api.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char *p;
} hml_json_cursor_t;

static hml_json_t *json_parse_value(hml_json_cursor_t *c);

static void json_skip_ws(hml_json_cursor_t *c)
{
    while (*c->p == ' ' || *c->p == '\t' || *c->p == '\r' || *c->p == '\n')
    {
        c->p++;
    }
}

static hml_json_t *json_new(hml_json_type_t type)
{
    hml_json_t *node = gui_malloc(sizeof(hml_json_t));
    if (node)
    {
        memset(node, 0, sizeof(hml_json_t));
        node->type = type;
    }
    return node;
}

static int json_hex4(const char *s)
{
    int v = 0;
    for (int i = 0; i < 4; i++)
    {
        char ch = s[i];
        if (ch == '\0') { return -1; }   // truncated escape: stop before the end
        v <<= 4;
        if (ch >= '0' && ch <= '9') { v |= ch - '0'; }
        else if (ch >= 'a' && ch <= 'f') { v |= ch - 'a' + 10; }
        else if (ch >= 'A' && ch <= 'F') { v |= ch - 'A' + 10; }
        else { return -1; }
    }
    return v;
}

/* Unescape the string at c->p (just after the opening quote) in place. */
static char *json_parse_string(hml_json_cursor_t *c)
{
    char *start = c->p;
    char *out = c->p;

    while (*c->p && *c->p != '"')
    {
        if (*c->p != '\\')
        {
            *out++ = *c->p++;
            continue;
        }
        c->p++;
        switch (*c->p)
        {
        case 'n': *out++ = '\n'; c->p++; break;
        case 't': *out++ = '\t'; c->p++; break;
        case 'r': *out++ = '\r'; c->p++; break;
        case 'b': *out++ = '\b'; c->p++; break;
        case 'f': *out++ = '\f'; c->p++; break;
        case 'u':
            {
                int cp = json_hex4(c->p + 1);
                if (cp < 0) { return NULL; }
                c->p += 5;
                // Surrogate pairs are not combined; BMP is enough for HML attributes
                if (cp < 0x80)
                {
                    *out++ = (char)cp;
                }
                else if (cp < 0x800)
                {
                    *out++ = (char)(0xC0 | (cp >> 6));
                    *out++ = (char)(0x80 | (cp & 0x3F));
                }
                else
                {
                    *out++ = (char)(0xE0 | (cp >> 12));
                    *out++ = (char)(0x80 | ((cp >> 6) & 0x3F));
                    *out++ = (char)(0x80 | (cp & 0x3F));
                }
                break;
            }
        case '\0':
            return NULL;
        default:
            *out++ = *c->p++;
            break;
        }
    }
    if (*c->p != '"') { return NULL; }
    c->p++;
    // out never passes c->p, so terminating here cannot clobber unread input
    *out = '\0';
    return start;
}

static hml_json_t *json_parse_container(hml_json_cursor_t *c, bool is_object)
{
    hml_json_t *node = json_new(is_object ? HML_JSON_OBJECT : HML_JSON_ARRAY);
    if (!node) { return NULL; }
    char close = is_object ? '}' : ']';
    hml_json_t **tail = &node->child;

    c->p++;
    json_skip_ws(c);
    if (*c->p == close)
    {
        c->p++;
        return node;
    }

    while (1)
    {
        const char *key = NULL;
        json_skip_ws(c);
        if (is_object)
        {
            if (*c->p != '"') { goto fail; }
            c->p++;
            key = json_parse_string(c);
            if (!key) { goto fail; }
            json_skip_ws(c);
            if (*c->p != ':') { goto fail; }
            c->p++;
        }
        hml_json_t *item = json_parse_value(c);
        if (!item) { goto fail; }
        item->key = key;
        *tail = item;
        tail = &item->next;

        json_skip_ws(c);
        if (*c->p == ',')
        {
            c->p++;
            continue;
        }
        if (*c->p == close)
        {
            c->p++;
            return node;
        }
        goto fail;
    }

fail:
    hml_json_free(node);
    return NULL;
}

static hml_json_t *json_parse_value(hml_json_cursor_t *c)
{
    json_skip_ws(c);
    char ch = *c->p;

    if (ch == '{') { return json_parse_container(c, true); }
    if (ch == '[') { return json_parse_container(c, false); }
    if (ch == '"')
    {
        c->p++;
        char *s = json_parse_string(c);
        if (!s) { return NULL; }
        hml_json_t *node = json_new(HML_JSON_STRING);
        if (node) { node->str = s; }
        return node;
    }
    bool is_true = strncmp(c->p, "true", 4) == 0;
    if (is_true || strncmp(c->p, "false", 5) == 0)
    {
        hml_json_t *node = json_new(HML_JSON_BOOL);
        if (node) { node->b = is_true; }
        c->p += is_true ? 4 : 5;
        return node;
    }
    if (strncmp(c->p, "null", 4) == 0)
    {
        c->p += 4;
        return json_new(HML_JSON_NULL);
    }

    char *end = NULL;
    double v = strtod(c->p, &end);
    if (end == c->p) { return NULL; }
    c->p = end;
    hml_json_t *node = json_new(HML_JSON_NUMBER);
    if (node) { node->num = v; }
    return node;
}

hml_json_t *hml_json_parse(const char *text)
{
    if (!text) { return NULL; }

    size_t len = strlen(text);
    char *buf = gui_malloc(len + 1);
    if (!buf) { return NULL; }
    memcpy(buf, text, len + 1);

    hml_json_cursor_t c = { .p = buf };
    hml_json_t *root = json_parse_value(&c);
    if (!root)
    {
        gui_free(buf);
        return NULL;
    }
    root->buf = buf;
    return root;
}

void hml_json_free(hml_json_t *root)
{
    while (root)
    {
        hml_json_t *next = root->next;
        hml_json_free(root->child);
        if (root->buf) { gui_free(root->buf); }
        gui_free(root);
        root = next;
    }
}

const hml_json_t *hml_json_get(const hml_json_t *obj, const char *key)
{
    if (!obj || obj->type != HML_JSON_OBJECT || !key) { return NULL; }
    for (const hml_json_t *it = obj->child; it; it = it->next)
    {
        if (it->key && strcmp(it->key, key) == 0) { return it; }
    }
    return NULL;
}

int hml_json_size(const hml_json_t *node)
{
    int n = 0;
    if (!node) { return 0; }
    for (const hml_json_t *it = node->child; it; it = it->next) { n++; }
    return n;
}

const hml_json_t *hml_json_at(const hml_json_t *arr, int index)
{
    if (!arr || index < 0) { return NULL; }
    const hml_json_t *it = arr->child;
    while (it && index--) { it = it->next; }
    return it;
}

static bool json_number_of(const hml_json_t *v, double *out)
{
    if (!v) { return false; }
    if (v->type == HML_JSON_NUMBER)
    {
        *out = v->num;
        return true;
    }
    if (v->type == HML_JSON_STRING && v->str[0])
    {
        char *end = NULL;
        // strtod does not accept "0x" prefixed integers on every libc, so handle hex explicitly
        if (v->str[0] == '0' && (v->str[1] == 'x' || v->str[1] == 'X'))
        {
            *out = (double)strtoul(v->str, &end, 16);
        }
        else
        {
            *out = strtod(v->str, &end);
        }
        return end != v->str;
    }
    if (v->type == HML_JSON_BOOL)
    {
        *out = v->b ? 1.0 : 0.0;
        return true;
    }
    return false;
}

float hml_json_float(const hml_json_t *obj, const char *key, float def)
{
    double v;
    return json_number_of(hml_json_get(obj, key), &v) ? (float)v : def;
}

int hml_json_int(const hml_json_t *obj, const char *key, int def)
{
    double v;
    return json_number_of(hml_json_get(obj, key), &v) ? (int)v : def;
}

bool hml_json_bool(const hml_json_t *obj, const char *key, bool def)
{
    const hml_json_t *v = hml_json_get(obj, key);
    if (!v) { return def; }
    if (v->type == HML_JSON_BOOL) { return v->b; }
    if (v->type == HML_JSON_STRING) { return strcmp(v->str, "true") == 0; }
    if (v->type == HML_JSON_NUMBER) { return v->num != 0.0; }
    return def;
}

const char *hml_json_str(const hml_json_t *obj, const char *key, const char *def)
{
    const hml_json_t *v = hml_json_get(obj, key);
    return (v && v->type == HML_JSON_STRING) ? v->str : def;
}

bool hml_json_has(const hml_json_t *obj, const char *key)
{
    return hml_json_get(obj, key) != NULL;
}
