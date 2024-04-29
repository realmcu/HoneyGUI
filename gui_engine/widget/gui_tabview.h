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
#include "guidef.h"
#include "gui_fb.h"

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
    T_SLIDE_STYLE style;
    int16_t release_x;
    int16_t release_y;
    bool loop;
    bool loop_status;
    bool loop_status2;
    bool enable_pre_load;
    bool tab_change_ready;
    bool tab_need_pre_load;
    bool tp_disable;
    bool initial;
    uint8_t *left_shot;
    uint8_t *center_shot;
    uint8_t *right_shot;
    uint8_t checksum;
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
typedef enum
{
    TABVIEW_EVENT_TAB_CHANGE = GUI_EVENT_3,
} T_GUI_TABVIEW_EVENT;

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
gui_tabview_t *gui_tabview_create(void       *parent,
                                  const char *filename,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

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
 * @param style refer to T_SLIDE_STYLE
 */
void gui_tabview_set_style(gui_tabview_t *this, T_SLIDE_STYLE style);

/**
 * @brief Config tabview loop sliding feature. The default setting is no looping.
 *
 * @param tabview tabview pointer.
 * @param loop_or_not Loops when set to true, does not loop when set to false.
 */
void gui_tabview_loop(gui_tabview_t *tabview, bool loop_or_not);

/**
 * @brief disable tp action for this tabview.
 *
 * @param tabview tabview pointer.
 * @param disable_tp true: disable tp action,  false: enable tp action.
 */
void gui_tabview_tp_disable(gui_tabview_t *tabview, bool disable_tp);

/**
 * @brief enable tab widget cache
 *
 * @param this
 * @param cache
 */
void gui_tabview_enable_pre_load(gui_tabview_t *this, bool enable);

/**
 * @brief Register callback for tab change.
 *
 * @param tabview tabview pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_tabview_tab_change(gui_tabview_t *tabview, void *callback, void *parameter);

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
