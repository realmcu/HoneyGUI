/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file guidef.h
  * @brief base define for RealGUI
  * @details base define for RealGUI
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUIDEF_H__
#define __GUIDEF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "def_compiler.h"
#include "def_type.h"
#include "def_list.h"
#include "def_event.h"
#include "def_color.h"
#include "def_vg.h"
#include "def_input.h"
#include "def_os.h"
#include "def_file.h"
#include "def_fs.h"
#include "def_fb.h"
#include "def_engine.h"
#include "def_msg.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/**
 * @brief Macro that defines the parameters for an animation callback function.
 *
 * This macro specifies the parameters that an animation callback function must take.
 * These parameters typically include a generic pointer `p`, a pointer to the widget
 * being animated `this_widget`, and a pointer to an `animate` structure.
 *
 * @param p            A generic pointer parameter that can be used to pass custom data.
 * @param this_widget  A pointer to the widget that is being animated.
 * @param animate      A pointer to the structure containing animation details.
 */
#define GUI_ANIMATION_CALLBACK_PARAMETER void *p, void *this_widget, struct gui_animate *animate

/**
 * @brief Macro to define an animation callback function.
 *
 * This macro is used to define the signature of an animation callback function.
 * The `function_name` parameter specifies the name of the function. The function
 * will take the parameters defined by `GUI_ANIMATION_CALLBACK_PARAMETER`.
 *
 * @param function_name  The name of the animation callback function to define.
 */
#define GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(function_name) void function_name(GUI_ANIMATION_CALLBACK_PARAMETER)


/*============================================================================*
 *                         Macros
 *============================================================================*/




#define GUI_UNUSED(x) (void)(x)             /* macro to get rid of 'unused parameter' warning */
#define GUI_TYPE(type, obj) ((type *)obj)


#define GUI_NEW(type, constructor_cb, param) type *this = gui_malloc(sizeof(type));\
    memset(this, 0, sizeof(type));\
    constructor_cb(param);\
    /*todo enter critical section*/\
    gui_list_init(&(GET_BASE(this)->child_list));\
    if ((GET_BASE(this)->parent) != NULL)\
    {\
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));\
    }\
    GET_BASE(this)->create_done = true;\
    /*todo exit critical section*/\
    return this;
#define GUI_CREATE_HELPER(type, constructor_cb, param) type *this = gui_malloc(sizeof(type));\
    memset(this, 0, sizeof(type));\
    constructor_cb(param);\
    /*todo enter critical section*/\
    gui_list_init(&(GET_BASE(this)->child_list));\
    if ((GET_BASE(this)->parent) != NULL)\
    {\
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));\
    }\
    GET_BASE(this)->create_done = true;
#define GET_BASE(_p) ((gui_obj_t *)_p)
#define GUI_BASE(_p) ((gui_obj_t *)_p)
#define _GUI_API_DEFINE(type) typedef struct __gui_api_##type{
#define _GUI_API_DECLARE(type) } _gui_api_##type;extern _gui_api_##type _gui_api_for_##type;
#define _GUI_API_ASSIGN(type) _gui_api_##type _gui_api_for_##type ={
#define GUI_API(type)  _gui_api_for_##type

#if defined _WIN32
#define GUI_FRAME_STEP 50
#else
#define GUI_FRAME_STEP 50
#endif

#define GUI_TOUCHPAD_IMPORT_AS_TP touch_info_t *tp = tp_get_info();
#define IMPORT_GUI_TOUCHPAD  extern touch_info_t *tp_get_info(void); touch_info_t *touch = tp_get_info();
#define IMPORT_GUI_WHEEL extern touch_info_t *wheel_get_info(void);\
    touch_info_t *wheel = wheel_get_info();
#define GUI_ERROR (gui_error_t)1
#define GUI_ERROR_NULL (gui_error_t)2
#define GUI_ERROR_FORMAT (gui_error_t)3
#define GUI_SUCCESS (gui_error_t)0
typedef int gui_error_t;
#define GUI_MAGIC_NUMBER 0x0b
#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT(obj != NULL);GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}
/**
 * @brief Macro to retrieve a GUI widget pointer by its name.
 *
 * This macro initializes a `gui_obj_t` pointer to `NULL`, retrieves the specified named widget
 * from the GUI object tree starting from the current application root, assigns the retrieved
 * widget to the pointer, and performs an exception check on the pointer.
 *
 * @param[in] pointer The name of the pointer variable that will hold the retrieved widget.
 * @param[in] name The name of the widget to retrieve.
 *
 * @note This macro assumes the existence of the function `gui_obj_tree_get_widget_by_name`,
 *       the function `gui_current_app()`, and the macro `GUI_WIDGET_TRY_EXCEPT`.
 *       The `gui_obj_tree_get_widget_by_name` function is declared as `extern` within the macro.
 */
#define GUI_WIDGET_POINTER_BY_NAME(pointer, name) gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_name(gui_obj_t *, const char *, gui_obj_t **);\
    extern gui_app_t *gui_current_app(void);\
    gui_obj_tree_get_widget_by_name((void *)gui_current_app(), name, &pointer);\
    GUI_WIDGET_TRY_EXCEPT(pointer)
/**
* @brief Macro to get a widget pointer by name from the root of a GUI object tree.
*
* This macro assigns a widget pointer by searching for a widget with the specified name within the given root.
* If the widget is found, the pointer is set to it; otherwise, it remains NULL. Additionally, the macro handles
* exceptions if the widget retrieval fails.
*
* @param pointer The name of the pointer that will hold the widget.
* @param name The name of the widget to search for.
* @param root_widget The root widget of the GUI object tree to search within.
*/
#define GUI_WIDGET_POINTER_BY_NAME_ROOT(pointer, name, root_widget) gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_name(gui_obj_t *, const char *, gui_obj_t **);\
    gui_obj_tree_get_widget_by_name((void *)root_widget, name, &pointer);\
    GUI_WIDGET_TRY_EXCEPT(pointer)
/**
 * @brief Macro to retrieve a GUI widget pointer by its type.
 *
 * This macro initializes a `gui_obj_t` pointer to `NULL`, retrieves the specified type of widget
 * from the GUI object tree starting from a given root, assigns the retrieved widget to the
 * pointer, and performs an exception check on the pointer.
 *
 * @param[in] pointer The name of the pointer variable that will hold the retrieved widget.
 * @param[in] type The type of widget to retrieve.
 * @param[in] root The root of the GUI object tree where the search begins.
 *
 */
#define GUI_WIDGET_POINTER_BY_TYPE(pointer, type, root) \
    GUI_WIDGET_TRY_EXCEPT(root)\
    gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **);\
    gui_obj_tree_get_widget_by_type(root, type, &pointer);\
    GUI_WIDGET_TRY_EXCEPT(pointer)
/**
 * @brief Macro to calculate the size of an array.
 *
 * This macro calculates the number of elements in an array by dividing the total size of the
 * array by the size of its first element.
 *
 * @param[in] array The array whose size is to be determined.
 *
 * @return The number of elements in the array.
 */
#define GUI_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
/**
 * @brief Macro to check the type of a GUI widget and log an error if it doesn't match the expected type.
 *
 * This macro first performs an exception check on the given object. Then, it compares the type of the
 * GUI widget with the expected type. If the types do not match, it logs an error message. Finally,
 * it asserts that the widget type matches the expected type.
 *
 * @param[in] obj The GUI object whose type is to be checked.
 * @param[in] widget_type The expected type of the GUI widget.
 *
 */
#define GUI_WIDGET_TYPE_TRY_EXCEPT(obj,widget_type) {GUI_WIDGET_TRY_EXCEPT(obj)if ((GUI_BASE(obj)->type != widget_type))\
        {\
            extern void gui_log(const char *format, ...);\
            gui_log("GUI_WIDGET_TYPE_TRY_EXCEPT: type is %d ;",GUI_BASE(obj)->type);\
        }\
        {GUI_ASSERT((GUI_BASE(obj)->type == widget_type))}}
#define GUI_SPEED_RECODE_LENGTH 5
#define GUI_SPEED_RECODE_DEFINE int speed_recode[GUI_SPEED_RECODE_LENGTH];
#define GUI_SPEED_MAX 60
#define GUI_SPEED_MIN 7
#define GUI_SET_ANIMATE_HELPER \
    gui_animate_t *animate = this->animate;\
    \
    if (!(animate))\
    {\
        animate = gui_malloc(sizeof(gui_animate_t));\
    }\
    \
    memset((animate), 0, sizeof(gui_animate_t));\
    animate->animate = true;\
    if (dur == 0)\
    {\
        dur = 1000;\
    }\
    animate->dur = dur;\
    animate->callback = (gui_animate_callback_t)callback;\
    animate->repeat_count = repeat_count;\
    animate->p = p;\
    this->animate = animate;
#define GUI_STRINGS_EQUAL(string1, string2) (!strcmp(string1, string2))
#define GUI_ANIMATION_CALLBACK(function_name) void function_name(void *p, void *this_widget, gui_animate_t *animate)




#ifdef __cplusplus
}
#endif

#endif

