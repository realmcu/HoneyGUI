/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stddef.h>
#include "guidef.h"
#include "gui_api.h"
#include "gui_shape_cache.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/

/** Bucket count, a power of two so the hash reduces with a mask. */
#define GUI_SHAPE_CACHE_BUCKETS 16u

/** Guards against a release() of memory this module never handed out. */
#define GUI_SHAPE_CACHE_MAGIC 0x53484150u    /* 'SHAP' */

#define GUI_SHAPE_CACHE_RESIZABLE_MIN_CAPACITY 1024u

#ifndef GUI_SHAPE_CACHE_ENABLE
#define GUI_SHAPE_CACHE_ENABLE 1
#endif

#ifndef GUI_SHAPE_CACHE_USE_LOWER_HEAP
#define GUI_SHAPE_CACHE_USE_LOWER_HEAP 0
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

/**
 * One shared payload.
 *
 * Layout after the header is [payload capacity][descriptor capacity]:
 *
 *  - the payload comes first so that release() can recover the node from a
 *    payload pointer by subtracting a fixed offset, rather than searching;
 *  - exact nodes reserve only their current payload and descriptor lengths;
 *  - resizable nodes reserve a geometric payload size class and the maximum
 *    descriptor length, so changing geometry can rekey in place.
 */
typedef struct gui_shape_cache_node
{
    struct gui_shape_cache_node *next;      /**< Next node in the same bucket. */
    uint32_t magic;                         /**< GUI_SHAPE_CACHE_MAGIC. */
    uint32_t hash;                          /**< Descriptor hash, bucket index. */
    uint32_t size;                          /**< Logical payload size. */
    uint32_t capacity;                      /**< Allocated payload capacity. */
    uint16_t ref_count;                     /**< Live references. */
    uint16_t desc_len;                      /**< Descriptor length. */
    uint16_t desc_capacity;                 /**< Allocated descriptor capacity. */
    uint16_t reserved;
    uint8_t data[];                         /**< Payload capacity, then descriptor. */
} gui_shape_cache_node_t;

/*============================================================================*
 *                           Variables
 *============================================================================*/

static gui_shape_cache_node_t *shape_buckets[GUI_SHAPE_CACHE_BUCKETS];

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void shape_bucket_add(gui_shape_cache_node_t *node);

/** FNV-1a over the descriptor; only used to pick a bucket. */
static uint32_t shape_hash(const uint8_t *desc, uint16_t len)
{
    uint32_t h = 2166136261u;

    for (uint16_t i = 0; i < len; i++)
    {
        h ^= desc[i];
        h *= 16777619u;
    }

    return h;
}

/** Recover the owning node from a payload pointer. */
static gui_shape_cache_node_t *shape_node_of(const void *data)
{
    gui_shape_cache_node_t *node =
        (gui_shape_cache_node_t *)((uint8_t *)data - offsetof(gui_shape_cache_node_t, data));

    GUI_ASSERT(node->magic == GUI_SHAPE_CACHE_MAGIC);
    return node;
}

/** The descriptor follows the reserved payload capacity. */
static uint8_t *shape_node_desc(gui_shape_cache_node_t *node)
{
    return node->data + node->capacity;
}

static uint32_t shape_resizable_capacity(uint32_t size)
{
    uint32_t capacity = GUI_SHAPE_CACHE_RESIZABLE_MIN_CAPACITY;

    while (capacity < size && capacity <= UINT32_MAX / 2u)
    {
        capacity <<= 1;
    }

    return capacity < size ? size : capacity;
}

static gui_shape_cache_node_t *shape_node_alloc(const void *desc, uint16_t desc_len,
                                                uint32_t size, uint32_t capacity,
                                                uint16_t desc_capacity)
{
    if (capacity < size || desc_capacity < desc_len)
    {
        return NULL;
    }

    size_t alloc_size = sizeof(gui_shape_cache_node_t) + (size_t)capacity + desc_capacity;
    gui_shape_cache_node_t *node;

#if GUI_SHAPE_CACHE_ENABLE && GUI_SHAPE_CACHE_USE_LOWER_HEAP
    node = gui_lower_malloc(alloc_size);
#else
    node = gui_malloc(alloc_size);
#endif
    if (node == NULL)
    {
        return NULL;
    }

    node->next = NULL;
    node->magic = GUI_SHAPE_CACHE_MAGIC;
    node->hash = shape_hash((const uint8_t *)desc, desc_len);
    node->size = size;
    node->capacity = capacity;
    node->ref_count = 1;
    node->desc_len = desc_len;
    node->desc_capacity = desc_capacity;
    node->reserved = 0u;
    memcpy(shape_node_desc(node), desc, desc_len);

    shape_bucket_add(node);
    return node;
}

/** Insert into the bucket implied by node->hash. */
static void shape_bucket_add(gui_shape_cache_node_t *node)
{
    uint32_t b = node->hash & (GUI_SHAPE_CACHE_BUCKETS - 1u);

    node->next = shape_buckets[b];
    shape_buckets[b] = node;
}

/** Unlink from the bucket implied by node->hash. */
static void shape_bucket_remove(gui_shape_cache_node_t *node)
{
    uint32_t b = node->hash & (GUI_SHAPE_CACHE_BUCKETS - 1u);
    gui_shape_cache_node_t **link = &shape_buckets[b];

    while (*link != NULL)
    {
        if (*link == node)
        {
            *link = node->next;
            node->next = NULL;
            return;
        }
        link = &(*link)->next;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

uint8_t *gui_shape_cache_find(const void *desc, uint16_t desc_len, uint32_t size)
{
    GUI_ASSERT(desc != NULL);
    GUI_ASSERT(desc_len > 0 && desc_len <= GUI_SHAPE_DESC_MAX);

    if (size == 0)
    {
        return NULL;
    }

#if !GUI_SHAPE_CACHE_ENABLE
    return NULL;
#endif

    uint32_t hash = shape_hash((const uint8_t *)desc, desc_len);

    /* Match on the descriptor itself, so a hash collision costs a memcmp
     * rather than handing back the wrong pixels. */
    for (gui_shape_cache_node_t *n = shape_buckets[hash & (GUI_SHAPE_CACHE_BUCKETS - 1u)];
         n != NULL; n = n->next)
    {
        if (n->hash != hash || n->desc_len != desc_len || n->size != size)
        {
            continue;
        }
        if (memcmp(shape_node_desc(n), desc, desc_len) != 0)
        {
            continue;
        }

        /* A widget count beyond 65535 on one shape is not a real scene, but
         * saturating beats wrapping to zero and freeing live pixels. */
        if (n->ref_count < UINT16_MAX)
        {
            n->ref_count++;
        }
        return n->data;
    }

    return NULL;
}

uint8_t *gui_shape_cache_acquire(const void *desc, uint16_t desc_len,
                                 uint32_t size, bool *is_new)
{
    GUI_ASSERT(is_new != NULL);

    uint8_t *hit = gui_shape_cache_find(desc, desc_len, size);
    if (hit != NULL)
    {
        *is_new = false;
        return hit;
    }

    if (size == 0)
    {
        *is_new = false;
        return NULL;
    }

    gui_shape_cache_node_t *node = shape_node_alloc(desc, desc_len, size, size, desc_len);
    if (node == NULL)
    {
        *is_new = false;
        return NULL;
    }

    *is_new = true;
    return node->data;
}

uint8_t *gui_shape_cache_refresh_resizable(const void *current,
                                           const void *desc, uint16_t desc_len,
                                           uint32_t size, bool *must_render)
{
    GUI_ASSERT(must_render != NULL);

    uint8_t *hit = gui_shape_cache_find(desc, desc_len, size);
    if (hit != NULL)
    {
        gui_shape_cache_release(current);
        *must_render = false;
        return hit;
    }

    if (gui_shape_cache_rekey(current, desc, desc_len, size))
    {
        *must_render = true;
        return (uint8_t *)current;
    }

    if (size == 0)
    {
        *must_render = false;
        return NULL;
    }

#if GUI_SHAPE_CACHE_ENABLE
    uint32_t capacity = shape_resizable_capacity(size);
    uint16_t desc_capacity = GUI_SHAPE_DESC_MAX;
#else
    uint32_t capacity = size;
    uint16_t desc_capacity = desc_len;
#endif

    gui_shape_cache_node_t *node =
        shape_node_alloc(desc, desc_len, size, capacity, desc_capacity);
    if (node == NULL)
    {
        *must_render = false;
        return NULL;
    }

    gui_shape_cache_release(current);
    *must_render = true;
    return node->data;
}

void gui_shape_cache_addref(const void *data)
{
    if (data == NULL) { return; }

    gui_shape_cache_node_t *node = shape_node_of(data);

    if (node->ref_count < UINT16_MAX)
    {
        node->ref_count++;
    }
}

void gui_shape_cache_release(const void *data)
{
    if (data == NULL) { return; }

    gui_shape_cache_node_t *node = shape_node_of(data);

    GUI_ASSERT(node->ref_count > 0);
    if (--node->ref_count > 0)
    {
        return;
    }

    shape_bucket_remove(node);
    node->magic = 0;
    gui_free(node);
}

bool gui_shape_cache_rekey(const void *data, const void *desc, uint16_t desc_len,
                           uint32_t size)
{
    GUI_ASSERT(desc != NULL);
    GUI_ASSERT(desc_len > 0 && desc_len <= GUI_SHAPE_DESC_MAX);

    if (data == NULL) { return false; }

    gui_shape_cache_node_t *node = shape_node_of(data);

    /* Someone else is drawing from these pixels, so they cannot be overwritten.
     * Payload and descriptor together must also still fit what was allocated. */
    if (node->ref_count != 1 || size == 0 ||
        size > node->capacity || desc_len > node->desc_capacity)
    {
        return false;
    }

    uint32_t hash = shape_hash((const uint8_t *)desc, desc_len);

    if (hash != node->hash)
    {
        shape_bucket_remove(node);
        node->hash = hash;
        shape_bucket_add(node);
    }

    node->size = size;
    node->desc_len = desc_len;
    memcpy(shape_node_desc(node), desc, desc_len);

    return true;
}

uint32_t gui_shape_cache_stats(uint32_t *payload_bytes, uint32_t *shared_bytes)
{
    uint32_t nodes = 0;
    uint32_t held = 0;
    uint32_t unshared = 0;

    for (uint32_t b = 0; b < GUI_SHAPE_CACHE_BUCKETS; b++)
    {
        for (gui_shape_cache_node_t *n = shape_buckets[b]; n != NULL; n = n->next)
        {
            nodes++;
            held += n->size;
            unshared += n->size * n->ref_count;
        }
    }

    if (payload_bytes != NULL) { *payload_bytes = held; }
    if (shared_bytes != NULL) { *shared_bytes = unshared; }

    return nodes;
}

uint32_t gui_shape_cache_capacity_bytes(void)
{
    uint32_t capacity = 0u;

    for (uint32_t b = 0; b < GUI_SHAPE_CACHE_BUCKETS; b++)
    {
        for (gui_shape_cache_node_t *n = shape_buckets[b]; n != NULL; n = n->next)
        {
            capacity += n->capacity;
        }
    }

    return capacity;
}
