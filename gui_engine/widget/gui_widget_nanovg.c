/*
 * File      : gui_widget_nanovg.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include <nanovg.h>
#include "gui_widget_nanovg.h"


static void widget_nanovg_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;


}


void gui_widget_nanovg_set_canvas_cb(gui_widget_nanovg_t *this,
                                     void (*cb)(gui_widget_nanovg_t *this))
{
    this->nanovg_canvas_cb = cb;
}

static void widget_nanovg_draw_cb(gui_obj_t *obj)
{
    gui_widget_nanovg_t *this = (gui_widget_nanovg_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    if (this->nanovg_canvas_cb != NULL)
    {
        NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
        nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

        gui_widget_nanovg_t *this = (gui_widget_nanovg_t *)obj;
        this->vg = vg;
        this->nanovg_canvas_cb(this);
        nvgEndFrame(this->vg);

        nvgDeleteAGGE(this->vg);
    }

}
static void widget_nanovg_end(gui_obj_t *obj)
{
    gui_widget_nanovg_t *this = (gui_widget_nanovg_t *)obj;

}
static void widget_nanovg_destory(gui_obj_t *obj)
{

}



static void widget_nanovg_ctor(gui_widget_nanovg_t *this, gui_obj_t *parent, const char *name,
                               void *data,
                               int16_t x,
                               int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_prepare = widget_nanovg_prepare;
    root->obj_draw = widget_nanovg_draw_cb;
    root->obj_end = widget_nanovg_end;
    root->obj_destory = widget_nanovg_destory;

    //for self

}



gui_widget_nanovg_t *gui_widget_nanovg_create(void *parent,  const char *name, void *data,
                                              int16_t x,
                                              int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_nanovg";
    }
    gui_widget_nanovg_t *this = gui_malloc(sizeof(gui_widget_nanovg_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_widget_nanovg_t));

    widget_nanovg_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }




    GET_BASE(this)->create_done = true;
    return this;
}



