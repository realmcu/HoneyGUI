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


void blit_uncompressed(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    gui_rgb_data_head_t *head = image->data;

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
    (void)image;
    // GUI_LINE(1);
    return;
}


void sw_acc_blit(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    gui_rgb_data_head_t *header = (gui_rgb_data_head_t *)image->data;

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

