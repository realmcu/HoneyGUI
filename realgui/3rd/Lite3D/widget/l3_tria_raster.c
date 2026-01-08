/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_raster.c
  * @brief deal with no rle filter mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2024/04/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2024 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "string.h"
#include "l3_sw_raster.h"
#include "l3_common.h"



// Calculate edge properties (slope and intercept)
static void l3_get_line_params(float x0, float y0, float x1, float y1,
                               bool *infinite, float *k, float *b)
{
    if (x0 == x1)
    {
        *infinite = true;
        *k = 1;
        *b = x0;
    }
    else
    {
        *infinite = false;
        *k = (y0 - y1) / (x0 - x1);
        *b = y0 - *k * x0;
    }
}

static void l3_get_z_interpolate(float y, float y0, float y1, float y2, \
                                 float k01infinite, float k01, float b01, \
                                 float k02infinite, float k02, float b02, \
                                 float inv_z0, float inv_z1, float inv_z2, \
                                 float *xleft, float *xright, \
                                 float *oneoverz_left, float *oneoverz_step)
{
    *xleft = k01infinite ? b01 : (y - b01) / k01;
    *xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    *oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                     inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    *oneoverz_step = (oneoverz_right - *oneoverz_left) / (*xright -
                                                          *xleft);   // Horizontal step size of 1/z
}

static void l3_get_z_s_t_interpolate(float y, float y0, float y1, float y2, \
                                     float k01infinite, float k01, float b01, \
                                     float k02infinite, float k02, float b02, \
                                     float inv_z0, float inv_z1, float inv_z2, \
                                     float soverz0, float soverz1, float soverz2, \
                                     float toverz0, float toverz1, float toverz2, \
                                     float *xleft, float *xright, \
                                     float *oneoverz_left, float *oneoverz_step, \
                                     float *soverz_left, float *soverz_step, \
                                     float *toverz_left, float *toverz_step)
{
    *xleft = k01infinite ? b01 : (y - b01) / k01;
    *xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    *oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                     inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    *oneoverz_step = (oneoverz_right - *oneoverz_left) / (*xright -
                                                          *xleft);   // Horizontal step size of 1/z

    *soverz_left = y_y0 * (soverz1 - soverz0) * inv_y1_y0 +
                   soverz0;      // Interpolation of texture coordinates s (u) on the left edge
    float soverz_right = y_y0 * (soverz2 - soverz0) * inv_y2_y0 +
                         soverz0;     // Interpolation of texture coordinates s (u) on the right edge
    *soverz_step = (soverz_right - *soverz_left) / (*xright -
                                                    *xleft);       // Horizontal step size of texture coordinates s (u)

    *toverz_left = y_y0 * (toverz1 - toverz0) * inv_y1_y0 +
                   toverz0;      // Interpolation of texture coordinates t (v) on the left edge
    float toverz_right = y_y0 * (toverz2 - toverz0) * inv_y2_y0 +
                         toverz0;     // Interpolation of texture coordinates t (v) on the right edge
    *toverz_step = (toverz_right - *toverz_left) / (*xright -
                                                    *xleft);       // Horizontal step size of texture coordinate t (v)

}

static void l3_fill_color_rgb565(float y, float xleft, float xright, float oneoverz,
                                 float oneoverz_step, \
                                 float *zbuffer, uint16_t *writebuf, int width, \
                                 uint16_t fill_color)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;

    if (zbuffer != NULL)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            // const float originalZ = 1.0f / oneoverz;

            if (zbuffer[pixelIdx] == 0 || oneoverz > zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = oneoverz;
                writebuf[pixelIdx] = fill_color;
            }
            oneoverz += oneoverz_step;
        }
    }
    else
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            writebuf[pixelIdx] = fill_color;

            oneoverz += oneoverz_step;
        }
    }

}

static void l3_fill_color_argb8888(float y, float xleft, float xright, float oneoverz,
                                   float oneoverz_step, \
                                   float *zbuffer, uint32_t *writebuf, int width, \
                                   uint32_t fill_color)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;
    const uint8_t alpha = (fill_color >> 24) & 0xFF;

    if (zbuffer != NULL)
    {
        if (alpha == 255)
        {
            for (int ix = startX; ix < endX; ++ix)
            {
                int pixelIdx = ix + rowOffset;
                // const float originalZ = 1.0f / oneoverz;

                if (zbuffer[pixelIdx] == 0 || oneoverz > zbuffer[pixelIdx])
                {
                    zbuffer[pixelIdx] = oneoverz;
                    writebuf[pixelIdx] = fill_color;
                }
                oneoverz += oneoverz_step;
            }
        }
        else if (alpha > 0)
        {
            const uint8_t srcR = (fill_color >> 16) & 0xFF;
            const uint8_t srcG = (fill_color >> 8) & 0xFF;
            const uint8_t srcB = fill_color & 0xFF;
            const uint8_t inv_alpha = 255 - alpha;
            for (int ix = startX; ix < endX; ++ix)
            {
                int pixelIdx = ix + rowOffset;
                const float originalZ = 1.0f / oneoverz;
                if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
                {
                    zbuffer[pixelIdx] = originalZ;
                    l3_color_argb8888_t *pixel = (l3_color_argb8888_t *)(uintptr_t)writebuf + pixelIdx;

                    pixel->a = (alpha * alpha + pixel->a * inv_alpha) / 255;
                    pixel->r = (srcR * alpha + pixel->r * inv_alpha) / 255;
                    pixel->g = (srcG * alpha + pixel->g * inv_alpha) / 255;
                    pixel->b = (srcB * alpha + pixel->b * inv_alpha) / 255;
                }

                oneoverz += oneoverz_step;
            }
        }
    }
    else
    {
        if (alpha == 255)
        {
            for (int ix = startX; ix < endX; ++ix)
            {
                int pixelIdx = ix + rowOffset;
                writebuf[pixelIdx] = fill_color;

                oneoverz += oneoverz_step;
            }
        }
        else if (alpha > 0)
        {
            const uint8_t srcR = (fill_color >> 16) & 0xFF;
            const uint8_t srcG = (fill_color >> 8) & 0xFF;
            const uint8_t srcB = fill_color & 0xFF;
            const uint8_t inv_alpha = 255 - alpha;
            for (int ix = startX; ix < endX; ++ix)
            {
                int pixelIdx = ix + rowOffset;

                l3_color_argb8888_t *pixel = (l3_color_argb8888_t *)(uintptr_t)writebuf + pixelIdx;

                pixel->a = (alpha * alpha + pixel->a * inv_alpha) / 255;
                pixel->r = (srcR * alpha + pixel->r * inv_alpha) / 255;
                pixel->g = (srcG * alpha + pixel->g * inv_alpha) / 255;
                pixel->b = (srcB * alpha + pixel->b * inv_alpha) / 255;

                oneoverz += oneoverz_step;
            }
        }
    }
}


static void l3_fill_color_I8(float y, float xleft, float xright, float oneoverz,
                             float oneoverz_step, \
                             float *zbuffer, uint8_t *writebuf, int width, \
                             uint8_t fill_color)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;

    if (zbuffer != NULL)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            // const float originalZ = 1.0f / oneoverz;

            if (zbuffer[pixelIdx] == 0 || oneoverz > zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = oneoverz;
                writebuf[pixelIdx] = fill_color;
            }
            oneoverz += oneoverz_step;
        }
    }
    else
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            writebuf[pixelIdx] = fill_color;

            oneoverz += oneoverz_step;
        }
    }
}


// Fill with image data rgb565
static void l3_fill_texture_rgb565(int y, float xleft, float xright, float oneoverz,
                                   float oneoverz_step, \
                                   float soverz, float soverz_step, float toverz, float toverz_step, \
                                   float *zbuffer, uint16_t *writebuf, int width, \
                                   l3_img_head_t *src_head, uint32_t image_addr)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        soverz += (0 - startX) * soverz_step;
        toverz += (0 - startX) * toverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;
    uint8_t source_red, source_green, source_blue, source_alpha;

    if (zbuffer != NULL)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            float originalZ = 1.0f / oneoverz;

            // Calculate texture coordinates
            int srcX = (int)(soverz * originalZ * (src_head->w - 1));
            int srcY = (int)(toverz * originalZ * (src_head->h - 1));

            if (srcX >= 0 && srcX < src_head->w && srcY >= 0 && srcY < src_head->h)
            {
                int pixelIdx = ix + rowOffset;

                gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                                     image_addr, srcX + srcY * src_head->w, src_head->type);

                if (source_alpha > 0 && (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx]))
                {
                    zbuffer[pixelIdx] = originalZ;

                    if (source_alpha == 255)
                    {
                        // Completely opaque pixel processing
                        writebuf[pixelIdx] = ((source_red & 0xF8) << 8) |
                                             ((source_green & 0xFC) << 3) |
                                             ((source_blue & 0xF8) >> 3);
                    }
                    else
                    {
                        // Alpha blending for semi-transparent pixels
                        l3_color_rgb565_t *dst = (l3_color_rgb565_t *)writebuf + pixelIdx;
                        uint8_t dstR = dst->r << 3;
                        uint8_t dstG = dst->g << 2;
                        uint8_t dstB = dst->b << 3;

                        uint8_t inv_alpha = 255 - source_alpha;
                        uint8_t outR = (source_red * source_alpha + dstR * inv_alpha) / 255;
                        uint8_t outG = (source_green * source_alpha + dstG * inv_alpha) / 255;
                        uint8_t outB = (source_blue * source_alpha + dstB * inv_alpha) / 255;

                        writebuf[pixelIdx] = ((outR & 0xF8) << 8) |
                                             ((outG & 0xFC) << 3) |
                                             ((outB & 0xF8) >> 3);
                    }
                }
            }

            oneoverz += oneoverz_step;
            soverz += soverz_step;
            toverz += toverz_step;
        }
    }
    else
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            float originalZ = 1.0f / oneoverz;

            // Calculate texture coordinates
            int srcX = (int)(soverz * originalZ * (src_head->w - 1));
            int srcY = (int)(toverz * originalZ * (src_head->h - 1));

            if (srcX >= 0 && srcX < src_head->w && srcY >= 0 && srcY < src_head->h)
            {
                int pixelIdx = ix + rowOffset;

                gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                                     image_addr, srcX + srcY * src_head->w, src_head->type);

                if (source_alpha > 0)
                {
                    if (source_alpha == 255)
                    {
                        writebuf[pixelIdx] = ((source_red & 0xF8) << 8) |
                                             ((source_green & 0xFC) << 3) |
                                             ((source_blue & 0xF8) >> 3);
                    }
                    else
                    {
                        // Alpha blending for semi-transparent pixels
                        l3_color_rgb565_t *dst = (l3_color_rgb565_t *)writebuf + pixelIdx;
                        uint8_t dstR = dst->r << 3;
                        uint8_t dstG = dst->g << 2;
                        uint8_t dstB = dst->b << 3;

                        uint8_t inv_alpha = 255 - source_alpha;
                        uint8_t outR = (source_red * source_alpha + dstR * inv_alpha) / 255;
                        uint8_t outG = (source_green * source_alpha + dstG * inv_alpha) / 255;
                        uint8_t outB = (source_blue * source_alpha + dstB * inv_alpha) / 255;

                        writebuf[pixelIdx] = ((outR & 0xF8) << 8) |
                                             ((outG & 0xFC) << 3) |
                                             ((outB & 0xF8) >> 3);
                    }
                }

            }

            oneoverz += oneoverz_step;
            soverz += soverz_step;
            toverz += toverz_step;
        }
    }
}


// Fill with image data argb8888
static void l3_fill_texture_argb8888(int y, float xleft, float xright, float oneoverz,
                                     float oneoverz_step, \
                                     float soverz, float soverz_step, float toverz, float toverz_step, \
                                     float *zbuffer, uint32_t *writebuf, int width, \
                                     l3_img_head_t *src_head, uint32_t image_addr)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        soverz += (0 - startX) * soverz_step;
        toverz += (0 - startX) * toverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;
    uint8_t source_red, source_green, source_blue, source_alpha;

    if (zbuffer != NULL)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            float originalZ = 1.0f / oneoverz;

            // Calculate texture coordinates
            int srcX = (int)(soverz * originalZ * (src_head->w - 1));
            int srcY = (int)(toverz * originalZ * (src_head->h - 1));

            if (srcX >= 0 && srcX < src_head->w && srcY >= 0 && srcY < src_head->h)
            {
                int pixelIdx = ix + rowOffset;

                gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                                     image_addr, srcX + srcY * src_head->w, src_head->type);

                if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
                {
                    zbuffer[pixelIdx] = originalZ;
                    // Completely opaque pixel processing
                    if (source_alpha == 255)
                    {
                        writebuf[pixelIdx] = ((uint32_t)source_alpha << 24) | ((uint32_t)source_red << 16) |
                                             ((uint32_t)source_green << 8) | (uint32_t)source_blue;
                    }
                    // Translucent pixel processing, without updating depth buffer (keeping the objects behind visible)
                    else if (source_alpha > 0)
                    {
                        uint32_t dstColor = writebuf[pixelIdx];

                        uint8_t dstR = (dstColor >> 16) & 0xFF;
                        uint8_t dstG = (dstColor >> 8) & 0xFF;
                        uint8_t dstB = dstColor & 0xFF;
                        uint8_t dstA = (dstColor >> 24) & 0xFF;

                        // Blending
                        uint8_t outA = (source_alpha * source_alpha + dstA * (255 - source_alpha)) / 255;
                        uint8_t outR = (source_red * source_alpha + dstR * (255 - source_alpha)) / 255;
                        uint8_t outG = (source_green * source_alpha + dstG * (255 - source_alpha)) / 255;
                        uint8_t outB = (source_blue * source_alpha + dstB * (255 - source_alpha)) / 255;

                        writebuf[pixelIdx] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
                    }
                }
            }

            oneoverz += oneoverz_step;
            soverz += soverz_step;
            toverz += toverz_step;
        }
    }
    else
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            float originalZ = 1.0f / oneoverz;

            // Calculate texture coordinates
            int srcX = (int)(soverz * originalZ * (src_head->w - 1));
            int srcY = (int)(toverz * originalZ * (src_head->h - 1));

            if (srcX >= 0 && srcX < src_head->w && srcY >= 0 && srcY < src_head->h)
            {
                int pixelIdx = ix + rowOffset;

                gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                                     image_addr, srcX + srcY * src_head->w, src_head->type);

                // Completely opaque pixel processing
                if (source_alpha == 255)
                {
                    writebuf[pixelIdx] = ((uint32_t)source_alpha << 24) | ((uint32_t)source_red << 16) |
                                         ((uint32_t)source_green << 8) | (uint32_t)source_blue;
                }
                // Translucent pixel processing, without updating depth buffer (keeping the objects behind visible)
                else if (source_alpha > 0)
                {
                    uint32_t dstColor = writebuf[pixelIdx];

                    uint8_t dstR = (dstColor >> 16) & 0xFF;
                    uint8_t dstG = (dstColor >> 8) & 0xFF;
                    uint8_t dstB = dstColor & 0xFF;
                    uint8_t dstA = (dstColor >> 24) & 0xFF;

                    // Blending
                    uint8_t outA = (source_alpha * source_alpha + dstA * (255 - source_alpha)) / 255;
                    uint8_t outR = (source_red * source_alpha + dstR * (255 - source_alpha)) / 255;
                    uint8_t outG = (source_green * source_alpha + dstG * (255 - source_alpha)) / 255;
                    uint8_t outB = (source_blue * source_alpha + dstB * (255 - source_alpha)) / 255;

                    writebuf[pixelIdx] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
                }

            }

            oneoverz += oneoverz_step;
            soverz += soverz_step;
            toverz += toverz_step;
        }
    }
}


static void l3_render_with_color_rgb565(float y, float y0, float y1, float y2, \
                                        float inv_z0, float inv_z1, float inv_z2, \
                                        bool k01infinite, float k01, float b01, \
                                        bool k02infinite, float k02, float b02, \
                                        float *zbuffer, uint8_t *writebuf, int width, \
                                        uint16_t fill_color)
{
    float xleft, xright, oneoverz_left, oneoverz_step;

    l3_get_z_interpolate(y, y0, y1, y2, \
                         k01infinite, k01, b01, \
                         k02infinite, k02, b02, \
                         inv_z0, inv_z1, inv_z2, \
                         &xleft, &xright, &oneoverz_left, &oneoverz_step);

    l3_fill_color_rgb565(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                         (uint16_t *)writebuf, width, fill_color);

}

static void l3_render_with_color_argb8888(float y, float y0, float y1, float y2, \
                                          float inv_z0, float inv_z1, float inv_z2, \
                                          bool k01infinite, float k01, float b01, \
                                          bool k02infinite, float k02, float b02, \
                                          float *zbuffer, uint8_t *writebuf, int width, \
                                          uint32_t fill_color)
{
    float xleft, xright, oneoverz_left, oneoverz_step;

    l3_get_z_interpolate(y, y0, y1, y2, \
                         k01infinite, k01, b01, \
                         k02infinite, k02, b02, \
                         inv_z0, inv_z1, inv_z2, \
                         &xleft, &xright, &oneoverz_left, &oneoverz_step);

    l3_fill_color_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                           (uint32_t *)writebuf, width, fill_color);

}


static void l3_render_with_color_I8(float y, float y0, float y1, float y2, \
                                    float inv_z0, float inv_z1, float inv_z2, \
                                    bool k01infinite, float k01, float b01, \
                                    bool k02infinite, float k02, float b02, \
                                    float *zbuffer, uint8_t *writebuf, int width, \
                                    uint8_t fill_color)
{
    float xleft, xright, oneoverz_left, oneoverz_step;

    l3_get_z_interpolate(y, y0, y1, y2, \
                         k01infinite, k01, b01, \
                         k02infinite, k02, b02, \
                         inv_z0, inv_z1, inv_z2, \
                         &xleft, &xright, &oneoverz_left, &oneoverz_step);

    l3_fill_color_I8(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                     writebuf, width, fill_color);
}


static void l3_render_with_texture_rgb565(float y, float y0, float y1, float y2, \
                                          float inv_z0, float inv_z1, float inv_z2, \
                                          float soverz0, float soverz1, float soverz2, \
                                          float toverz0, float toverz1, float toverz2, \
                                          bool k01infinite, float k01, float b01, \
                                          bool k02infinite, float k02, float b02, \
                                          float *zbuffer, uint8_t *writebuf, int width, \
                                          l3_img_head_t *src_head, uint32_t image_addr)
{
    float xleft, xright, oneoverz_left, oneoverz_step, soverz_left, soverz_step, toverz_left,
          toverz_step;

    l3_get_z_s_t_interpolate(y, y0, y1, y2, \
                             k01infinite, k01, b01, \
                             k02infinite, k02, b02, \
                             inv_z0, inv_z1, inv_z2, \
                             soverz0, soverz1, soverz2, \
                             toverz0, toverz1, toverz2, \
                             &xleft, &xright, &oneoverz_left, &oneoverz_step, \
                             &soverz_left, &soverz_step, &toverz_left, &toverz_step);


    l3_fill_texture_rgb565(y, xleft, xright, oneoverz_left, oneoverz_step,
                           soverz_left, soverz_step, toverz_left, toverz_step,
                           zbuffer, (uint16_t *)writebuf, width,
                           src_head, image_addr);
}

static void l3_render_with_texture_argb8888(float y, float y0, float y1, float y2, \
                                            float inv_z0, float inv_z1, float inv_z2, \
                                            float soverz0, float soverz1, float soverz2, \
                                            float toverz0, float toverz1, float toverz2, \
                                            bool k01infinite, float k01, float b01, \
                                            bool k02infinite, float k02, float b02, \
                                            float *zbuffer, uint8_t *writebuf, int width, \
                                            l3_img_head_t *src_head, uint32_t image_addr)
{
    float xleft, xright, oneoverz_left, oneoverz_step, soverz_left, soverz_step, toverz_left,
          toverz_step;

    l3_get_z_s_t_interpolate(y, y0, y1, y2, \
                             k01infinite, k01, b01, \
                             k02infinite, k02, b02, \
                             inv_z0, inv_z1, inv_z2, \
                             soverz0, soverz1, soverz2, \
                             toverz0, toverz1, toverz2, \
                             &xleft, &xright, &oneoverz_left, &oneoverz_step, \
                             &soverz_left, &soverz_step, &toverz_left, &toverz_step);

    l3_fill_texture_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step,
                             soverz_left, soverz_step, toverz_left, toverz_step,
                             zbuffer, (uint32_t *)writebuf, width,
                             src_head, image_addr);

}


// Helper function to render a triangle half (top or bottom)
static void l3_render_triangle_half(l3_draw_tria_img_t *image,
                                    l3_vertex_t *p0, l3_vertex_t *p1, l3_vertex_t *p2,
                                    float y_start, float y_end,
                                    float *zbuffer, uint8_t *writebuf, int width)
{
    bool k01infinite, k02infinite;
    float k01, b01, k02, b02;

    float x0 = p0->position.x, y0 = p0->position.y, z0 = p0->position.z;
    float x1 = p1->position.x, y1 = p1->position.y, z1 = p1->position.z;
    float x2 = p2->position.x, y2 = p2->position.y, z2 = p2->position.z;

    float s0 = p0->u, t0 = p0->v;
    float s1 = p1->u, t1 = p1->v;
    float s2 = p2->u, t2 = p2->v;

    // Calculate slopes
    l3_get_line_params(x0, y0, x1, y1, &k01infinite, &k01, &b01);
    l3_get_line_params(x0, y0, x2, y2, &k02infinite, &k02, &b02);

    // Pre-calculate depth
    float inv_z0 = 1.0f / z0;
    float inv_z1 = 1.0f / z1;
    float inv_z2 = 1.0f / z2;

    // Render based on fill type
    if (image->fill_type == L3_FILL_COLOR_RGB565)
    {
        uint16_t fill_color = *((uint16_t *)image->fill_data);
        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                        k01infinite, k01, b01, k02infinite, k02, b02,
                                        zbuffer, writebuf, width, fill_color);
        }
    }
    else if (image->fill_type == L3_FILL_COLOR_I8)
    {
        uint8_t fill_color = *((uint8_t *)image->fill_data);
        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_I8(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                    k01infinite, k01, b01, k02infinite, k02, b02,
                                    zbuffer, writebuf, width, fill_color);
        }
    }
    else if (image->fill_type == L3_FILL_COLOR_ARGB8888)
    {
        uint32_t fill_color = *((uint32_t *)image->fill_data);
        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                          k01infinite, k01, b01, k02infinite, k02, b02,
                                          zbuffer, writebuf, width, fill_color);
        }
    }
    else if (image->fill_type == L3_FILL_IMAGE_RGB565 || image->fill_type == L3_FILL_IMAGE_ARGB8888)
    {
        // Pre-calculate texture coordinates
        float soverz0 = s0 * inv_z0, soverz1 = s1 * inv_z1, soverz2 = s2 * inv_z2;
        float toverz0 = t0 * inv_z0, toverz1 = t1 * inv_z1, toverz2 = t2 * inv_z2;

        l3_img_head_t *src_head = (l3_img_head_t *)image->fill_data;
        uint32_t image_addr = sizeof(l3_img_head_t) + (uintptr_t)(image->fill_data);

        if (image->fill_type == L3_FILL_IMAGE_RGB565)
        {
            for (float y = y_start; y <= y_end; y++)
            {
                l3_render_with_texture_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                              soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                              k01infinite, k01, b01, k02infinite, k02, b02,
                                              zbuffer, writebuf, width, src_head, image_addr);
            }
        }
        else
        {
            for (float y = y_start; y <= y_end; y++)
            {
                l3_render_with_texture_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                                soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                                k01infinite, k01, b01, k02infinite, k02, b02,
                                                zbuffer, writebuf, width, src_head, image_addr);
            }
        }
    }
}

void (*l3_draw_tria_to_canvas_imp)(l3_draw_tria_img_t *image, l3_draw_rect_img_t *combined_image,
                                   float *zbuffer) = NULL;

void l3_draw_tria_to_canvas(l3_draw_tria_img_t *image, l3_draw_rect_img_t *combined_image,
                            float *zbuffer)
{
    if (l3_draw_tria_to_canvas_imp)
    {
        l3_draw_tria_to_canvas_imp(image, combined_image, zbuffer);
        return;
    }

    l3_vertex_t p0 = image->p0;
    l3_vertex_t p1 = image->p1;
    l3_vertex_t p2 = image->p2;

    l3_img_head_t *src_head = (l3_img_head_t *)combined_image->data;
    int width = src_head->w;
    int height = src_head->h;
    uint8_t *writebuf = (uint8_t *)combined_image->data + sizeof(l3_img_head_t);

    // For I8 format: header + clut_count(4) + clut_data + pixel_index
    if (src_head->type == LITE_I8)
    {
        writebuf = (uint8_t *)combined_image->data + sizeof(l3_img_head_t) + 257 * 4;
    }

    //    p0
    // p1   p2
    // First pass: top part of the triangle
    if (p1.position.y < p0.position.y)
    {
        l3_vertex_t vt;
        vt = p1;
        p1 = p0;
        p0 = vt;
    }
    if (p2.position.y < p0.position.y)
    {
        l3_vertex_t vt;
        vt = p2;
        p2 = p0;
        p0 = vt;
    }

    float midy = fminf(p1.position.y, p2.position.y);
    l3_adjust_triangle_winding(&p0, &p1, &p2);

    // Render top half
    float y_start = fmaxf((int)(p0.position.y + 0.5f) + 0.5f, 0.5f);
    float y_end = fminf(midy, (float)height);
    l3_render_triangle_half(image, &p0, &p1, &p2, y_start, y_end, zbuffer, writebuf, width);

    // p1   p2
    //    p0
    // Second pass: bottom part of the triangle
    if (p1.position.y > p0.position.y)
    {
        l3_vertex_t vt = p1;
        p1 = p0;
        p0 = vt;
    }
    if (p2.position.y > p0.position.y)
    {
        l3_vertex_t vt = p2;
        p2 = p0;
        p0 = vt;
    }

    midy = fmaxf(p1.position.y, p2.position.y);
    l3_adjust_triangle_winding(&p0, &p1, &p2);

    // Render bottom half
    y_start = fmaxf((int)(midy + 0.5f) + 0.5f, 0.5f);
    y_end = fminf(p0.position.y, (float)height);
    l3_render_triangle_half(image, &p0, &p1, &p2, y_start, y_end, zbuffer, writebuf, width);
}


