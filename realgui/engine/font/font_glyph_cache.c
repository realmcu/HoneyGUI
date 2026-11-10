/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "font_glyph_cache.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct font_glyph_cache_entry
{
    uint8_t  *font_file;
    uint32_t unicode;
    uint8_t  type;
    uint32_t data_size;
    uint8_t  *data;
    struct font_glyph_cache_entry *next;  /* hash chain */
} font_glyph_cache_entry_t;

typedef struct
{
    uint8_t  *pool;           /* pre-allocated memory */
    uint32_t pool_size;
    uint32_t used;            /* bump allocator offset */
    uint32_t hit_count;
    uint32_t miss_count;
    font_glyph_cache_entry_t *buckets[FONT_GLYPH_CACHE_HASH_SIZE];
} font_glyph_cache_t;

/*============================================================================*
 *                         Variables
 *============================================================================*/

static font_glyph_cache_t g_cache = {0};

/*============================================================================*
 *                      Private Functions
 *============================================================================*/

static uint32_t font_glyph_hash(uint8_t *font_file, uint32_t unicode,
                                uint8_t type)
{
    uint32_t h = (uint32_t)(uintptr_t)font_file;
    h ^= unicode * 2654435761u;
    h ^= type;
    return h & (FONT_GLYPH_CACHE_HASH_SIZE - 1);
}


static void *pool_alloc(uint32_t size)
{
    /* Align to 4 bytes */
    size = (size + 3u) & ~3u;
    if (g_cache.used + size > g_cache.pool_size)
    {
        return NULL;
    }
    void *ptr = g_cache.pool + g_cache.used;
    g_cache.used += size;
    return ptr;
}

/*============================================================================*
 *                      Public Functions
 *============================================================================*/

void font_glyph_cache_init(void *pool, uint32_t pool_size)
{
    if (pool == NULL || pool_size == 0)
    {
        memset(&g_cache, 0, sizeof(g_cache));
        return;
    }
    memset(&g_cache, 0, sizeof(g_cache));
    g_cache.pool = (uint8_t *)pool;
    g_cache.pool_size = pool_size;
}

void *font_glyph_cache_get(uint8_t *font_file, uint32_t unicode,
                           uint8_t type, uint32_t *out_size)
{
    if (g_cache.pool == NULL)
    {
        return NULL;
    }

    uint32_t idx = font_glyph_hash(font_file, unicode, type);
    font_glyph_cache_entry_t *e = g_cache.buckets[idx];
    while (e != NULL)
    {
        if (e->font_file == font_file &&
            e->unicode == unicode &&
            e->type == type)
        {
            g_cache.hit_count++;
            if (out_size) { *out_size = e->data_size; }
            return e->data;
        }
        e = e->next;
    }
    g_cache.miss_count++;
    return NULL;
}

void *font_glyph_cache_put(uint8_t *font_file, uint32_t unicode,
                           uint8_t type, const void *header,
                           uint32_t header_size, const void *dot_data,
                           uint32_t dot_size)
{
    if (g_cache.pool == NULL || dot_data == NULL || dot_size == 0)
    {
        return NULL;
    }

    uint32_t total_size = header_size + dot_size;

    /* Check if already cached */
    uint32_t idx = font_glyph_hash(font_file, unicode, type);
    font_glyph_cache_entry_t *e = g_cache.buckets[idx];
    while (e != NULL)
    {
        if (e->font_file == font_file &&
            e->unicode == unicode &&
            e->type == type)
        {
            return e->data;
        }
        e = e->next;
    }

    /* Allocate entry + data from pool */
    font_glyph_cache_entry_t *entry = (font_glyph_cache_entry_t *)pool_alloc(
                                          sizeof(font_glyph_cache_entry_t));
    if (entry == NULL)
    {
        return NULL;
    }
    uint8_t *buf = (uint8_t *)pool_alloc(total_size);
    if (buf == NULL)
    {
        /* Roll back entry allocation (best effort) */
        g_cache.used -= (sizeof(font_glyph_cache_entry_t) + 3u) & ~3u;
        return NULL;
    }

    if (header && header_size > 0)
    {
        memcpy(buf, header, header_size);
    }
    memcpy(buf + header_size, dot_data, dot_size);

    entry->font_file = font_file;
    entry->unicode = unicode;
    entry->type = type;
    entry->data = buf;
    entry->data_size = total_size;
    entry->next = g_cache.buckets[idx];
    g_cache.buckets[idx] = entry;

    return buf;
}

void font_glyph_cache_clear(void)
{
    if (g_cache.pool == NULL)
    {
        return;
    }
    g_cache.used = 0;
    g_cache.hit_count = 0;
    g_cache.miss_count = 0;
    memset(g_cache.buckets, 0, sizeof(g_cache.buckets));
}

bool font_glyph_cache_contains(const void *ptr)
{
    if (g_cache.pool == NULL || ptr == NULL)
    {
        return false;
    }
    const uint8_t *p = (const uint8_t *)ptr;
    return (p >= g_cache.pool && p < g_cache.pool + g_cache.pool_size);
}

bool font_glyph_cache_is_ready(void)
{
    return (g_cache.pool != NULL);
}

void font_glyph_cache_stats(uint32_t *hit, uint32_t *miss,
                            uint32_t *used, uint32_t *total)
{
    if (hit)   { *hit   = g_cache.hit_count; }
    if (miss)  { *miss  = g_cache.miss_count; }
    if (used)  { *used  = g_cache.used; }
    if (total) { *total = g_cache.pool_size; }
}
