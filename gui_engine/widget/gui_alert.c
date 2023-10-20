/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file  gui_alert.c
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
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_alert.h>
#include <gui_server.h>
#include "gui_obj.h"


/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */


/**
  * @brief  alert constructor
  * @note   private
  * @param this widget pointer
  * @param parent parent widget
  * @param widgetname widgetname
  * @param x left
  * @param y top
  * @param w width
  * @param h hight
  * @param timeout disappear time cost
  * @return void
  * @retval void
  */
static void gui_alert_ctor(struct gui_alert *this, gui_obj_t *parent, const char *widgetname,
                           int16_t x,
                           int16_t y, int16_t w, int16_t h, uint32_t timeout)
{
    gui_obj_ctor(&this->base, parent, widgetname, x, y, w, h);
    uint32_t *p = gui_malloc(sizeof(int) * 2);
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_alert_t *gui_alert_create(gui_obj_t *screen, const char *widget_name, int16_t x, int16_t y,
                              int16_t w, int16_t h, uint32_t timeout)
{
#define _gui_alert_param this, screen,widget_name, x, y, w, h, timeout
    GUI_NEW(gui_alert_t, gui_alert_ctor, _gui_alert_param)
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */









