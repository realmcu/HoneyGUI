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
#include "acc_sw_rle.h"
#include "acc_sw_raster.h"
#include "acc_sw.h"
#include "rgb565_2_rgb565.h"
#include "argb8565_2_rgb565.h"
#include "a8_2_rgb565.h"
#include "a8_2_a8.h"
#include "gui_post_process.h"

#ifndef GUI_A8_BLUR_DEGREE
#define GUI_A8_BLUR_DEGREE 225
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/


static void sw_acc_blur_a8(draw_img_t *image, gui_dispdev_t *dc)
{
    gui_rect_t img_rect =
    {
        .x1 = image->img_target_x,
        .y1 = image->img_target_y,
        .x2 = image->img_target_x + (int16_t)image->img_target_w - 1,
        .y2 = image->img_target_y + (int16_t)image->img_target_h - 1,
    };
    if (image->acc_user == NULL)
    {
        blur_prepare(&img_rect, &image->acc_user);
    }

    gui_rect_t blur_rect = {0};
    if (!rect_intersect(&blur_rect, &img_rect, &dc->section))
    {
        return;
    }

    int16_t blur_w = blur_rect.x2 - blur_rect.x1 + 1;
    int16_t blur_h = blur_rect.y2 - blur_rect.y1 + 1;
    uint8_t  bpp   = dc->bit_depth / 8;

    const uint8_t *a8_base = (const uint8_t *)image->data + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = dc->frame_buf +
                      ((blur_rect.y1 - dc->section.y1) * dc->fb_width + blur_rect.x1) * bpp;

    uint8_t *orig_copy = (uint8_t *)gui_malloc((uint32_t)blur_w * (uint32_t)blur_h * bpp);
    if (orig_copy != NULL)
    {
        for (int16_t row = 0; row < blur_h; row++)
        {
            memcpy(orig_copy + (uint32_t)row * blur_w * bpp,
                   buffer    + (uint32_t)row * dc->fb_width * bpp,
                   (uint32_t)blur_w * bpp);
        }
    }

    if (orig_copy != NULL)
    {
        int16_t img_w = img_rect.x2 - img_rect.x1 + 1;
        int16_t img_h = img_rect.y2 - img_rect.y1 + 1;
        gui_rect_t local_rect = {0, 0, img_w - 1, img_h - 1};

        gui_dispdev_t fake_dc = {0};
        fake_dc.frame_buf     = orig_copy;
        fake_dc.fb_width      = (uint16_t)blur_w;
        fake_dc.screen_width  = (uint16_t)img_w;
        fake_dc.screen_height = dc->screen_height;
        fake_dc.bit_depth     = dc->bit_depth;
        fake_dc.section.x1    = blur_rect.x1 - img_rect.x1;
        fake_dc.section.y1    = blur_rect.y1 - img_rect.y1;
        fake_dc.section.x2    = blur_rect.x2 - img_rect.x1;
        fake_dc.section.y2    = blur_rect.y2 - img_rect.y1;

        gui_get_acc()->blur(&fake_dc, &local_rect, GUI_A8_BLUR_DEGREE, image->acc_user);

        {
            int16_t x, y;
            for (y = 0; y < blur_h; y++)
            {
                int16_t       screen_y = blur_rect.y1 + y;
                const uint8_t *a8_row  = a8_base + (screen_y - image->img_target_y) * (int16_t)image->img_w;
                uint8_t       *p_fb    = buffer  + (uint32_t)y * dc->fb_width * bpp;
                uint8_t       *p_blur  = orig_copy + (uint32_t)y * blur_w * bpp;

                if (dc->bit_depth == 32)
                {
                    uint32_t *fb32    = (uint32_t *)p_fb;
                    uint32_t *blur32  = (uint32_t *)p_blur;

                    for (x = 0; x < blur_w; x++)
                    {
                        int16_t screen_x = blur_rect.x1 + x;
                        uint8_t a        = a8_row[screen_x - image->img_target_x];

                        if (a == 0)
                        {
                        }
                        else if (a == 255)
                        {
                            fb32[x] = blur32[x];
                        }
                        else
                        {
                            uint32_t blurred  = blur32[x];
                            uint32_t orig_val = fb32[x];
                            uint16_t a_inv    = 255 - a;

                            uint8_t r = (uint8_t)((((blurred >> 16) & 0xFF) * a + ((orig_val >> 16) & 0xFF) * a_inv) / 255);
                            uint8_t g = (uint8_t)((((blurred >>  8) & 0xFF) * a + ((orig_val >>  8) & 0xFF) * a_inv) / 255);
                            uint8_t b_ = (uint8_t)(((blurred         & 0xFF) * a + (orig_val         & 0xFF) * a_inv) / 255);

                            fb32[x] = (0xFF000000) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b_;
                        }
                    }
                }
                else
                {
                    uint16_t *fb16    = (uint16_t *)p_fb;
                    uint16_t *blur16  = (uint16_t *)p_blur;

                    for (x = 0; x < blur_w; x++)
                    {
                        int16_t screen_x = blur_rect.x1 + x;
                        uint8_t a        = a8_row[screen_x - image->img_target_x];

                        if (a == 0)
                        {
                        }
                        else if (a == 255)
                        {
                            fb16[x] = blur16[x];
                        }
                        else
                        {
                            uint16_t blurred  = blur16[x];
                            uint16_t orig_val = fb16[x];
                            uint16_t a_inv    = 255 - a;

                            uint16_t r = (((blurred >> 11)        * a + (orig_val >> 11)        * a_inv) / 255) & 0x1F;
                            uint16_t g = ((((blurred >>  5) & 0x3F) * a + ((orig_val >>  5) & 0x3F) * a_inv) / 255) & 0x3F;
                            uint16_t b_ = (((blurred        & 0x1F) * a + (orig_val        & 0x1F) * a_inv) / 255) & 0x1F;

                            fb16[x] = (uint16_t)((r << 11) | (g << 5) | b_);
                        }
                    }
                }
            }
        }

        gui_free(orig_copy);
    }
}


void blit_uncompressed(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image == NULL || image->data == NULL || dc == NULL) { return; }
    if (image->img_w <= 0 || image->img_h <= 0) { return; }

    gui_rgb_data_head_t *head = image->data;

    if (image->blend_mode == IMG_A8_BLUR && (head->type == A8 || head->type == ALPHAMASK))
    {
        sw_acc_blur_a8(image, dc);
        return;
    }

    if ((dc->bit_depth == 16) && (rect == NULL))
    {
        if (head->type == RGB565)
        {
            rgb565_2_rgb565(image, dc, rect);
            return;
        }
        else if (head->type == ARGB8565)
        {
            argb8565_2_rgb565(image, dc, rect);
            return;
        }
        else if (head->type == ALPHAMASK || head->type == A8)
        {
            a8_2_rgb565(image, dc, rect);
            return;
        }

    }
    else if ((dc->bit_depth == 8) && (rect == NULL))
    {
        if (head->type == GRAY)
        {
            a8_2_a8(image, dc, rect);
            return;
        }
    }
    do_raster(image, dc, rect);
}




/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief show image to display
 *
 * @param image image
 * @param dc display
 * @param rect scope
 */

void sw_acc_prepare_cb(draw_img_t *image, gui_rect_t *rect)
{
    (void)image;
    (void)rect;
    // GUI_LINE(1);
    return;
}
void sw_acc_end_cb(draw_img_t *image)
{
    if (image->acc_user != NULL && blur_depose != NULL)
    {
        blur_depose(&image->acc_user);
    }
    return;
}


void sw_acc_blit(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    gui_rgb_data_head_t *header = (gui_rgb_data_head_t *)image->data;

    if (header == NULL)
    {
        return;
    }

    if (header->compress)
    {
        blit_compressed(image, dc, rect);
    }
    else
    {
        blit_uncompressed(image, dc, rect);
    }
}
void sw_acc_init(void)
{
    extern void (* draw_img_acc_prepare_cb)(struct draw_img * image, gui_rect_t *rect);
    extern void (* draw_img_acc_end_cb)(struct draw_img * image);
    draw_img_acc_prepare_cb = sw_acc_prepare_cb;
    draw_img_acc_end_cb = sw_acc_end_cb;
}

