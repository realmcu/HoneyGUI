/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_rendering_utils.c
  * @brief realui font rendering utils
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/17
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "font_rendering_utils.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/
// static const uint8_t alpha_list_2bpp[4] = {0, 0x55, 0xaa, 0xff};
static const uint8_t alpha_list_4bpp[16] = {0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

/*============================================================================*
 *                           Private Variables
 *============================================================================*/
// Pre-multiplied alpha lookup tables for 2/4 bit fonts
static uint8_t alpha_premul_2bpp[4];
static uint8_t alpha_premul_4bpp[16];
/*============================================================================*
 *                            Macros
 *============================================================================*/
/* User-configurable options (can be placed in header or compile options) */
#define CONFIG_USE_MVE     1  // [Config] Enable MVE acceleration
#define CONFIG_USE_TURBO   1  // [Config] Enable Turbo mode

/* Final rendering mode decision */
#if CONFIG_USE_MVE && defined(__ARM_FEATURE_MVE)
#include "arm_mve.h"
#define FONT_RENDERING_MVE   1
#else
#define FONT_RENDERING_MVE   0
#endif
#define FONT_RENDERING_TURBO CONFIG_USE_TURBO
#define FONT_RENDERING_STABLE 1
/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static inline void init_alpha_premul_2bpp(uint8_t font_alpha)
{
    // 2-bit font: 4 levels (0, 0x55, 0xaa, 0xff)
    alpha_premul_2bpp[0] = 0;
    alpha_premul_2bpp[1] = _UI_UDIV255(0x55 * font_alpha);
    alpha_premul_2bpp[2] = _UI_UDIV255(0xaa * font_alpha);
    alpha_premul_2bpp[3] = font_alpha;
}
static inline void init_alpha_premul_4bpp(uint8_t font_alpha)
{
    // 4-bit font: 16 levels (0x00, 0x11, 0x22, ..., 0xff)
    for (int i = 0; i < 16; i++)
    {
        alpha_premul_4bpp[i] = _UI_UDIV255(alpha_list_4bpp[i] * font_alpha);
    }
}

#if FONT_RENDERING_TURBO
static void font_render_1bpp_to_RGB565_turbo(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    const uint16_t color_output = rgba2565(font->color);
    const uint8_t font_alpha = font->color.color.rgba.a;
    const uint8_t ppb = 8;
    const int x_start = font->clip_rect.x1;
    const int y_start = font->clip_rect.y1;
    const int x_end = font->clip_rect.x2 + 1;
    const int y_end = font->clip_rect.y2 + 1;

    const int write_stride = font->target_buf_stride / 2;
    const int dots_stride = glyph->stride / ppb;
    const int glyph_x = glyph->pos_x;
    const int glyph_y = glyph->pos_y;

    int left_offset = 0, right_offset = 0;
    uint32_t x_start_right = x_start;
    if (glyph_x + glyph->width > x_end)
    {
        const int overflow_bytes = (x_end - glyph_x + ppb - 1) / ppb;
        right_offset = x_end - glyph_x - ppb * overflow_bytes + ppb;
    }
    if (glyph_x < x_start)
    {
        const int underflow_bytes = (x_start - glyph_x + ppb - 1) / ppb;
        left_offset = glyph_x + ppb * underflow_bytes - x_start;
        x_start_right = _UI_MIN((x_start + left_offset), x_end);
    }

    int write_off = (y_start - font->target_rect.y1) * write_stride;
    int dots_off = (y_start - glyph_y) * dots_stride;

    if (font_alpha == 0xff)
    {
        for (int i = y_start; i < y_end; i++)
        {
            uint8_t *current_byte = &dots[dots_off];

            for (uint32_t j = x_start; j < x_start_right; j++)
            {
                const uint32_t byte_pos = (j - glyph_x) / ppb;
                const uint8_t bit_pos = (j - glyph_x) % ppb;
                if (dots[dots_off + byte_pos] & (1 << bit_pos))
                {
                    writebuf[write_off + j - font->target_rect.x1] = color_output;
                }
            }

            const int middle_start = x_start + left_offset;
            const int middle_end = x_end - right_offset;
            const int pixel_offset = middle_start - glyph_x;
            uint8_t *temp_p = current_byte + (pixel_offset / ppb);

            for (int j = middle_start; j < middle_end; j += ppb, temp_p++)
            {
                const uint8_t byte = *temp_p;
                if (!byte) { continue; }

                const int base_offset = write_off + j - font->target_rect.x1;

                if (byte & 0x0F)
                {
                    if (byte & 0x01) { writebuf[base_offset + 0] = color_output; }
                    if (byte & 0x02) { writebuf[base_offset + 1] = color_output; }
                    if (byte & 0x04) { writebuf[base_offset + 2] = color_output; }
                    if (byte & 0x08) { writebuf[base_offset + 3] = color_output; }
                }
                if (byte & 0xF0)
                {
                    if (byte & 0x10) { writebuf[base_offset + 4] = color_output; }
                    if (byte & 0x20) { writebuf[base_offset + 5] = color_output; }
                    if (byte & 0x40) { writebuf[base_offset + 6] = color_output; }
                    if (byte & 0x80) { writebuf[base_offset + 7] = color_output; }
                }
            }

            for (int j = x_end - right_offset; j < x_end; j++)
            {
                const uint32_t byte_pos = (j - glyph_x) / ppb;
                const uint8_t bit_pos = (j - glyph_x) % ppb;
                if (dots[dots_off + byte_pos] & (1 << bit_pos))
                {
                    writebuf[write_off + j - font->target_rect.x1] = color_output;
                }
            }

            write_off += write_stride;
            dots_off += dots_stride;
        }
    }
    else
    {
        for (int i = y_start; i < y_end; i++)
        {
            uint8_t *current_byte = &dots[dots_off];

            for (uint32_t j = x_start; j < x_start_right; j++)
            {
                const uint32_t byte_pos = (j - glyph_x) / ppb;
                const uint8_t bit_pos = (j - glyph_x) % ppb;
                if (dots[dots_off + byte_pos] & (1 << bit_pos))
                {
                    uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                      font_alpha);
                }
            }

            const int middle_start = x_start + left_offset;
            const int middle_end = x_end - right_offset;
            const int pixel_offset = middle_start - glyph_x;
            uint8_t *temp_p = current_byte + (pixel_offset / ppb);

            for (int j = middle_start; j < middle_end; j += ppb, temp_p++)
            {
                const uint8_t byte = *temp_p;
                if (!byte) { continue; }

                const int base_offset = write_off + j - font->target_rect.x1;

                if (byte & 0x0F)
                {
                    if (byte & 0x01) { uint16_t cb = writebuf[base_offset + 0]; writebuf[base_offset + 0] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x02) { uint16_t cb = writebuf[base_offset + 1]; writebuf[base_offset + 1] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x04) { uint16_t cb = writebuf[base_offset + 2]; writebuf[base_offset + 2] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x08) { uint16_t cb = writebuf[base_offset + 3]; writebuf[base_offset + 3] = alphaBlendRGB565(color_output, cb, font_alpha); }
                }
                if (byte & 0xF0)
                {
                    if (byte & 0x10) { uint16_t cb = writebuf[base_offset + 4]; writebuf[base_offset + 4] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x20) { uint16_t cb = writebuf[base_offset + 5]; writebuf[base_offset + 5] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x40) { uint16_t cb = writebuf[base_offset + 6]; writebuf[base_offset + 6] = alphaBlendRGB565(color_output, cb, font_alpha); }
                    if (byte & 0x80) { uint16_t cb = writebuf[base_offset + 7]; writebuf[base_offset + 7] = alphaBlendRGB565(color_output, cb, font_alpha); }
                }
            }

            for (int j = x_end - right_offset; j < x_end; j++)
            {
                const uint32_t byte_pos = (j - glyph_x) / ppb;
                const uint8_t bit_pos = (j - glyph_x) % ppb;
                if (dots[dots_off + byte_pos] & (1 << bit_pos))
                {
                    uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                      font_alpha);
                }
            }

            write_off += write_stride;
            dots_off += dots_stride;
        }
    }
}
#elif FONT_RENDERING_STABLE
static void font_render_1bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    writebuf[write_off + j - font->target_rect.x1] = color_output;
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                      font_alpha);
                }
            }
        }
    }
}
#endif
static void font_render_1bpp_to_ARGB8565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t color_outputh = color_output >> 8;
    uint8_t color_outputl = color_output & 0xff;
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
            {
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 2] = font->color.color.rgba.a;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 1] = color_outputh;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 0] = color_outputl;
            }
        }
    }
}
static void font_render_1bpp_to_RGB888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t color_output[3];
    color_output[0] = font->color.color.rgba.b;
    color_output[1] = font->color.color.rgba.g;
    color_output[2] = font->color.color.rgba.r;
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color_output, 3);
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    uint8_t color_back[3];
                    memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                    uint8_t color[3];
                    color[0] = _UI_UDIV255(color_output[0] * font_alpha) + _UI_UDIV255(color_back[0] *
                                                                                       (255 - font_alpha));
                    color[1] = _UI_UDIV255(color_output[1] * font_alpha) + _UI_UDIV255(color_back[1] *
                                                                                       (255 - font_alpha));
                    color[2] = _UI_UDIV255(color_output[2] * font_alpha) + _UI_UDIV255(color_back[2] *
                                                                                       (255 - font_alpha));
                    memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
                }
            }
        }
    }
}

static void font_render_1bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        uint32_t color_output = font->color.color.argb_full;
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    writebuf[write_off + j - font->target_rect.x1] = color_output;
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
                {
                    uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back,
                                                                                    font_alpha);
                }
            }
        }
    }
}
#if FONT_RENDERING_TURBO
static void font_render_2bpp_to_RGB565_turbo(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 4;//pixel_per_byte = 8 / rendor_mode
    const int x_start = font->clip_rect.x1;
    const int y_start = font->clip_rect.y1;
    const int x_end = font->clip_rect.x2 + 1;
    const int y_end = font->clip_rect.y2 + 1;

    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;

    int write_off = (y_start - font->target_rect.y1) * write_stride;
    int dots_off = (y_start - glyph->pos_y) * dots_stride;
    init_alpha_premul_2bpp(font->color.color.rgba.a);

    int left_offset = 0, right_offset = 0, byte = 0;
    uint16_t color_back;
    uint8_t alpha_2bit;
    if (glyph->pos_x + glyph->width > x_end)
    {
        while (glyph->pos_x + ppb * byte < x_end)
        {
            byte++;
        }
        right_offset = x_end - glyph->pos_x - ppb * byte + ppb;
        byte = 0;
    }
    if (glyph->pos_x < x_start)
    {
        byte = 0;
        while (glyph->pos_x + ppb * byte < x_start)
        {
            byte++;
        }
        left_offset = glyph->pos_x + ppb * byte - x_start;
    }
    for (int i = y_start; i < y_end; i++)
    {
        uint8_t *temp_p = &dots[dots_off + byte];

        for (int j = x_start; j < x_start + left_offset; j++)
        {
            uint8_t alpha_idx = dots[(i - glyph->pos_y) * (glyph->stride / ppb) + (j - glyph->pos_x) / ppb] >>
                                ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx != 0)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
        for (int j = x_start + left_offset; j < x_end - right_offset;)
        {
            uint8_t alpha = *temp_p;
            if (alpha != 0)
            {
                if (alpha & 0x03)
                {
                    alpha_2bit = alpha & 0x03;
                    uint8_t alpha_val = alpha_premul_2bpp[alpha_2bit];
                    if (alpha_val == 0xff)
                    {
                        writebuf[write_off + j - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        color_back = writebuf[write_off + j - font->target_rect.x1];
                        writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                          alpha_val);
                    }
                }
                if (alpha & 0x0C)
                {
                    alpha_2bit = (alpha >> 2) & 0x03;
                    uint8_t alpha_val = alpha_premul_2bpp[alpha_2bit];
                    if (alpha_val == 0xff)
                    {
                        writebuf[write_off + j + 1 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        color_back = writebuf[write_off + j + 1 - font->target_rect.x1];
                        writebuf[write_off + j + 1 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_val);
                    }
                }
                if (alpha & 0x30)
                {
                    alpha_2bit = (alpha >> 4) & 0x03;
                    uint8_t alpha_val = alpha_premul_2bpp[alpha_2bit];
                    if (alpha_val == 0xff)
                    {
                        writebuf[write_off + j + 2 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        color_back = writebuf[write_off + j + 2 - font->target_rect.x1];
                        writebuf[write_off + j + 2 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_val);
                    }
                }
                if (alpha & 0xC0)
                {
                    alpha_2bit = (alpha >> 6) & 0x03;
                    uint8_t alpha_val = alpha_premul_2bpp[alpha_2bit];
                    if (alpha_val == 0xff)
                    {
                        writebuf[write_off + j + 3 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        color_back = writebuf[write_off + j + 3 - font->target_rect.x1];
                        writebuf[write_off + j + 3 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_val);
                    }
                }
            }
            temp_p++;
            j += ppb;
        }
        for (int j = x_end - right_offset; j < x_end; j++)
        {
            uint8_t alpha_idx = dots[(i - glyph->pos_y) * (glyph->stride / ppb) + (j - glyph->pos_x) / ppb] >>
                                ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx != 0)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
        write_off += write_stride;
        dots_off += (glyph->stride / ppb);
    }
}
#elif FONT_RENDERING_STABLE
static void font_render_2bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 4; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_2bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
    }
}
#endif
static void font_render_2bpp_to_RGB888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t color_output[3];
    color_output[0] = font->color.color.rgba.b;
    color_output[1] = font->color.color.rgba.g;
    color_output[2] = font->color.color.rgba.r;
    uint8_t ppb = 4; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_2bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                uint8_t color_back[3];
                memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                uint8_t color[3];
                color[0] = _UI_UDIV255(color_output[0] * alpha) + _UI_UDIV255(color_back[0] * (255 - alpha));
                color[1] = _UI_UDIV255(color_output[1] * alpha) + _UI_UDIV255(color_back[1] * (255 - alpha));
                color[2] = _UI_UDIV255(color_output[2] * alpha) + _UI_UDIV255(color_back[2] * (255 - alpha));
                memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
            }
        }
    }
}

/*Consider ARGB8565 format as cache, no need to compute background color.*/
static void font_render_2bpp_to_ARGB8565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t color_outputh = color_output >> 8;
    uint8_t color_outputl = color_output & 0xff;
    uint8_t ppb = 4; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_2bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 2] = alpha;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 1] = color_outputh;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 0] = color_outputl;
            }
        }
    }
}

static void font_render_2bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint8_t ppb = 4; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_2bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha_idx &= 0x03;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_2bpp[alpha_idx];
                uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back, alpha);
            }
        }
    }
}

static void font_render_4bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 2; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_4bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha_idx &= 0x0f;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_4bpp[alpha_idx];
                uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
    }
}

static void font_render_4bpp_to_RGB888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t color_output[3];
    color_output[0] = font->color.color.rgba.b;
    color_output[1] = font->color.color.rgba.g;
    color_output[2] = font->color.color.rgba.r;
    uint8_t ppb = 2; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_4bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha_idx &= 0x0f;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_4bpp[alpha_idx];
                uint8_t color_back[3];
                memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                uint8_t color[3];
                color[0] = _UI_UDIV255(color_output[0] * alpha) + _UI_UDIV255(color_back[0] * (255 - alpha));
                color[1] = _UI_UDIV255(color_output[1] * alpha) + _UI_UDIV255(color_back[1] * (255 - alpha));
                color[2] = _UI_UDIV255(color_output[2] * alpha) + _UI_UDIV255(color_back[2] * (255 - alpha));
                memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
            }
        }
    }
}

/*Consider ARGB8565 format as cache, no need to compute background color.*/
static void font_render_4bpp_to_ARGB8565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t color_outputh = color_output >> 8;
    uint8_t color_outputl = color_output & 0xff;
    uint8_t ppb = 2; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_4bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha_idx &= 0x0f;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_4bpp[alpha_idx];
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 2] = alpha;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 1] = color_outputh;
                writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 0] = color_outputl;
            }
        }
    }
}

static void font_render_4bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint8_t ppb = 2; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;
    init_alpha_premul_4bpp(font->color.color.rgba.a);

    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha_idx = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha_idx &= 0x0f;
            if (alpha_idx)
            {
                uint8_t alpha = alpha_premul_4bpp[alpha_idx];
                uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back, alpha);
            }
        }
    }
}

#if FONT_RENDERING_MVE
static void font_render_8bpp_to_RGB565_mve(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint16_t color_back;
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride;

    const int x_start = font->clip_rect.x1;
    const int y_start = font->clip_rect.y1;
    const int x_end = font->clip_rect.x2 + 1;
    const int y_end = font->clip_rect.y2 + 1;

    uint32_t loopCount = (x_end - x_start) / 8;
    uint32_t loopsLeft = (x_end - x_start) % 8;


    bool max_opacity = false;

    if (font->color.color.rgba.a == 0xff)
    {
        max_opacity = true;
    }
    for (int i = y_start; i < y_end; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride - font->target_rect.x1;
        int dots_off = (i - glyph->pos_y) * dots_stride - glyph->pos_x;

        /*helium code start*/
        for (uint32_t loopc = 0; loopc < loopCount; loopc ++)
        {
            uint16_t js = x_start + 8 * loopc;
            uint16x8_t alphav = vldrbq_u16(&dots[dots_off + js]);

            if (vaddvq_u16(alphav) == 0)
            {
                continue;
            }
            if (!max_opacity)
            {
                alphav = vmulq_n_u16(alphav, (uint16_t)font->color.color.rgba.a);
                alphav = vrshrq_n_u16(alphav, 8);
            }

            uint16x8_t outrv = vmulq_n_u16(alphav, (uint16_t)font->color.color.rgba.r);
            uint16x8_t outgv = vmulq_n_u16(alphav, (uint16_t)font->color.color.rgba.g);
            uint16x8_t outbv = vmulq_n_u16(alphav, (uint16_t)font->color.color.rgba.b);

            //read back color
            uint16x8_t color_backv = vld1q(&writebuf[write_off + js]);

            uint16x8_t color_backr = vbicq_n_u16(color_backv, 0x0700);
            color_backr = vshrq_n_u16(color_backr, 8);
            uint16x8_t color_backg = vbicq_n_u16(color_backv, 0xF800);
            color_backg = vbicq_n_u16(color_backg, 0x001F);
            color_backg = vshrq_n_u16(color_backg, 3);
            uint16x8_t color_backb = vbicq_n_u16(color_backv, 0xFF00);
            color_backb = vbicq_n_u16(color_backb, 0x00E0);
            color_backb = vshlq_n_u16(color_backb, 3);

            uint16x8_t alphabv = vdupq_n_u16(0xff);
            alphabv = vsubq_u16(alphabv, alphav);

            color_backr = vmulq_u16(color_backr, alphabv);
            color_backg = vmulq_u16(color_backg, alphabv);
            color_backb = vmulq_u16(color_backb, alphabv);

            outrv = vaddq_u16(outrv, color_backr);
            outgv = vaddq_u16(outgv, color_backg);
            outbv = vaddq_u16(outbv, color_backb);

            /*vsriq, 6 instructions*/
            // uint16x8_t resultv = vsriq_n_u16(outgv, outbv, 6);
            // resultv = vsriq_n_u16(outrv, resultv, 5);

            /*work around, 10 instructions*/
            uint16x8_t resultv = vshrq_n_u16(outbv, 11);
            outrv = vandq_u16(outrv, vdupq_n_u16(0xF800));
            outgv = vshlq_n_u16(vshrq_n_u16(outgv, 10), 5);
            resultv = vorrq_u16(resultv, outrv);
            resultv = vorrq_u16(resultv, outgv);

            vst1q_u16(&writebuf[write_off + js], resultv);
        }
        /*helium code end*/
        for (int j = x_end - loopsLeft; j < x_end; j++)
        {
            uint8_t alpha = dots[dots_off + j];
            if (alpha)
            {
                alpha = _UI_UDIV255(font->color.color.rgba.a * alpha);
                color_back = writebuf[write_off + j];
                writebuf[write_off + j] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
    }
}
#elif FONT_RENDERING_STABLE
static void font_render_8bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    alpha = _UI_UDIV255(font_alpha * alpha);
                    uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
                }
            }
        }
    }
}
#endif

static void font_render_8bpp_to_ARGB8565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t color_outputh = color_output >> 8;
    uint8_t color_outputl = color_output & 0xff;
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 2] = alpha;
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 1] = color_outputh;
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 0] = color_outputl;
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    alpha = _UI_UDIV255(font_alpha * alpha);
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 2] = alpha;
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 1] = color_outputh;
                    writebuf[write_off + j * 3 - font->target_rect.x1 * 3 + 0] = color_outputl;
                }
            }
        }
    }
}

static void font_render_8bpp_to_RGB888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t color_output[3];
    color_output[0] = font->color.color.rgba.b;
    color_output[1] = font->color.color.rgba.g;
    color_output[2] = font->color.color.rgba.r;
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x)  / ppb];
                if (alpha)
                {
                    uint8_t color_back[3];
                    memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                    uint8_t color[3];
                    color[0] = _UI_UDIV255(color_output[0] * alpha) + _UI_UDIV255(color_back[0] * (255 - alpha));
                    color[1] = _UI_UDIV255(color_output[1] * alpha) + _UI_UDIV255(color_back[1] * (255 - alpha));
                    color[2] = _UI_UDIV255(color_output[2] * alpha) + _UI_UDIV255(color_back[2] * (255 - alpha));
                    memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x)  / ppb];
                if (alpha)
                {
                    alpha = _UI_UDIV255(font_alpha * alpha);
                    uint8_t color_back[3];
                    memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                    uint8_t color[3];
                    color[0] = _UI_UDIV255(color_output[0] * alpha) + _UI_UDIV255(color_back[0] * (255 - alpha));
                    color[1] = _UI_UDIV255(color_output[1] * alpha) + _UI_UDIV255(color_back[1] * (255 - alpha));
                    color[2] = _UI_UDIV255(color_output[2] * alpha) + _UI_UDIV255(color_back[2] * (255 - alpha));
                    memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
                }
            }
        }
    }
}

static void font_render_8bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint8_t font_alpha = font->color.color.rgba.a;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;

    if (font_alpha == 0xff)
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back, alpha);
                }
            }
        }
    }
    else
    {
        for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
        {
            int write_off = (i - font->target_rect.y1) * write_stride;
            int dots_off = (i - glyph->pos_y) * dots_stride;
            for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
            {
                uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
                if (alpha)
                {
                    alpha = _UI_UDIV255(font_alpha * alpha);
                    uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                    writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back, alpha);
                }
            }
        }
    }
}

static void font_render_8bpp_to_ALPHA8BIT(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    for (int16_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;

        memcpy(&writebuf[write_off + font->clip_rect.x1 - font->target_rect.x1],
               &dots[dots_off + font->clip_rect.x1 - glyph->pos_x],
               font->clip_rect.x2 - font->clip_rect.x1 + 1);
        // for (int16_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        // {
        //     uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb];
        //     writebuf[write_off + j - font->target_rect.x1] = alpha;
        // }
    }
}

static void font_render_1bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    /* Version performance markers:
     * Stable   - 100% speed baseline
     * Turbo    - 200% faster (2x speedup) in tests
     */
#if FONT_RENDERING_TURBO
    font_render_1bpp_to_RGB565_turbo(font, glyph);
#elif FONT_RENDERING_STABLE
    font_render_1bpp_to_RGB565_stable(font, glyph);
#endif
}
static void font_render_1bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_ARGB8565_stable(font, glyph);
}
static void font_render_1bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_ARGB8888_stable(font, glyph);
}
static void font_render_1bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_RGB888_stable(font, glyph);
}

static void font_render_2bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    /* Version performance markers:
     * Stable   - 100% speed baseline
     * Turbo    - 154% faster (1.54x speedup) in tests
     */
#if FONT_RENDERING_TURBO
    font_render_2bpp_to_RGB565_turbo(font, glyph);
#elif FONT_RENDERING_STABLE
    font_render_2bpp_to_RGB565_stable(font, glyph);
#endif
}
static void font_render_2bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_RGB888_stable(font, glyph);
}
static void font_render_2bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_ARGB8565_stable(font, glyph);
}
static void font_render_2bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_ARGB8888_stable(font, glyph);
}

static void font_render_4bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_RGB565_stable(font, glyph);
}
static void font_render_4bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_RGB888_stable(font, glyph);
}
static void font_render_4bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_ARGB8565_stable(font, glyph);
}
static void font_render_4bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_ARGB8888_stable(font, glyph);
}

static void font_render_8bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    /* Version performance markers:
     * Stable   - 100% speed baseline
     * MVE      - 250% faster (2.5x speedup) in tests
     */
#if FONT_RENDERING_MVE
    font_render_8bpp_to_RGB565_mve(font, glyph);
#elif FONT_RENDERING_STABLE
    font_render_8bpp_to_RGB565_stable(font, glyph);
#endif
}
static void font_render_8bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_ARGB8565_stable(font, glyph);
}
static void font_render_8bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_RGB888_stable(font, glyph);
}
static void font_render_8bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_ARGB8888_stable(font, glyph);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void font_glyph_render(draw_font_t *font, font_glyph_t *glyph)
{
    if (glyph->data == NULL) { return; }

    switch (font->render_mode)
    {
    case 1:
        switch (font->target_format)
        {
        case RGB565:
            font_render_1bpp_to_RGB565(font, glyph);
            break;
        case RGB888:
            font_render_1bpp_to_RGB888(font, glyph);
            break;
        case ARGB8565:
            font_render_1bpp_to_ARGB8565(font, glyph);
            break;
        case ARGB8888:
            font_render_1bpp_to_ARGB8888(font, glyph);
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (font->target_format)
        {
        case RGB565:
            font_render_2bpp_to_RGB565(font, glyph);
            break;
        case RGB888:
            font_render_2bpp_to_RGB888(font, glyph);
            break;
        case ARGB8565:
            font_render_2bpp_to_ARGB8565(font, glyph);
            break;
        case ARGB8888:
            font_render_2bpp_to_ARGB8888(font, glyph);
            break;
        default:
            break;
        }
        break;
    case 4:
        switch (font->target_format)
        {
        case RGB565:
            font_render_4bpp_to_RGB565(font, glyph);
            break;
        case RGB888:
            font_render_4bpp_to_RGB888(font, glyph);
            break;
        case ARGB8565:
            font_render_4bpp_to_ARGB8565(font, glyph);
            break;
        case ARGB8888:
            font_render_4bpp_to_ARGB8888(font, glyph);
            break;
        default:
            break;
        }
        break;
    case 8:
        switch (font->target_format)
        {
        case RGB565:
            font_render_8bpp_to_RGB565(font, glyph);
            break;
        case RGB888:
            font_render_8bpp_to_RGB888(font, glyph);
            break;
        case ARGB8565:
            font_render_8bpp_to_ARGB8565(font, glyph);
            break;
        case ARGB8888:
            font_render_8bpp_to_ARGB8888(font, glyph);
            break;
        case GRAY8:
            font_render_8bpp_to_ALPHA8BIT(font, glyph);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

uint8_t font_get_bitdepth_by_cf(GUI_FormatType cf)
{
    uint8_t bitdepth = 0;
    switch (cf)
    {
    case RGB565:
        bitdepth = 16;
        break;
    case ARGB8565:
    case RGB888:
        bitdepth = 24;
        break;
    case ARGB8888:
        bitdepth = 32;
        break;
    default:
        break;
    }
    return bitdepth;
}

