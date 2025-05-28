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



/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_UNUSED(x)           (void)(x)             /* macro to get rid of 'unused parameter' warning */
#define GUI_TYPE(type, obj)     ((type *)obj)
#define GET_BASE(_p)            ((gui_obj_t *)_p)
#define GUI_BASE(_p)            ((gui_obj_t *)_p)




#define GUI_SPEED_RECODE_LENGTH 5
#define GUI_SPEED_RECODE_DEFINE int speed_recode[GUI_SPEED_RECODE_LENGTH];
#define GUI_SPEED_MAX 60
#define GUI_SPEED_MIN 7
#define GUI_FRAME_STEP 50
#define GUI_MAGIC_NUMBER 0x0b




#define IMPORT_GUI_TOUCHPAD  extern touch_info_t *tp_get_info(void); \
    touch_info_t *tp = tp_get_info();

#define IMPORT_GUI_WHEEL extern touch_info_t *wheel_get_info(void); \
    touch_info_t *wheel = wheel_get_info();

#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT(obj != NULL);GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}

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

#define GUI_RETURN_HELPER(tp, screen_w, return_cb) \
    if (tp->released && abs(tp->deltaX) > 50 && \
        (tp->x < 20 || tp->x > screen_w - 20))\
    {\
        return_cb();\
    }\










#ifdef __cplusplus
}
#endif

#endif

