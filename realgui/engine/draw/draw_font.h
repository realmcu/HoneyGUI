/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __DRAW_FONT_H__
#define __DRAW_FONT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/* Text rect struct start */
typedef struct gui_text_rect
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    int16_t xboundleft;
    int16_t xboundright;
    int16_t yboundtop;
    int16_t yboundbottom;
} gui_text_rect_t;
/* Text rect struct end */


/* Memory char struct start */
typedef struct
{
    uint32_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint8_t char_y;
    uint8_t char_w;
    uint8_t char_h;
    uint8_t *dot_addr;
    uint8_t *buf;
    void *emoji_img;
} mem_char_t;
/* Memory char struct end */


/** @brief  Text encoding format enum */
typedef enum
{
    UTF_8                = 0,       //UTF: Unicode Transformation Format

    UTF_16               = 1,       //default LE, does not support surrogate pairs
    UTF_16LE             = 1,
    UNICODE_ENCODING     = 1,       //utf-16 equal to unicode on BMP

    UTF_16BE             = 2,

    UTF_32LE             = 3,

    UTF_32BE             = 4,
} TEXT_CHARSET;

typedef struct
{
    uint8_t char_offset;
    uint16_t char_end_form;
    int8_t char_beginning_form_offset;
    int8_t char_middle_form_offset;
    int8_t char_isolated_form_offset;
    struct
    {
        uint8_t conj_to_previous;
        uint8_t conj_to_next;
    } ap_chars_conjunction;
} ap_chars_map_t;

typedef enum
{
    THAI_NONE = 0,
    THAI_BELOW,
    THAI_ABOVE,
    THAI_TOPMOST,
    THAI_LEFT,
    THAI_RIGHT
} THAI_MARK_POS;

typedef struct
{
    uint32_t base_index;
    uint32_t mark_unicode;
    THAI_MARK_POS mark_pos;
} THAI_MARK_INFO;
/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Converts content from a specified charset to Unicode code points.
 *
 * @param charset_type Charset type of the content.
 * @param content Input content to be converted.
 * @param len Length of the input content in bytes.
 * @param p_buf_ptr Pointer to the buffer that will hold the Unicode code points.
 * @return Length of the Unicode code points array.
 */
uint16_t process_content_by_charset(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len,
                                    uint32_t **p_buf_ptr);

/**
 * @brief Get the len by char num object
 *
 * @param utf8 UTF8 string pointer.
 * @param char_num Number of characters.
 * @return Length of the UTF8 string in bytes.
 */
uint32_t get_len_by_char_num(uint8_t *utf8, uint32_t char_num);

/**
 * @brief Function to generate file path based on a given Unicode sequence
 *
 * @param unicode_buf Unicode buffer to generate file path from.
 * @param len Length of the Unicode buffer.
 * @param file_path Output file path buffer.
 * @return Length of the generated file path.
 */
uint32_t generate_emoji_file_path_from_unicode(const uint32_t *unicode_buf, uint32_t len,
                                               char *file_path);

/**
 * @brief Check if the content has any Arabic or Persian Unicode characters.
 *
 * @param unicode_buf Unicode buffer to check.
 * @param len Length of the Unicode buffer.
 * @return true If the content has any Arabic or Persian Unicode characters.
 * @return false If the content does not have any Arabic or Persian Unicode characters.
 */
bool content_has_ap_unicode(uint32_t *unicode_buf, uint32_t len);

/**
 * @brief Check if the content has any Arabic or Persian Unicode characters.
 *
 * @param charset_type The charset type of the content.
 * @param content Input content to be checked.
 * @param len Length of the input content in bytes.
 * @return true If the content has any Arabic or Persian Unicode characters.
 * @return false If the content does not have any Arabic or Persian Unicode characters.
 */
bool content_has_ap(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len);

/**
 * @brief Process Arabic or Persian Unicode characters in the content.
 *
 * @param unicode_buf Unicode buffer to process.
 * @param unicode_len Length of the Unicode buffer.
 * @return uint32_t The length of the processed Unicode buffer.
 */
uint32_t process_ap_unicode(uint32_t *unicode_buf, uint32_t unicode_len);

/**
 * @brief Check if the content has any Thai Unicode characters.
 *
 * @param unicode_buf Unicode buffer to check.
 * @param len Length of the Unicode buffer.
 * @return true If the content has any Thai Unicode characters.
 * @return false If the content does not have any Thai Unicode characters.
 */
bool content_has_thai_unicode(uint32_t *unicode_buf, uint32_t len);

/**
 * @brief Check if the content has any Thai Unicode characters.
 *
 * @param charset_type The charset type of the content.
 * @param content Input content to be checked.
 * @param len Length of the input content in bytes.
 * @return true If the content has any Thai Unicode characters.
 * @return false If the content does not have any Thai Unicode characters.
 */
bool content_has_thai(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len);

/**
 * @brief Get the width sum of mark Thai characters.
 *
 * @param chr The memory character array.
 * @param char_count The count of characters.
 * @return uint32_t The width sum of Thai characters.
 */
uint32_t get_thai_mark_char_width(mem_char_t *chr, uint32_t char_count);

/**
 * @brief Process Thai character struct.
 *
 * @param chr The memory character array.
 * @param unicode_len The length of the Unicode buffer.
 * @param mark_array_out Pointer to the array that will hold the Thai mark information.
 * @param mark_count_out Pointer to the variable that will hold the count of Thai marks.
 * @return uint32_t The length of the processed base Thai Unicode buffer.
 */
uint32_t process_thai_char_struct(mem_char_t *chr, uint32_t unicode_len,
                                  THAI_MARK_INFO **mark_array_out, uint32_t *mark_count_out);

/**
 * @brief Post process Thai character struct.
 *
 * @param chr The memory character array.
 * @param base_count The count of base Thai characters.
 * @param mark_count The count of mark Thai characters.
 * @param active_base The count of active base Thai characters.
 * @param marks The Thai mark information array.
 * @return uint32_t Finally active length include base and mark.
 */
uint32_t post_process_thai_char_struct(mem_char_t *chr, uint32_t base_count, uint32_t active_base,
                                       uint32_t mark_count, THAI_MARK_INFO *marks);


/**
 * @brief Check if the content has any Hebrew Unicode characters.
 *
 * @param unicode_buf Unicode buffer to check.
 * @param len Length of the Unicode buffer.
 * @return true If the content has any Hebrew Unicode characters.
 * @return false If the content does not have any Hebrew Unicode characters.
 */
bool content_has_hebrew_unicode(uint32_t *unicode_buf, uint32_t len);

/**
 * @brief Check if the content has any Hebrew Unicode characters.
 *
 * @param charset_type The charset type of the content.
 * @param content Input content to be checked.
 * @param len Length of the input content in bytes.
 * @return true If the content has any Hebrew Unicode characters.
 * @return false If the content does not have any Hebrew Unicode characters.
 */
bool content_has_hebrew(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
