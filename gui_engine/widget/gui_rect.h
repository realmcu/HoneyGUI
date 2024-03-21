/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_rect.h
  * @brief image rect widget header file
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#ifndef __GUI_RECT_H__
#define __GUI_RECT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/





/*============================================================================*
 *                         Types
 *============================================================================*/





/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/


/**
 * @brief
 *
 * @param parent
 * @param x
 * @param y
 * @param w
 * @param h
 * @param color
 * @return gui_img_t*
 */
gui_img_t *gui_rect(gui_obj_t *parent, int x, int y, int w, int h, gui_color_t color);
/**
 * @brief
 *
 * @param parent
 * @param x
 * @param y
 * @param w
 * @param h
 * @param color
 * @param image_file_addr
 * @return gui_img_t*
 */
gui_img_t *gui_rect_round(gui_obj_t *parent, int x, int y, int w, int h, gui_color_t color,
                          uint32_t *image_file_addr);
#ifdef __cplusplus
}
#endif

#endif

