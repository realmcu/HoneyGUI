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
/* VIEW_CHANGE_STYLE enum start*/
typedef enum
{
    VIEW_CLASSIC          = 0x0000,
    VIEW_REDUCTION        = 0x0001,
    VIEW_ROTATE           = 0x0002,
    VIEW_CUBE             = 0x0003,
    VIEW_ROTATE_BOOK      = 0x0004,
    VIEW_PAGE             = 0x0005,
    VIEW_ANIMATION_NULL   = 0x0006,
    VIEW_ANIMATION_1,              ///< Recommended for startup
    VIEW_ANIMATION_2,              ///< Recommended for startup
    VIEW_ANIMATION_3,              ///< Recommended for startup
    VIEW_ANIMATION_4,              ///< Recommended for startup
    VIEW_ANIMATION_5,              ///< Recommended for startup
    VIEW_ANIMATION_6,              ///< Recommended for shutdown
    VIEW_ANIMATION_7,              ///< Recommended for shutdown
    VIEW_ANIMATION_8,              ///< Recommended for shutdown
} VIEW_CHANGE_STYLE;
/* VIEW_CHANGE_STYLE enum end*/

typedef struct gui_view_dsc
{
    void **obj_pointer;
    VIEW_CHANGE_STYLE style_out;
    VIEW_CHANGE_STYLE style_in;
    void (* view_design)(void *parent);
} gui_view_dsc_t;

struct gui_view_dsc_t;
/** @brief  view structure */
typedef struct gui_view
{
    gui_obj_t base;
    gui_win_t *window;      //!< view transition window
    gui_animate_t *animate;
    gui_view_id_t cur_id;
    VIEW_CHANGE_STYLE style;
    float scale_x;
    float scale_y;
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
    uint32_t view_click              : 1;
    uint32_t view_dsc_cnt            : 5;

    uint8_t *shot;
    gui_obj_t *rte_obj; //run time envriment
    gui_obj_t *shot_obj; // shot
    gui_obj_t *shot_pave_obj; // shot

    gui_view_dsc_t **view_dsc;

    uint8_t checksum;
} gui_view_t;

typedef enum
{
    VIEW_EVENT_MOVE_LEFT = GUI_EVENT_4,
    VIEW_EVENT_MOVE_RIGHT = GUI_EVENT_5,
    VIEW_EVENT_MOVE_UP = GUI_EVENT_6,
    VIEW_EVENT_MOVE_DOWN = GUI_EVENT_7,
    VIEW_EVENT_CLICK = GUI_EVENT_TOUCH_CLICKED,
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
 * @brief create a view widget.
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
                            int16_t     h);

/**
 * @brief add a event for this view widget to change to another view, "style_out" and "style_in" could be diffrent when event is VIEW_EVENT_CLICK.
 * @param this current view.
 * @param obj_pointer the pointer's address of target view, if don't need to specify, could be NULL.
 * @param style_out current view change style.
 * @param style_in target view change style.
 * @param cb the target view's design function.
 * @param event the way to change to the target view.
 */
void gui_view_add_change_event(gui_view_t *this, void **obj_pointer, VIEW_CHANGE_STYLE style_out,
                               VIEW_CHANGE_STYLE style_in,
                               void (* cb)(void *parent), T_GUI_VIEW_EVENT event);

/**
 * @brief enable view widget caching.
 *
 * @param _this view pointer.
 * @param enable true: enable view widget caching, false: disable view widget caching.
 */
void gui_view_enable_pre_load(gui_view_t *this, bool enable);

/**
 * @brief disable tp action for _this view.
 *
 * @param view view pointer.
 * @param disable_tp true: disable tp action,  false: enable tp action.
 */
void gui_view_tp_disable(gui_view_t *view, bool disable_tp);

/**
 * @brief
 *
 * @param obj view widget pointer
 */
void gui_view_reduction(gui_obj_t *obj);

/**
 * @brief
 *
 * @param obj view widget pointer
 */
void gui_view_cube(gui_obj_t *obj);

/**
 * @brief
 *
 * @param obj view widget pointer
 */
void gui_view_rotate(gui_obj_t *obj);

/**
 * @brief
 *
 * @param obj view widget pointer
 */
void gui_view_rotate_book(gui_obj_t *obj);

#ifdef __cplusplus
}
#endif
#endif
