/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_progressbar.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <gui_img.h>


static void set_progress(gui_progressbar_t *this, size_t progress)
{
    this->per = ((float)progress) / ((float)this->max);
    if (GET_BASE(this->c)->type == IMAGE_SCOPE)
    {
        if (this->color == 1)
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = progress;
        }
        else
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = (1.0f - this->per) * this->max;
        }
    }
    else if (GET_BASE(this->c)->type == IMAGE_FROM_MEM)
    {
        size_t p = progress;
        if (p > this->max - 2)
        {
            p = this->max - 2;
        }

        GUI_TYPE(gui_img_t, this->c)->draw_img.data = ((void **)(this->color_hl))[p];
    }


}
static size_t get_progress(gui_progressbar_t *this)
{
    return (size_t)(this->per  * ((float)this->max));
}
static void set_percentage(gui_progressbar_t *this, float percentage)
{
    this->per = percentage;
    if (GET_BASE(this->c)->type == IMAGE_SCOPE)
    {
        if (this->color == 1)
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = percentage * this->max;
        }
        else
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = (1.0f - percentage) * this->max;
        }
    }
    else if (GET_BASE(this->c)->type == IMAGE_FROM_MEM)
    {
        size_t p = percentage * this->max;
        if (p > this->max - 2)
        {
            p = this->max - 2;
        }

        GUI_TYPE(gui_img_t, this->c)->draw_img.data = ((void **)(this->color_hl))[p];
    }
}
static float get_percentage(gui_progressbar_t *this)
{
    return this->per;
}
static size_t get_max(gui_progressbar_t *this)
{
    return this->max;
}

void gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress)
{
    set_progress(this, progress);
}
size_t gui_progressbar_get_progress(gui_progressbar_t *this)
{
    return get_progress(this);
}
void gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage)
{
    set_percentage(this, percentage);
}
float gui_progressbar_get_percentage(gui_progressbar_t *this)
{
    return get_percentage(this);
}
size_t gui_progressbar_get_max(gui_progressbar_t *this)
{
    return get_max(this);
}
//void draw_h(gui_canvas_t *c)
//{

//}
//void draw_v(gui_canvas_t *c)
//{

//}

void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);


    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = w;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    //this->c = gui_canvas_create(this, "pro", 0, 0, w, h, 0xffffffff);
    //this->c->draw = draw_h;
}
void gui_progressbar_h_img_ctor(gui_progressbar_t *this, gui_obj_t *parent, void *picture,
                                int16_t x,
                                int16_t y)
{

    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;
    this->color = 1;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->c = (void *)gui_img_scope_create(this, picture, 0, 0);

    int w = gui_img_get_width((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = gui_img_get_height((void *)this->c);
    GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = this->max * this->per;


}
void gui_progressbar_v_img_ctor(gui_progressbar_t *this, gui_obj_t *parent, void *picture,
                                int16_t x,
                                int16_t y)
{

    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->c = (void *)gui_img_scope_create(this, picture, 0, 0);

    int w = gui_img_get_height((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = gui_img_get_width((void *)this->c);
    GET_BASE(this)->h = w;
    GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = this->max * (1.0f - this->per);


}
#if 0
/*
   gui_img_scope_t *s = gui_img_scope_create(&app->screen, gui_get_file_address("app/system/resource/back_left.bin"), 100,100);
s->scope_x1=10;
s->scope_x2=100;
s->scope_y1=10;
s->scope_y2 = 80;
*/
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
#endif
gui_progressbar_t *gui_progressbar_img_h_create(void *parent, void *picture, int16_t x, int16_t y)
{

//#define _progressbar_create_parameter_ this, parent, filename, x, y, w, h
//      GUI_NEW(gui_progressbar_t, gui_progressbar_ctor, _progressbar_create_parameter_)
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_h_img_ctor(this, parent, picture, x, y);

    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
gui_progressbar_t *gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y)
{

//#define _progressbar_create_parameter_ this, parent, filename, x, y, w, h
//      GUI_NEW(gui_progressbar_t, gui_progressbar_ctor, _progressbar_create_parameter_)
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_v_img_ctor(this, parent, picture, x, y);

    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);


    //this->color = 0xffffffaa;
    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = h;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    //this->c = gui_canvas_create(this, "pro", 0, 0, w, h, 0xffffffff);
    //this->c->draw = draw_v;
}
void gui_progressbar_movie_ctor(gui_progressbar_t *this, gui_obj_t *parent, void  **picture_array,
                                uint16_t array_number,
                                int16_t x,
                                int16_t y)
{

    gui_obj_ctor(&(this->base), parent, "progressbar_movie", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = array_number - 1;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    { gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list), &(((gui_obj_t *)this)->brother_list)); }
    this->c = (void *)gui_img_create_from_mem(this, "pro", picture_array[0], 0, 0, 0, 0);
    this->color_hl = (uint32_t)picture_array;
}
gui_progressbar_t *gui_progressbar_movie_create(void *parent, void  **picture_array,
                                                uint16_t array_length, int16_t x, int16_t y)
{

//#define _progressbar_create_parameter_ this, parent, filename, x, y, w, h
//      GUI_NEW(gui_progressbar_t, gui_progressbar_ctor, _progressbar_create_parameter_)
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_movie_ctor(this, parent, picture_array, array_length, x, y);

    ((gui_obj_t *)this)->create_done = 1;
    return this;
}
#if 0
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
#endif
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
