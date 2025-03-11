/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.h
  * @brief  object widget
  * @details create a object
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
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
#ifndef __GUI_OBJ_SUPPRESS_H__
#define __GUI_OBJ_SUPPRESS_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
*                         Macros
*============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief skip all left slide hold actions of the parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_left_hold(gui_obj_t *obj);

/**
 * @brief skip all left slide hold actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_left_hold(gui_obj_t *obj);

/**
 * @brief skip all left slide hold actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_left_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_right_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_right_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_right_hold(gui_obj_t *obj);

/**
 * @brief skip all down slide hold actions of the parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_down_hold(gui_obj_t *obj);

/**
 * @brief skip all down slide hold actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_down_hold(gui_obj_t *obj);

/**
 * @brief skip all down slide hold actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_down_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_up_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_up_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_up_hold(gui_obj_t *obj);

/**
 * @brief skip all short click actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_short(gui_obj_t *obj);

/**
 * @brief skip all short click actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_short(gui_obj_t *obj);

/**
 * @brief skip click actions of other objects of parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_short(gui_obj_t *obj);

/**
 * @brief skip all long click actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_long(gui_obj_t *obj);

/**
 * @brief skip all long click actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_long(gui_obj_t *obj);

/**
 * @brief skip long press actions of other objects of parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_long(gui_obj_t *obj);

/**
 * @brief skip all pressed actions of the child object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_pressed(gui_obj_t *obj);

/**
 * @brief skip all pressed actions of the other object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_pressed(gui_obj_t *obj);

/**
 * @brief skip pressed actions of the other parent, only keep top layer widget serial parent.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_pressed(gui_obj_t *obj);


#ifdef __cplusplus
}
#endif

#endif
