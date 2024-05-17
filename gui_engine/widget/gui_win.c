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

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

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

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void gui_win_update_att(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    //size_t frame_count;
    uint32_t cur_time_gap;
    if (this->animate && this->animate->animate)
    {
        this->animate->Beginning_frame = 0;
        this->animate->end_frame = 0;
        if (this->animate->progress_percent == 0 && !this->animate->init)
        {
            this->animate->init = 1;
            this->animate->init_time_ms = gui_ms_get();
            this->animate->Beginning_frame = 1;
        }

        this->animate->cur_time_ms = gui_ms_get();
        cur_time_gap = this->animate->cur_time_ms - this->animate->init_time_ms;

        if (this->animate->repeat_count == 0)
        {
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            if (cur_time_gap / this->animate->dur >= 1)
            {
                this->animate->end_frame = 1;
                this->animate->progress_percent = 1;
                this->animate->animate = 0;
            }
            this->animate->callback(this->animate->p);

        }
        else if (this->animate->repeat_count == -1)
        {
            uint32_t  round_count = cur_time_gap / this->animate->dur;
            if (round_count > this->animate->last_round)
            {
                this->animate->Beginning_frame = 1;
            }
            this->animate->last_round = round_count;
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            this->animate->callback(this->animate->p);
        }
        else
        {
            uint32_t  round_count = cur_time_gap / this->animate->dur;
            if (round_count > this->animate->repeat_count)
            {
                this->animate->animate = 0;
                return;
            }

            if (round_count > this->animate->last_round)
            {
                this->animate->Beginning_frame = 1;
            }
            this->animate->last_round = round_count;
            this->animate->progress_percent = (float)(cur_time_gap % this->animate->dur) /
                                              (float)this->animate->dur;
            this->animate->callback(this->animate->p);
        }
    }
}

static void gui_win_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_win_t *this = (void *)obj;

    gui_win_update_att(obj);

    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0))
    {
        gui_obj_event_set(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
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
                this->long_flag = true;
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
            }
            break;

        default:
            break;
        }

        if (tp->pressed)
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);
            this->long_flag = false;
            this->press_flag = true;
        }
        if (tp->pressing)
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSING);
        }
    }

    if (this->release_flag)
    {
        this->press_flag = false;
        this->release_flag = false;
        gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
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
        this->scale = 0;
    }
}

static void gui_win_destory(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
}

static void gui_win_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;

        case OBJ_DESTORY:
            gui_win_destory(obj);
            break;

        default:
            break;
        }
    }
}

void gui_win_ctor(gui_win_t  *this,
                  gui_obj_t  *parent,
                  const char *filename,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    GET_BASE(this)->obj_cb = gui_win_cb;
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
                         void      *callback,
                         void      *p)
{
    gui_animate_t *animate = ((gui_win_t *)this)->animate;

    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }

    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    ((gui_win_t *)this)->animate = animate;
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

void gui_win_press(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_PRESSED, parameter);
}

void gui_win_release(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, parameter);
}

void gui_win_long(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, parameter);
}

void gui_win_click(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}

gui_win_t *gui_win_create(void       *parent,
                          const char *filename,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h)
{
    gui_win_t *this = gui_malloc(sizeof(gui_win_t));

    memset(this, 0, sizeof(gui_win_t));
    gui_win_ctor(this, parent, filename, x, y, w, h);

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }

    ((gui_obj_t *)this)->create_done = true;

    return this;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
