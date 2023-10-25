/**
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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
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
void gui_obj_ctor(struct _gui_obj_t *this, struct _gui_obj_t *parent, const char *filename,
                  int16_t x, int16_t y, int16_t w, int16_t h);


/**
  * @param  this
  * @return void
  */
void gui_obj_dtor(struct _gui_obj_t *this);

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
void gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data);

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
  *    gui_tree_free(&app->screen);
  *
  * }
  *
  * \endcode
  */
void gui_tree_free(void *obj);

/**
  * @brief    print the widget tree recursively,from the root to the leaves.Preorder traversal.
  * @param  obj the root of the widget tree.
  * @return void
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_app_t *app)
  * {
  *    gui_tree_print(&app->screen);
  *
  * }
  *
  * \endcode
  */
void gui_tree_print(gui_obj_t *obj);

/**
  * @brief   hide widget
  * @param  obj the root of the widget tree.
  * @return void
  */
void gui_tree_not_show(gui_obj_t *obj);

/**
  * @brief   show widget
  * @param  obj the root of the widget tree.
  * @return void
  */
void gui_tree_show(gui_obj_t *obj, bool enable);

/**
  * @brief  object show is not
  * @param  obj
  * @param  show_info
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
void gui_obj_show(void *obj, bool show_info);

/**
  * @brief   show the root of this tree
  * @param  obj
  * @return gui_obj_t*
  */
gui_obj_t *gui_tree_get_root(gui_obj_t *obj);

/**
  * @brief   get child type
  * @param  obj
  * @param  child_type
  * @return gui_obj_t*
  */
gui_obj_t *gui_get_child_handle(gui_obj_t *obj, obj_type_t child_type);

/**
  * @brief   get child type
  * @param  obj
  * @param  target
  * @param  rst
  * @return gui_obj_t*
  */
void gui_tree_get_cover(gui_obj_t *obj, obj_type_t target, bool *rst);

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

