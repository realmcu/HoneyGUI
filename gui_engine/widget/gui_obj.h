/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.h
  * @brief  object widget
  * @details create a object
  * @author luke_sun@realsil.com.cn
  * @date 2023/10/25
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
#ifndef __GUI_OBJ_H__
#define __GUI_OBJ_H__



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

/** @brief  ... */


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
  * @param  this
  * @param  parent switching events
  * @param  filename how to trigger events
  * @param  x left
  * @param  y top
  * @param  w width
  * @param  h height
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *parent, const char *name, int16_t x,
                               int16_t y, int16_t w, int16_t h)
  * {
  *    gui_obj_t *base = (gui_obj_t *)this;
  *    gui_obj_ctor(base, parent, name, x, y, w, h);
  *
  * }
  * \endcode
  */
void gui_obj_ctor(gui_obj_t  *this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h);

/**
 * @brief
 *
 * @param parent
 * @param filename
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_obj_t*
 */
gui_obj_t *gui_obj_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);
/**
  * @brief create event widget
  * @param  obj
  * @param  event_cb switching events
  * @param  filter how to trigger events
  * @param  user_data
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *parent)
  * {
  *    gui_win_t *clock = gui_win_create(parent, "clock", 0, 84, 320, 300);
  *    gui_obj_add_event_cb(clock, (gui_event_cb_t)show_clock, GUI_EVENT_TOUCH_CLICKED, NULL);
  *
  * }
  * \endcode
  */
void gui_obj_add_event_cb(void           *obj,
                          gui_event_cb_t  event_cb,
                          gui_event_t     filter,
                          void           *user_data);

/**
  * @brief    this API only for Widget, not for Application
  * @param  obj
  * @param  event_code
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_obj_t *obj)
  * {
  *    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
  *
  * }
  * \endcode
  */
void gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code);

/**
  * @brief    free the widget tree recursively,from the root to the leaves.Preorder traversal.
  * @param  obj the root of the widget tree.
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_app_t *app)
  * {
  *    gui_obj_tree_free(&app->screen);
  *
  * }
  *
  * \endcode
  */
void gui_obj_tree_free(void *obj);

/**
  * @brief    print the widget tree recursively,from the root to the leaves.Preorder traversal.
  * @param  obj the root of the widget tree.
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_app_t *app)
  * {
  *    gui_obj_tree_print(&app->screen);
  *
  * }
  *
  * \endcode
  */
void gui_obj_tree_print(gui_obj_t *obj);
/**
  * @brief  get count of one type on tree
  * @param  obj the root of the widget tree.
  * @param  type widget type.
  * @param  count count result.
  * @return void
  */
void gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count);
/**
  * @brief   show or hide the widget
  * @param  obj the root of the widget tree.
  * @param  enable true for show, false for hide.
  * @return void
  */
void gui_obj_tree_show(gui_obj_t *obj, bool enable);

/**
  * @brief  object show or not
  * @param  obj
  * @param  enable
  * @return void
  * * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_app_t *app)
  * {
  *     gui_img_t *hour;
  *     gui_obj_show(hour,false);
  *     gui_obj_show(hour,true);
  *
  * }
  *
  * \endcode
  */
void gui_obj_show(void *obj, bool enable);

/**
  * @brief   show the root of this tree
  * @param  obj the root of the widget tree.
  * @return gui_obj_t*
  */
gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj);

/**
  * @brief   get child type
  * @param  obj the root of the widget tree.
  * @param  child_type
  * @return gui_obj_t*
  */
gui_obj_t *gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type);

/**
 * @brief judge the obj if in range of this rect
 *
 * @param obj
 * @param x
 * @param y
 * @param w
 * @param h
 * @return true
 * @return false
 */
bool gui_obj_in_rect(gui_obj_t *obj,
                     int16_t    x,
                     int16_t    y,
                     int16_t    w,
                     int16_t    h);

/**
 * @brief skip all left slide hold actions of the parent object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_left_hold(gui_obj_t *obj);

/**
 * @brief skip all left slide hold actions of the child object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_left_hold(gui_obj_t *obj);

/**
 * @brief skip all left slide hold actions of the other object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_left_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the parent object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_right_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the child object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_right_hold(gui_obj_t *obj);

/**
 * @brief skip all right slide hold actions of the other object
 *
 * @param obj
 */
void gui_obj_skip_other_right_hold(gui_obj_t *obj);

/**
 * @brief skip all down slide hold actions of the parent object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_down_hold(gui_obj_t *obj);

/**
 * @briefskip all down slide hold actions of the child object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_down_hold(gui_obj_t *obj);

/**
 * @brief skip all down slide hold actions of the other object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_down_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the parent object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_parent_up_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the child object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_up_hold(gui_obj_t *obj);

/**
 * @brief skip all up slide hold actions of the other object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_up_hold(gui_obj_t *obj);

/**
 * @brief get the area of this obj
 *
 * @param obj
 * @param x
 * @param y
 * @param w
 * @param h
 */
void gui_obj_get_area(gui_obj_t *obj,
                      int16_t   *x,
                      int16_t   *y,
                      int16_t   *w,
                      int16_t   *h);

/**
 * @brief judge the point if in range of this obj rect
 *
 * @param obj
 * @param x
 * @param y
 * @return true
 * @return false
 */
bool gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y);

/**
 * @brief
 *
 * @param seed
 * @param data
 * @param len
 * @return uint8_t
 */
uint8_t gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len);

/**
 * @brief  get widget in tree by name
 *
 * @param obj tree
 * @param name widget name
 * @param output widget
 * @return uint8_t
 */
void gui_obj_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output);
/**
 * @brief  get widget in tree by type
 *
 * @param obj tree
 * @param type widget type
 * @param output widget
 * @return uint8_t
 */
void gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output);
/**
 * @brief  update animate onn every frame
 *
 * @param animate
 * @param obj widget
 */
void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj);
/**
 * @brief  set animate
 *
 * @param animate pointer
 * @param dur animation time cost in ms
 * @param repeat_count rounds to repeat
 * @param callback every frame callback
 * @param p callback's parameter
 */
gui_animate_t *gui_obj_set_animate(gui_animate_t *animate,
                                   uint32_t      dur,
                                   int           repeat_count,
                                   void         *callback,
                                   void         *p);
/**
 * @brief  print name by bfs order
 *
 * @param root
 */
void gui_obj_tree_print_bfs(gui_obj_t *root);
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
