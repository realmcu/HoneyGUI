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
#include <acc_sw_raster.h>
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

void uncompressed_rle_line(imdc_file_t *file, uint32_t line, int16_t x, int16_t w, uint8_t *buf)
{
#if 0
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];

    uint16_t *linebuf = (uint16_t *)buf;
    uint16_t section = 0;
    for (uint32_t addr = start; addr < end; addr = addr + sizeof(imdc_rgb565_node_t))
    {
        imdc_rgb565_node_t *node = (imdc_rgb565_node_t *)addr;

        uint16_t right = section + node->len;
        uint16_t left = section;

        if ((left < x) && (right < x))
        {
            ;
        }
        else if ((left > (x + w)) && (right > (x + w)))
        {
            break;
        }
        else if ((left < x) && (right > x))
        {
            gui_memset16(linebuf, node->pixel16, right - x);
            linebuf++;
        }
        else if ((left < (x + w)) && (right > (x + w)))
        {
            gui_memset16(linebuf, node->pixel16, 1);
            linebuf++;
        }
        else if ((left < x) && (right > (x + w)))
        {
            gui_memset16(linebuf, node->pixel16, 1);
            linebuf++;
        }
        else
        {
            gui_memset16(linebuf, node->pixel16, node->len);
            linebuf = linebuf + node->len;
        }

        section = section + node->len;

    }
#endif
    uint8_t linebuf[1024 * 2];
    uncompressed_rle_rgb565(file, line, linebuf);
    memcpy(buf, linebuf + x * 2, w * 2);
}

void uncompressed_rle_rect(imdc_file_t *file, int16_t x, int16_t y, int16_t w, int16_t h,
                           uint8_t *buf)
{
    for (uint32_t i = 0; i < h; i++)
    {
        uncompressed_rle_line(file, i + y, x, w, buf + i * 2 * w);
    }
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

    uint8_t *line_buf = buf;
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
    gui_img_file_t *img_file = (gui_img_file_t *)image->data;
    imdc_file_t *file = (imdc_file_t *)img_file->data.unzip_data;

    memcpy(buf, head, sizeof(struct gui_rgb_data_head));
    head = buf;
    line_buf = (uint8_t *)(sizeof(struct gui_rgb_data_head) + (uint32_t)(buf));
    if (head->type == RGB565)//rle_rgb565
    {
        uint8_t source_bytes_per_pixel = 2;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_rgb565(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGB565;
    }
    else if (head->type == ARGB8565)//rle_argb8565
    {
        uint8_t source_bytes_per_pixel = 3;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_argb8565(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = ARGB8565;
    }
    else if (head->type == RGB888)//rle_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_rgb888(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGB888;
    }
    else if (head->type == RGBA8888)//rle_rgba8888
    {
        uint8_t source_bytes_per_pixel = 4;
        for (int k = 0; k < source_h; k++)
        {
            uncompressed_rle_argb8888(file, k, (uint8_t *)(line_buf + k * source_w * source_bytes_per_pixel));
        }
        head->type = RGBA8888;
    }
    head->compress = 0;
    return;
}

void rle(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    gui_rgb_data_head_t *head = image->data;
    char img_type = head->type;
    gui_matrix_t *matrix = &image->matrix;
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

    if ((dc_bytes_per_pixel == 2) && (identity == true) && (img_type == RGB565))
    {
        if (image->blend_mode == IMG_COVER_MODE)
        {
            rle_cover_blit_2_rgb565(image, dc, rect);
            return;
        }
        else if (image->blend_mode == IMG_BYPASS_MODE)
        {
            rle_bypass_blit_2_rgb565(image, dc, rect);
            return;
        }
        else if (image->blend_mode == IMG_FILTER_BLACK)
        {
            rle_filter_blit_2_rgb565(image, dc, rect);
            return;
        }

    }

    do_raster_use_rle(image, dc, rect);
    return;
}
