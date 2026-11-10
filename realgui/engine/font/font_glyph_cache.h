/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __FONT_GLYPH_CACHE_H__
#define __FONT_GLYPH_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum
{
    FONT_GLYPH_CACHE_BMP_DOT    = 0,
    FONT_GLYPH_CACHE_TTF_LINE   = 1,
    FONT_GLYPH_CACHE_TTF_BITMAP = 2,
} font_glyph_cache_type_t;

/*============================================================================*
 *                         Macros
 *============================================================================*/

#ifndef FONT_GLYPH_CACHE_HASH_BITS
#define FONT_GLYPH_CACHE_HASH_BITS  8
#endif
#define FONT_GLYPH_CACHE_HASH_SIZE  (1u << FONT_GLYPH_CACHE_HASH_BITS)

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Initialize glyph cache with pre-allocated memory pool
 *
 * @param pool      Pre-allocated memory buffer
 * @param pool_size Size of the buffer in bytes
 */
void font_glyph_cache_init(void *pool, uint32_t pool_size);

/**
 * @brief Look up cached glyph data
 *
 * @param font_file Font file identifier
 * @param unicode   Unicode code point
 * @param type      Cache data type
 * @param out_size  Output: size of cached data (can be NULL)
 * @return Pointer to cached data, or NULL if not found / not initialized
 */
void *font_glyph_cache_get(uint8_t *font_file, uint32_t unicode,
                           uint8_t type, uint32_t *out_size);

/**
 * @brief Store glyph data into cache pool
 *
 * Stores [header + dot_data] combined in the pool.
 *
 * @param font_file  Font file identifier
 * @param unicode    Unicode code point
 * @param type       Cache data type
 * @param header     Header bytes (char metrics)
 * @param header_size Size of header
 * @param dot_data   Dot/glyph data
 * @param dot_size   Size of dot data
 * @return Pointer to cached copy (starts at header), or NULL if pool full / not initialized
 */
void *font_glyph_cache_put(uint8_t *font_file, uint32_t unicode,
                           uint8_t type, const void *header,
                           uint32_t header_size, const void *dot_data,
                           uint32_t dot_size);

/**
 * @brief Reset entire cache (e.g. on page switch)
 */
void font_glyph_cache_clear(void);

/**
 * @brief Check if a pointer belongs to the glyph cache pool
 *
 * @param ptr Pointer to check
 * @return true if ptr is within the cache pool range
 */
bool font_glyph_cache_contains(const void *ptr);

/**
 * @brief Check if glyph cache is initialized and available
 *
 * @return true if cache is ready, false otherwise
 */
bool font_glyph_cache_is_ready(void);

/**
 * @brief Get cache usage statistics
 *
 * @param hit       Output: cache hit count (can be NULL)
 * @param miss      Output: cache miss count (can be NULL)
 * @param used      Output: used bytes in pool (can be NULL)
 * @param total     Output: total pool size (can be NULL)
 */
void font_glyph_cache_stats(uint32_t *hit, uint32_t *miss,
                            uint32_t *used, uint32_t *total);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_GLYPH_CACHE_H__ */
