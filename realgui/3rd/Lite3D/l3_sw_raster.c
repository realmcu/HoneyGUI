/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_raster.c
  * @brief deal with no rle filter mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2024/04/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "draw_img.h"
#include "def_color.h"
#include "l3_common.h"

typedef struct
{
    uint8_t *writebuf;
    int write_off;
    uint32_t image_base;
    uint32_t image_off;
    char input_type;
    uint8_t dc_bytes_per_pixel;
    uint8_t opacity_value;
    uint32_t blend_mode;
} gui_raster_params_t;

static void gui_get_source_color(uint8_t *source_red, uint8_t *source_green, uint8_t *source_blue,
                                 uint8_t *source_alpha,
                                 uint32_t image_base, uint32_t image_off, char input_type)
{
    switch (input_type)
    {
    case RGB565:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case RGB888:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
        }
    case ARGB8565:
        {
            color_argb8565_t *pixel = (color_argb8565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case ARGB8888:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
        }
    }
}

static void gui_get_target_color(uint8_t *target_red, uint8_t *target_green, uint8_t *target_blue,
                                 uint8_t *target_alpha,
                                 uint8_t *writebuf, int write_off, uint8_t dc_bytes_per_pixel)
{
    switch (dc_bytes_per_pixel)
    {
    case 2:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r << 3;
            *target_green = pixel->g << 2;
            *target_blue = pixel->b << 3;
            break;
        }
    case 3:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r;
            *target_green = pixel->g;
            *target_blue = pixel->b;
            break;
        }
    case 4:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = pixel->a;
            *target_red = pixel->r;
            *target_green = pixel->g;
            *target_blue = pixel->b;
            break;
        }
    }
}

static void blend_colors(uint8_t *target_channel, uint8_t source_channel, uint8_t source_alpha)
{
    *target_channel = ((255 - source_alpha) * *target_channel + source_alpha * source_channel) / 255;
}

static void gui_apply_blend_mode(uint8_t *target_red, uint8_t *target_green, uint8_t *target_blue,
                                 uint8_t *target_alpha,
                                 uint8_t source_red, uint8_t source_green, uint8_t source_blue, uint8_t source_alpha,
                                 uint8_t opacity_value, uint32_t blend_mode)
{

    switch (blend_mode)
    {
    case IMG_COVER_MODE:
        {
            *target_alpha = source_alpha;
            *target_red = source_red;
            *target_green = source_green;
            *target_blue = source_blue;
            break;
        }
    case IMG_BYPASS_MODE:
        {
            uint8_t blend_opacity = opacity_value;
            blend_colors(target_alpha, source_alpha, blend_opacity);
            blend_colors(target_red, source_red, blend_opacity);
            blend_colors(target_green, source_green, blend_opacity);
            blend_colors(target_blue, source_blue, blend_opacity);
            break;
        }
    case IMG_FILTER_BLACK:
        {
            if (source_red == 0 && source_green == 0 && source_blue == 0)
            {
                return; // If source is black, do nothing
            }
            source_alpha = (source_alpha * opacity_value) / 255;
            blend_colors(target_alpha, source_alpha, source_alpha);
            blend_colors(target_red, source_red, source_alpha);
            blend_colors(target_green, source_green, source_alpha);
            blend_colors(target_blue, source_blue, source_alpha);
            break;
        }
    case IMG_SRC_OVER_MODE:
    case IMG_RECT:
        {
            source_alpha = (source_alpha * opacity_value) / 255;
            blend_colors(target_alpha, source_alpha, source_alpha);
            blend_colors(target_red, source_red, source_alpha);
            blend_colors(target_green, source_green, source_alpha);
            blend_colors(target_blue, source_blue, source_alpha);
            break;
        }
    default:
        break;
    }
}

static void gui_set_pixel_color(uint8_t *writebuf, int write_off, uint8_t dc_bytes_per_pixel,
                                uint8_t target_red, uint8_t target_green, uint8_t target_blue, uint8_t target_alpha)
{
    switch (dc_bytes_per_pixel)
    {
    case 2:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red >> 3;
            pixel->g = target_green >> 2;
            pixel->b = target_blue >> 3;
            break;
        }
    case 3:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    case 4:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)writebuf + write_off;
            pixel->a = target_alpha;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    }
}
static void do_raster_pixel(const gui_raster_params_t *params)
{
    uint8_t source_red, source_green, source_blue, source_alpha;
    uint8_t target_red, target_green, target_blue, target_alpha;

    gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                         params->image_base, params->image_off, params->input_type);

    gui_get_target_color(&target_red, &target_green, &target_blue, &target_alpha,
                         params->writebuf, params->write_off, params->dc_bytes_per_pixel);

    gui_apply_blend_mode(&target_red, &target_green, &target_blue, &target_alpha,
                         source_red, source_green, source_blue, source_alpha,
                         params->opacity_value, params->blend_mode);

    gui_set_pixel_color(params->writebuf, params->write_off, params->dc_bytes_per_pixel,
                        target_red, target_green, target_blue, target_alpha);
}

static bool l3_draw_img_target_area(l3_draw_img_t *image, l3_canvas_t *dc, l3_rect_t *rect,
                                    int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end)
{
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;

    int16_t image_w = image->img_target_w ;
    int16_t image_h = image->img_target_h ;

    *x_start = _UI_MAX(image_x, dc->section.x1);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->section.x2);

    *y_start = _UI_MAX(image_y, dc->section.y1);
    *y_end = _UI_MIN(image_y + image_h - 1, dc->section.y2);

    if ((*x_start >= *x_end) || (*y_start >= *y_end))
    {
        return false;
    }
    return true;
}


#ifdef __WIN32
void l3_draw_rect_img_to_canvas(l3_draw_img_t *image, l3_canvas_t *dc, l3_rect_t *rect)
#else
__attribute__((weak)) void l3_draw_rect_img_to_canvas(l3_draw_img_t *image, l3_canvas_t *dc,
                                                      l3_rect_t *rect)
#endif
{
    int32_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
    if (!l3_draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end)) { return; }

    uint32_t image_base = sizeof(l3_img_head_t) + (uint32_t)(uintptr_t)(image->data);
    l3_img_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w, source_h = image->img_h;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3, opacity_value = image->opacity_value;
    uint8_t *writebuf = dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    l3_3x3_matrix_t *inverse = &image->inverse;

    gui_raster_params_t params = { writebuf, 0, image_base, 0, input_type, dc_bytes_per_pixel, opacity_value, blend_mode };

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        for (uint32_t j = x_start; j <= x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = round(X / Z);
            int y = round(Y / Z);

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
            if (rect && ((x > rect->x2) || (x < rect->x1) || (y < rect->y1) || (y > rect->y2))) { continue; }

            int read_off = (image->blend_mode == IMG_RECT) ? 0 : y * source_w + x;
            params.image_off = read_off;
            params.write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j -
                               dc->section.x1;

            do_raster_pixel(&params);
        }
    }
}


// Helper function to determine triangle orientation
static void determine_triangle_orientation(l3_vertex_t *p0, l3_vertex_t *p1, l3_vertex_t *p2)
{
    float x01m;
    float x0 = p0->position.x;
    float y0 = p0->position.y;
    float x1 = p1->position.x;
    float y1 = p1->position.y;
    float x2 = p2->position.x;
    float y2 = p2->position.y;

    if (x0 == x1)
    {
        x01m = x0;
    }
    else
    {
        float k01 = (y0 - y1) / (x0 - x1);
        float b01 = y0 - k01 * x0;
        x01m = (y2 - b01) / k01;
    }

    if (x01m > x2)
    {
        l3_vertex_t t = *p2;
        *p2 = *p1;
        *p1 = t;
    }
}

// Calculate edge properties (slope and intercept)
static void calculate_edge_properties(float x0, float y0, float x1, float y1,
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

static void l3_fill_color_rgb565(float y, float xleft, float xright, float oneoverz,
                                 float oneoverz_step, \
                                 float *zbuffer, uint16_t *writebuf, uint32_t width, \
                                 l3_color_t *fill_color)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;

    for (int ix = startX; ix < endX; ++ix)
    {
        int pixelIdx = ix + rowOffset;
        const float originalZ = 1.0f / oneoverz;

        if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
        {
            zbuffer[pixelIdx] = originalZ;
            writebuf[pixelIdx] = ((fill_color->r & 0xF8) << 8) |
                                 ((fill_color->g & 0xFC) << 3) |
                                 ((fill_color->b & 0xF8) >> 3);
        }
        oneoverz += oneoverz_step;
    }
}

static void l3_fill_color_argb8888(float y, float xleft, float xright, float oneoverz,
                                   float oneoverz_step, \
                                   float *zbuffer, uint32_t *writebuf, uint32_t width, \
                                   l3_color_t *fill_color)
{
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = (int)y * width;

    if (fill_color->a == 255)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            const float originalZ = 1.0f / oneoverz;

            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = originalZ;
                writebuf[pixelIdx] = (fill_color->a << 24) | (fill_color->r << 16) |
                                     (fill_color->g << 8) | fill_color->b;
            }
            oneoverz += oneoverz_step;
        }
    }
    else if (fill_color->a > 0)
    {
        const uint8_t inv_alpha = 255 - fill_color->a;
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            const float originalZ = 1.0f / oneoverz;
            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = originalZ;
                color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)writebuf + pixelIdx;

                pixel->a = (fill_color->a * fill_color->a + pixel->a * inv_alpha) / 255;
                pixel->r = (fill_color->r * fill_color->a + pixel->r * inv_alpha) / 255;
                pixel->g = (fill_color->g * fill_color->a + pixel->g * inv_alpha) / 255;
                pixel->b = (fill_color->b * fill_color->a + pixel->b * inv_alpha) / 255;
            }

            oneoverz += oneoverz_step;
        }
    }
}

// Fill with image data rgb565
static void l3_fill_texture_rgb565(int y, float xleft, float xright, float oneoverz,
                                   float oneoverz_step, \
                                   float soverz, float soverz_step, float toverz, float toverz_step, \
                                   float *zbuffer, uint16_t *writebuf, uint32_t width, \
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
                // Completely opaque pixel processing
                zbuffer[pixelIdx] = originalZ;
                writebuf[pixelIdx] = ((source_red & 0xF8) << 8) |
                                     ((source_green & 0xFC) << 3) |
                                     ((source_blue & 0xF8) >> 3);
            }
        }

        oneoverz += oneoverz_step;
        soverz += soverz_step;
        toverz += toverz_step;
    }
}


// Fill with image data argb8888
static void l3_fill_texture_argb8888(int y, float xleft, float xright, float oneoverz,
                                     float oneoverz_step, \
                                     float soverz, float soverz_step, float toverz, float toverz_step, \
                                     float *zbuffer, uint32_t *writebuf, uint32_t width, \
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




static void l3_render_with_color_rgb565(float y, float y0, float y1, float y2, \
                                        float inv_z0, float inv_z1, float inv_z2, \
                                        bool k01infinite, float k01, float b01, \
                                        bool k02infinite, float k02, float b02, \
                                        float *zbuffer, uint8_t *writebuf, uint32_t width, \
                                        l3_color_t *fill_color)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z

    l3_fill_color_rgb565(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                         (uint16_t *)writebuf, width, fill_color);
}

static void l3_render_with_color_argb8888(float y, float y0, float y1, float y2, \
                                          float inv_z0, float inv_z1, float inv_z2, \
                                          bool k01infinite, float k01, float b01, \
                                          bool k02infinite, float k02, float b02, \
                                          float *zbuffer, uint8_t *writebuf, uint32_t width, \
                                          l3_color_t *fill_color)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z

    l3_fill_color_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                           (uint32_t *)writebuf, width, fill_color);
}


static void l3_render_with_texture_rgb565(float y, float y0, float y1, float y2, \
                                          float inv_z0, float inv_z1, float inv_z2, \
                                          float soverz0, float soverz1, float soverz2, \
                                          float toverz0, float toverz1, float toverz2, \
                                          bool k01infinite, float k01, float b01, \
                                          bool k02infinite, float k02, float b02, \
                                          float *zbuffer, uint8_t *writebuf, uint32_t width, \
                                          l3_img_head_t *src_head, uint32_t image_addr)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z


    float soverz_left = y_y0 * (soverz1 - soverz0) * inv_y1_y0 +
                        soverz0;      // Interpolation of texture coordinates s (u) on the left edge
    float soverz_right = y_y0 * (soverz2 - soverz0) * inv_y2_y0 +
                         soverz0;     // Interpolation of texture coordinates s (u) on the right edge
    float soverz_step = (soverz_right - soverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinates s (u)

    float toverz_left = y_y0 * (toverz1 - toverz0) * inv_y1_y0 +
                        toverz0;      // Interpolation of texture coordinates t (v) on the left edge
    float toverz_right = y_y0 * (toverz2 - toverz0) * inv_y2_y0 +
                         toverz0;     // Interpolation of texture coordinates t (v) on the right edge
    float toverz_step = (toverz_right - toverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinate t (v)


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
                                            float *zbuffer, uint8_t *writebuf, uint32_t width, \
                                            l3_img_head_t *src_head, uint32_t image_addr)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z


    float soverz_left = y_y0 * (soverz1 - soverz0) * inv_y1_y0 +
                        soverz0;      // Interpolation of texture coordinates s (u) on the left edge
    float soverz_right = y_y0 * (soverz2 - soverz0) * inv_y2_y0 +
                         soverz0;     // Interpolation of texture coordinates s (u) on the right edge
    float soverz_step = (soverz_right - soverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinates s (u)

    float toverz_left = y_y0 * (toverz1 - toverz0) * inv_y1_y0 +
                        toverz0;      // Interpolation of texture coordinates t (v) on the left edge
    float toverz_right = y_y0 * (toverz2 - toverz0) * inv_y2_y0 +
                         toverz0;     // Interpolation of texture coordinates t (v) on the right edge
    float toverz_step = (toverz_right - toverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinate t (v)


    l3_fill_texture_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step,
                             soverz_left, soverz_step, toverz_left, toverz_step,
                             zbuffer, (uint32_t *)writebuf, width,
                             src_head, image_addr);
}


void l3_draw_tria_to_canvas(l3_vertex_t p0, l3_vertex_t p1, l3_vertex_t p2,  \
                            void *fill_data, uint8_t *writebuf, uint32_t width, uint32_t height, \
                            float *zbuffer, L3_FILL_TYPE fill_type)
{
    bool k01infinite, k02infinite;
    float k01, b01, k02, b02;
    float x0, y0, z0, s0, t0;
    float x1, y1, z1, s1, t1;
    float x2, y2, z2, s2, t2;

    float inv_z0, inv_z1, inv_z2;
    float soverz0, soverz1, soverz2;
    float toverz0, toverz1, toverz2;


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

    float midy = p2.position.y < p1.position.y ? p2.position.y : p1.position.y;

    determine_triangle_orientation(&p0, &p1, &p2);

    // Reassignment and reordering of vertex coordinates and texture coordinates
    x0 = p0.position.x; y0 = p0.position.y; z0 = p0.position.z; s0 = p0.u; t0 = p0.v;
    x1 = p1.position.x; y1 = p1.position.y; z1 = p1.position.z; s1 = p1.u; t1 = p1.v;
    x2 = p2.position.x; y2 = p2.position.y; z2 = p2.position.z; s2 = p2.u; t2 = p2.v;

    // Calculate slope and intercept
    calculate_edge_properties(x0, y0, x1, y1, &k01infinite, &k01, &b01);
    calculate_edge_properties(x0, y0, x2, y2, &k02infinite, &k02, &b02);

    // Pre calculate constants
    inv_z0 = 1.0f / z0;
    inv_z1 = 1.0f / z1;
    inv_z2 = 1.0f / z2;

    float y_start = fmaxf((int)(y0 + 0.5f) + 0.5f, 0.5f);
    float y_end = fminf(midy, height);

    if (fill_type == L3_FILL_COLOR_RGB565)
    {
        l3_color_t *fill_color = (l3_color_t *)fill_data;

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_rgb565(y, y0, y1, y2, \
                                        inv_z0, inv_z1, inv_z2, \
                                        k01infinite, k01, b01, \
                                        k02infinite, k02, b02, \
                                        zbuffer, writebuf, width, \
                                        fill_color);
        }
    }
    else if (fill_type == L3_FILL_COLOR_ARGB8888)
    {
        l3_color_t *fill_color = (l3_color_t *)fill_data;

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_argb8888(y, y0, y1, y2, \
                                          inv_z0, inv_z1, inv_z2, \
                                          k01infinite, k01, b01, \
                                          k02infinite, k02, b02, \
                                          zbuffer, writebuf, width, \
                                          fill_color);
        }
    }
    else if (fill_type == L3_FILL_IMAGE_RGB565)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        l3_img_head_t *src_head = (l3_img_head_t *)fill_data;
        uint32_t image_addr = sizeof(l3_img_head_t) + (uint32_t)(uintptr_t)(fill_data);


        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_texture_rgb565(y, y0, y1, y2, \
                                          inv_z0, inv_z1, inv_z2, \
                                          soverz0, soverz1, soverz2, \
                                          toverz0, toverz1, toverz2, \
                                          k01infinite, k01, b01, \
                                          k02infinite, k02, b02, \
                                          zbuffer, writebuf, width, \
                                          src_head, image_addr);
        }
    }
    else if (fill_type == L3_FILL_IMAGE_ARGB8888)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        l3_img_head_t *src_head = (l3_img_head_t *)fill_data;
        uint32_t image_addr = sizeof(l3_img_head_t) + (uint32_t)(uintptr_t)(fill_data);

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_texture_argb8888(y, y0, y1, y2, \
                                            inv_z0, inv_z1, inv_z2, \
                                            soverz0, soverz1, soverz2, \
                                            toverz0, toverz1, toverz2, \
                                            k01infinite, k01, b01, \
                                            k02infinite, k02, b02, \
                                            zbuffer, writebuf, width, \
                                            src_head, image_addr);
        }
    }



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

    midy = p2.position.y < p1.position.y ? p1.position.y : p2.position.y;

    determine_triangle_orientation(&p0, &p1, &p2);

    x0 = p0.position.x; y0 = p0.position.y; z0 = p0.position.z; s0 = p0.u; t0 = p0.v;
    x1 = p1.position.x; y1 = p1.position.y; z1 = p1.position.z; s1 = p1.u; t1 = p1.v;
    x2 = p2.position.x; y2 = p2.position.y; z2 = p2.position.z; s2 = p2.u; t2 = p2.v;

    calculate_edge_properties(x0, y0, x1, y1, &k01infinite, &k01, &b01);
    calculate_edge_properties(x0, y0, x2, y2, &k02infinite, &k02, &b02);

    // Pre calculate constants
    inv_z0 = 1.0f / z0;
    inv_z1 = 1.0f / z1;
    inv_z2 = 1.0f / z2;

    y_start = fmaxf((int)(midy + 0.5f) + 0.5f, 0.5f);
    y_end = fminf(y0, height);

    if (fill_type == L3_FILL_COLOR_RGB565)
    {
        l3_color_t *fill_color = (l3_color_t *)fill_data;

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_rgb565(y, y0, y1, y2, \
                                        inv_z0, inv_z1, inv_z2, \
                                        k01infinite, k01, b01, \
                                        k02infinite, k02, b02, \
                                        zbuffer, writebuf, width, \
                                        fill_color);
        }
    }
    else if (fill_type == L3_FILL_COLOR_ARGB8888)
    {
        l3_color_t *fill_color = (l3_color_t *)fill_data;

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_color_argb8888(y, y0, y1, y2, \
                                          inv_z0, inv_z1, inv_z2, \
                                          k01infinite, k01, b01, \
                                          k02infinite, k02, b02, \
                                          zbuffer, writebuf, width, \
                                          fill_color);
        }
    }
    else if (fill_type == L3_FILL_IMAGE_RGB565)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        l3_img_head_t *src_head = (l3_img_head_t *)fill_data;
        uint32_t image_addr = sizeof(l3_img_head_t) + (uint32_t)(uintptr_t)(fill_data);


        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_texture_rgb565(y, y0, y1, y2, \
                                          inv_z0, inv_z1, inv_z2, \
                                          soverz0, soverz1, soverz2, \
                                          toverz0, toverz1, toverz2, \
                                          k01infinite, k01, b01, \
                                          k02infinite, k02, b02, \
                                          zbuffer, writebuf, width, \
                                          src_head, image_addr);
        }
    }
    else if (fill_type == L3_FILL_IMAGE_ARGB8888)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        l3_img_head_t *src_head = (l3_img_head_t *)fill_data;
        uint32_t image_addr = sizeof(l3_img_head_t) + (uint32_t)(uintptr_t)(fill_data);

        for (float y = y_start; y <= y_end; y++)
        {
            l3_render_with_texture_argb8888(y, y0, y1, y2, \
                                            inv_z0, inv_z1, inv_z2, \
                                            soverz0, soverz1, soverz2, \
                                            toverz0, toverz1, toverz2, \
                                            k01infinite, k01, b01, \
                                            k02infinite, k02, b02, \
                                            zbuffer, writebuf, width, \
                                            src_head, image_addr);
        }
    }

}


