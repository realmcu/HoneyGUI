/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <gui_path.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "acc_engine.h"


static void path_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);

}
static void path_draw_cb(gui_obj_t *obj)
{
    struct gui_dispdev *dc = gui_get_dc();
    gui_path_t *path = (gui_path_t *)obj;
    draw_path_t *draw_path = &path->draw_path;

    gui_get_acc()->draw_path(draw_path, dc);
}
static void path_end(gui_obj_t *obj)
{

}
static void path_destory(gui_obj_t *obj)
{

}




void gui_path_ctor(gui_path_t *this, gui_obj_t *parent, const char *name, void *addr,
                   int16_t x,
                   int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = IMAGE_FROM_MEM;
    root->obj_prepare = path_prepare;
    root->obj_draw = path_draw_cb;
    root->obj_end = path_end;
    root->obj_destory = path_destory;
    //for self


}

gui_path_t *gui_path_create(void *parent,  const char *name, void *data,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_PATH";
    }
    gui_path_t *path = gui_malloc(sizeof(gui_path_t));
    GUI_ASSERT(path != NULL);
    memset(path, 0x00, sizeof(gui_path_t));

    gui_path_ctor(path, (gui_obj_t *)parent, name, data, x, y, w, h);
    gui_list_init(&(GET_BASE(path)->child_list));
    if ((GET_BASE(path)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(path)->parent)->child_list), &(GET_BASE(path)->brother_list));
    }
    GET_BASE(path)->create_done = true;
    return path;
}

void gui_path_update(gui_path_t *path, void *data)
{

}


