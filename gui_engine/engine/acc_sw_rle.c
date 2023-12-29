/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/08
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_engine.h"
#include "acc_sw_blend.h"
#include "acc_sw.h"
#include "acc_sw_rle.h"
#include "acc_sw_rle_bypass.h"
#include "acc_sw_rle_bypass_matrix.h"
#include "acc_sw_rle_filter.h"
#include "acc_sw_rle_filter_matrix.h"
#include "acc_sw_rle_source_over.h"
#include "acc_sw_rle_source_over_matrix.h"


#if defined ( __CC_ARM ) && !defined(TARGET_RTL8773E)
#define __FPU_PRESENT                  1            /* FPU present                                                             */
#include "arm_math.h"
#else
#include "math.h"
#endif

void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) //rgb565
{
#if defined ( __CC_ARM ) && !defined(TARGET_RTL8773E)
    arm_fill_q15(pixel, (int16_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len)  //argb8888
{
#if defined ( __CC_ARM ) && !defined(TARGET_RTL8773E)
    arm_fill_q31(pixel, (int32_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
void uncompressed_rle_rgb565(imdc_file_t *file, uint32_t line,  uint8_t *buf)
{
    //imdc_file_header_t *header = (imdc_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint16_t *linebuf = (uint16_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        imdc_rgb565_node_t *node = (imdc_rgb565_node_t *)addr;
        gui_memset16(linebuf, node->pixel16, node->len);

        addr = addr + sizeof(imdc_rgb565_node_t);
        linebuf = linebuf + node->len;
    }
}
void uncompressed_rle_rgb888(imdc_file_t *file, uint32_t line,  uint8_t *buf)
{
    //imdc_file_header_t *header = (imdc_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint8_t *linebuf = (uint8_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        imdc_rgb888_node_t *node = (imdc_rgb888_node_t *)addr;
        for (uint32_t i = 0; i < node->len; i++)
        {
            linebuf[i * 3]     = node->pixel_b;
            linebuf[i * 3 + 1] = node->pixel_g;
            linebuf[i * 3 + 2] = node->pixel_r;
        }
        addr = addr + sizeof(imdc_rgb888_node_t);
        linebuf = linebuf + node->len * 3;
    }
}
void uncompressed_rle_argb8888(imdc_file_t *file, uint32_t line,  uint8_t *buf)
{
    //imdc_file_header_t *header = (imdc_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint32_t *linebuf = (uint32_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        imdc_argb8888_node_t *node = (imdc_argb8888_node_t *)addr;
        gui_memset32(linebuf, node->pixel32, node->len);

        addr = addr + sizeof(imdc_argb8888_node_t);
        linebuf = linebuf + node->len;
    }
}

void rle(draw_img_t *image, struct gui_dispdev *dc,
         gui_rect_t *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    gui_matrix_t *matrix = image->matrix;
    bool identity = true;
    if (
        (matrix->m[0][0] == 1) && \
        (matrix->m[1][1] == 1) && \
        (matrix->m[2][2] == 1) && \
        (matrix->m[0][1] == 0) && \
        (matrix->m[1][0] == 0) && \
        (matrix->m[2][0] == 0) && \
        (matrix->m[2][1] == 0)
    )
    {
        identity = true;
    }
    else
    {
        identity = false;
    }
    if (image->blend_mode == IMG_BYPASS_MODE && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_bypass_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_bypass_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_bypass_blit_2_argb8888(image, dc, rect);
        }

    }
    if (image->blend_mode == IMG_BYPASS_MODE && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_bypass_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_bypass_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_bypass_matrix_blit_2_argb8888(image, dc, rect);
        }

    }
    else if (image->blend_mode == IMG_FILTER_BLACK && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_filter_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_filter_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_filter_blit_2_argb8888(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_FILTER_BLACK && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_filter_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_filter_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_filter_matrix_blit_2_argb8888(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_alpha_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_alpha_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_alpha_matrix_blit_2_argb8888(image, dc, rect);
        }

    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_alpha_blend_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_alpha_blend_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_alpha_blend_blit_2_argb8888(image, dc, rect);
        }

    }
    return;

}

