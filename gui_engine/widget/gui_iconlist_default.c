/*
 * File      : gui_iconview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_iconlist.h>
#include <string.h>
#include <gui_server.h>
#include "gui_obj.h"
#include <tp_algo.h>


void iconlist_prepare_default(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_iconlist_default_t *this = (gui_iconlist_default_t *)obj;
    if (!(((obj->parent->dx < (int)gui_get_screen_width()) &&
           (obj->parent->dx + obj->parent->w) >= 0) && \
          (obj->parent->dy < (int)gui_get_screen_height()) && ((obj->parent->dy + obj->parent->h) >= 0)))
    {
        this->base.dy_static = (int)(this->icon_gap) / 10;
        obj->cover = false;
        this->base.show_flag = false;
        return;
    }
    if (obj->parent->dx == 0 && obj->parent->dy == 0)
    {
        obj->cover = true;
        if ((tp->type == TOUCH_HOLD_Y))
        {
            obj->dy = tp->deltaY;
        }
        else if (tp->type == TOUCH_ORIGIN_FROM_Y || tp->type == TOUCH_UP_SLIDE ||
                 tp->type == TOUCH_DOWN_SLIDE)
        {
            if (1)
            {
                this->base.dy_static += tp->deltaY;
            }
            else
            {
                this->base.show_flag = true;
            }
        }
        if (this->base.dy_static >= 0)
        {
            this->base.dy_static = SPRING_HEAD;
        }
        else
        {
            int spring = (int)(((int)gui_get_screen_height() / (int)(this->icon_gap) - (int)(
                                    this->base.icon_cnt)) *
                               ((int)(this->icon_gap)));
            if (this->base.dy_static < spring)
            {
                if (spring >= 0)
                {
                    this->base.dy_static = SPRING_HEAD;
                }
                else
                {
                    this->base.dy_static = spring;
                }
            }
        }

    }






}
static void set_background(gui_iconlist_t *this, void *m1, void *m2)
{
    if (gui_iocnlist_get_theme((char *)(this->base.parent->name)) == ICON_LIST_THEME_0)
    {
        gui_iconlist_default_t *theme1 = (gui_iconlist_default_t *)this;
        theme1->back_img_src = m1;
        theme1->back_img_highlight_src = m2;
    }
}

void gui_iconlist_default_ctor(gui_obj_t *parent, const char *filename, int16_t x,
                               int16_t y, int16_t w, int16_t h, gui_iconlist_default_t *this)
{
    this->base.ctor = gui_iconlist_ctor;
    this->base.ctor(parent, filename, x, y, w, h, &this->base);
    struct gui_dispdev *dc = gui_get_dc();
    if (dc->adaption)
    {
        this->icon_gap = 100 * dc->scale_y;
    }
    else
    {
        this->icon_gap = 100;
    }
    this->base.dy_static = SPRING_HEAD;
    this->base.set_background = set_background;
    ((gui_obj_t *)this)->obj_prepare = iconlist_prepare_default;
}
gui_iconlist_default_t *gui_iconlist_create_default(void *parent, const char *filename,
                                                    int16_t x,
                                                    int16_t y,
                                                    int16_t w, int16_t h)
{
#define _gui_new_gui_iconlist_create_default_param parent, filename, x, y, w, h, this
    GUI_NEW(gui_iconlist_default_t, gui_iconlist_default_ctor,
            _gui_new_gui_iconlist_create_default_param)
}
