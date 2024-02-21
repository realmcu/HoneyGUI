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
#include "acc_sw_rle.h"
#include "acc_sw_rle_bypass.h"
#include "acc_sw_rle_cover.h"
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

    // gui_log("file->compressed_addr[%d] %d\n", line, file->compressed_addr[line]);
    for (uint32_t addr = start; addr < end;)
    {
        imdc_rgb565_node_t *node = (imdc_rgb565_node_t *)addr;
        // gui_log("%d 0x%x\n", node->len, node->pixel16);
        gui_memset16(linebuf, node->pixel16, node->len);

        addr = addr + sizeof(imdc_rgb565_node_t);
        linebuf = linebuf + node->len;
    }
}
void uncompressed_rle_argb8565(imdc_file_t *file, uint32_t line,  uint8_t *buf)
{
    //imdc_file_header_t *header = (imdc_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint8_t *linebuf = (uint8_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        imdc_argb8565_node_t *node = (imdc_argb8565_node_t *)addr;
        for (uint32_t i = 0; i < node->len; i++)
        {
            linebuf[i * 3 + 0] = node->pixel & 0xff;
            linebuf[i * 3 + 1] = (node->pixel >> 8) & 0xff;
            linebuf[i * 3 + 2] = node->alpha;
        }
        addr = addr + sizeof(imdc_argb8565_node_t);
        linebuf = linebuf + node->len * 3;
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

void sw_acc_rle_uncompress(draw_img_t *image, void *buf)
{
    int source_w = image->img_w;
    int source_h = image->img_h;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t img_color_type = *((uint8_t *)image_off + 1);
    imdc_file_t *file = (imdc_file_t *)image_off;
    uint8_t *line_buf = buf;
    struct gui_rgb_data_head *head = image->data;

    memcpy(buf, head, sizeof(struct gui_rgb_data_head));
    head = buf;
    line_buf = (uint8_t *)(sizeof(struct gui_rgb_data_head) + (uint32_t)(buf));
    if (img_type == 4)//rle_rgb565
    {
        uint8_t source_bytes_per_pixel = 2;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_rgb565(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGB565;
    }
    else if (img_type == 68 && img_color_type == ARGB8565)//rle_argb8565
    {
        uint8_t source_bytes_per_pixel = 3;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_argb8565(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = ARGB8565;
    }
    else if (img_type == 68 && img_color_type == RGB888)//rle_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_rgb888(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGB888;
    }
    else if (img_type == 132)//rle_rgba8888
    {
        uint8_t source_bytes_per_pixel = 4;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_argb8888(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGBA8888;
    }
    return;
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
    if (image->blend_mode == IMG_COVER_MODE && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_cover_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_cover_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_cover_blit_2_argb8888(image, dc, rect);
        }

    }
    else if (image->blend_mode == IMG_BYPASS_MODE && (identity == true))//no matrix
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
    else if (image->blend_mode == IMG_BYPASS_MODE && (identity == false))//matrix
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
