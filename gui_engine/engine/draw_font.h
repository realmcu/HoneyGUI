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


/**
 * @brief
 *
 * @param utf8
 * @param len
 * @param unicode_array
 * @param unicode_buf_len
 * @return uint16_t
 */
uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint16_t *unicode_array,
                         uint16_t unicode_buf_len);

/**
 * @brief Get the len by char num object
 *
 * @param utf8
 * @param char_num
 * @return uint32_t
 */
uint32_t get_len_by_char_num(uint8_t *utf8, uint32_t char_num);

/**
 * @brief Converts a UTF-16 encoded array from Big-Endian (BE) format to Little-Endian (LE) format.
 *
 * This function takes an array of UTF-16 encoded data in Big-Endian format and converts it to Little-Endian format.
 * It ensures that the length of the input array is an even number since each UTF-16 character consists of 2 bytes.
 * The output array must be pre-allocated and of the same length as the input array.
 *
 * @param input Pointer to the input array containing UTF-16 BE encoded data.
 * @param output Pointer to the output array where UTF-16 LE encoded data will be stored.
 * @param length Length of the input and output arrays in bytes. Must be an even number.
 */
void utf16_be_to_le(const uint8_t *input, uint8_t *output, size_t length);


#ifdef __cplusplus
}
#endif

#endif
