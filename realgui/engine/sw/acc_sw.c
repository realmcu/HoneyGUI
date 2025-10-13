/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
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
#include "acc_sw_rle.h"
#include "acc_sw_bypass.h"
#include "acc_sw_filter.h"
#include "acc_sw_raster.h"
#include "acc_sw_transform.h"
#include "acc_sw_blend.h"
#include "acc_sw.h"

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


void no_rle(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    gui_rgb_data_head_t *head = image->data;
    uint8_t opacity = image->opacity_value;

    bool identity = (image->matrix.m[0][0] == 1) &&
                    (image->matrix.m[1][1] == 1) &&
                    (image->matrix.m[2][2] == 1) &&
                    (image->matrix.m[0][1] == 0) &&
                    (image->matrix.m[1][0] == 0) &&
                    (image->matrix.m[2][0] == 0) &&
                    (image->matrix.m[2][1] == 0);

    if (\
        (dc_bytes_per_pixel == 2) && \
        identity && \
        (head->type == RGB565 || head->type == ARGB8565 || head->type == ALPHAMASK) && \
        (opacity == 255) && \
        (rect == NULL) \
       )
    {
        switch (image->blend_mode)
        {
        case IMG_COVER_MODE:
        case IMG_BYPASS_MODE:
            bypass_blit_2_rgb565(image, dc, rect);
            return;
        case IMG_FILTER_BLACK:
            filter_blit_2_rgb565(image, dc, rect);
            return;
        case IMG_2D_SW_SRC_OVER_MODE:
            GUI_ASSERT(head->type == ARGB8565);
            src_over_blit_2_rgb565(image, dc, rect);
            return;
        case IMG_2D_SW_FIX_A8_FG:
            GUI_ASSERT(head->type == ALPHAMASK);
            preconfig_a8_fg(image, dc, rect);
            return;
        case IMG_2D_SW_FIX_A8_BGFG:
            GUI_ASSERT(head->type == ALPHAMASK);
            preconfig_a8_fgbg(image, dc, rect);
            return;
        default:
            break;
        }
    }

    if (image->blend_mode == IMG_2D_SW_RGB565_ONLY)
    {
        sw_transform_for_rgb565(image, dc, rect);
    }
    else
    {
        do_raster(image, dc, rect);
    }


}




/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief show image to display
 *
 * @param image image
 * @param dc dispaly
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
        rle(image, dc, rect);
    }
    else
    {
        no_rle(image, dc, rect);
    }
}
void sw_acc_init(void)
{
    extern void (* draw_img_acc_prepare_cb)(struct draw_img * image, gui_rect_t *rect);
    extern void (* draw_img_acc_end_cb)(struct draw_img * image);
    draw_img_acc_prepare_cb = sw_acc_prepare_cb;
    draw_img_acc_end_cb = sw_acc_end_cb;
}

