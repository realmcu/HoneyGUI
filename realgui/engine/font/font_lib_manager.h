/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __FONT_LIB_MANAGER_H__
#define __FONT_LIB_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_api.h"
#include "font_types.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief Dynamic font library node with reference counting
 */
typedef struct FONT_LIB_NODE
{
    uint8_t *font_file;             /**< Font file path or memory address */
    uint8_t font_size;              /**< Font size in pixels (for BMP) or default size (for TTF) */
    FONT_SRC_MODE src_mode;         /**< Font source: MEM/FTL/FS */
    FONT_SRC_TYPE font_type;        /**< Font type: BMP or TTF */
    uint8_t *cached_data;           /**< Cached header + index data */
    uint32_t cached_size;           /**< Size of cached data */
    uint32_t ref_count;             /**< Reference count for memory management */
    struct FONT_LIB_NODE *next;     /**< Next node in linked list */
} FONT_LIB_NODE;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Find font node by file path/address
 *
 * @param font_file Font file path or memory address
 * @return Pointer to font node, or NULL if not found
 */
FONT_LIB_NODE *gui_font_lib_find(uint8_t *font_file);

/**
 * @brief Find font node by size and type
 *
 * @param font_size Font size to match
 * @param font_type Font type (BMP or TTF)
 * @return Pointer to font node, or NULL if not found
 */
FONT_LIB_NODE *gui_font_lib_find_by_size(uint8_t font_size, FONT_SRC_TYPE font_type);

/**
 * @brief Register a font to the library (increase ref_count if exists)
 *
 * @param font_file Font file path or memory address
 * @param font_size Font size
 * @param src_mode Font source mode (MEM/FTL/FS)
 * @param font_type Font type (BMP/TTF)
 * @param cached_data Cached header + index data (can be NULL for MEM mode)
 * @param cached_size Size of cached data
 * @return Pointer to font node, or NULL on failure
 */
FONT_LIB_NODE *gui_font_lib_register(uint8_t *font_file,
                                     uint8_t font_size,
                                     FONT_SRC_MODE src_mode,
                                     FONT_SRC_TYPE font_type,
                                     uint8_t *cached_data,
                                     uint32_t cached_size);

/**
 * @brief Add reference to an existing font node
 *
 * @param node Font node to add reference
 * @return Updated reference count
 */
uint32_t gui_font_lib_addref(FONT_LIB_NODE *node);

/**
 * @brief Release reference to a font (decrease ref_count)
 * Font will be freed when ref_count reaches 0
 *
 * @param font_file Font file path or memory address
 */
void gui_font_lib_release(uint8_t *font_file);

/**
 * @brief Release reference by node pointer
 *
 * @param node Font node to release
 */
void gui_font_lib_release_node(FONT_LIB_NODE *node);

/**
 * @brief Get total number of loaded fonts
 *
 * @return Number of fonts in the library
 */
uint32_t gui_font_lib_get_count(void);

/**
 * @brief Get the head of font library linked list (for iteration)
 *
 * @return Head pointer of the linked list
 */
FONT_LIB_NODE *gui_font_lib_get_head(void);

/**
 * @brief Clear all fonts from the library
 * Warning: This will free all cached data regardless of ref_count
 */
void gui_font_lib_clear_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __FONT_LIB_MANAGER_H__ */
