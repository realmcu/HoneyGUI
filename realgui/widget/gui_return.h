/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_return.h
  * @brief slide to return to home
  * @details return to launcher
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
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
#ifndef __GUI_RETURN_H__
#define __GUI_RETURN_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_seekbar.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_return
{
    gui_seekbar_t base;
    gui_obj_t *ignore_gesture_widget;
} gui_return_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define RETURN_HEIGHT           100 //widget hight
#define RETURN_ENABLE_THREHOLD  10 //threhold to drag
#define RETURN_THREHOLD         80 //threhold to return

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a return widget.
 * @param parent The parent widget it nested in.
 * @param frame_array Return effect image frame array.
 * @param array_size Return effect image frame array's index count.
 * @param return_cb Return behavior instance .
 * @param ignore_gesture_widget The widget that need to be blocked for gesture response.
 * @return return the widget object pointer.
 *
 */
gui_return_t *gui_return_create(void           *parent,
                                const uint32_t *frame_array[],
                                int             array_size,
                                void           *return_cb,
                                gui_obj_t      *ignore_gesture_widget);




#ifdef __cplusplus
}
#endif

#endif


