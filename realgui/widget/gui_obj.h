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


/*============================================================================*
 *                         Types
 *============================================================================*/

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
    CUBE                     = 0x0018,
    GRID                     = 0x0019,
    RADIO                    = 0x001A,
    RADIOSWITCH              = 0x001B,
    MACRO_CANVAS_ARC         = 0x001C,
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
    TYPE_SCROLL_WHEEL_NEW    = 0x0034,
    MACRO_ONON               = 0x0035,
    MACRO_ONOFF              = 0x0036,
    MACRO_ONLOAD,
    MACRO_CALENDAR,
    MACRO_CIRCLE,
    MACRO_SECTOR,
    MACRO_ROUNDED_RECT,
    MACRO_ONTIME,
    MACRO_COMBO,
    MACRO_PAGE_LIST_NEW,
    MACRO_ON_PERIPHERAL,
    GUI_VENDOR_TYPE,
    MACRO_CHART,
    VIEW,
} T_OBJ_TYPE;
typedef enum
{
    OBJ_INPUT_PREPARE,
    OBJ_PREPARE,
    OBJ_DRAW,
    OBJ_END,
    OBJ_DESTROY,
} T_OBJ_CB_TYPE;

typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    gui_list_t  child_list;
    gui_list_t  brother_list;

    //start of run time
    void (* obj_cb)(struct _gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
    //end of run time
    T_OBJ_TYPE type;                      //no need this , only use name
    uint32_t active               : 1;    // this_widget flag means obj location in screen
    uint32_t not_show             : 1;

    uint32_t skip_tp_left_hold    : 1;
    uint32_t skip_tp_right_hold   : 1;
    uint32_t skip_tp_up_hold      : 1;
    uint32_t skip_tp_down_hold    : 1;
    uint32_t skip_tp_short        : 1;
    uint32_t skip_tp_long         : 1;
    uint32_t skip_tp_pressed      : 1;
    uint32_t gesture              : 1;

    uint32_t create_done          : 1;
    uint32_t event_dsc_cnt        : 5;
    uint32_t opacity_value        : 8;
    uint32_t has_input_prepare_cb : 1;
    uint32_t has_prepare_cb       : 1;
    uint32_t has_draw_cb          : 1;
    uint32_t has_end_cb           : 1;
    uint32_t has_destroy_cb       : 1;
    uint32_t magic                : 4;
    gui_event_dsc_t *event_dsc;

    gui_matrix_t *matrix;

} gui_obj_t;

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
  * @param this_widget pointer to the GUI image object.
  * @param parent the father widget it nested in.
  * @param filename the obj widget name.
  * @param x the X-axis coordinate of the widget.
  * @param y the Y-axis coordinate of the widget.
  * @param w the width of the widget.
  * @param h the hight of the widget.
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
 * @brief creat an obj widget.
 *
 * @param parent the father widget it nested in.
 * @param filename the obj widget name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return gui_obj_t*.
 */
gui_obj_t *gui_obj_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);
/**
  * @brief create event widget.
  * @param obj pointer to the GUI object.
  * @param event_cb switching events.
  * @param filter how to trigger events.
  * @param user_data data to be transmitted.
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
  * @brief this_widget API only for Widget, not for Application.
  * @param obj Pointer to the GUI object.
  * @param event_code switching events.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(gui_obj_t *obj)
  * {
  *    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
  *
  * }
  * \endcode
  */
void gui_obj_enable_event(gui_obj_t *obj, gui_event_t event_code);

/**
  * @brief free the widget tree recursively,from the root to the leaves.Preorder traversal.
  * @param obj the root of the widget tree.
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
  * @brief free the widget tree recursively, without the root. Preorder traversal.
  * @param obj the root of the widget tree.
  */
void gui_obj_child_free(gui_obj_t *object);
/**
  * @brief print the widget tree recursively,from the root to the leaves.Preorder traversal.
  * @param obj the root of the widget tree.
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
  * @brief get count of one type on tree.
  * @param obj the root of the widget tree.
  * @param type widget type.
  * @param count count result.
  */
void gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count);

/**
  * @brief show or hide the widget.
  * @param obj the root of the widget tree.
  * @param enable true for show, false for hide.
  */
void gui_obj_tree_show(gui_obj_t *obj, bool enable);

/**
  * @brief set object show or not.
  * @param obj the root of the widget tree.
  * @param enable true for show, false for hide.
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
  * @brief show the root of this_widget tree.
  * @param obj the root of the widget tree.
  * @return gui_obj_t*.
  */
gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj);

/**
  * @brief get child type.
  * @param obj the root of the widget tree.
  * @param child_type type of obj widget.
  * @return gui_obj_t*.
  */
gui_obj_t *gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type);

/**
 * @brief judge the obj if in range of this_widget rect.
 *
 * @param obj pointer to the GUI object.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return true.
 * @return false.
*/
bool gui_obj_in_rect(gui_obj_t *obj,
                     int16_t    x,
                     int16_t    y,
                     int16_t    w,
                     int16_t    h);

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
 * @brief enable all short click actions from parent object to the root object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_enable_this_parent_short(gui_obj_t *obj);

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
 * @brief enable all long press actions from parent object to the root object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_enable_this_parent_short(gui_obj_t *obj);

/**
 * @brief skip long press actions of other objects of parent object.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_short(gui_obj_t *obj);

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
 * @brief skip long press actions of the other parent, only keep top layer widget serial parent.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_long(gui_obj_t *obj);

/**
 * @brief skip pressed actions of the other parent, only keep top layer widget serial parent.
 *
 * @param obj the root of the widget tree.
 */
void gui_obj_skip_other_parent_pressed(gui_obj_t *obj);

/**
 * @brief get the area of this_widget obj.
 *
 * @param obj pointer to the GUI object.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 */
void gui_obj_get_area(gui_obj_t *obj,
                      int16_t   *x,
                      int16_t   *y,
                      int16_t   *w,
                      int16_t   *h);

/**
 * @brief judge the point if in range of this_widget obj rect.
 *
 * @param obj widget object pointer.
 * @param x the X-axis coordinate.
 * @param y the Y-axis coordinate.
 * @return true.
 * @return false.
 */
bool gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y);

/**
 * @brief judge the point if in range of this_widget obj circle.
 *
 * @param obj widget object pointer.
 * @param x the X-axis coordinate.
 * @param y the Y-axis coordinate.
 * @return true.
 * @return false.
 */
bool gui_obj_point_in_obj_circle(gui_obj_t *obj, int16_t x, int16_t y);

/**
 * @brief do crc check.
 *
 * @param seed the initial value to start the checksum calculation.
 * @param data pointer to the array of bytes for which the checksum is to be calculated.
 * @param len the number of bytes in the array.
 * @return uint8_t.
 */
uint8_t gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len);

/**
 * @brief get widget in tree by name.
 *
 * @param obj tree.
 * @param name widget name.
 * @param output widget.
 */
void gui_obj_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output);

/**
 * @brief get widget in tree by type.
 *
 * @param root tree.
 * @param type widget type.
 * @param output widget.
 * @return void.
 */
void gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output);

/**
 * @brief update animate on every frame.
 *
 * @param animate widget object pointer.
 * @param obj widget.
 */
void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj);

/**
 * @brief set animate.
 *
 * @param animate pointer.
 * @param dur animation time cost in ms.
 * @param repeat_count rounds to repeat.
 * @param callback every frame callback.
 * @param p callback's parameter.
 */
gui_animate_t *gui_obj_set_animate(gui_animate_t *animate,
                                   uint32_t      dur,
                                   int           repeat_count,
                                   void         *callback,
                                   void         *p);

/**
 * @brief print name by bfs order.
 *
 * @param root tree.
 */
void gui_obj_tree_print_bfs(gui_obj_t *root);

/**
 * @brief print name by bfs order.
 *
 * @param object widget pointer.
 * @return gui_obj_t * root.
 */
gui_obj_t *gui_get_root(gui_obj_t *object);

/**
 * @brief calculate the absolute coordinates of a GUI object.
 *
 * This function calculates the absolute (global) X and Y coordinates
 * of a given GUI object based on its local position within the parent hierarchy.
 *
 * @param obj pointer to the GUI object for which to calculate absolute coordinates.
 * @param absolute_x pointer to an integer where the absolute X coordinate will be stored.
 * @param absolute_y pointer to an integer where the absolute Y coordinate will be stored.
 *
 * @note This function assumes that @p obj is a valid pointer and
 *       that @p absolute_x and @p absolute_y are valid pointers to integers.
 */
void gui_obj_absolute_xy(gui_obj_t *obj, int *absolute_x, int *absolute_y);

/**
 * @brief set the visibility of a GUI object.
 *
 * This function sets the visibility of a given GUI object by adjusting its hidden state.
 *
 * @param obj pointer to the GUI object that will be updated.
 * @param hidden boolean flag indicating whether the object should be hidden (true) or shown (false).
 */
void gui_obj_hidden(gui_obj_t *obj, bool hidden);

/**
 * @brief set or retrieve the name of a GUI widget.
 *
 * This function sets the name of a given GUI widget if the provided name is valid.
 * It returns the current name of the widget.
 *
 * @param widget pointer to the GUI widget whose name will be set or retrieved.
 * @param name pointer to a string containing the new name for the widget. If the name is valid, it will be set as the widget's name.
 *
 * @return the current name of the widget.
 */
const char *gui_widget_name(gui_obj_t *widget, const char *name);

/**
 * @brief update touch pad speed vertical.
 *
 * This function updates the current speed and records the speed change history.
 *
 * @param speed pointer to the current speed, which will be updated by the function.
 * @param speed_recode array to record speed changes, which will be updated by the function.
 */
void gui_update_speed(int *speed, int speed_recode[]);

/**
 * @brief inertial calculation.
 *
 * This function performs inertial calculations based on the current speed, end speed, and offset.
 *
 * @param speed pointer to the current speed, which will be updated by the function.
 * @param end_speed target end speed.
 * @param offset pointer to the offset, which will be updated by the function.
 */
void gui_inertial(int *speed, int end_speed, int *offset);

/**
 * @brief tree print mermaid.
 *
 * @param obj Pointer to the GUI object root.
 */
void gui_obj_tree_print_mmd(gui_obj_t *obj);

/**
 * @brief get widget count.
 */
uint32_t gui_get_obj_count(void);
/**
 * @brief Set the location of a GUI object.
 *
 * This function sets the X and Y coordinates of the specified GUI object.
 *
 * @param obj Pointer to the GUI object to set location for.
 * @param x The X coordinate to set.
 * @param y The Y coordinate to set.
 */
void gui_set_location(gui_obj_t *obj, uint16_t x, uint16_t y);

/**
 * @brief Retrieve a widget from the GUI object tree by type and index.
 *
 * This function searches the GUI object tree starting from the root and retrieves
 * a widget of the specified type and index.
 *
 * @param root Pointer to the root of the GUI object tree.
 * @param type The type of the object to find.
 * @param output Pointer to where the found object should be stored.
 * @param index The index of the object to retrieve.
 */
void gui_obj_tree_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output,
                                               int index);

/**
 * @brief Update the speed based on displacement.
 *
 * This function updates the speed value based on the given displacement.
 * It also uses a speed record array to achieve this.
 *
 * @param speed Pointer to the speed variable to update.
 * @param speed_recode Array holding the speed records.
 * @param displacement The displacement value to consider for speed update.
 */
void gui_update_speed_by_displacement(int *speed, int speed_recode[], int displacement);

/**
 * @brief Move a widget object to specified coordinates.
 *
 * This function moves the specified widget object to a new (x, y) coordinate
 * position.
 *
 * @param obj Pointer to the widget object to be moved.
 * @param x   The new x-coordinate for the widget object.
 * @param y   The new y-coordinate for the widget object.
 */
void gui_obj_move(gui_obj_t *obj, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
