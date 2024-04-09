#ifndef __DRAW_FONT_H__
#define __DRAW_FONT_H__

#include <guidef.h>
#include <gui_api.h>
#include <gui_text.h>
#include "font_mem.h"
#include "font_stb.h"
#ifdef __cplusplus
extern "C" {
#endif
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

#ifdef __cplusplus
}
#endif

#endif
