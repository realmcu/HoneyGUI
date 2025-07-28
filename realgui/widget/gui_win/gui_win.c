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
#include <string.h>
#include "guidef.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "gui_matrix.h"
#include "gui_obj.h"
#include "gui_win.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_win_update_att(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    animate_frame_update(this->animate, obj);
    {
        for (size_t i = 0; i < this->animate_array_length; i++)
        {
            animate_frame_update(this->animate_array[i], obj);
        }

    }

}
static void gui_win_input_prepare(gui_obj_t *obj)
{
}

static void prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_win_t *this = (void *)obj;
    GUI_UNUSED(this);

    gui_win_update_att(obj);

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL);

}

static void gui_win_destroy(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    if (this->animate_array)
    {
        for (size_t i = 0; i < this->animate_array_length; i++)
        {
            gui_free(this->animate_array[i]);
            this->animate_array[i] = NULL;
        }
        gui_free(this->animate_array);
        this->animate_array = 0;
        this->animate_array_length = 0;
    }
}

static void gui_win_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_win_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;

        case OBJ_DESTROY:
            gui_win_destroy(obj);
            break;

        default:
            break;
        }
    }
}

void gui_win_ctor(gui_win_t  *this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h)
{
    gui_obj_ctor(&this->base, parent, name, x, y, w, h);
    GET_BASE(this)->obj_cb = gui_win_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->type = WINDOW;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_win_left(gui_win_t *this, void *callback, void *parameter)
{
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
    GUI_ASSERT(0);
}

void gui_win_right(gui_win_t *this, void *callback, void *parameter)
{
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
    GUI_ASSERT(0);
}

void gui_win_up(gui_win_t *this, void *callback, void *parameter)
{
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_3, parameter);
    GUI_ASSERT(0);
}

void gui_win_down(gui_win_t *this, void *callback, void *parameter)
{
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_4, parameter);
    GUI_ASSERT(0);
}

void gui_win_press(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, callback, GUI_EVENT_TOUCH_PRESSED, parameter);
}

void gui_win_release(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, parameter);
}

void gui_win_long(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, parameter);
}

void gui_win_click(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}

void gui_win_hold_tp(gui_win_t *this, bool hold_tp)
{
    this->hold_tp = hold_tp;
}

gui_win_t *gui_win_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h)
{
    gui_win_t *this = gui_malloc(sizeof(gui_win_t));

    memset(this, 0, sizeof(gui_win_t));
    gui_win_ctor(this, parent, name, x, y, w, h);

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }

    ((gui_obj_t *)this)->create_done = true;

    return this;
}
float gui_win_get_animation_progress_percent(gui_win_t *win)
{
    if (win == NULL || win->animate == NULL)
    {
        // Handle error: window or animate structure is NULL
        gui_log("Cannot get_aniamtion_progress_percent: win or win->animate is NULL\n");
        return 0;
    }
    return win->animate->progress_percent;
}
void gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set_scale_rate: win is NULL\n");
        return;
    }
    win->scale = scale_rate_horizontal;
    win->scale_y = scale_rate_vertical;
}
void gui_win_set_scope(gui_win_t *win, bool enable)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set scope: win is NULL\n");
        return;
    }
    win->scope = enable;
}
void gui_win_move(gui_win_t *win, int x, int y)
{
    GUI_ASSERT(win != NULL);
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    GUI_BASE(win)->x = x;
    GUI_BASE(win)->y = y;
}
int gui_win_get_x(gui_win_t *win)
{
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    return GUI_BASE(win)->x;
}
int gui_win_get_y(gui_win_t *win)
{
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    return GUI_BASE(win)->y;
}
void gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set opacity: win is NULL\n");
        return;
    }
    GUI_BASE(win)->opacity_value = opacity_value;
}
// Function to retrieve the end_frame value from the window's animation structure
bool gui_win_is_animation_end_frame(gui_win_t *win)
{
    if (win == NULL || win->animate == NULL)
    {
        // Handle error: window or animate structure is NULL
        gui_log("Cannot judge animation_end_frame: win or win->animate is NULL\n");
        return 0;
    }
    return win->animate->end_frame != 0;
}
// Function to start the animation, sets animate to 1
void gui_win_start_animation(gui_win_t *win)
{
    if (win == NULL || win->animate == NULL)
    {
        // Handle error: window or animate structure is NULL
        gui_log("Cannot start animation: win or win->animate is NULL\n");
        return;
    }
    win->animate->animate = 1;
}
// Function to stop the animation, sets animate to 0
void gui_win_stop_animation(gui_win_t *win)
{
    if (win == NULL || win->animate == NULL)
    {
        // Handle error: window or animate structure is NULL
        gui_log("Cannot stop animation: win or win->animate is NULL\n");
        return;
    }
    win->animate->animate = 0;
}
void gui_win_prepare_globle(gui_obj_t *obj)
{
    prepare(obj);
}
void gui_win_prepare(gui_obj_t *obj)
{
    prepare(obj);
}

