/*
 * File      : gui_d_island.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_d_island.h>
#include <string.h>
#include <gui_server.h>

#include "gui_obj.h"
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
    (this->island)->set((this->island), x, 0, w, h, 0xffffffff);
    this->base.w = w;
    this->base.h = h;
}
static void cb2(void *p)
{
    gui_d_island_t *this = p;
    int fromx, tox, fromy, toy, w, x, h;
    fromx = this->from_w;
    tox = this->target_w;
    w = (tox - fromx) * (((this))->animate->progress_percent) + fromx;
    x = (gui_get_screen_width() - w) / 2;
    fromy = this->from_h;
    toy = this->target_h;
    h = (toy - fromy) * (((this))->animate->progress_percent) + fromy;
    if (h <= this->threshold_h)
    {
        (this->island)->set((this->island), x, 0, w, h, 0xffffffff);
    }
    else
    {
        if (this->island)
        {
            gui_tree_free(this->island);
            this->island = NULL;
            this->island2 = gui_round_corner_rect_create(this, "_island", x, 0, w, h, 33, 0xffffffff);
        }
        else
        {
            this->island2->set(this->island2, x, 0, w, h, 33, 0xffffffff);
        }

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
    (this->island2)->set((this->island2), x, 0, w, h, 33, 0xffffffff);
    this->base.w = w;
    this->base.h = h;
}
static void cb4(void *p)
{
    gui_d_island_t *this = p;
    int fromx, tox, fromy, toy, w, x, h;
    fromx = this->from_w;
    tox = this->target_w;
    w = (tox - fromx) * (((this))->animate->progress_percent) + fromx;
    x = (gui_get_screen_width() - w) / 2;
    fromy = this->from_h;
    toy = this->target_h;
    h = (toy - fromy) * (((this))->animate->progress_percent) + fromy;
    if (h > this->threshold_h)
    {
        this->island2->set(this->island2, x, 0, w, h, 33, 0xffffffff);
    }
    else
    {
        if (this->island2)
        {
            gui_tree_free(this->island2);
            this->island2 = NULL;
            this->island = gui_rount_rect_create(this, "_island", x, 0, w, h, 0xffffffff);
        }
        else
        {
            this->island->set(this->island, x, 0, w, h, 0xffffffff);
        }

    }
    this->base.w = w;
    this->base.h = h;
}
static void set_island(gui_d_island_t *this, int w, int h)
{
    memset((this->animate), 0, sizeof(gui_animate_t));
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
static void island_ctor(gui_d_island_t *this, gui_obj_t *parent)
{
    gui_obj_ctor((void *)this, parent, "_island", 0, 0, 0, 0);
    this->base.w = 100;
    this->base.h = 34;
    this->base.y = 20;
    this->init_h = this->base.h;
    this->init_w = this->base.w;
    this->threshold_h = this->base.h * 2;
    this->island = gui_rount_rect_create(this, "_island", (gui_get_screen_width() - this->base.w) / 2,
                                         0, this->base.w, this->base.h, 0xffffffff);
    this->set_island = set_island;
    this->set_animate = set_animate;
}
gui_d_island_t *gui_d_island_create(void *parent)
{
#define _d_island_ctor_param this, parent
    GUI_NEW(gui_d_island_t, island_ctor, _d_island_ctor_param)
}





