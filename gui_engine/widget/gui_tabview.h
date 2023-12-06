/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tabview.h
  * @brief Extended in the cross direction
  * @details switch the currently displayed screen by sliding
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/24
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
#ifndef __GUI_TABVIEW_H__
#define __GUI_TABVIEW_H__
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


typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gui_tabview_tab_id_t;

/** @brief  tabview structure */
typedef struct gui_tabview
{
    gui_obj_t base;
    uint16_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_tabview_tab_id_t cur_id;
    gui_jump_t jump;
    SLIDE_STYLE style;
    int16_t release_x;
    int16_t release_y;
} gui_tabview_t;


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
 * @brief create a tabview widget, which can nest tabs.
 * @param parent the father widget it nested in.
 * @param filename this tabview widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return return the widget object pointer.
 *
 */
gui_tabview_t *gui_tabview_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h);
/**
 * @brief jump to a specify tab
 *
 * @param parent_tabview tabview pointer
 * @param idx Horizontal index value
 * @param idy Vertical index value
 */
void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy);
/**
 * @brief config slide effect
 *
 * @param this tabview pointer
 * @param style refer to SLIDE_STYLE
 */
void gui_tabview_set_style(gui_tabview_t *this, SLIDE_STYLE style);



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




