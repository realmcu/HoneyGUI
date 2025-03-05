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

    gui_win_update_att(obj);

    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0) && !(obj->event_dsc->event_code))
    {
        gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if (tp->pressing)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
        }
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                gui_obj_enable_event(obj, GUI_EVENT_3);
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                gui_obj_enable_event(obj, GUI_EVENT_4);
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                gui_obj_enable_event(obj, GUI_EVENT_1);
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                gui_obj_enable_event(obj, GUI_EVENT_2);
            }
            break;

        case TOUCH_LONG:
            {
                if (this->long_flag == false)
                {
                    this->long_flag = true;
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG);
                }
            }
            break;

        default:
            break;
        }

        if (tp->pressed)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
            this->long_flag = false;
            this->press_flag = true;
        }
    }

    if (this->release_flag)
    {
        this->press_flag = false;
        this->release_flag = false;
        if (obj->event_dsc_cnt && obj->event_dsc->event_code == GUI_EVENT_INVALIDE)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
        }
        this->long_flag = false;
    }

    if (tp->released && this->press_flag)
    {
        this->release_flag = true;
    }

    if (this->scale != 0 || this->scale_y != 0)
    {
        matrix_translate(GET_BASE(obj)->w / 2, GET_BASE(obj)->h / 2, GET_BASE(obj)->matrix);
        if (this->scale == 0)
        {
            matrix_scale(1, this->scale_y, GET_BASE(obj)->matrix);
        }
        else if (this->scale_y == 0)
        {
            matrix_scale(this->scale, 1, GET_BASE(obj)->matrix);
        }
        else
        {
            matrix_scale(this->scale, this->scale_y, GET_BASE(obj)->matrix);
        }
        matrix_translate(GET_BASE(obj)->w / -2, GET_BASE(obj)->h / -2, GET_BASE(obj)->matrix);
    }
    if (this->hold_tp)
    {
        gui_obj_skip_other_up_hold(obj);
        gui_obj_skip_other_down_hold(obj);
        gui_obj_skip_other_left_hold(obj);
        gui_obj_skip_other_right_hold(obj);
    }
    if (this->event5_flag)
    {
        gui_obj_enable_event(obj, GUI_EVENT_5);
    }


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

void gui_win_set_animate(gui_win_t *this,
                         uint32_t   dur,
                         int        repeat_count,
                         gui_animate_callback_t callback,
                         void      *p)
{
    if (this->animate_array_length != 0)
    {
        return;
    }
    GUI_SET_ANIMATE_HELPER
}
void gui_win_append_animate(gui_win_t  *win,
                            uint32_t    dur,
                            int         repeat_count,
                            void       *callback,
                            void       *p,
                            const char *name)
{
    win->animate_array_length++;
    win->animate_array = gui_realloc(win->animate_array,
                                     sizeof(*(win->animate_array)) * win->animate_array_length);
    win->animate_array[win->animate_array_length - 1] = gui_malloc(sizeof(gui_animate_t));
    gui_animate_t *animate = win->animate_array[win->animate_array_length - 1];

    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    if (dur == 0)
    {
        animate->dur = 1000;
    }
    animate->callback = (gui_animate_callback_t)callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    animate->name = name;
}
void gui_win_left(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}

void gui_win_right(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
}

void gui_win_up(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_3, parameter);
}

void gui_win_down(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_4, parameter);
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
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
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
    GUI_WIDGET_TYPE_TRY_EXCEPT(win, WINDOW)
    GUI_BASE(win)->x = x;
    GUI_BASE(win)->y = y;
}
int gui_win_get_x(gui_win_t *win)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(win, WINDOW)
    return GUI_BASE(win)->x;
}
int gui_win_get_y(gui_win_t *win)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(win, WINDOW)
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

