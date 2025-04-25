/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_rendering_utils.c
  * @brief realui font rendering utils
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/17
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
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
static const uint8_t alpha_list_2bpp[4] = {0, 0x55, 0xaa, 0xff};
static const uint8_t alpha_list_4bpp[16] = {0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
/*============================================================================*
 *                            Macros
 *============================================================================*/
/* Architecture feature detection */
#if defined(__ARM_FEATURE_MVE)
#define FONT_RENDERING_MVE  1  // [Config] Enable MVE acceleration (0-disable)
#include "arm_mve.h"
#else
#define FONT_RENDERING_MVE  0  // Auto-set based on arch
#endif

/* Rendering pipeline configuration */
#if FONT_RENDERING_MVE
#define FONT_RENDERING_TURBO  0  // Disable Turbo when MVE active
#define FONT_RENDERING_STABLE 0  // Disable Stable when MVE active
#else
#define FONT_RENDERING_TURBO  1  // [Config] Turbo mode (0-enables Stable)
#define FONT_RENDERING_STABLE (!FONT_RENDERING_TURBO) // Auto-fallback
#endif
/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
#if FONT_RENDERING_STABLE
static void font_render_1bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
            {
                writebuf[write_off + j - font->target_rect.x1] = color_output;
            }
        }
    }
}
#endif
#if FONT_RENDERING_TURBO
static void font_render_1bpp_to_RGB565_turbo(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    const uint16_t color_output = rgba2565(font->color);
    const uint8_t ppb = 8;
    const int x_start = font->clip_rect.x1;
    const int y_start = font->clip_rect.y1;
    const int x_end = font->clip_rect.x2 + 1;
    const int y_end = font->clip_rect.y2 + 1;

    const int write_stride = font->target_buf_stride / 2;
    const int dots_stride = glyph->stride / ppb;
    const int glyph_x = glyph->pos_x;
    const int glyph_y = glyph->pos_y;

    int write_off = (y_start - font->target_rect.y1) * write_stride;
    int dots_off = (y_start - glyph_y) * dots_stride;

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

    for (uint32_t i = y_start; i < y_end; i++)
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

        for (uint32_t j = x_end - right_offset; j < x_end; j++)
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
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
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
            {
                memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color_output, 3);
            }
        }
    }
}

static void font_render_1bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint32_t color_output = font->color.color.argb_full;
    uint8_t ppb = 8; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            if ((dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb)) & 0x01)
            {
                writebuf[write_off + j - font->target_rect.x1] = color_output;
            }
        }
    }
}
#if FONT_RENDERING_STABLE
static void font_render_2bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 4; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha &= 0x03;
            if (alpha)
            {
                alpha = alpha_list_2bpp[alpha];
                uint16_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
    }
}
#endif
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
    for (uint32_t i = y_start; i < y_end; i++)
    {
        uint8_t *temp_p = &dots[dots_off + byte];

        for (uint32_t j = x_start; j < x_start + left_offset; j++)
        {
            uint8_t alpha = dots[(i - glyph->pos_y) * (glyph->stride / ppb) + (j - glyph->pos_x) / ppb] >>
                            ((j - glyph->pos_x) % ppb * 2);
            if (alpha != 0)
            {
                alpha = alpha & 0x03;
                alpha = alpha * 85;
                // alpha = font->color.color.rgba.a * alpha / 0xff;
                color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
        for (uint32_t j = x_start + left_offset; j < x_end - right_offset;)
        {
            uint8_t alpha = *temp_p;
            if (alpha != 0)
            {
                if (alpha & 0x03)
                {
                    alpha_2bit = alpha & 0x03;
                    if (alpha_2bit == 0x03)
                    {
                        writebuf[write_off + j - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        alpha_2bit *= 85;
                        // alpha_2bit = font->color.color.rgba.a * alpha_2bit / 0xff;
                        color_back = writebuf[write_off + j - font->target_rect.x1];
                        writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                          alpha_2bit);
                    }
                }
                if (alpha & 0x0C)
                {
                    alpha_2bit = (alpha >> 2) & 0x03;
                    if (alpha_2bit == 0x03)
                    {
                        writebuf[write_off + j + 1 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        alpha_2bit *= 85;
                        // alpha_2bit = font->color.color.rgba.a * alpha_2bit / 0xff;
                        color_back = writebuf[write_off + j + 1 - font->target_rect.x1];
                        writebuf[write_off + j + 1 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_2bit);
                    }
                }
                if (alpha & 0x30)
                {
                    alpha_2bit = (alpha >> 4) & 0x03;
                    if (alpha_2bit == 0x03)
                    {
                        writebuf[write_off + j + 2 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        alpha_2bit *= 85;
                        // alpha_2bit = font->color.color.rgba.a * alpha_2bit / 0xff;
                        color_back = writebuf[write_off + j + 2 - font->target_rect.x1];
                        writebuf[write_off + j + 2 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_2bit);
                    }
                }
                if (alpha & 0xC0)
                {
                    alpha_2bit = (alpha >> 6) & 0x03;
                    if (alpha_2bit == 0x03)
                    {
                        writebuf[write_off + j + 3 - font->target_rect.x1] = color_output;
                    }
                    else
                    {
                        alpha_2bit *= 85;
                        // alpha_2bit = font->color.color.rgba.a * alpha_2bit / 0xff;
                        color_back = writebuf[write_off + j + 3 - font->target_rect.x1];
                        writebuf[write_off + j + 3 - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back,
                                                                                              alpha_2bit);
                    }
                }
            }
            temp_p++;
            j += ppb;
        }
        for (uint32_t j = x_end - right_offset; j < x_end; j++)
        {
            uint8_t alpha = dots[(i - glyph->pos_y) * (glyph->stride / ppb) + (j - glyph->pos_x) / ppb] >>
                            ((j - glyph->pos_x) % ppb * 2);
            if (alpha != 0)
            {
                alpha = alpha & 0x03;
                alpha = alpha * 85;
                // alpha = font->color.color.rgba.a * alpha / 0xff;
                color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGB565(color_output, color_back, alpha);
            }
        }
        write_off += font->target_rect.x2 - font->target_rect.x1 + 1;
        dots_off += (glyph->stride / ppb);
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha &= 0x03;
            if (alpha)
            {
                alpha = alpha_list_2bpp[alpha];
                uint8_t color_back[3];
                memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                uint8_t color[3];
                color[0] = color_output[0] * alpha / 255 + color_back[0] * (255 - alpha) / 255;
                color[1] = color_output[1] * alpha / 255 + color_back[1] * (255 - alpha) / 255;
                color[2] = color_output[2] * alpha / 255 + color_back[2] * (255 - alpha) / 255;
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha &= 0x03;
            if (alpha)
            {
                alpha = alpha_list_2bpp[alpha];
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 2);
            alpha &= 0x03;
            if (alpha)
            {
                alpha = alpha_list_2bpp[alpha];
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha &= 0x0f;
            if (alpha)
            {
                alpha = alpha_list_4bpp[alpha];
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha &= 0x0f;
            if (alpha)
            {
                alpha = alpha_list_4bpp[alpha];
                uint8_t color_back[3];
                memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                uint8_t color[3];
                color[0] = color_output[0] * alpha / 255 + color_back[0] * (255 - alpha) / 255;
                color[1] = color_output[1] * alpha / 255 + color_back[1] * (255 - alpha) / 255;
                color[2] = color_output[2] * alpha / 255 + color_back[2] * (255 - alpha) / 255;
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha &= 0x0f;
            if (alpha)
            {
                alpha = alpha_list_4bpp[alpha];
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
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x) / ppb] >> ((j - glyph->pos_x) % ppb * 4);
            alpha &= 0x0f;
            if (alpha)
            {
                alpha = alpha_list_4bpp[alpha];
                uint32_t color_back = writebuf[write_off + j - font->target_rect.x1];
                writebuf[write_off + j - font->target_rect.x1] = alphaBlendRGBA(font->color, color_back, alpha);
            }
        }
    }
}

static void font_render_8bpp_to_RGB565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint16_t *writebuf = (uint16_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 2;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
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

static void font_render_8bpp_to_ARGB8565_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint16_t color_output = rgba2565(font->color);
    uint8_t color_outputh = color_output >> 8;
    uint8_t color_outputl = color_output & 0xff;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
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

static void font_render_8bpp_to_RGB888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t color_output[3];
    color_output[0] = font->color.color.rgba.b;
    color_output[1] = font->color.color.rgba.g;
    color_output[2] = font->color.color.rgba.r;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;

    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
        {
            uint8_t alpha = dots[dots_off + (j - glyph->pos_x)  / ppb];
            if (alpha)
            {
                uint8_t color_back[3];
                memcpy(color_back, &writebuf[write_off + j * 3 - font->target_rect.x1 * 3], 3);
                uint8_t color[3];
                color[0] = color_output[0] * alpha / 255 + color_back[0] * (255 - alpha) / 255;
                color[1] = color_output[1] * alpha / 255 + color_back[1] * (255 - alpha) / 255;
                color[2] = color_output[2] * alpha / 255 + color_back[2] * (255 - alpha) / 255;
                memcpy(&writebuf[write_off + j * 3 - font->target_rect.x1 * 3], color, 3);
            }
        }
    }
}

static void font_render_8bpp_to_ARGB8888_stable(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint32_t *writebuf = (uint32_t *)font->target_buf;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride / 4;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;
        for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
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

static void font_render_8bpp_to_ALPHA8BIT(draw_font_t *font, font_glyph_t *glyph)
{
    uint8_t *dots = glyph->data;
    uint8_t *writebuf = (uint8_t *)font->target_buf;
    uint8_t ppb = 1; //pixel_per_byte = 8 / rendor_mode
    uint32_t write_stride = font->target_buf_stride;
    uint32_t dots_stride = glyph->stride / ppb;
    for (uint32_t i = font->clip_rect.y1; i <= font->clip_rect.y2; i++)
    {
        int write_off = (i - font->target_rect.y1) * write_stride;
        int dots_off = (i - glyph->pos_y) * dots_stride;

        memcpy(&writebuf[write_off + font->clip_rect.x1 - font->target_rect.x1],
               &dots[dots_off + font->clip_rect.x1 - glyph->pos_x],
               font->clip_rect.x2 - font->clip_rect.x1 + 1);
        // for (uint32_t j = font->clip_rect.x1; j <= font->clip_rect.x2; j++)
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
    font_render_1bpp_to_RGB565_turbo(font, glyph);      // 200% optimized
#endif
#if FONT_RENDERING_STABLE
    font_render_1bpp_to_RGB565_stable(font, glyph);     // 100% baseline
#endif
}
static void font_render_1bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_ARGB8565_stable(font, glyph);     // 100% baseline
}
static void font_render_1bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_ARGB8888_stable(font, glyph);     // 100% baseline
}
static void font_render_1bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_1bpp_to_RGB888_stable(font, glyph);     // 100% baseline
}

static void font_render_2bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    /* Version performance markers:
     * Stable   - 100% speed baseline
     * Turbo    - 154% faster (1.54x speedup) in tests
     */
#if FONT_RENDERING_TURBO
    font_render_2bpp_to_RGB565_turbo(font, glyph);      // 154% optimized
#endif
#if FONT_RENDERING_STABLE
    font_render_2bpp_to_RGB565_stable(font, glyph);     // 100% baseline
#endif
}
static void font_render_2bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_RGB888_stable(font, glyph);     // 100% baseline
}
static void font_render_2bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_ARGB8565_stable(font, glyph);     // 100% baseline
}
static void font_render_2bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_2bpp_to_ARGB8888_stable(font, glyph);     // 100% baseline
}

static void font_render_4bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_RGB565_stable(font, glyph);     // 100% baseline
}
static void font_render_4bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_RGB888_stable(font, glyph);     // 100% baseline
}
static void font_render_4bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_ARGB8565_stable(font, glyph);     // 100%
}
static void font_render_4bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_4bpp_to_ARGB8888_stable(font, glyph);     // 100% baseline
}

static void font_render_8bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_RGB565_stable(font, glyph);     // 100% baseline
}
static void font_render_8bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_ARGB8565_stable(font, glyph);     // 100% baseline
}
static void font_render_8bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_RGB888_stable(font, glyph);     // 100% baseline
}
static void font_render_8bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
{
    font_render_8bpp_to_ARGB8888_stable(font, glyph);     // 100% baseline
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
        case ALPHA8BIT:
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

