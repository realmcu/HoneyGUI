/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __FONT_TTF_H__
#define __FONT_TTF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#include "font_rendering_utils.h"
#include "font_lib_manager.h"
#include "font_mem.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

#pragma pack(1)
typedef struct
{
    uint8_t head_length;                /*font file head length*/
    uint8_t file_type;                  /*0x2-FONT_FILE_TTF_FLAG is font file*/
    uint8_t version[4];                 /*version*/
    uint8_t font_size;                  /*font size*/
    uint8_t render_mode;                /*support 1/2/4/8*/
    uint8_t bold :          1;          /*bold*/
    uint8_t italic :        1;          /*italic*/
    uint8_t scan_mode :     1;          /*scan_mode*/
    uint8_t index_method :  1;          /*0 offset ; 1 address*/
    uint8_t crop :          1;          /*Unavailable*/
    uint8_t rsvd :          3;          /*rsvd*/
    uint32_t index_area_size;           /*length of index area*/
    uint8_t font_name_length;           /*length of font name*/
    int16_t ascent;
    int16_t descent;
    int16_t lineGap;
    uint8_t *font_name;
} GUI_FONT_HEAD_TTF;

typedef struct
{
    short x0;
    short y0;
    short x1;
    short y1;
    short advance;
    uint8_t winding_count;
    uint8_t *winding_lengths;
    short *windings;
} FontGlyphData;
#pragma pack()

typedef struct
{
    short x;
    short y;
} FontWindings;

typedef struct
{
    float x;
    float y;
} ttf_point;

typedef enum
{
    POS_POS,  // m[0][0] >= 0, m[0][1] >= 0
    POS_NEG,  // m[0][0] >= 0, m[0][1] <  0
    NEG_POS,  // m[0][0] <  0, m[0][1] >= 0
    NEG_NEG   // m[0][0] <  0, m[0][1] <  0
} TransformCase;

typedef enum
{
    FONT_IDENTITY,
    FONT_TRANSFORM,
    FONT_SCALE,
    FONT_HOMOGENEOUS
} FONT_MATRIX_TYPE;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define FONT_FILE_TTF_FLAG  0x02

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Initialize ttf font from FTL storage
 *
 * @param font_bin_addr Font file address in FTL
 * @return 0 on success, UINT8_MAX on failure
 */
uint8_t gui_font_ttf_init_ftl(uint8_t *font_bin_addr);

/**
 * @brief Initialize ttf font from filesystem
 *
 * @param font_path Font file path
 * @return 0 on success, UINT8_MAX on failure
 */
uint8_t gui_font_ttf_init_fs(uint8_t *font_path);

/**
 * @brief Initialize ttf font from memory
 *
 * @param font_bin_addr Font data address in memory
 * @return 0 on success, UINT8_MAX on failure
 */
uint8_t gui_font_ttf_init_mem(uint8_t *font_bin_addr);

/**
 * @brief Delete ttf font from library
 *
 * @param font_bin_addr Font file address
 * @return 0 on success, UINT8_MAX on failure
 */
uint8_t gui_font_ttf_delete(uint8_t *font_bin_addr);

/**
 * @brief Preprocessing of ttf font drawing with ttf
 *
 * @param text Widget pointer
 */
void gui_font_ttf_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of ttf fonts using ttf-bin
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_ttf_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing ttf fonts using ttf
 *
 * @param text Widget pointer
 */
void gui_font_ttf_unload(gui_text_t *text);

/**
 * @brief calculate rect
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_ttf_adapt_rect(gui_text_t *text, gui_text_rect_t *rect);

#if ENABLE_FONT_V3_TYPO
/**
 * @brief Build typography context from TTF font header.
 *
 * For V3 headers (version[0] >= 3), populates all fields from header metrics.
 * Reads units_per_em from the header extension (last 2 bytes of header).
 * For legacy headers (version[0] < 3), returns is_v3=false with baseline_px=0.
 *
 * @param header   Pointer to TTF font header.
 * @param font_height Font height in pixels (em-size for V3, canvas size for legacy).
 * @return Typography context for layout use.
 */
gui_font_typo_context_t gui_font_ttf_get_typo_context(const GUI_FONT_HEAD_TTF *header,
                                                      uint16_t font_height);
#endif /* ENABLE_FONT_V3_TYPO */

/**
 * @brief Search for a glyph in all registered TTF fonts (fallback).
 * Iterates font_lib TTF nodes ordered by priority, skipping the primary font.
 * TTF fonts are scalable so font_size matching is not required.
 * Only populates char_w/char_h/dot_addr for MEMADDR mode.
 *
 * @param unicode Unicode code point to search.
 * @param font_height Desired font height in pixels (for scale calculation).
 * @param bold_weight Bold weight for advance calculation.
 * @param skip_file Primary font file to skip (already searched).
 * @param out_chr Output character info (populated on success).
 * @return 0 on success, -1 if not found in any fallback TTF font.
 */
int gui_font_ttf_fallback_search(uint32_t unicode, uint16_t font_height,
                                 uint8_t bold_weight, uint8_t *skip_file,
                                 mem_char_t *out_chr);

#ifdef __cplusplus
}
#endif

#endif
