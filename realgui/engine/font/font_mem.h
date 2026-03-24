/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __FONT_MEM_H__
#define __FONT_MEM_H__

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#include "font_rendering_utils.h"
#include "font_lib_manager.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FONT_MALLOC_PSRAM(x)  gui_malloc(x)
#define FONT_FREE_PSRAM(x)  gui_free(x)

#define FONT_FILE_BMP_FLAG 0x01

typedef struct
{
    uint8_t char_y;
    uint8_t baseline;
    uint8_t char_w;
    uint8_t char_h;
} GUI_CHAR_HEAD;

/*============================================================================*
 *                V2 Typography Types (Standard Typography Model)
 *============================================================================*/

/** @brief Typography metrics from V2 header extension fields (font units) */
typedef struct
{
    int16_t ascender;        /**< font ascender (positive, font units) */
    int16_t descender;       /**< font descender (negative, font units) */
    int16_t line_gap;        /**< font lineGap (font units) */
    uint16_t units_per_em;   /**< font design units per em */
} gui_font_typo_metrics_t;

/** @brief V2 per-glyph header (bearing-based, 6 bytes packed) */
#pragma pack(push, 1)
typedef struct
{
    int8_t bearing_x;        /**< horizontal bearing from origin to glyph left (pixels) */
    int8_t bearing_y;        /**< vertical bearing from baseline to glyph top (pixels) */
    uint8_t width;           /**< tight bbox width (pixels) */
    uint8_t height;          /**< tight bbox height (pixels) */
    uint8_t advance;         /**< horizontal advance width (pixels) */
    uint8_t reserved;        /**< reserved, set to 0 */
} GUI_BMP_GLYPH_HEAD_V2;
#pragma pack(pop)

/** @brief Pixel-level typography layout parameters */
typedef struct
{
    int16_t baseline;        /**< baseline Y offset in pixels (= ascent_px) */
    int16_t ascent_px;       /**< ascender in pixels */
    int16_t descent_px;      /**< |descender| in pixels */
    int16_t line_height;     /**< default line height in pixels */
} gui_font_typo_layout_t;

/** @brief Typography context, computed once per text widget */
typedef struct
{
    bool is_v2;                          /**< true if V2 font detected */
    int16_t baseline_px;                 /**< baseline Y offset (pixels), 0 for V1 */
    int16_t default_line_height;         /**< default line height (pixels) */
    gui_font_typo_metrics_t metrics;     /**< raw metrics (valid only when is_v2=true) */
} gui_font_typo_context_t;

typedef struct
{
    uint8_t *font_file;
    uint8_t font_size;
    FONT_SRC_MODE type;
    uint8_t *data;
} MEM_FONT_LIB;

#pragma pack(1)
typedef struct
{
    uint8_t head_length;                /*font file head length*/
    uint8_t file_type;                  /*0x1-FONT_FILE_BMP_FLAG is font file*/
    uint8_t version[4];                 /*version*/
    uint8_t font_size;                  /*font size*/
    uint8_t render_mode;                /*support 1/2/4/8*/
    uint8_t bold :          1;          /*bold*/
    uint8_t italic :        1;          /*italic*/
    uint8_t scan_mode :     1;          /*scan_mode*/
    uint8_t index_method :  1;          /*0 offset ; 1 address*/
    uint8_t crop :          1;          /*enable crop can crop font file*/
    uint8_t rsvd :          3;          /*rsvd*/
    uint32_t index_area_size;           /*length of index area*/
    uint8_t font_name_length;           /*length of font name*/
    uint8_t *font_name;
} GUI_FONT_HEAD_BMP;
#pragma pack()

/**
 * @brief Parse typography metrics from a V2 binary font header.
 *
 * @param header Pointer to the font header.
 * @param out_metrics Output typography metrics (populated on success).
 * @return true if V2 metrics were successfully parsed, false for V1 or invalid data.
 */
bool gui_font_bmp_parse_typo_metrics(const GUI_FONT_HEAD_BMP *header,
                                     gui_font_typo_metrics_t *out_metrics);

/**
 * @brief Calculate pixel-level typography layout from font metrics.
 *
 * @param metrics Typography metrics (from V2 header).
 * @param font_size Font size in pixels (em-size).
 * @return Computed layout parameters.
 */
gui_font_typo_layout_t gui_font_typo_calc_layout(const gui_font_typo_metrics_t *metrics,
                                                 uint8_t font_size);

/**
 * @brief Build typography context from font header.
 *
 * For V2 headers, populates all fields from extension data.
 * For V1 headers, returns is_v2=false with baseline_px=0 and default_line_height=font_size.
 *
 * @param header Pointer to the font header.
 * @param font_size Font size in pixels.
 * @return Typography context for layout use.
 */
gui_font_typo_context_t gui_font_bmp_get_typo_context(const GUI_FONT_HEAD_BMP *header,
                                                      uint8_t font_size);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list.
 *
 * @param font_bin_addr Binary file address of this font type.
 * @return Font library index.
 */
uint8_t gui_font_mem_init(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list.
 *
 * @param font_bin_addr Font file address.
 * @return Font library index.
 */
uint8_t gui_font_mem_init_ftl(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list.
 *
 * @param font_bin_addr Font file address.
 * @return Font library index.
 */
uint8_t gui_font_mem_init_fs(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list.
 *
 * @param font_bin_addr Font file address.
 * @return Font library index.
 */
uint8_t gui_font_mem_init_mem(uint8_t *font_bin_addr);

/**
 * @brief Destroy this font type in font list.
 *
 * @param font_bin_addr Font file address.
 * @return Font library index.
 */
uint8_t gui_font_mem_delete(uint8_t *font_bin_addr);

/**
 * @brief Preprocessing of bitmap fonts using internal engines.
 *
 * @param text Widget pointer.
 * @param rect Widget boundary.
 */
void gui_font_mem_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of bitmap fonts using internal engine.
 *
 * @param text Widget pointer.
 * @param rect Widget boundary.
 */
void gui_font_mem_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing bitmap fonts using internal engines.
 *
 * @param text Widget pointer.
 */
void gui_font_mem_unload(gui_text_t *text);

/**
 * @brief GUI_FONT_SRC_BMP text widget destroy function.
 *
 * @param text Widget pointer.
 */
void gui_font_mem_destroy(gui_text_t *text);

/**
 * @brief Get the font library node by size.
 *
 * @param font_size Font size.
 * @return Font library node pointer, or NULL if not found.
 */
FONT_LIB_NODE *get_fontlib_by_size(uint16_t font_size);

/**
 * @brief Get the font library node by name.
 *
 * @param font_file Font file.
 * @return Font library node pointer, or NULL if not found.
 */
FONT_LIB_NODE *get_fontlib_by_name(uint8_t *font_file);

/**
 * @brief Text layout by mode.
 *
 * @param text Widget pointer.
 * @param rect Widget boundary.
 */
void gui_font_mem_layout(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Get dot information by utf-8 or utf-16.
 *
 * @param text Widget pointer.
 */
void gui_font_get_dot_info(gui_text_t *text);

#ifdef __cplusplus
}
#endif

#endif

