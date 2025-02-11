/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.h
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
#ifndef __GUI_VIEW_H__
#define __GUI_VIEW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_matrix.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct
{
    int32_t x;
    int32_t y;
} gui_view_id_t;
/* VIEW_SLIDE_STYLE enum start*/
typedef enum
{
    VIEW_CLASSIC          = 0x0000,
    VIEW_REDUCTION        = 0x0001,

    VIEW_ROTATE       = 0x0005,
    VIEW_CUBE         = 0x0006,
    VIEW_PAGE         = 0x0007,
    VIEW_ROTATE_BOOK  = 0x0008,
} VIEW_SLIDE_STYLE;
/* VIEW_SLIDE_STYLE enum end*/
/** @brief  view structure */
typedef struct gui_view
{
    gui_obj_t base;
    gui_view_id_t cur_id;
    VIEW_SLIDE_STYLE style;
    int16_t release_x;
    int16_t release_y;
    gui_vertex_t normal;
    uint32_t enable_pre_load         : 1;
    uint32_t view_change_ready       : 1;
    uint32_t view_need_pre_load      : 1;
    uint32_t tp_disable              : 1;
    uint32_t initial                 : 1;
    uint32_t view_left               : 1;
    uint32_t view_right              : 1;
    uint32_t view_up                 : 1;
    uint32_t view_down               : 1;

    uint8_t *shot;
    gui_obj_t *rte_obj; //run time envriment
    gui_obj_t *shot_obj; // shot
    gui_obj_t *shot_pave_obj; // shot
    uint8_t checksum;
} gui_view_t;

typedef enum
{
    VIEW_EVENT_TAB_CHANGE = GUI_EVENT_3,
    VIEW_EVENT_MOVE_LEFT = GUI_EVENT_4,
    VIEW_EVENT_MOVE_RIGHT = GUI_EVENT_5,
    VIEW_EVENT_MOVE_UP = GUI_EVENT_6,
    VIEW_EVENT_MOVE_DOWN = GUI_EVENT_7,
} T_GUI_VIEW_EVENT;

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
 * @brief create a view widget, which can nest views.
 * @param parent the father widget it nested in.
 * @param filename _this view widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return return the widget object pointer.
 *
 */
gui_view_t *gui_view_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h,
                            int16_t     idx,
                            int16_t     idy);

/**
 * @brief jump to a specify view
 *
 * @param parent_view view pointer
 * @param idx Horizontal index value
 * @param idy Vertical index value
 */
void gui_view_jump_view(gui_view_t *parent_view, int8_t idx, int8_t idy);

/**
 * @brief config slide effect
 *
 * @param _this view pointer
 * @param style refer to VIEW_SLIDE_STYLE
 */
void gui_view_set_style(gui_view_t *_this, VIEW_SLIDE_STYLE style);


/**
 * @brief disable tp action for _this view.
 *
 * @param view view pointer.
 * @param disable_tp true: disable tp action,  false: enable tp action.
 */
void gui_view_tp_disable(gui_view_t *view, bool disable_tp);

/**
 * @brief enable view widget cache
 *
 * @param _this view pointer.
 */
void gui_view_enable_pre_load(gui_view_t *_this);

void gui_view_add_change_event(gui_view_t *view, gui_view_t *target_view, void *callback,
                               T_GUI_VIEW_EVENT event);


void gui_view_reduction(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

void gui_view_cube(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

void gui_view_rotate(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

void gui_view_rotate_book(gui_obj_t *obj, int16_t tab_x_gap, int16_t tab_y_gap);

#ifdef __cplusplus
}
#endif

#endif
