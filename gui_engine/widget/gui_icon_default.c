/*
 * File      : gui_iconview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_iconlist.h>
#include <string.h>
#include <gui_server.h>
#include "gui_obj.h"
#include <tp_algo.h>

static void icon_update_att_default(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if (obj->parent->type == ICONLIST)
    {
        gui_icon_default_t *ext = (gui_icon_default_t *)obj;
        gui_obj_t *parent = obj->parent;
        gui_iconlist_default_t *parent_ext = (gui_iconlist_default_t *)parent;

        obj->y = (ext->base.id * (int)(parent_ext->icon_gap) + parent_ext->base.dy_static);
    }
}
static void icon_prepare_default(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if (obj->parent->type == ICONLIST)
    {
        if (((gui_iconlist_default_t *)(obj->parent))->back_img_src != NULL)
        {
            gui_img_set_attribute(((gui_icon_default_t *)(obj))->back_img, NULL,
                                  ((gui_iconlist_default_t *)(obj->parent))->back_img_src, 0, 0);
        }
    }
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if (tp->type == TOUCH_SHORT)
        {
            if ((tp->x >= obj->dx && tp->x <= (obj->dx + obj->w)) && (tp->y >= obj->dy &&
                                                                      tp->y <= (obj->dy + obj->h)))
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
            }
        }
    }
}
void gui_icon_default_ctor(gui_icon_default_t *this, gui_obj_t *parent, const char *filename,
                           int16_t x,
                           int16_t y, int16_t w, int16_t h, int16_t id)
{
    this->base.ctor = gui_icon_ctor;
    this->base.ctor(&this->base, parent, filename, x, y, w, h, id);

    ((gui_obj_t *)this)->obj_update_att = icon_update_att_default;
    ((gui_obj_t *)this)->obj_prepare = icon_prepare_default;

}
static const unsigned char _aciconback[] =
{
    0x00, 0x00, 0x2, 0x0, 0x2, 0x00, 0x00, 0x00, 0x4B, 0x12, 0x4B, 0x12, 0x4B, 0x12, 0x4B, 0x12,
};
gui_icon_default_t *gui_icon_create_default(void *parent, const char *filename, int16_t x,
                                            int16_t y, int16_t w, int16_t h, int16_t id)
{
    gui_icon_default_t *this = gui_malloc(sizeof(gui_icon_default_t));
    memset(this, 0x00, sizeof(gui_icon_default_t));
    this->ctor = gui_icon_default_ctor;
    this->ctor(this, parent, filename, x, y, w, h, id);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    if (GET_BASE(parent)->type == ICONLIST)
    {
        this->back_img = gui_img_create_from_mem(this, "back_img", (void *)_aciconback, 0, 0, 0, 0);
//        this->back_img = gui_img_create_from_fs(this, "back_img", "icon_demo/iconback.bin", 0, 0, 0, 0);

        struct gui_dispdev *dc = gui_get_dc();
        this->base.base.x = 40 * dc->scale_x;
    }
    GET_BASE(this)->create_done = true;
    return this;
}













