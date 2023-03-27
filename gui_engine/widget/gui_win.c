/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_win.h>
#include <string.h>
#include <gui_server.h>
#include <tp_algo.h>
#include "gui_obj.h"
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
                if (tp->type == TOUCH_RIGHT_SLIDE)
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE);
                }
                else if (tp->type == TOUCH_LEFT_SLIDE)
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE);
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
void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                  int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_prepare = win_prepare;
    this->set_animate = set_animate;
    GET_BASE(this)->obj_update_att = obj_update_att;
}
gui_win_t *gui_win_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h)
{
#define _GUI_NEW_gui_win_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_win_t, gui_win_ctor, _GUI_NEW_gui_win_create_param)
}









