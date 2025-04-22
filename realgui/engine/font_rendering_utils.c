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

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void font_render_1bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_1bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_1bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
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

static void font_render_1bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_2bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_2bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_2bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
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

static void font_render_2bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_4bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
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

static void font_render_4bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_4bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_4bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_8bpp_to_RGB565(draw_font_t *font, font_glyph_t *glyph)
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

static void font_render_8bpp_to_ARGB8565(draw_font_t *font, font_glyph_t *glyph)
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

static void font_render_8bpp_to_RGB888(draw_font_t *font, font_glyph_t *glyph)
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
static void font_render_8bpp_to_ARGB8888(draw_font_t *font, font_glyph_t *glyph)
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

