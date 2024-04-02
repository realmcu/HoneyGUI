/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wheel_list.h
  * @brief wheel_list design
  * @details Evenly layout vertically and horizontally
  * @author luke_sun@realsil.com.cn
  * @date 2024/4/1
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
#ifndef __GUI_WHEEL_LIST_H__
#define __GUI_WHEEL_LIST_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"
#include "gui_img.h"
#include "math.h"

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

/** @brief  support two style */

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
#define RADIAN_60    M_PI/3


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
typedef enum layer_state
{
    RESTING,
    ANTICLOCKWISE,
    CLOCKWISE,
} LAYER_STATE_T;

typedef struct wheel_angle
{
    LAYER_STATE_T state;
    float angle;
} WHEEL_ANGLE_T;

typedef struct gui_wheel_list
{
    gui_obj_t base;
    uint8_t layer;
    uint8_t radius;
    uint8_t distence;
    WHEEL_ANGLE_T *rotation;
    gui_img_t ** *icon_list;
} gui_wheel_list_t;

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
 * @brief Create a scroll wheel icon list widget.
 *
 * @param parent the father widget the page nested in.
 * @param layer the layer of the wheel list, 0 means there is only one icon in the middle.
 * @param radius Icon image radius.
 * @param distence Distance between the shortest circle center of two layer icon images.
 * @return gui_wheel_list_t*
 */
gui_wheel_list_t *gui_wheel_list_create(void *parent,
                                        uint8_t layer,
                                        uint8_t radius,
                                        uint8_t distence);


/**
 * @brief Add icon images in the default order
 *
 * @param this widget object pointer
 * @param event_cb click cb of this image
 * @param icon_addr picture address
 */
void gui_wheel_list_add_icon_default(gui_wheel_list_t *this,
                                     void *icon_addr,
                                     gui_event_cb_t event_cb);

/**
 * @brief Add icon images
 *
 * @param this widget object pointer
 * @param icon_addr picture address
 * @param event_cb click cb of this image
 * @param layer layer
 * @param index index
 */
void gui_wheel_list_set_icon(gui_wheel_list_t *this,
                             void *icon_addr,
                             gui_event_cb_t event_cb,
                             uint8_t layer,
                             uint8_t index);


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



