/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_WIDGET_LITEGFX_H__
#define __GUI_WIDGET_LITEGFX_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <gui_obj.h>
#include <lx_vglite_api.h>

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
typedef struct gui_widget_litegfx
{
    gui_obj_t base;
    uintptr_t handler;
    uintptr_t instance;
    lx_vglite_user_cb_t callback;
} gui_widget_litegfx_t;


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
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */


/**
 * @brief
 *
 * @param parent
 * @param name
 * @param instance
 * @param cb
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_widget_litegfx_t*
 */
gui_widget_litegfx_t *gui_widget_litegfx_create(void *parent,  const char *name,
                                                uintptr_t instance,
                                                lx_vglite_user_cb_t cb,
                                                int16_t x, int16_t y, int16_t w, int16_t h);


/**
 * @brief
 *
 * @param obj
 * @param index
 * @param data
 * @return void
 */
void gui_widget_litegfx_set_image(gui_obj_t *obj, uint16_t index, void *data);

/**
 * @brief
 *
 * @param obj
 * @param cmd
 * @param param
 * @return void
 */
void gui_widget_litegfx_set_param(gui_obj_t *obj, uint16_t cmd, uintptr_t param);


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

