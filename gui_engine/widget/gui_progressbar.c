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
    this->progress_rectangle->set(this->progress_rectangle, ((gui_obj_t *)this->progress_rectangle)->x,
                                  ((gui_obj_t *)this->progress_rectangle)->y, progress, ((gui_obj_t *)this->progress_rectangle)->h,
                                  this->progress_rectangle->base.color);
}
static size_t get_progress(gui_progressbar_t *this)
{
    return this->progress_rectangle->base.base.base.w;
}
static size_t get_max(gui_progressbar_t *this)
{
    return this->max_rectangle->base.base.base.w;
}
void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{
    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);

    this->get_max = get_max;

    this->base.type = PROGRESSBAR;
}

gui_progressbar_t *gui_progressbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t)); this->ctor = gui_progressbar_ctor;
    this->ctor(this, parent, filename, x, y, w, h); gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL) { gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list)); }     this->max_rectangle
        = gui_rectangle_create(this, "max_r", 0, 0, w, h, 0xf0f0);
    this->progress_rectangle = gui_rectangle_create(this, "progress_r", 0, 0,
                                                    (int16_t)(w * 0.6f), h, 0x0f0f);    this->get_progress = get_progress;
    this->set_progress = set_progress; GET_BASE(this)->create_done = true; return this;
}

