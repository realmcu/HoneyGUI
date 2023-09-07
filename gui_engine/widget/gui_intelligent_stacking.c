/*
 * File      : gui_intelligent_stacking.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "gui_intelligent_stacking.h"


static void intelligent_stacking_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
}

static void intelligent_stacking_draw_cb(gui_obj_t *obj)
{
    gui_intelligent_stacking_t *this = (gui_intelligent_stacking_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    GUI_UNUSED(this);
    GUI_UNUSED(dc);


}
static void intelligent_stacking_end(gui_obj_t *obj)
{

}
static void intelligent_stacking_destory(gui_obj_t *obj)
{

}




gui_intelligent_stacking_t *gui_intelligent_stacking_create(void *parent,  const char *name,
                                                            int16_t x, int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "INTELLIGENT_STACKING";
    }
    gui_intelligent_stacking_t *this = gui_malloc(sizeof(gui_intelligent_stacking_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_intelligent_stacking_t));


    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_prepare = intelligent_stacking_prepare;
    root->obj_draw = intelligent_stacking_draw_cb;
    root->obj_end = intelligent_stacking_end;
    root->obj_destory = intelligent_stacking_destory;

    //for self


    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    gui_list_init(&this->tab_list);



    GET_BASE(this)->create_done = true;
    return this;
}



