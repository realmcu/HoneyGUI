/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_progressbar.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>


static void set_progress(gui_progressbar_t *this, size_t progress)
{
    this->per = ((float)progress) / ((float)this->max);
}
static size_t get_progress(gui_progressbar_t *this)
{
    return (size_t)(this->per  * ((float)this->max));
}
static void set_percentage(gui_progressbar_t *this, float percentage)
{
    this->per = percentage;
}
static float get_percentage(gui_progressbar_t *this)
{
    return this->per;
}
static size_t get_max(gui_progressbar_t *this)
{
    return this->max;
}
_gui_api_progressbar_t gui_progressbar_api =
{
    .get_max = get_max,
    .get_percentage = get_percentage,
    .get_progress = get_progress,
    .set_percentage = set_percentage,
    .set_progress = set_progress,
};
#include "acc_engine.h"
static void draw_h(gui_canvas_t *c)
{
    gui_progressbar_t *p = (void *)(GET_BASE(c)->parent);
    {
        canvas_rectangle_t r = {0};
        r.width = GET_BASE(p)->w;
        r.height = GET_BASE(p)->h;
        r.rx = r.height / 2;
        r.fill.color_data.rgba = p->color;
        gui_canvas_api.rectangle(p->c, &r);
    }
    {
        canvas_rectangle_t r2 = {0};
        r2.width = GET_BASE(p)->w * p->per;
        r2.height = GET_BASE(p)->h;
        r2.rx = r2.height / 2;
        r2.fill.color_data.rgba = p->color_hl;
        gui_canvas_api.rectangle(p->c, &r2);
    }
}
static void draw_v(gui_canvas_t *c)
{
    gui_progressbar_t *p = (void *)(GET_BASE(c)->parent);
    {
        canvas_rectangle_t r = {0};
        r.width = GET_BASE(p)->w;
        r.height = GET_BASE(p)->h;
        r.rx = r.width / 2;
        r.fill.color_data.rgba = p->color;
        gui_canvas_api.rectangle(p->c, &r);
    }
    {
        canvas_rectangle_t r2 = {0};
        r2.width = GET_BASE(p)->w;
        r2.height = GET_BASE(p)->h * p->per;
        r2.y = GET_BASE(p)->h - r2.height;
        r2.rx = r2.width / 2;
        r2.fill.color_data.rgba = p->color_hl;
        gui_canvas_api.rectangle(p->c, &r2);
    }
}
void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);


    this->color = 0xffffffaa;
    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = w;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->c = gui_canvas_create(this, "pro", 0, 0, w, h, 0xffffffff);
    this->c->draw = draw_h;
}

gui_progressbar_t *gui_progressbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h)
{

//#define _progressbar_create_parameter_ this, parent, filename, x, y, w, h
//      GUI_NEW(gui_progressbar_t, gui_progressbar_ctor, _progressbar_create_parameter_)
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_ctor(this, parent, filename, x, y, w, h);

    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);


    this->color = 0xffffffaa;
    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = h;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->c = gui_canvas_create(this, "pro", 0, 0, w, h, 0xffffffff);
    this->c->draw = draw_v;
}

gui_progressbar_t *gui_progressbar_v_create(void *parent, const char *filename, int16_t x,
                                            int16_t y,
                                            int16_t w, int16_t h)
{

//#define _progressbar_create_parameter_ this, parent, filename, x, y, w, h
//      GUI_NEW(gui_progressbar_t, gui_progressbar_ctor, _progressbar_create_parameter_)
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_v_ctor(this, parent, filename, x, y, w, h);

    ((gui_obj_t *)this)->create_done = 1;
    return this;
}

/*
void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{
    this->base.ctor = gui_obj_ctor;
    this->base.ctor(parent, filename, x, y, w, h, &(this->base));
        this->c = gui_canvas_create(this, "pro", 0,0,w,h, 0xffffffff);
    this->c->draw_bar_v(this->c, 0,0,w,h);
    this->c->base.color = 0xffffffaa;
    this->c->base.color_stop = 0xffffffff;
    this->c->base.stop = 0.3f;
    this->c->base.v = 1;
    this->base.type = PROGRESSBAR;
    this->max = h;
}

gui_progressbar_t *gui_progressbar_v_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    this->ctor = gui_progressbar_v_ctor;
    this->ctor(this, parent, filename, x, y, w, h);
    GET_BASE(this)->create_done = true;
    return this;
}
void gui_progressbar_h_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{
    this->base.ctor = gui_obj_ctor;
    this->base.ctor(parent, filename, x, y, w, h, &(this->base));
    this->c = gui_canvas_create(this, "pro", 0,0,w,h, 0xffffffff);
    //this->c->draw_bar_v(this->c, 0,0,5,30);

    this->c->draw_round_rect(this->c, 0,0,w,h);
    this->c->base.color = 0xffffffff;
    this->c->base.color_stop = 0xffffffaa;
    this->c->base.stop = 0.3f;
    this->c->base.v = 0;
    this->base.type = PROGRESSBAR;
    this->max = w;
}

gui_progressbar_t *gui_progressbar_h_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    this->ctor = gui_progressbar_h_ctor;
    this->ctor(this, parent, filename, x, y, w, h);
    GET_BASE(this)->create_done = true;
    return this;
}
*/
