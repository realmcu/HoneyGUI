/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_rendering_utils.h
  * @brief
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/17
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __FONT_RENDERING_UTILES_H__
#define __FONT_RENDERING_UTILES_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "def_color.h"
#include "def_list.h"
#include "def_type.h"
#include "draw_img.h"
/*============================================================================*
 *                         Types
 *============================================================================*/
typedef struct draw_font
{
    uint8_t render_mode;

    gui_color_t color;
    gui_rect_t clip_rect;

    uint8_t *target_buf;
    gui_rect_t target_rect;
    uint16_t target_buf_stride;
    GUI_FormatType target_format;

    // gui_matrix_t transform;
    // BLEND_MODE_TYPE blend_mode;
} draw_font_t;

typedef struct font_glyph
{
    uint8_t *data;
    int16_t pos_x;
    int16_t pos_y;
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} font_glyph_t;

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

// Fast RGB565 pixel blending
// Found in a pull request for the Adafruit framebuffer library. Clever!
// https://github.com/tricorderproject/arducordermini/pull/1/files#diff-d22a481ade4dbb4e41acc4d7c77f683d
gui_inline uint16_t alphaBlendRGB565(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31]
    alpha = (alpha + 4) >> 3;
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply
    // bg = (bg | (bg << 16)) & 0b00000111111000001111100000011111;
    // fg = (fg | (fg << 16)) & 0b00000111111000001111100000011111;
    bg = (bg | (bg << 16)) & 0x7e0f81f;
    fg = (fg | (fg << 16)) & 0x7e0f81f;
    // This implements the linear interpolation formula: result = bg * (1.0 - alpha) + fg * alpha
    // This can be factorized into: result = bg + (fg - bg) * alpha
    // alpha is in Q1.5 format, so 0.0 is represented by 0, and 1.0 is represented by 32
    uint32_t result = (fg - bg) * alpha; // parallel fixed-point multiply of all components
    result >>= 5;
    result += bg;
    // result &= 0b00000111111000001111100000011111; // mask out fractional parts
    result &= 0x7e0f81f;
    return (uint16_t)((result >> 16) | result); // contract result
}

gui_inline uint32_t alphaBlendRGBA(gui_color_t fg, uint32_t bg, uint8_t alpha)
{
    uint32_t mix;
    uint8_t back_a = 0xff - alpha;

    mix = 0xff000000;
    mix += ((bg >> 16 & 0xff) * back_a + fg.color.rgba.r * alpha) / 0xff << 16;
    mix += ((bg >>  8 & 0xff) * back_a + fg.color.rgba.g * alpha) / 0xff <<  8;
    mix += ((bg >>  0 & 0xff) * back_a + fg.color.rgba.b * alpha) / 0xff <<  0;

    return mix;
}

gui_inline uint16_t rgba2565(gui_color_t rgba)
{
    uint16_t red = rgba.color.rgba.r * 0x1f / 0xff << 11;
    uint16_t gre = rgba.color.rgba.g * 0x3f / 0xff << 5;
    uint16_t blu = rgba.color.rgba.b * 0x1f / 0xff;
    return red + gre + blu;
}

/**
 * @brief Render glyph to target buffer
 *
 * @param font Font rendering context containing color, clipping rect, target buffer config
 * @param glyph Glyph data including bitmap, position and dimensions
 */
void font_glyph_render(draw_font_t *font, font_glyph_t *glyph);

/**
 * @brief Get bit depth for given color format
 *
 * @param cf Color format type, supported values:
 *          - RGB565     : 16-bit (5-6-5)
 *          - ARGB8565   : 24-bit (8-5-6-5)
 *          - RGB888     : 24-bit (8-8-8)
 *          - ARGB8888   : 32-bit (8-8-8-8)
 * @return uint8_t Corresponding bit depth (16/24/32), returns 0 for unsupported formats
 */
uint8_t font_get_bitdepth_by_cf(GUI_FormatType cf);

#ifdef __cplusplus
}
#endif

#endif
