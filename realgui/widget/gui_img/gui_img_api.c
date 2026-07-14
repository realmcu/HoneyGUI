/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_fb.h"

static void gui_img_rect_copy(uint8_t       *target,
                              uint8_t       *source,
                              uint16_t       x,
                              uint16_t       y,
                              uint16_t       w,
                              uint16_t       h,
                              gui_dispdev_t *dc)
{
    (void)x;
    (void)y;
    uint16_t byte = dc->bit_depth / 8;

    for (uint16_t i = 0; i < h; i++)
    {
        // Use size_t type for intermediate multiplications to avoid sign extension issues
        size_t target_offset = (size_t)i * (size_t)w * (size_t)byte;
        size_t source_offset = (size_t)i * (size_t)dc->fb_width * (size_t)byte;

        memcpy(target + target_offset, source + source_offset, (size_t)w * (size_t)byte);
    }
}

static void gui_img_virtual_dc_update(struct gui_dispdev *dc)
{
    if (dc->virtual_lcd_update != NULL)
    {
        dc->virtual_lcd_update(dc);
        return;
    }

    float scale = 1;
    uint16_t w = (uint16_t)(dc->fb_width * scale);
    uint16_t h = (uint16_t)(dc->fb_height * scale);
    uint16_t byte = dc->bit_depth / 8;

    uint32_t total_section_cnt = (uint32_t)(dc->screen_height / dc->fb_height);
    if (dc->screen_height % dc->fb_height != 0)
    {
        total_section_cnt += 1;
    }

    if (dc->type == DC_SINGLE)
    {
        for (uint16_t y = 0; y < h; y++)
        {
            size_t target_offset = 8 + (size_t)y * (size_t)w * (size_t)byte;
            size_t source_offset = (size_t)y * (size_t)dc->fb_width * (size_t)byte;

            memcpy(dc->shot_buf + target_offset, dc->frame_buf + source_offset, (size_t)w * (size_t)byte);
        }
        // gui_log("[GUI warning] please use user method for improve! \n");
    }
    else if (dc->type == DC_RAMLESS)
    {
        uint8_t *dst = dc->shot_buf + 8 + (size_t)w * (size_t)dc->fb_height * (size_t)dc->section_count *
                       (size_t)byte;
        if (dc->section_count == 0)
        {
            gui_img_rect_copy(dst, dc->frame_buf, 0, 0, w, dc->fb_height, dc);
        }
        else if (dc->section_count == total_section_cnt - 1)
        {
            uint32_t last_height = dc->screen_height - dc->section_count * dc->fb_height;
            gui_img_rect_copy(dst, dc->frame_buf, 0, 0, w, last_height, dc);
            // gui_log("[GUI warning] please use user method for improve! \n");
        }
        else
        {
            gui_img_rect_copy(dst, dc->frame_buf, 0, 0, w, dc->fb_height, dc);
        }
    }
}





void gui_img_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf)
{
    (void)matrix;
    gui_dispdev_t *dc = gui_get_dc();
    gui_dispdev_t *dc_bak = gui_malloc(sizeof(gui_dispdev_t));
    GUI_ASSERT(dc_bak != NULL);
    gui_matrix_t *matrix_bak = gui_malloc(sizeof(gui_matrix_t));
    GUI_ASSERT(matrix_bak != NULL);
    memcpy(dc_bak, dc, sizeof(gui_dispdev_t));
    memcpy(matrix_bak, obj->matrix, sizeof(gui_matrix_t));
    matrix_scale(1.0f, 1.0f, obj->matrix);

    dc->bit_depth = 16;

    dc->lcd_update = gui_img_virtual_dc_update;
    dc->shot_buf = shot_buf;

    gui_fb_change();
    gui_fb_disp(obj, false);
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)(dc->shot_buf);

    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = 0;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = obj->w * 1.0f;
    head->h = obj->h * 1.0f;

    memcpy(dc, dc_bak, sizeof(gui_dispdev_t));
    memcpy(obj->matrix, matrix_bak, sizeof(gui_matrix_t));
    gui_free(dc_bak);
    gui_free(matrix_bak);
}

uint8_t *gui_rle_solid_color_create(uint16_t width, uint16_t height, gui_color_t color)
{
    /* RLE compression algorithm constants (compress/rle.ts: COMPRESS_RLE = 0) */
#define PURE_RLE_ALGORITHM   0
#define PURE_RLE_FEATURE_1   1   /* RLECompression default runLength1 = 1 */
#define PURE_RLE_FEATURE_2   0   /* default runLength2 = 0 */
#define PURE_RLE_MAX_RUN     255 /* max repeat count per RLE node */

    /* gui_rgb_data_head_t flags byte: set compress bit only (bit4) */
#define PURE_RLE_HEAD_FLAGS  0x10

    gui_dispdev_t *dc = gui_get_dc();
    uint8_t bpp = dc->bit_depth / 8;
    bpp = bpp > 3 ? 3 : bpp;

    size_t total = 0;
    {
        /* width identical pixels per row -> ceil(width/255) RLE nodes */
        size_t nodes_per_line = ((size_t)width + (PURE_RLE_MAX_RUN - 1)) / PURE_RLE_MAX_RUN;
        size_t bytes_per_line = nodes_per_line * (size_t)(1 + bpp);

        size_t header_bytes = 8                                   /* gui_rgb_data_head_t */
                              + 12                                /* imdc_file_header_t  */
                              + (size_t)(height + 1) * 4;         /* row offset table    */

        total = header_bytes + bytes_per_line * (size_t)height;
    }

    void *buffer = gui_malloc(total);
    GUI_ASSERT(buffer != NULL);
    uint8_t *p = buffer;

    /* ---- 1. gui_rgb_data_head_t (8B) ---- */
    p[0] = PURE_RLE_HEAD_FLAGS;             /* flags: compress=1 */
    p[1] = bpp == 2 ? 0 : 3;                /* type               */
    p[2] = (uint8_t)(width & 0xFF);         /* width  (int16 LE)  */
    p[3] = (uint8_t)((width >> 8) & 0xFF);
    p[4] = (uint8_t)(height & 0xFF);        /* height (int16 LE)  */
    p[5] = (uint8_t)((height >> 8) & 0xFF);
    p[6] = 0;                               /* version            */
    p[7] = 0;                               /* rsvd2              */
    p += 8;

    /* ---- 2. imdc_file_header_t (12B) ---- */
    p[0] = (uint8_t)((PURE_RLE_ALGORITHM & 0x03)
                     | ((PURE_RLE_FEATURE_1 & 0x03) << 2)
                     | ((PURE_RLE_FEATURE_2 & 0x03) << 4)
                     | (((bpp - 2) & 0x03) << 6));
    p[1] = 0;                       /* reserved[0] */
    p[2] = 0;                       /* reserved[1] */
    p[3] = 0;                       /* reserved[2] */
    *((uint32_t *)(p + 4)) = (uint32_t)(width);    /* raw_pic_width  */
    *((uint32_t *)(p + 8)) = (uint32_t)(height);   /* raw_pic_height */
    p += 12;

    /* ---- 3. Row offset table (height+1) * 4B ----
     * offset base = start of imdc header (file offset 8), imdcOffset = 12 + (height+1)*4 */
    size_t nodes_per_line = ((size_t)width + (PURE_RLE_MAX_RUN - 1)) / PURE_RLE_MAX_RUN;
    size_t bytes_per_line = nodes_per_line * (size_t)(1 + bpp);
    uint32_t imdc_offset = (uint32_t)(12 + (size_t)(height + 1) * 4);

    for (uint16_t line = 0; line < height; line++)
    {
        /* All rows have the same byte count: lineOffset = line * bytes_per_line */
        *((uint32_t *)p) = imdc_offset + (uint32_t)((size_t)line * bytes_per_line);
        p += 4;
    }
    /* Last entry = imdc_offset + total compressed data length (end of file) */
    *((uint32_t *)p) = imdc_offset + (uint32_t)(bytes_per_line * (size_t)height);
    p += 4;

    /* ---- 4. RLE compressed data ---- */
    uint8_t pix[3];
    uint8_t r = color.color.rgba.r;
    uint8_t g = color.color.rgba.g;
    uint8_t b = color.color.rgba.b;
    switch (bpp)
    {
    case 2:
        *((uint16_t *)pix) = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        break;
    case 3:  /* BGR */
        pix[0] = b;
        pix[1] = g;
        pix[2] = r;
        break;
    default:
        break;
    }

    for (uint16_t line = 0; line < height; line++)
    {
        uint16_t remain = width;
        while (remain > 0)
        {
            uint8_t run = (remain > PURE_RLE_MAX_RUN)
                          ? (uint8_t)PURE_RLE_MAX_RUN
                          : (uint8_t)remain;
            *p++ = run;                      /* [len] */
            memcpy(p, pix, (size_t)bpp);     /* [pixel] */
            p += bpp;
            remain = (uint16_t)(remain - run);
        }
    }

    return buffer;
}


