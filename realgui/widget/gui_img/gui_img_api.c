/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_api.c
  * @brief image widget
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "acc_api.h"
#include "tp_algo.h"
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
    gui_matrix_t *matrix_bak = gui_malloc(sizeof(gui_matrix_t));
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


