/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file  gui_alert.h
  * @brief alert widget
  * @details pop a alert on screen
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/17
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_ALERT_H__
#define __GUI_ALERT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>

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

/** @brief  define alert structure */
typedef struct gui_alert
{
    gui_obj_t base;                 //!< base structure
    void (*ctor)(struct gui_alert *this, gui_obj_t *parent, const char *widgetname, int16_t x,
                 int16_t y, int16_t w, int16_t h, uint32_t timeout);//!< constructure
} gui_alert_t;

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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
  * @brief  create a alert
  * @param screen parent widget
  * @param widget_name name
  * @param x left
  * @param y top
  * @param w width
  * @param h hight
  * @param timeout pop timeout
  * @return void
  * @retval void
  * <b>Example usage</b>
  * \code{.c}
  * gui_alert_create(parent, "alert", 0,0,0,0,1000);
  * \endcode
  */
gui_alert_t *gui_alert_create(gui_obj_t *screen, const char *widget_name, int16_t x, int16_t y,
                              int16_t w, int16_t h, uint32_t timeout);


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */






#ifdef __cplusplus
}
#endif

#endif

