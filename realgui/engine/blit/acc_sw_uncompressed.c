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
#include "acc_sw_generic.h"
#include "acc_sw_uncompressed.h"
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


void sw_acc_blit_uncompressed(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image == NULL || image->data == NULL || dc == NULL) { return; }
    if (image->img_w <= 0 || image->img_h <= 0) { return; }

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
    sw_acc_blit_generic(image, dc, rect);
}
