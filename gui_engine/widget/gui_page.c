/*
 * File      : gui_page.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_page.h>
#include <string.h>
#include <gui_server.h>
#include <gui_img.h>
#include "gui_obj.h"
#include <gui_curtain.h>
#include <tp_algo.h>
static void deal_img_in_root(gui_obj_t *object, int dyend, int *out)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->dx = obj->x + obj->parent->dx;
        obj->dy = obj->y + obj->parent->dy;
        if (dyend < obj->dy + obj->h) { dyend = obj->dy + obj->h; }
        *out = dyend;
        deal_img_in_root(obj, dyend, out);
    }
}
void page_update(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if (((gui_page_t *)obj)->get_yend < 2)
    {
        int dy = 0;
        deal_img_in_root(obj, obj->y + obj->h, &dy);
        obj->h = dy - obj->y;
        obj->w = 320;
        //gui_log("deal_img_in_root %d",obj->h);
        ((gui_page_t *)obj)->get_yend++;
    }

    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if ((tp->x > ((gui_page_t *)obj)->start_x) && (tp->x < ((gui_page_t *)obj)->start_x + obj->w))
        {
            if ((tp->y > ((gui_page_t *)obj)->start_y) && (tp->y < ((gui_page_t *)obj)->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    ////gui_log("obj->y:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                    obj->y = tp->deltaY + ((gui_page_t *)obj)->yold;
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        obj->y = ((gui_page_t *)obj)->start_y;

                    }
                    else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)
                    {
                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                        obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                    }
                }
                else
                {
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        obj->y = ((gui_page_t *)obj)->start_y;

                    }
                    else if (obj->y < (((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                             obj->y != 0)
                    {
                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_page_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                        obj->y = ((gui_page_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                    }

                    ((gui_page_t *)obj)->yold = obj->y;
                }
            }
        }


        if (((gui_page_t *)obj)->scroll_bar)
        {
            ((gui_page_t *)obj)->scroll_bar->base.y = ((((gui_page_t *)obj)->start_y - obj->y) *
                                                       gui_get_screen_height() / obj->h);
        }


    }
}

static void gui_page_add_scroll_bar(gui_page_t *this, void *bar_pic)
{
    this->scroll_bar = gui_img_create_from_mem(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);
    gui_img_get_height(this->scroll_bar);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}
static void set_offset(gui_page_t *this, int offset)
{
    this->yold = offset;
    this->base.y = offset;
}
static int get_offset(gui_page_t *this)
{
    return this->base.y;
}
_gui_api_page_t gui_page_api =
{
    .set_offset = set_offset,
    .get_offset = get_offset,
    .gui_page_add_scroll_bar = gui_page_add_scroll_bar,
};
void gui_page_ctor(gui_page_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h
                );
    GET_BASE(this)->type = PAGE;
    GET_BASE(this)->obj_update_att = page_update;
    this->base.type = PAGE;
    this->start_x = x;
    this->start_y = y;
}
gui_page_t *gui_page_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h)
{
#define _GUI_NEW_gui_page_create_param this, parent, filename, x, y, w, h
    GUI_NEW(gui_page_t, gui_page_ctor, _GUI_NEW_gui_page_create_param)
}







