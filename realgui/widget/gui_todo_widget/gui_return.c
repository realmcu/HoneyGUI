/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_return.c
  * @brief slide to return to home
  * @details return to launcher
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_return.h"
#include "tp_algo.h"
#include "gui_obj.h"

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

static void gui_return_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_seekbar_t *circle = &(((gui_return_t *)obj)->base);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        static bool enable, return_flag;
        int pro = 0;

        if ((tp->pressed)
            && (tp->x < RETURN_ENABLE_THREHOLD)
            && (tp->y > obj->y + RETURN_HEIGHT / 2))
        {
            enable = 1;

            if (GUI_TYPE(gui_return_t, obj)->ignore_gesture_widget)
            {
                //GUI_TYPE(gui_return_t, obj)->ignore_gesture_widget->gesture = 1;
                //because no gesture
                //comment for compile
                GUI_ASSERT(0);
            }

            GET_BASE(circle->base.c)->y = tp->y - RETURN_HEIGHT / 2 - obj->y;

            if ((GET_BASE(circle->base.c)->y + RETURN_HEIGHT + obj->y) > (int)gui_get_screen_height())
            {
                GET_BASE(circle->base.c)->y = (int)gui_get_screen_height() - RETURN_HEIGHT - obj->y;
            }
            gui_progressbar_set_progress((void *)circle, pro);
        }

        if (tp->released)
        {
            enable = 0;

            if (GUI_TYPE(gui_return_t, obj)->ignore_gesture_widget)
            {
                //GUI_TYPE(gui_return_t, obj)->ignore_gesture_widget->gesture = 0;
                //because no gesture
                //comment for compile
                GUI_ASSERT(0);
            }

            gui_progressbar_set_progress((void *)circle, pro);

            if (return_flag)
            {
                return_flag = 0;
                //gui_obj_enable_event(obj, GUI_EVENT_8);
                GUI_ASSERT(0);
            }
        }

        if (enable)
        {
            if ((tp->type == TOUCH_HOLD_X)
                || (tp->type == TOUCH_HOLD_Y)
                || (tp->pressed))
            {
                if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
                {
                    pro = tp->x + tp->deltaX - 0;

                    if (pro <= 0)
                    {
                        pro = 1;
                    }

                    if (pro >= obj->w)
                    {
                        pro = obj->w;
                    }

                    if (GET_BASE(circle->base.c)->type == IMAGE_FROM_MEM)
                    {
                        pro = pro * (circle->base.max - 2) / obj->w;
                    }

                    gui_progressbar_set_progress((void *)circle, pro);

                    if (tp->deltaX >= RETURN_THREHOLD)
                    {
                        return_flag = 1;
                    }
                    else
                    {
                        return_flag = 0;
                    }
                }
            }
        }
    }
}

static void gui_return_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_return_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_return_ctor(gui_return_t   *this,
                            gui_obj_t      *parent,
                            const uint32_t *frame_array[],
                            int             array_size,
                            void           *return_cb,
                            gui_obj_t      *ignore_gesture_widget)
{
    extern void gui_seekbar_ctor_movie_h(gui_seekbar_t *this, gui_obj_t *parent, void  **picture_array,
                                         uint16_t array_length, int16_t x, int16_t y);
    gui_seekbar_ctor_movie_h((void *)this, parent, (void *)frame_array, array_size, 0,
                             (int)gui_get_screen_height() - (int)gui_get_screen_height() * 2 / 3);

    this->ignore_gesture_widget = ignore_gesture_widget;
    GET_BASE(this)->obj_cb = gui_return_cb;
    GET_BASE(this)->has_prepare_cb = true;
    //gui_obj_add_event_cb(this, (gui_event_cb_t)return_cb, GUI_EVENT_8, this);
    GUI_ASSERT(0);

    GET_BASE(this)->w = RETURN_HEIGHT;
    GET_BASE(this)->h = (int)gui_get_screen_height() * 2 / 3;
    gui_img_set_mode((void *)this->base.base.c, IMG_SRC_OVER_MODE);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_return_t *gui_return_create(void           *parent,
                                const uint32_t *frame_array[],
                                int             array_size,
                                void           *return_cb,
                                gui_obj_t      *ignore_gesture_widget)
{
    gui_return_t *this = gui_malloc(sizeof(gui_return_t));

    memset(this, 0, sizeof(gui_return_t));
    gui_return_ctor(this, parent, frame_array, array_size, return_cb, ignore_gesture_widget);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

