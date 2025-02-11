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
#include "gui_obj.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct
{
    int32_t x;
    int32_t y;
} gui_tabview_tab_id_t;
typedef struct gui_index
{
    int8_t x;
    int8_t y;
} gui_index_t;
typedef struct gui_jump
{
    bool jump_flag;
    gui_index_t jump_id;
} gui_jump_t;
/* T_SLIDE_STYLE enum start*/
typedef enum t_slide_style
{
    CLASSIC          = 0x0000,
    REDUCTION        = 0x0001,
    FADE             = 0x0002,
    REDUCTION_FADE   = 0x0003,
    STACKING         = 0x0004,

    TAB_ROTATE       = 0x0005,
    TAB_CUBE         = 0x0006,
    TAB_PAGE         = 0x0007,
    TAB_ROTATE_BOOK  = 0x0008,
} T_SLIDE_STYLE;
/* T_SLIDE_STYLE enum end*/
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
    uint8_t loop_x : 1;
    uint8_t loop_y : 1;
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

typedef enum
{
    TABVIEW_EVENT_TAB_CHANGE = GUI_EVENT_3,
} T_GUI_TABVIEW_EVENT;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief create a tabview widget, which can nest tabs.
 * @param parent the father widget it nested in.
 * @param filename _this tabview widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return return the widget object pointer.
 *
 */
gui_tabview_t *gui_tabview_create(void       *parent,
                                  const char *name,
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
 * @param _this tabview pointer
 * @param style refer to T_SLIDE_STYLE
 */
void gui_tabview_set_style(gui_tabview_t *_this, T_SLIDE_STYLE style);

/**
 * @brief Config tabview loop_x sliding feature. The default setting is no looping.
 *
 * @param tabview tabview pointer.
 * @param loop Loops when set to true, does not loop when set to false.
 */
void gui_tabview_loop_x(gui_tabview_t *tabview, bool loop);

/**
 * @brief Config tabview loop_y sliding feature. The default setting is no looping.
 *
 * @param tabview tabview pointer.
 * @param loop Loops when set to true, does not loop when set to false.
 */
void gui_tabview_loop_y(gui_tabview_t *tabview, bool loop);

/**
 * @brief disable tp action for _this tabview.
 *
 * @param tabview tabview pointer.
 * @param disable_tp true: disable tp action,  false: enable tp action.
 */
void gui_tabview_tp_disable(gui_tabview_t *tabview, bool disable_tp);

/**
 * @brief enable tab widget cache
 *
 * @param _this tabview pointer.
 * @param enable true: enable tab widget cache,  false: disable tab widget cache.
 */
void gui_tabview_enable_pre_load(gui_tabview_t *_this, bool enable);

/**
 * @brief Register callback for tab change.
 *
 * @param tabview tabview pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_tabview_tab_change(gui_tabview_t *tabview, void *callback, void *parameter);


#ifdef __cplusplus
}
#endif

#endif
