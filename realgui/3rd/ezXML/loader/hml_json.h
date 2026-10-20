/**
 * @file hml_json.h
 * @brief Minimal JSON reader for HML attribute values (timers, transform, ...)
 *
 * The designer stores structured attributes as JSON strings inside XML
 * attributes. This reader parses such a string into a small node tree.
 * Strings are unescaped in a private copy of the input, so the caller's
 * buffer is never modified and stays independent of the tree lifetime.
 */

#ifndef HML_JSON_H
#define HML_JSON_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HML_JSON_NULL = 0,
    HML_JSON_BOOL,
    HML_JSON_NUMBER,
    HML_JSON_STRING,
    HML_JSON_ARRAY,
    HML_JSON_OBJECT,
} hml_json_type_t;

typedef struct hml_json
{
    hml_json_type_t type;
    const char *key;          // member name when the parent is an object
    const char *str;          // HML_JSON_STRING
    double num;               // HML_JSON_NUMBER
    bool b;                   // HML_JSON_BOOL
    struct hml_json *child;   // first element / member
    struct hml_json *next;    // next sibling
    char *buf;                // root only: owned copy of the input text
} hml_json_t;

/** Parse a JSON text. Returns NULL on syntax error. Free with hml_json_free(). */
hml_json_t *hml_json_parse(const char *text);
void hml_json_free(hml_json_t *root);

/** Member of an object by name, or NULL. */
const hml_json_t *hml_json_get(const hml_json_t *obj, const char *key);
/** Number of elements of an array or object. */
int hml_json_size(const hml_json_t *node);
/** Element of an array by index, or NULL. */
const hml_json_t *hml_json_at(const hml_json_t *arr, int index);

/* Typed member readers. Numbers given as strings ("0xFF00FF00", "12") are accepted. */
float hml_json_float(const hml_json_t *obj, const char *key, float def);
int hml_json_int(const hml_json_t *obj, const char *key, int def);
bool hml_json_bool(const hml_json_t *obj, const char *key, bool def);
const char *hml_json_str(const hml_json_t *obj, const char *key, const char *def);
bool hml_json_has(const hml_json_t *obj, const char *key);

#ifdef __cplusplus
}
#endif

#endif /* HML_JSON_H */
