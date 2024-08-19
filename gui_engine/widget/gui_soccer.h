/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_soccer.h
  * @brief soccer widget
  * @details soccer widget
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/16
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
#ifndef __GUI_SOCCER_H__
#define __GUI_SOCCER_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"
#include "gui_perspective.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */



/** @brief  SOCCER widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif
typedef struct gui_soccer
{
    gui_obj_t base;                 //!< base structure
    draw_img_t draw_img[20];         // please refer to enum T_SOCCER_SIDE_TYPE.
    float xrot;
    float yrot;
    float zrot;
    gui_vertex_t normal[20];
    // float nz[20];

} gui_soccer_t;

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad
#define PHI           ((1 + sqrt(5)) / 2)

/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */


/**
 * @brief soccer create, images can be loaded from filesystem or memory address
 *
 * @param parent parent widget
 * @param name  widget name
 * @param img_file the image file data, set flg_fs true when using filesystem
 * @param x  left
 * @param y top
 * @return gui_soccer_t* widget pointer
 * <b>Example usage</b>
 * \code{.c}
 *{
 *    gui_soccer_imgfile_t imgfile =
 *    {
 *        .flg_fs = true,
 *        .img_path.img_path_front = "Activity.bin",
 *        .img_path.img_path_back = "Weather.bin",
 *        .img_path.img_path_up = "HeartRate.bin",
 *        .img_path.img_path_down = "Clockn.bin",
 *        .img_path.img_path_left = "Music.bin",
 *        .img_path.img_path_right = "QuickCard.bin"
 *    };
 *
 *    gui_soccer_t *soccer4 = gui_soccer_create(parent, "soccer", &imgfile, 0, 0);
 *    gui_soccer_set_size(soccer4, 100);
 *    gui_soccer_set_center(soccer4, 200, 200);
 *}
 * \endcode
 */
gui_soccer_t *gui_soccer_create(void               *parent,
                                const char         *name,
                                void *addr,
                                int16_t             x,
                                int16_t             y);


/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
