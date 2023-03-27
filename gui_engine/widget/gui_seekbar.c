/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_seekbar.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>

static void seekbar_update_att(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y)
        {
            gui_obj_t *circle_bitmap = &(((gui_seekbar_t *)obj)->slider.slider_circle->base.base.base);
            if (!(((gui_seekbar_t *)obj)->hit_slider))
            {
                if ((tp->x >= (circle_bitmap->x + obj->x + obj->parent->dx) &&
                     tp->x <= (circle_bitmap->x + obj->x + obj->parent->dx + circle_bitmap->w)) \
                    && (tp->y >= (circle_bitmap->y + obj->y + obj->parent->dy) &&
                        tp->y <= (circle_bitmap->y + obj->y + obj->parent->dy + circle_bitmap->h)))
                {
                    (((gui_seekbar_t *)obj)->hit_slider) = true;
                    if (((gui_seekbar_t *)obj)->hit_cb)
                    {
                        rtgui_msg_t msg;
                        msg.type = GUI_SRV_CB;
                        msg.cb = ((gui_seekbar_t *)obj)->hit_cb;
                        GUI_ASSERT(NULL != NULL);
                        GUI_UNUSED(msg);
                    }
                }
            }

        }
        else
        {
            (((gui_seekbar_t *)obj)->hit_slider) = false;
        }

    }
    obj->cover = ((gui_seekbar_t *)obj)->hit_slider;
}
static void seekbar_preapre(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    //gui_seekbar_t *circle = (gui_seekbar_t *)obj;
    //circle->slider.slider_circle->set((gui_circle_t *)circle, circle->base.get_progress(&(circle->base))+obj->x, (int16_t)(circle->slider.slider_circle->circle.center.y));
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {

        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_HOLD_Y)
        {
            gui_obj_t *circle_bitmap = &(((gui_seekbar_t *)obj)->slider.slider_circle->base.base.base);

            if (!(((gui_seekbar_t *)obj)->hit_slider))
            {
                if ((tp->x >= (circle_bitmap->x + obj->dx) &&
                     tp->x <= (circle_bitmap->x + obj->dx + circle_bitmap->w)) \
                    && (tp->y >= (circle_bitmap->y + obj->dy) &&
                        tp->y <= (circle_bitmap->y + obj->dy + circle_bitmap->h)))
                {
                    (((gui_seekbar_t *)obj)->hit_slider) = true;
                }
            }
            else
            {
                gui_circle_t *circle = ((gui_seekbar_t *)obj)->slider.slider_circle;
                int progress = circle->circle.center.x + tp->deltaX - ((gui_seekbar_t *)obj)->deltaX_old;
                if (progress >= 0 &&
                    progress <= ((gui_seekbar_t *)obj)->base.get_max(&(((gui_seekbar_t *)obj)->base)))
                {
                    if (((gui_seekbar_t *)obj)->slider_cb)
                    {
                        rtgui_msg_t msg;
                        msg.type = GUI_SRV_CB;
                        msg.cb = ((gui_seekbar_t *)obj)->slider_cb;
                        GUI_ASSERT(NULL != NULL);
                        GUI_UNUSED(msg);
                    }
                    circle->set(circle, (float)progress, (float)(circle->circle.center.y), circle->circle.radius,
                                circle->base.color);
                    ((gui_seekbar_t *)obj)->base.set_progress((gui_progressbar_t *)obj, progress);
                }
                ((gui_seekbar_t *)obj)->deltaX_old = tp->deltaX;
            }
        }
        else
        {
            (((gui_seekbar_t *)obj)->hit_slider) = false;
            ((gui_seekbar_t *)obj)->deltaX_old = 0;
        }

    }
    obj->cover = ((gui_seekbar_t *)obj)->hit_slider;

}
void gui_seekbar_ctor(gui_seekbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y, int16_t w, int16_t h)
{
    this->base.ctor = gui_progressbar_ctor;
    this->base.ctor(&(this->base), parent, filename, x, y, w, h);
    //this->slider.slider_circle = gui_circle_create(this, "slider", this->base.get_progress(&(this->base)),
    //this->base.base.y+this->base.max_rectangle->base.base.base.h/2, this->base.max_rectangle->base.base.base.h/2, 0x0f0f);

    this->base.base.obj_prepare = seekbar_preapre;
    this->base.base.obj_update_att = seekbar_update_att;
    this->base.base.type = SEEKBAR;
}
static void set_progress(gui_progressbar_t *this, size_t progress)
{
    this->progress_rectangle->set(this->progress_rectangle, ((gui_obj_t *)this->progress_rectangle)->x,
                                  ((gui_obj_t *)this->progress_rectangle)->y, progress, ((gui_obj_t *)this->progress_rectangle)->h,
                                  this->progress_rectangle->base.color);
}
static size_t get_progress(gui_progressbar_t *this)
{
    return this->progress_rectangle->base.base.base.w;
}
gui_seekbar_t *gui_seekbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h)
{
    gui_seekbar_t *this = gui_malloc(sizeof(gui_seekbar_t));
    memset(this, 0, sizeof(gui_seekbar_t));
    this->ctor = gui_seekbar_ctor;
    this->ctor(this, parent, filename, x, y, w, h);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    ((gui_progressbar_t *)this)->max_rectangle = gui_rectangle_create(this, "max_r", 0, 0, w, h,
                                                                      0xf0f0);
    ((gui_progressbar_t *)this)->progress_rectangle = gui_rectangle_create(this, "progress_r", 0, 0,
                                                                           (int16_t)(w * 0.6f), h, 0x0f0f);
    ((gui_progressbar_t *)this)->get_progress = get_progress;
    ((gui_progressbar_t *)this)->set_progress = set_progress;
    this->slider.slider_circle = gui_circle_create(this, "v", this->base.get_progress(&(this->base)),
                                                   h / 2, h, 0x8080);
    GET_BASE(this)->create_done = true;
    return this;
}
