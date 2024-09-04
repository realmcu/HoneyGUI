#ifndef __DRAW_FONT_H__
#define __DRAW_FONT_H__

#include <guidef.h>
#include <gui_api.h>
#ifdef __cplusplus
extern "C" {
#endif

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

/** @brief  text encoding format enum */
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

/**
 * @brief Converts content from a specified charset to Unicode code points.
 *
 * @param charset_type The charset type of the content.
 * @param content Input content to be converted.
 * @param len Length of the input content in bytes.
 * @param p_buf_ptr Pointer to the buffer that will hold the Unicode code points.
 * @return The length of the Unicode code points array.
 */
uint16_t process_content_by_charset(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len,
                                    uint32_t **p_buf_ptr);

/**
 * @brief Get the len by char num object
 *
 * @param utf8
 * @param char_num
 * @return uint32_t
 */
uint32_t get_len_by_char_num(uint8_t *utf8, uint32_t char_num);

/**
 * @brief Function to generate file path based on a given Unicode sequence
 *
 * @param unicode_buf
 * @param len
 * @param file_path
 * @return int
 */
uint32_t generate_emoji_file_path_from_unicode(const uint32_t *unicode_buf, uint32_t len,
                                               char *file_path);

#ifdef __cplusplus
}
#endif

#endif
