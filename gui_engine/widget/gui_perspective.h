/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_perspective.h
  * @brief perspective widget
  * @details create 3D widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/20
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
#ifndef __GUI_PERSPECTIVE_H__
#define __GUI_PERSPECTIVE_H__



#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"

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

/** @brief  ... */
typedef struct VertexRec
{
    float x;                    //!< x axis
    float y;                    //!< y axis
    float z;                    //!< z axis
} Vertex_t;

typedef struct NormalRec
{
    float x;                    //!< x axis
    float y;                    //!< y axis
    float z;                    //!< z axis
} Normal_t;

typedef struct gui_perspective
{
    gui_obj_t base;             //!< base structure
    draw_img_t img[6];
    int16_t release_x;
} gui_perspective_t;

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


#define RAD(d)        (d*3.1415926f/180.0f)     //!< angle to rad


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
  * @brief  create 3D perspective
  * @param  parent parent widget
  * @param  name  widget name
  * @param  addr the pictures frame
  * @param  x  left
  * @param  y top
  * @param  w  width
  * @param  h hight
  * @return gui_perspective_t* widget pointer
  *
  *
  *
  *
  *
  * <b>Example usage</b>
  * \code{.c}
  * void perspctive_example(void *parent)
  * {
  *     gui_perspective_t *img_test = gui_perspective_create(parent, "test", pic, 0, 0, 454, 454);
  * }
  * \endcode
  */
gui_perspective_t *gui_perspective_create(void *parent,  const char *name, void *addr,
                                          int16_t x, int16_t y, int16_t w, int16_t h);


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
