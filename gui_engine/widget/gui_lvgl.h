/**
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
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_LVGL_H__
#define __GUI_LVGL_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>

/** @defgroup LVGL LVGL
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup LVGL_Exported_Types LVGL Exported Types
  * @brief
  * @{
  */

/** @brief  ... */
typedef struct gui_lvgl_template
{
    gui_obj_t base;
    void *user_data;
} gui_lvgl_t;


/** End of LVGL_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup LVGL_Exported_Constants LVGL Exported Constants
  * @brief
  * @{
  */


/** End of LVGL_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup LVGL_Exported_Macros LVGL Exported Macros
  * @brief
  * @{
  */





/** End of LVGL_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup LVGL_Exported_Variables LVGL Exported Variables
  * @brief
  * @{
  */


/** End of LVGL_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup LVGL_Exported_Functions LVGL Exported Functions
  * @brief
  * @{
  */


/**
 * @brief
 *
 * @param parent
 * @param name
 * @param addr
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_lvgl_t*
 */
gui_lvgl_t *gui_lvgl_create(void *parent,  const char *name, void *addr,
                            int16_t x, int16_t y, int16_t w, int16_t h);


/** End of LVGL_Exported_Functions
  * @}
  */

/** End of LVGL
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

