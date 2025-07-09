/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
  * @brief the view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details switch the current view by sliding or clicking
  * @author shel_deng@realsil.com.cn
  * @date 2025/02/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_VIEW_TRANSITON_H__
#define __GUI_VIEW_TRANSITON_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_matrix.h"


void gui_view_translation(gui_view_t *_this, int16_t release);

void gui_view_cube(gui_view_t *_this, int16_t release);

void gui_view_rotate(gui_view_t *_this, int16_t release);

void gui_view_reduction(gui_view_t *_this, int16_t release);

void gui_view_blur(gui_view_t *this, int16_t release);

void view_transition_animation(void *obj, float pro);


#ifdef __cplusplus
}
#endif
#endif
