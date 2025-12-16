/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_tree.h
  * @brief  object tree operations
  * @details tree operations related functions and structures
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_TREE_H__
#define __GUI_OBJ_TREE_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj_type.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
  * @brief Free the widget tree recursively, from the root to the leaves. Preorder traversal.
  * @param obj Root of the widget tree.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void)
  * {
  *    gui_obj_tree_free(gui_obj_get_root());
  *
  * }
  *
  * \endcode
  */
void gui_obj_tree_free(void *obj);


/**
  * @brief Free the widget tree asynchronously.
  * @param obj Root of the widget tree.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void)
  * {
  *    gui_obj_tree_free_async(gui_obj_get_root());
  *
  * }
  *
  * \endcode
  */
void gui_obj_tree_free_async(void *obj);

/**
  * @brief Free the widget tree recursively, without the root. Preorder traversal.
  * @param object Root of the widget tree.
  */
void gui_obj_child_free(gui_obj_t *object);

/**
  * @brief Print the widget tree recursively, from the root to the leaves. Preorder traversal.
  * @param obj Root of the widget tree.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void)
  * {
  *    gui_obj_tree_print(gui_obj_get_root());
  *
  * }
  *
  * \endcode
  */
void gui_obj_tree_print(gui_obj_t *obj);

/**
  * @brief Get count of one type on tree.
  * @param obj Root of the widget tree.
  * @param type Widget type.
  * @param count Count result.
  */
void gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count);

/**
  * @brief Show or hide the widget.
  * @param obj Root of the widget tree.
  * @param enable True for show, false for hide.
  */
void gui_obj_tree_show(gui_obj_t *obj, bool enable);

/**
  * @brief Show the root of this_widget tree.
  * @param obj Root of the widget tree.
  * @return Pointer to the root object.
  */
gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj);

/**
  * @brief Get child type.
  * @param obj Root of the widget tree.
  * @param child_type Type of obj widget.
  * @return Pointer to the child object.
  */
gui_obj_t *gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type);

/**
 * @brief Get widget in tree by name.
 *
 * @param obj Tree.
 * @param name Widget name.
 * @param output Widget.
 */
void gui_obj_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output);

/**
 * @brief Print name by bfs order.
 *
 * @param root Tree.
 */
void gui_obj_tree_print_bfs(gui_obj_t *root);




#ifdef __cplusplus
}
#endif

#endif
