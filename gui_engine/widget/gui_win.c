/*
 * File      : gui_win.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_win.h>
#include <string.h>
//#include <gui_server.h>
#include <tp_algo.h>
#include "gui_obj.h"
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
static void set_animate(gui_win_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
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
void win_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        {
            if (
                ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                       tp->y <= (obj->dy + obj->h))))
            {
                gui_win_t *b = (void *)obj;
                switch (tp->type)
                {
                case TOUCH_SHORT:
                    {
                        ////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);
                            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                            {
                                //gui_log("%d\n", __LINE__);
                                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                            }
                        }
                    }
                    break;
                case TOUCH_UP_SLIDE:
                    {
                        ////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);
                            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                            {
                                //gui_log("%d\n", __LINE__);
                                gui_obj_event_set(obj, GUI_EVENT_3);
                            }
                        }
                    }
                    break;
                case TOUCH_DOWN_SLIDE:
                    {
                        ////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);
                            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                            {
                                //gui_log("%d\n", __LINE__);
                                gui_obj_event_set(obj, GUI_EVENT_4);
                            }
                        }
                    }
                    break;
                case TOUCH_LEFT_SLIDE:
                    {
                        ////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);
                            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                            {
                                //gui_log("%d\n", __LINE__);
                                gui_obj_event_set(obj, GUI_EVENT_1);
                            }
                        }
                    }
                    break;
                case TOUCH_RIGHT_SLIDE:
                    {
                        ////gui_log("%s\n", "TOUCH_SHORT");
                        //

                        //if (callback)
                        {
                            //gui_log("%d\n", __LINE__);
                            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                            {
                                //gui_log("%d\n", __LINE__);
                                gui_obj_event_set(obj, GUI_EVENT_2);
                            }
                        }
                    }
                    break;
                case TOUCH_LONG:
                    {
                        if (b->long_flag == false)
                        {

                            //if (b->long_click_cb)
                            {
                                if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                                    (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                                {
                                    b->long_flag = true;
                                    //gui_send_callback_p_to_server(b->long_click_cb, b->long_click_cb_p);
                                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_LONG);
                                }
                            }
                        }
                    }
                    break;

                default:
                    break;
                }


                {
                    if (tp->pressed)
                    {

                        if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) &&
                            (tp->y >= obj->dy && tp->y <= (obj->dy + obj->h)))
                        {

                            //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);

                            //gui_log("%d\n", __LINE__);
                            gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);  //gui_log("%d\n", __LINE__);
                            b->long_flag = false;
                            b->press_flag = true;
                        }
                    }

                    if (b->release_flag)
                    {

                        {
                            b->press_flag = false;
                            b->release_flag = false;
                            //gui_send_callback_p_to_server(b->press_cb, b->press_cb_p);
                            //gui_log("%d\n", __LINE__);

                            //if (callback)
                            {
                                gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
                            }  //gui_log("%d\n", __LINE__);
                            b->long_flag = false;
                        }
                    }
                    if (tp->released && b->press_flag)
                    {
                        b->release_flag = true;
                    }
                }

            }
        }
    }
}
#include "gui_server.h"
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_win_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * (1000 / 15) / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

    }
}
static void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                         int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    GET_BASE(this)->obj_prepare = win_prepare;
    GET_BASE(this)->obj_update_att = obj_update_att;
}
gui_win_t *gui_win_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h)
{
#define _GUI_NEW_gui_win_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_win_t, gui_win_ctor, _GUI_NEW_gui_win_create_param)
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
    .set_animate = set_animate,
};








