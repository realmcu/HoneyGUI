/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>
#include "acc_sw_idu.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define CLIP(X)  ((X) < 0) ? 0 : (((X) > 255) ? 255 : (X))

#define CYCbCr2R(Y, Cb, Cr) (Y +(91881 * Cr >> 16) - 179)
#define CYCbCr2G(Y, Cb, Cr) (Y - ((22544 * Cb + 46793 * Cr) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) (Y + (116129 * Cb >> 16) - 226)

typedef enum
{
    LITERAL_RUN = 1,
    SHORT_MATCH = 2,
    LONG_MATCH = 3
} FastLZ_instruction;

#ifdef REFER_TO_SW_ACC_RLE
typedef struct
{
    uint8_t len;
    uint16_t pixel16;
} rle_16bit_node_t;

typedef struct
{
    uint8_t len;
    uint8_t pixel_byte1;
    uint8_t pixel_byte2;
    uint8_t pixel_byte3;
} rle_24bit_node_t;

typedef struct imdc_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;
} rle_32bit_node_t;

static void decode_RLE_line_16(uint8_t *in, uint8_t *out, uint8_t *limit)
{
    uint8_t *ip = in;
    uint8_t *ref = out;
    uint32_t len = 0;
    uint16_t *linebuf = (uint16_t *)out;
    uint16_t pixel = 0;
    while (ip < limit)
    {
        rle_16bit_node_t *node = (rle_16bit_node_t *)ip;
        ip += sizeof(rle_16bit_node_t);
        len = node->len;
        pixel = node->pixel16;
        for (uint32_t i = 0; i < len; i++)
        {
            *linebuf++ = pixel;
        }
    }
}

static void decode_RLE_line_24(uint8_t *in, uint8_t *out, uint8_t *limit)
{
    uint8_t *ip = in;
    uint8_t *ref = out;
    uint32_t len = 0;
    uint8_t *linebuf = (uint8_t *)out;
    while (ip < limit)
    {
        len = *ip++;
        uint8_t pixel_R = *ip++;
        uint8_t pixel_G = *ip++;
        uint8_t pixel_B = *ip++;
        for (uint32_t i = 0; i < len; i++)
        {
            *linebuf++ = pixel_R;
            *linebuf++ = pixel_G;
            *linebuf++ = pixel_B;
        }
    }
}

static void decode_RLE_line_32(uint8_t *in, uint8_t *out, uint8_t *limit)
{
    uint8_t *ip = in;
    uint8_t *ref = out;
    uint32_t len = 0;
    uint16_t pixel = 0;
    uint32_t *linebuf = (uint32_t *)out;
    while (ip < limit)
    {
        rle_32bit_node_t *node = (rle_32bit_node_t *)ip;
        ip += sizeof(rle_32bit_node_t);
        len = node->len;
        pixel = node->pixel32;
        for (uint32_t i = 0; i < len; i++)
        {
            *linebuf++ = pixel;
        }
    }
}

static void decode_RLE_16bit(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1);
    uint32_t line_addr = 0;
    uint32_t next_addr = 0;
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_16(in + addr_set[line], out, in + addr_set[line + 1]);
            out += range->target_stride;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * 2);
        uint32_t line_offset = range->start_column * 2;
        uint32_t line_length = width * 2;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_16(in + addr_set[line], line_buffer, in + addr_set[line + 1]);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
        }
    }
}

static void decode_RLE_24bit(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1);
    uint32_t line_addr = 0;
    uint32_t next_addr = 0;
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_24(in + addr_set[line], out, in + addr_set[line + 1]);
            out += range->target_stride;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * 3);
        uint32_t line_offset = range->start_column * 3;
        uint32_t line_length = width * 3;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_24(in + addr_set[line], line_buffer, in + addr_set[line + 1]);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
        }
    }
}

static void decode_RLE_32bit(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1);
    uint32_t line_addr = 0;
    uint32_t next_addr = 0;
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_32(in + addr_set[line], out, in + addr_set[line + 1]);
            out += range->target_stride;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * 4);
        uint32_t line_offset = range->start_column * 4;
        uint32_t line_length = width * 4;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            line_addr = (uintptr_t)in + addr_set[line];
            next_addr = (uintptr_t)in + addr_set[line + 1];
            decode_RLE_line_32(in + addr_set[line], line_buffer, in + addr_set[line + 1]);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
        }
    }
}
#endif

static void decode_RLE_1stage_line(uint8_t *in, uint8_t *out, uint8_t *limit, uint8_t run_length,
                                   uint8_t pixel_size)
{
    uint8_t *ip = in;
    uint8_t *op = out;
    uint32_t len = 0;
    uint32_t run_pixel = 0;
    while (ip < limit)
    {
        for (uint32_t i = 0; i < run_length; i++)
        {
            len = len + (*ip++ << 8 * i);
        }
        for (uint32_t i = 0; i < pixel_size; i++)
        {
            run_pixel = run_pixel + (*ip++ << 8 * i);
        }
        for (uint32_t i = 0; i < len; i++)
        {
            for (uint32_t j = 0; j < pixel_size; j++)
            {
                *op++ = ((run_pixel >> 8 * j) & 0xFF);
            }

        }
        len = 0;
        run_pixel = 0;
    }
}

static void decode_RLE_1stage(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint8_t pixel_size = (head->algorithm_type.pixel_bytes + 2);
    uint32_t width = (range->end_column - range->start_column + 1) * pixel_size;
    uint32_t line_addr = addr_set[range->start_line];
    uint32_t next_addr = 0;
    uint8_t run_len = head->algorithm_type.feature_1;

    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_RLE_1stage_line(in + line_addr, out, in + next_addr, run_len, pixel_size);
            out += range->target_stride;
            line_addr = next_addr;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * pixel_size);
        uint32_t line_offset = range->start_column * pixel_size;
        uint32_t line_length = width;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_RLE_1stage_line(in + line_addr, line_buffer, in + next_addr, run_len, pixel_size);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
            line_addr = next_addr;
        }
        gui_free(line_buffer);
    }
}

static void decode_RLE_2stage_line(uint8_t *in, uint8_t *out, uint8_t *limit, uint8_t run_length1,
                                   uint8_t run_length2, uint8_t pixel_size)
{
    uint8_t *ip = in;
    uint32_t len2 = 0;
    uint32_t len1 = 0;
    uint32_t run_pixel = 0;
    uint8_t *op = out;
    while (ip < limit)
    {
        for (uint32_t i = 0; i < run_length2; i++)
        {
            len2 = len2 + (*ip++ << 8 * i);;
        }
        for (uint32_t i = 0; i < run_length1; i++)
        {
            len1 = len1 + (*ip++ << 8 * i);
        }
        for (uint32_t i = 0; i < len2; i++)
        {
            for (uint32_t j = 0; j < pixel_size; j++)
            {
                run_pixel = run_pixel + (*ip++ << 8 * j);
            }
            for (uint32_t j = 0; j < len1; j++)
            {
                for (int k = 0; k < pixel_size; k++)
                {
                    *op++ = ((run_pixel >> 8 * k) & 0xFF);
                }
            }
            run_pixel = 0;

        }
        len1 = 0;
        len2 = 0;
    }
}

static void decode_RLE_2stage(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
    uint8_t pixel_size = (head->algorithm_type.pixel_bytes + 2);
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1) * pixel_size;
    uint32_t line_addr = addr_set[range->start_line];
    uint32_t next_addr = 0;
    uint8_t run_len1 = head->algorithm_type.feature_1;
    uint8_t run_len2 = head->algorithm_type.feature_2;

    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_RLE_2stage_line(in + line_addr, out, in + next_addr, run_len1, run_len2, pixel_size);
            out += range->target_stride;
            line_addr = next_addr;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * pixel_size);
        uint32_t line_offset = range->start_column * pixel_size;
        uint32_t line_length = width;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_RLE_2stage_line(in + line_addr, line_buffer, in + next_addr, run_len1, run_len2, pixel_size);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
            line_addr = next_addr;
        }
        gui_free(line_buffer);
    }
}

static void decode_FastLZ_line(uint8_t *in, uint8_t *out, uint8_t *limit)
{
    uint8_t ctrl = 0;
    uint8_t *ip = in;
    uint8_t *ref = out;
    FastLZ_instruction inst = LITERAL_RUN;
    uint32_t len = 0;
    uint32_t offset = 0;
    ctrl = *ip++;
    while (ip < limit)
    {
        if (inst == LITERAL_RUN)
        {
            len = (ctrl & 0x1F) + 1;
            memcpy(out, ip, len);
            out += len;
            ip += len;
        }
        else if (inst == SHORT_MATCH)
        {
            offset = ((ctrl & 0x1f) << 8);
            len = (ctrl >> 5) + 2;
            ctrl = *ip++;
            offset += ctrl;
            ref = out - offset - 1;
            for (uint32_t i = 0; i < len; i++)
            {
                *out++ = *ref++;
            }
        }
        else if (inst == LONG_MATCH)
        {
            offset = ((ctrl & 0x1f) << 8);
            len = (ctrl >> 5) + (*ip++) + 2;
            offset += (*ip++);
            ref = out - offset - 1;
            for (uint32_t i = 0; i < len; i++)
            {
                *out++ = *ref++;
            }
        }
        ctrl = *ip++;
        uint8_t cmd = (ctrl >> 5);
        if (cmd == 0)
        {
            inst = LITERAL_RUN;
        }
        else if (cmd == 7)
        {
            inst = LONG_MATCH;
        }
        else
        {
            inst = SHORT_MATCH;
        }
    }

}

static void decode_FastLZ(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1);
    uint32_t line_addr = addr_set[range->start_line];
    uint32_t next_addr = 0;
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_FastLZ_line(in + line_addr, out, in + next_addr);
            out += range->target_stride;
            line_addr = next_addr;
        }
    }
    else
    {
        gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
        uint8_t pixel_size = (head->algorithm_type.pixel_bytes + 2);
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * pixel_size);
        uint32_t line_offset = range->start_column * pixel_size;
        uint32_t line_length = width * pixel_size;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_FastLZ_line(in + line_addr, line_buffer, in + next_addr);
            memcpy(out, line_buffer + line_offset, line_length);
            out += range->target_stride;
            line_addr = next_addr;
        }
        gui_free(line_buffer);
    }
}

static void convert_YUV444_to_RGB(uint8_t *in, uint8_t *out, uint32_t width)
{
    for (uint32_t i = 0; i < width; i++)
    {
        uint8_t Y = *in++;
        uint8_t U = *in++;
        uint8_t V = *in++;
        *out++ = CYCbCr2R(Y, U, V);
        *out++ = CYCbCr2G(Y, U, V);
        *out++ = CYCbCr2B(Y, U, V);
    }
}

static void convert_YUV422_to_RGB(uint8_t *in, uint8_t *out, uint32_t width)
{
    for (uint32_t i = 0; i < width / 4; i ++)
    {
        uint8_t Y0 = *in++;
        uint8_t U0 = *in++;
        uint8_t Y1 = *in++;
        uint8_t V0 = *in++;
        uint8_t Y2 = *in++;
        uint8_t U1 = *in++;
        uint8_t Y3 = *in++;
        uint8_t V1 = *in++;

        *out++ = CYCbCr2R(Y0, U0, V0);
        *out++ = CYCbCr2G(Y0, U0, V0);
        *out++ = CYCbCr2B(Y0, U0, V0);
        *out++ = CYCbCr2R(Y1, U0, V0);
        *out++ = CYCbCr2G(Y1, U0, V0);
        *out++ = CYCbCr2B(Y1, U0, V0);
        *out++ = CYCbCr2R(Y2, U1, V1);
        *out++ = CYCbCr2G(Y2, U1, V1);
        *out++ = CYCbCr2B(Y2, U1, V1);
        *out++ = CYCbCr2R(Y3, U1, V1);
        *out++ = CYCbCr2G(Y3, U1, V1);
        *out++ = CYCbCr2B(Y3, U1, V1);
    }
    uint8_t rest = width % 4;
    if (rest)
    {
        for (uint32_t i = 0; i < rest; i++)
        {
            uint8_t Y = *in++;
            uint8_t U = *in++;
            uint8_t V = *in++;
            *out++ = CYCbCr2R(Y, U, V);
            *out++ = CYCbCr2G(Y, U, V);
            *out++ = CYCbCr2B(Y, U, V);
        }
    }
}

static void convert_YUV411_to_RGB(uint8_t *in, uint8_t *out, uint32_t width)
{
    for (uint32_t i = 0; i < width / 4; i ++)
    {
        uint8_t Y0 = *in++;
        uint8_t U0 = *in++;
        uint8_t Y1 = *in++;
        uint8_t Y2 = *in++;
        uint8_t V0 = *in++;
        uint8_t Y3 = *in++;

        *out++ = CYCbCr2R(Y0, U0, V0);
        *out++ = CYCbCr2G(Y0, U0, V0);
        *out++ = CYCbCr2B(Y0, U0, V0);
        *out++ = CYCbCr2R(Y1, U0, V0);
        *out++ = CYCbCr2G(Y1, U0, V0);
        *out++ = CYCbCr2B(Y1, U0, V0);
        *out++ = CYCbCr2R(Y2, U0, V0);
        *out++ = CYCbCr2G(Y2, U0, V0);
        *out++ = CYCbCr2B(Y2, U0, V0);
        *out++ = CYCbCr2R(Y3, U0, V0);
        *out++ = CYCbCr2G(Y3, U0, V0);
        *out++ = CYCbCr2B(Y3, U0, V0);
    }
    uint8_t rest = width % 4;
    if (rest)
    {
        for (uint32_t i = 0; i < rest; i++)
        {
            uint8_t Y = *in++;
            uint8_t U = *in++;
            uint8_t V = *in++;
            *out++ = CYCbCr2R(Y, U, V);
            *out++ = CYCbCr2G(Y, U, V);
            *out++ = CYCbCr2B(Y, U, V);
        }
    }
}

static void decode_YUV_Sample_Blur_line(uint8_t *in, uint8_t *out, uint8_t *limit, uint8_t blur_bit,
                                        uint32_t width)
{
    uint8_t *ip = in;
    uint8_t *op = out;
    uint32_t counter_max = width;
    uint32_t counter = 0;
    if (blur_bit == 0)
    {
        while (ip < limit)
        {
            *op++ = *ip++;
        }
    }
    else
    {
        while (counter < counter_max)
        {
            uint8_t remainer_holder = ((0xFF >> blur_bit) << blur_bit);
            uint8_t lsb_holder = (0xFF >> blur_bit);
            uint8_t align_num = 8 / blur_bit;
            if (counter % align_num == 0)
            {
                *op++ = (*ip & remainer_holder);
                counter++;
            }
            else
            {
                int remain = counter % align_num;
                {
                    if (remain < (align_num - 1))
                    {
                        uint8_t shift = 8 - remain * blur_bit;
                        uint8_t first_part = (*ip++ << shift);
                        uint8_t second_part = (*ip >> (remain + 1) * blur_bit) << blur_bit;
                        *op++ = first_part + second_part;
                        counter++;
                    }
                    else
                    {
                        *op++ = ((*ip++ &lsb_holder) << blur_bit);
                        counter++;
                    }
                }
            }
        }
    }
}

static void decode_YUV_Sample_Blur(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1) * 3;
    uint32_t compress_line_length = 0;
    if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
    {
        compress_line_length = head->raw_pic_width * 3;
    }
    else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
    {
        compress_line_length = head->raw_pic_width / 4 * 8 + (head->raw_pic_width % 4) * 3;
    }
    else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
    {
        compress_line_length = head->raw_pic_width / 4 * 6 + (head->raw_pic_width % 4) * 3;
    }

    uint32_t line_addr = addr_set[range->start_line];
    uint32_t next_addr = 0;
    uint8_t *YUVbuf = (uint8_t *)gui_malloc(head->raw_pic_width * 3);
    uint8_t blur_bit = 0;
    if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_1BIT)
    {
        blur_bit = 1;
    }
    else if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_2BIT)
    {
        blur_bit = 2;
    }
    else if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_4BIT)
    {
        blur_bit = 4;
    }
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_YUV_Sample_Blur_line(in + line_addr, YUVbuf, in + next_addr, blur_bit, compress_line_length);
            if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
            {
                convert_YUV444_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
            {
                convert_YUV422_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
            {
                convert_YUV411_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            out += range->target_stride;
            line_addr = next_addr;
        }
    }
    else
    {
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * 3);
        uint32_t line_offset = range->start_column * 3;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_YUV_Sample_Blur_line(in + line_addr, YUVbuf, in + next_addr, blur_bit, compress_line_length);
            if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
            {
                convert_YUV444_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
            {
                convert_YUV422_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
            {
                convert_YUV411_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            memcpy(out, line_buffer + line_offset, width);
            out += range->target_stride;
            line_addr = next_addr;
        }
        gui_free(line_buffer);
    }
    gui_free(YUVbuf);
}

static void decode_YUV_Sample_Blur_FastLZ(uint8_t *in, uint8_t *out, gui_sw_idu_range_t *range)
{
    gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
    uint32_t *addr_set = (uint32_t *)(in + sizeof(gui_sw_idu_head_t));
    uint32_t width = (range->end_column - range->start_column + 1) * 3;
    uint32_t compress_line_length = 0;
    if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
    {
        compress_line_length = head->raw_pic_width * 3;
    }
    else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
    {
        compress_line_length = head->raw_pic_width / 4 * 8 + (head->raw_pic_width % 4) * 3;
    }
    else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
    {
        compress_line_length = head->raw_pic_width / 4 * 6 + (head->raw_pic_width % 4) * 3;
    }
    uint32_t line_addr = addr_set[range->start_line];
    uint32_t next_addr = 0;
    uint8_t *YUVbuf = (uint8_t *)gui_malloc(head->raw_pic_width * 3);
    uint8_t blur_bit = 0;
    uint32_t yuv_blur_compressed_line_length = compress_line_length;
    if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_1BIT)
    {
        blur_bit = 1;
    }
    else if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_2BIT)
    {
        blur_bit = 2;
    }
    else if (head->algorithm_type.feature_2 == GUI_SW_IDU_YUV_BLUR_4BIT)
    {
        blur_bit = 4;
    }
    if (blur_bit == 1)
    {
        yuv_blur_compressed_line_length = compress_line_length / 8 * 7 + compress_line_length % 8;
    }
    else if (blur_bit == 2)
    {
        yuv_blur_compressed_line_length = compress_line_length / 4 * 3 + compress_line_length % 4;
    }
    else if (blur_bit == 4)
    {
        yuv_blur_compressed_line_length = compress_line_length / 2 + compress_line_length % 2;
    }
    uint8_t *fastLZ_decoded_data = (uint8_t *)gui_malloc(yuv_blur_compressed_line_length);
    if (range->target_stride == width)
    {
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_FastLZ_line(in + line_addr, fastLZ_decoded_data, in + next_addr);
            decode_YUV_Sample_Blur_line(fastLZ_decoded_data, YUVbuf,
                                        fastLZ_decoded_data + yuv_blur_compressed_line_length, blur_bit, compress_line_length);
            if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
            {
                convert_YUV444_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
            {
                convert_YUV422_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
            {
                convert_YUV411_to_RGB(YUVbuf, out, head->raw_pic_width);
            }
            out += range->target_stride;
            line_addr = next_addr;
        }
    }
    else
    {
        uint8_t *line_buffer = (uint8_t *)gui_malloc(head->raw_pic_width * 3);
        uint32_t line_offset = range->start_column * 3;
        for (uint32_t line = range->start_line; line <= range->end_line; line++)
        {
            next_addr = addr_set[line + 1];
            decode_FastLZ_line(in + line_addr, fastLZ_decoded_data, in + next_addr);
            decode_YUV_Sample_Blur_line(fastLZ_decoded_data, YUVbuf,
                                        fastLZ_decoded_data + yuv_blur_compressed_line_length, blur_bit, compress_line_length);
            if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV444_SAMPLE)
            {
                convert_YUV444_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV422_SAMPLE)
            {
                convert_YUV422_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            else if (head->algorithm_type.feature_1 == GUI_SW_IDU_YUV411_SAMPLE)
            {
                convert_YUV411_to_RGB(YUVbuf, line_buffer, head->raw_pic_width);
            }
            memcpy(out, line_buffer + line_offset, width);
            out += range->target_stride;
            line_addr = next_addr;
        }
        gui_free(line_buffer);
    }
    gui_free(YUVbuf);
}


void gui_sw_idu_decode(void *in, gui_rect_t *rect, void *out, uint32_t stride)
{
    gui_sw_idu_head_t *head = (gui_sw_idu_head_t *)in;
    gui_sw_idu_range_t range = {.start_line = rect->y1, .end_line = rect->y2,
                                .start_column = rect->x1, .end_column = rect->x2,
                                .target_stride = stride
                               };
    switch (head->algorithm_type.algorithm)
    {
    case GUI_SW_IDU_FASTLZ:
        decode_FastLZ(in, out, &range);
        break;
    case GUI_SW_IDU_RLE:
        if (head->algorithm_type.feature_2)
        {
            decode_RLE_2stage(in, out, &range);
        }
        else if (head->algorithm_type.feature_1)
        {
            decode_RLE_1stage(in, out, &range);
        }
        break;
    case GUI_SW_IDU_YUV_SAMPLE_BLUR:
        decode_YUV_Sample_Blur(in, out, &range);
        break;
    case GUI_SW_IDU_YUV_SAMPLE_BLUR_FASTLZ:
        decode_YUV_Sample_Blur_FastLZ(in, out, &range);
        break;
    default:
        break;
    }
}


