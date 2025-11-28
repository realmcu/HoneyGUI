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
    int8_t x;
    int8_t y;
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
#define GUI_TABVIEW_HISTORY_MAX 10
/* T_SLIDE_STYLE enum end*/
/** @brief  Tabview structure. */
typedef struct gui_tabview
{
    gui_obj_t base;
    uint8_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_tabview_tab_id_t cur_id;
    gui_tabview_tab_id_t last_id;
    gui_jump_t jump;
    T_SLIDE_STYLE style;
    int16_t release_x;
    int16_t release_y;
    uint16_t loop_x : 1;
    uint16_t loop_y : 1;
    uint16_t enable_pre_load : 1;
    uint16_t tab_change_ready : 1;
    uint16_t tab_need_pre_load : 1;
    uint16_t tp_disable : 1;
    uint16_t initial : 1;
    uint16_t back_flag : 1;
    uint16_t force_right : 1;
    uint16_t force_left : 1;
    uint8_t *left_shot;
    uint8_t *center_shot;
    uint8_t *right_shot;
    uint8_t checksum;
    uint8_t back_count;
    gui_tabview_tab_id_t history_id_array[GUI_TABVIEW_HISTORY_MAX];
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
 * @brief Creates a tabview widget, which can nest tabs.
 *
 * @param parent The parent widget it is nested in.
 * @param filename The tabview widget's name.
 * @param x The X-axis coordinate relative to the parent widget.
 * @param y The Y-axis coordinate relative to the parent widget.
 * @param w Width.
 * @param h Height.
 * @return The widget object pointer.
 */
gui_tabview_t *gui_tabview_create(void       *parent,
                                  const char *name,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief Jumps to a specified tab.
 *
 * @warning This function must be called from the UI thread.
 *
 * @param parent_tabview Tabview pointer.
 * @param idx Horizontal index value.
 * @param idy Vertical index value.
 */
void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy);

/**
 * @brief Configures the slide effect.
 *
 * @param _this Tabview pointer.
 * @param style Refer to T_SLIDE_STYLE.
 */
void gui_tabview_set_style(gui_tabview_t *_this, T_SLIDE_STYLE style);

/**
 * @brief Configures the tabview loop_x sliding feature.
 *
 * @param tabview Tabview pointer.
 * @param loop Loops when set to true, does not loop when set to false.
 */
void gui_tabview_loop_x(gui_tabview_t *tabview, bool loop);

/**
 * @brief Configures the tabview loop_y sliding feature.
 *
 * @param tabview Tabview pointer.
 * @param loop Loops when set to true, does not loop when set to false.
 */
void gui_tabview_loop_y(gui_tabview_t *tabview, bool loop);

/**
 * @brief Disables touchpad action for the tabview.
 *
 * @param tabview Tabview pointer.
 * @param disable_tp True to disable touchpad action, false to enable it.
 */
void gui_tabview_tp_disable(gui_tabview_t *tabview, bool disable_tp);

/**
 * @brief Enables or disables tab widget cache.
 *
 * @param _this Tabview pointer.
 * @param enable True to enable tab widget cache, false to disable it.
 */
void gui_tabview_enable_pre_load(gui_tabview_t *_this, bool enable);

/**
 * @brief Registers a callback for tab change.
 *
 * @param tabview Tabview pointer.
 * @param callback Callback function.
 * @param parameter Callback parameter.
 */
void gui_tabview_tab_change(gui_tabview_t *tabview, void *callback, void *parameter);
/**
 * @brief Jump to the history tab.
 *
 * @param parent_tabview Tabview pointer.
 * @param end_idx Horizontal end index value.
 * @param end_idy Vertical end index value.
 *
 *
 * This function is used to implement the back key function in the tabview.
 */
void gui_tabview_back(gui_tabview_t *parent_tabview, int8_t end_idx, int8_t end_idy);
#ifdef __cplusplus
}
#endif

#endif
