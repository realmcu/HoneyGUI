/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_win.c
  * @brief Window widget
  * @details A rectangular container widget which can listen to many gestures
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_win.h>
#include <string.h>
#include <tp_algo.h>
#include "gui_obj.h"


/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** @brief  ... */
typedef enum
{
    EXAMPLE_SUCCESS,                //!< ...
    EXAMPLE_INVALID_STATE,          //!< ...
} T_EXAMPLE;

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */

#define EXAMPLE_A              a     //!< ...
#define EXAMPLE_B              b     //!< ...


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */
static void (onLeft)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}
static void (onRight)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}
static void (onUp)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_3, parameter);
}
static void (onDown)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_4, parameter);
}
static void (onPress)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_PRESSED, parameter);
}
static void (onRelease)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, parameter);
}
static void (onLong)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, parameter);
}
static void (onClick)(gui_win_t *b, void *callback, void *parameter)
{
    gui_obj_add_event_cb(b, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}
void win_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if ((obj->ax < (int)gui_get_screen_width()) && ((obj->ax + obj->w) >= 0) && \
        (obj->ay < (int)gui_get_screen_height()) && ((obj->ay + obj->h) >= 0))
    {
        if (((tp->x >= obj->ax && tp->x <= (obj->ax + obj->w)) &&
             (tp->y >= obj->ay && tp->y <= (obj->ay + obj->h))))
        {
            gui_win_t *b = (void *)obj;
            switch (tp->type)
            {
            case TOUCH_SHORT:
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                }
                break;
            case TOUCH_UP_SLIDE:
                {
                    gui_obj_event_set(obj, GUI_EVENT_3);
                }
                break;
            case TOUCH_DOWN_SLIDE:
                {
                    gui_obj_event_set(obj, GUI_EVENT_4);
                }
                break;
            case TOUCH_LEFT_SLIDE:
                {
                    gui_obj_event_set(obj, GUI_EVENT_1);
                }
                break;
            case TOUCH_RIGHT_SLIDE:
                {
                    gui_obj_event_set(obj, GUI_EVENT_2);
                }
                break;
            case TOUCH_LONG:
                {
                    b->long_flag = true;
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                }
                break;

            default:
                break;
            }
            if (tp->pressed)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);
                b->long_flag = false;
                b->press_flag = true;
            }
            if (b->release_flag)
            {
                b->press_flag = false;
                b->release_flag = false;
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
                b->long_flag = false;
            }
            if (tp->released && b->press_flag)
            {
                b->release_flag = true;
            }
        }
    }
}
gui_api_win_t gui_win_api =
{
    .onClick = onClick,
    .onLong = onLong,
    .onPress = onPress,
    .onRelease = onRelease,
    .onLeft = onLeft,
    .onRight = onRight,
    .onUp = onUp,
    .onDown = onDown,
    .set_animate = gui_win_set_animate,
};
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */


static void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                         int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    GET_BASE(this)->obj_prepare = win_prepare;
    GET_BASE(this)->type = WINDOW;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_win_set_animate(gui_win_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_win_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_win_t *)o)->animate = animate;
}

gui_win_t *gui_win_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h)
{
#define _GUI_NEW_gui_win_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_win_t, gui_win_ctor, _GUI_NEW_gui_win_create_param)
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */






