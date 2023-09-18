/*
 * File      : gui_cardview.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "gui_cardview.h"


static void cardview_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_cardview_t *this = (gui_cardview_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
    switch (tp->type)
    {
    case TOUCH_HOLD_Y:
        obj->dy += tp->deltaY;
        this->release_y = obj->dy;
        break;
    case TOUCH_DOWN_SLIDE:
        gui_log("TOUCH_DOWN_SLIDE\n");
        if (this->tab_cnt_down == 0 && this->cur_id.y == 0)
        {
            break;
        }

        if (this->cur_id.y == this->tab_cnt_down)
        {
            return;
        }
        this->cur_id.y = this->cur_id.y + 1;
        this->release_y = this->release_y  + dc->screen_width;
        break;
    case TOUCH_UP_SLIDE:
        gui_log("TOUCH_UP_SLIDE\n");
        if (this->tab_cnt_up == 0 && this->cur_id.y == 0)
        {
            break;
        }

        if (this->cur_id.y == this->tab_cnt_up)
        {
            return;
        }
        this->cur_id.y = this->cur_id.y - 1;
        this->release_y = this->release_y  - dc->screen_width;
        break;
    default:
        break;
    }

    if (this->release_y > 0)
    {
        this->release_y--;
    }
    if (this->release_y < 0)
    {
        this->release_y++;
    }
    obj->dy = this->release_y;

}

static void cardview_draw_cb(gui_obj_t *obj)
{
    gui_cardview_t *this = (gui_cardview_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    GUI_UNUSED(this);
    GUI_UNUSED(dc);


}
static void cardview_end(gui_obj_t *obj)
{

}
static void cardview_destory(gui_obj_t *obj)
{

}


void gui_cardview_set_style(gui_cardview_t *this, SLIDE_STYLE style)
{
    this->style = style;
}

gui_cardview_t *gui_cardview_create(void *parent,  const char *name,
                                    int16_t x, int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "cardview";
    }
    gui_cardview_t *this = gui_malloc(sizeof(gui_cardview_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_cardview_t));


    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = CARDVIEW;
    root->obj_prepare = cardview_prepare;
    root->obj_draw = cardview_draw_cb;
    root->obj_end = cardview_end;
    root->obj_destory = cardview_destory;

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



