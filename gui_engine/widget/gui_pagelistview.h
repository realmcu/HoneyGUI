/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelistview.h
  * @brief pagelistview widget
  * @details Pagelistview widget, to add top and bottom mask for pagelist widget.
*            Please refer to demo "example\screen_454_454_bb2plus_watch\gui_record\pagelist_test.c"
  * @author haonan_shi@realsil.com
  * @date 2023/12/15
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_PAGELISTVIEW_H__
#define __GUI_PAGELISTVIEW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <gui_img.h>

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
/** @brief  Pagelistview widget structure */
typedef struct gui_pagelistview
{
    gui_obj_t base;
    gui_img_t *top_mask;
    gui_img_t *bottom_mask;
    int16_t top_mask_border;
    int16_t bottom_mask_border;
} gui_pagelistview_t;

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
 * @param this pagelistview
 * @param mask_bottom top mask pickture, should be created by app
 * @return gui_img_t*
 */

void gui_pagelistview_add_top_mask(gui_pagelistview_t *this, gui_img_t *mask_top);

/**
 * @brief
 *
 * @param this pagelistview
 * @param mask_bottom bottom mask pickture, should be created by app
 * @return
 */

void gui_pagelistview_add_bottom_mask(gui_pagelistview_t *this, gui_img_t *mask_bottom);
/**
 * @brief
 *
 * @param parent parent of this widget
 * @param name name of this widget
 * @param x x of this widget
 * @param y y of this widget
 * @param w w of this widget
 * @param h h of this widget
 * @return gui_pagelistview_t*
 */
gui_pagelistview_t *gui_pagelistview_create(void *parent,  const char *name,
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

