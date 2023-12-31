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
#include "acc_engine.h"
#include "acc_sw_rle.h"
#include "acc_sw_bypass.h"
#include "acc_sw_bypass_matrix.h"
#include "acc_sw_filter.h"
#include "acc_sw_filter_matrix.h"
#include "acc_sw_source_over.h"
#include "acc_sw_source_over_matrix.h"
#include "acc_sw.h"

/** @defgroup ENGINE ENGINE
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup ENGINE_Exported_Types ENGINE Exported Types
  * @{
  */


/** End of ENGINE_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup ENGINE_Exported_Constants ENGINE Exported Constants
  * @{
  */


/** End of ENGINE_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup ENGINE_Exported_Macros ENGINE Exported Macros
  * @{
  */


/** End of ENGINE_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup ENGINE_Exported_Variables ENGINE Exported Variables
  * @{
  */


/** End of ENGINE_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup ENGINE_Exported_Functions ENGINE Exported Functions
  * @{
  */





void no_rle(draw_img_t *image, struct gui_dispdev *dc,
            gui_rect_t *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_rgb_data_head *head = image->data;
    char img_type = head->type;
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

    if ((image->blend_mode == IMG_BYPASS_MODE) && (identity == true))
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                bypass_blit_rgb565_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                bypass_blit_argb8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                bypass_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {

                bypass_blit_argb8888_2_rgb888(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                bypass_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                bypass_blit_argb8888_2_argb8888(image, dc, rect);
            }
        }
    }
    if ((image->blend_mode == IMG_BYPASS_MODE) && (identity == false))
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                bypass_matrix_blit_rgb565_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_matrix_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                bypass_matrix_blit_argb8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                bypass_matrix_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_matrix_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {

                bypass_matrix_blit_argb8888_2_rgb888(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                bypass_matrix_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                bypass_matrix_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                bypass_matrix_blit_argb8888_2_argb8888(image, dc, rect);
            }
        }
    }
    if ((image->blend_mode == IMG_FILTER_BLACK) && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                filter_blit_rgb565_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                filter_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                filter_blit_rgba8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                filter_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                filter_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                filter_blit_argb8888_2_rgb888(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                filter_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                filter_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                filter_blit_rgba8888_2_argb8888(image, dc, rect);
            }
        }
    }
    if ((image->blend_mode == IMG_FILTER_BLACK) && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                cpu_filter_matrix_blit_rgb565_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                cpu_filter_matrix_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                cpu_filter_matrix_blit_argb8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                cpu_filter_matrix_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                cpu_filter_matrix_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                cpu_filter_matrix_blit_argb8888_2_rgb888(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                cpu_filter_matrix_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                cpu_filter_matrix_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                cpu_filter_matrix_blit_argb8888_2_argb8888(image, dc, rect);
            }
        }
    }
    if ((image->blend_mode == IMG_SRC_OVER_MODE) && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                alpha_matrix_blit_rgb565_2_rgb565(image, dc, rect);
            }
            if (img_type == ARGB8565)
            {
                //alpha_matrix_blitargb8565_to_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_matrix_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_matrix_blit_rgba8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                alpha_matrix_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_matrix_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_matrix_blit_rgba8888_2_rgb888(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                alpha_matrix_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_matrix_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_matrix_blit_argb8888_2_argb8888(image, dc, rect);
            }
        }
    }
    if ((image->blend_mode == IMG_SRC_OVER_MODE) && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            if (img_type == RGB565)
            {
                alpha_blend_blit_rgb565_2_rgb565(image, dc, rect);
            }
            if (img_type == ARGB8565)
            {
                alpha_blend_blit_argb8565_to_rgb565(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_blend_blit_rgb888_2_rgb565(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_blend_blit_argb8888_2_rgb565(image, dc, rect);
            }
        }
        else if (dc_bytes_per_pixel == 3)
        {
            if (img_type == RGB565)
            {
                alpha_blend_blit_rgb565_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_blend_blit_rgb888_2_rgb888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_blend_blit_rgba8888_2_rgb888(image, dc, rect);
            }
        }
        if (dc_bytes_per_pixel == 4)
        {
            if (img_type == RGB565)
            {
                alpha_blend_blit_rgb565_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGB888)
            {
                alpha_blend_blit_rgb888_2_argb8888(image, dc, rect);
            }
            else if (img_type == RGBA8888)
            {
                alpha_blend_blit_argb8888_2_argb8888(image, dc, rect);
            }
        }
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



void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    struct gui_rgb_data_head *head = image->data;

    char img_type = head->type;

    if (img_type == IMDC_COMPRESS)
    {
        rle(image, dc, rect);
    }
    else if (img_type != IMDC_COMPRESS)
    {
        no_rle(image, dc, rect);
    }

}
/** End of ENGINE_Exported_Functions
  * @}
  */

/** End of ENGINE
  * @}
  */
