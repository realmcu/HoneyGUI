/**
 \internal
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_mem_img.h
  * @brief memory font image engine header file
  * @details memory font image engine header file
  * @author luke_sun@realsil.com.cn
  * @date 2024/05/21
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2024 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

#ifndef __FONT_MEM_IMG_H__
#define __FONT_MEM_IMG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#include "font_mem.h"
#include "font_rendering_utils.h"

/**
 * @brief Post-processing work for drawing bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_scale_destroy(gui_text_t *text);

/**
 * @brief transform bmp text to img
 *
 * @param text text pointer
 * @param font_img_type img type
 * @param img_x img x
 * @param img_y img y
 * @return void* text img buffer
 */
void *gui_text_bmp2img(gui_text_t *text, GUI_FormatType font_img_type, int16_t *img_x,
                       int16_t *img_y);


#ifdef __cplusplus
}
#endif

#endif

