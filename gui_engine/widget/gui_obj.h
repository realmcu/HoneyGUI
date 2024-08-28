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
#include <gui_api.h>

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
typedef enum t_obj_type
{
    SCREEN                   = 0x0000,
    WINDOW                   = 0x0001,
    TABVIEW                  = 0x0002,
    TAB                      = 0x0003,
    CURTAINVIEW              = 0x0004,
    CURTAIN                  = 0x0005,
    IMAGE                    = 0x0006,
    BUTTON                   = 0x0007,
    ICONLIST                 = 0x0008,
    ICON                     = 0x0009,
    IMAGE_FROM_MEM           = 0x000A,
    TEXTBOX                  = 0x000B,
    SCROLLTEXTBOX            = 0x000C,
    SEEKBAR                  = 0x000D,
    PROGRESSBAR              = 0x000E,
    CLICKSWITCH              = 0x000F,
    PAGE                     = 0x0010,
    SCROLL_WHEEL             = 0x0011,
    PAGEBAR                  = 0x0012,
    RETURNWIDGET             = 0x0013,
    RECTANGLE                = 0x0014,
    WINDOWWITHBORDER         = 0x0015,
    CANVAS                   = 0x0016,
    VG_LITE_CLOCK            = 0x0017,
    VG_LITE_CUBE             = 0x0018,
    GRID                     = 0x0019,
    RADIO                    = 0x001A,
    RADIOSWITCH              = 0x001B,
    ARC                      = 0x001C,
    JAVASCRIPT               = 0x001D,
    MOVIE                    = 0x001E,
    IMAGE_SCOPE              = 0x001F,
    CARDVIEW                 = 0x0020,
    CARD                     = 0x0021,
    PAGELIST                 = 0x0022,
    PAGELISTVIEW             = 0x0023,
    MACRO_ANIMATETRANSFORM   = 0x0024,
    HONEYCOMB_LIST           = 0x0025,
    WHEEL_LIST               = 0x0026,
    QRCODE                   = 0x0027,
    GALLERY                  = 0x0028,
    TURN_TABLE               = 0x0029,
    KEYBOARD                 = 0x002A,
    MACRO_MOTORIZED_CURTAIN  = 0x002B,
    MULTI_LEVEL              = 0x002C,
    U8G2                     = 0x002D,
    MACRO_ONCLICK            = 0x002E,
    MACRO_BACKICON           = 0x002F,
    CANVAS_IMG               = 0x0030,
    VG_LITE_SOCCER           = 0x0031,
    SLIDER                   = 0x0032,
    MACRO_ONCHANGE           = 0x0033,
} T_OBJ_TYPE;
typedef enum
{
    OBJ_INPUT_PREPARE,
    OBJ_PREPARE,
    OBJ_DRAW,
    OBJ_END,
    OBJ_DESTORY,
} T_OBJ_CB_TYPE;

typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
#ifdef ENABLE_GUI_CONTAIN_FUNCTION //means range of this object
    int16_t range_x;
    int16_t range_y;
    int16_t range_w;
    int16_t range_h;
#endif
    gui_list_t  child_list;
    gui_list_t  brother_list;

    //start of run time
    void (* obj_cb)(struct _gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
    //end of run time
    T_OBJ_TYPE type; //no need this , only use name
    uint32_t active               : 1;    // this_widget flag means obj location in screen
    uint32_t not_show             : 1;

    uint32_t skip_tp_left_hold    : 1;
    uint32_t skip_tp_right_hold   : 1;
    uint32_t skip_tp_up_hold      : 1;
    uint32_t skip_tp_down_hold    : 1;
    uint32_t skip_tp_short        : 1;
    uint32_t create_done          : 1;
    uint32_t flag_3d              : 1;
    uint32_t gesture              : 1;
    uint32_t event_dsc_cnt        : 5;
    uint32_t opacity_value        : 8;
    uint32_t has_input_prepare_cb : 1;
    uint32_t has_prepare_cb       : 1;
    uint32_t has_draw_cb          : 1;
    uint32_t has_end_cb           : 1;
    uint32_t has_destroy_cb       : 1;

    gui_event_dsc_t *event_dsc;

    gui_matrix_t *matrix;

} gui_obj_t;
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
  * @param  this_widget
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
  *    gui_obj_t *base = (gui_obj_t *)this_widget;
  *    gui_obj_ctor(base, parent, name, x, y, w, h);
  *
  * }
  * \endcode
  */
void gui_obj_ctor(gui_obj_t  *this_widget,
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
  * @brief    this_widget API only for Widget, not for Application
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
  * @brief   show the root of this_widget tree
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
 * @brief judge the obj if in range of this_widget rect
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
 * @brief skip all down slide hold actions of the child object
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
 * @brief skip all short click actions of the child object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_all_child_short(gui_obj_t *obj);

/**
 * @brief skip all short click actions of the other object
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_short(gui_obj_t *obj);

/**
 * @brief get the area of this_widget obj
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
 * @brief judge the point if in range of this_widget obj rect
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
 * @brief  update animate on every frame
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
/**
 * @brief  print name by bfs order
 *
 * @param object widget pointer
 * @return gui_obj_t * root
 */
gui_obj_t *gui_get_root(gui_obj_t *object);
/**
 * @brief Calculate the absolute coordinates of a GUI object.
 *
 * This function calculates the absolute (global) X and Y coordinates
 * of a given GUI object based on its local position within the parent hierarchy.
 *
 * @param[in]  obj          Pointer to the GUI object for which to calculate absolute coordinates.
 * @param[out] absolute_x   Pointer to an integer where the absolute X coordinate will be stored.
 * @param[out] absolute_y   Pointer to an integer where the absolute Y coordinate will be stored.
 *
 * @note This function assumes that @p obj is a valid pointer and
 *       that @p absolute_x and @p absolute_y are valid pointers to integers.
 */
void gui_obj_absolute_xy(gui_obj_t *obj, int *absolute_x, int *absolute_y);
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
