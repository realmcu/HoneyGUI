/*
 * File      : gui_widget_template.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "gui_widget_template.h"


static void widget_template_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
}

static void widget_template_draw_cb(gui_obj_t *obj)
{
    gui_widget_template_t *this = (gui_widget_template_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();


}
static void widget_template_end(gui_obj_t *obj)
{

}
static void widget_template_destory(gui_obj_t *obj)
{

}



static void widget_template_ctor(gui_widget_template_t *this, gui_obj_t *parent, const char *name,
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
    root->obj_prepare = widget_template_prepare;
    root->obj_draw = widget_template_draw_cb;
    root->obj_end = widget_template_end;
    root->obj_destory = widget_template_destory;

    //for self

}



gui_widget_template_t *gui_widget_template_create(void *parent,  const char *name, void *data,
                                                  int16_t x,
                                                  int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_TEMPLATE";
    }
    gui_widget_template_t *this = gui_malloc(sizeof(gui_widget_template_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_widget_template_t));

    widget_template_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }




    GET_BASE(this)->create_done = true;
    return this;
}



