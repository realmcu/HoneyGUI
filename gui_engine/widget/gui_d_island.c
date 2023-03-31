/*
 * File      : gui_d_island.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_d_island.h>
#include <string.h>
#include <gui_server.h>

#include "gui_obj.h"
static int height = 50;
static int width = 100;
static int radius = 25;
static canvas_fill_t color = {.color_data.rgba = 0xffffffff};
static void cb1(void *p)
{
    gui_d_island_t *this = p;
    int from = this->from_w;
    int to = this->target_w;
    int w = (to - from) * (((this))->animate->progress_percent) + from;
    int x = (gui_get_screen_width() - w) / 2;
    from = this->from_h;
    to = this->target_h;
    int h = (to - from) * (((this))->animate->progress_percent) + from;
    width = w;
    height = h;
    radius = h / 2;
    this->base.w = w;
    this->base.h = h;
}
static void cb2(void *p)
{
    gui_d_island_t *this = p;
    int fromx, tox, fromy, toy, w, h;
    fromx = this->from_w;
    tox = this->target_w;
    w = (tox - fromx) * (((this))->animate->progress_percent) + fromx;
    fromy = this->from_h;
    toy = this->target_h;
    h = (toy - fromy) * (((this))->animate->progress_percent) + fromy;
    if (h <= this->threshold_h)
    {
        width = w;
        height = h;
    }
    else
    {
        width = w;
        height = h;
        radius = 25;
    }
    this->base.w = w;
    this->base.h = h;

}
static void cb3(void *p)
{
    gui_d_island_t *this = p;
    int from = this->from_w;
    int to = this->target_w;
    int w = (to - from) * (((this))->animate->progress_percent) + from;
    int x = (gui_get_screen_width() - w) / 2;
    from = this->from_h;
    to = this->target_h;
    int h = (to - from) * (((this))->animate->progress_percent) + from;
    width = w;
    height = h;
    radius = 25;
    this->base.w = w;
    this->base.h = h;
}
static void cb4(void *p)
{
    gui_d_island_t *this = p;
    int fromx, tox, fromy, toy, w, h;
    fromx = this->from_w;
    tox = this->target_w;
    w = (tox - fromx) * (((this))->animate->progress_percent) + fromx;
    fromy = this->from_h;
    toy = this->target_h;
    h = (toy - fromy) * (((this))->animate->progress_percent) + fromy;
    if (h > this->threshold_h)
    {
        width = w;
        height = h;
        radius = 25;
    }
    else
    {
        width = w;
        height = h;
        radius = h / 2;
    }
    this->base.w = w;
    this->base.h = h;
}
static void set_island(gui_d_island_t *this, int w, int h)
{
    //memset((this->animate), 0, sizeof(gui_animate_t));
    this->target_h = h;
    this->target_w = w;
    this->from_h = this->base.h;
    this->from_w = this->base.w;
    if (this->base.h <= this->threshold_h)
    {
        if (h <= this->threshold_h)
        {
            this->set_animate((void *)this, 200, 0, cb1, this);
        }
        else
        {
            this->set_animate((void *)this, 200, 0, cb2, this);
        }


    }
    else
    {
        if (h > this->threshold_h)
        {
            this->set_animate((void *)this, 200, 0, cb3, this);
        }
        else
        {
            this->set_animate((void *)this, 200, 0, cb4, this);
        }
    }


}
static void set_island_color(gui_d_island_t *this, canvas_fill_t *c)
{
    memcpy(&color, c, sizeof(color));
}
static void set_animate(gui_d_island_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_d_island_t *)o)->animate;
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
    ((gui_d_island_t *)o)->animate = animate;
}

static void draw(gui_canvas_t *c)
{
    canvas_rectangle_t r = {0};
    memcpy(&(r.fill), &color, sizeof(color));
    r.height = height;
    r.width = width;
    r.rx = radius;
    r.x = (gui_get_screen_width() - r.width) / 2;
    r.y = 20;
    gui_canvas_api.rectangle(c, &r);
}
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_d_island_t *obj = (void *)o;
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
static void island_ctor(gui_d_island_t *this, gui_obj_t *parent)
{
    gui_obj_ctor((void *)this, parent, "_island", 0, 0, 0, 0);
    this->base.w = 100;
    this->base.h = 50;
    this->base.y = 20;
    this->init_h = this->base.h;
    this->init_w = this->base.w;
    this->threshold_h = this->base.h * 2;

    this->set_island = set_island;
    this->set_animate = set_animate;
    this->base.obj_update_att = obj_update_att;
    this->set_island_color = set_island_color;
}
gui_d_island_t *gui_d_island_create(void *parent)
{
    gui_d_island_t *this = gui_malloc(sizeof(gui_d_island_t));
    memset(this, 0, sizeof(gui_d_island_t));
    island_ctor(this, parent);
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->canvas = gui_canvas_create(this, "_island", 0,
                                     0, this->base.w, this->base.h, 0xffffffff);
    this->canvas->draw = draw;
    ((gui_obj_t *)this)->create_done = 1;
    return this;
}





