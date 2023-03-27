/*
 * File      : gui_page.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_page.h>
#include <string.h>
#include <gui_server.h>
#include <gui_img.h>
#include "gui_obj.h"
#include <gui_iconlist.h>
#include <tp_algo.h>


void page_update(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
    {
        if ((tp->x > ((gui_page_t *)obj)->start_x) && (tp->x < ((gui_page_t *)obj)->start_x + obj->w))
        {
            if ((tp->y > ((gui_page_t *)obj)->start_y) && (tp->y < ((gui_page_t *)obj)->start_y + obj->h))
            {
                if ((tp->type == TOUCH_HOLD_Y))
                {
                    obj->y = tp->deltaY + ((gui_page_t *)obj)->yold;
                }
                else
                {
                    if (obj->y > ((gui_page_t *)obj)->start_y)
                    {
                        obj->y = ((gui_page_t *)obj)->start_y;
                        gui_app_exec(gui_current_app());
                    }
                    ((gui_page_t *)obj)->yold = obj->y;
                }
            }
        }
        if (((gui_page_t *)obj)->scroll_bar)
        {
            ((gui_page_t *)obj)->scroll_bar->base.y = ((((gui_page_t *)obj)->start_y - obj->y) *
                                                       gui_get_screen_height() / obj->h);
        }
    }
}
struct page_icon_cb_p
{
    gui_pagebar_t *bar;
    uint32_t current_page;
};

void gui_page_add_scroll_bar(gui_page_t *this, void *bar_pic)
{
    this->scroll_bar = gui_img_create_from_mem(this->base.parent, "scroll_bar", bar_pic, 0, 0, 0, 0);
    rtgui_image_load_scale(&this->scroll_bar->draw_img);
    this->scroll_bar->base.x = this->base.w - 3 - this->scroll_bar->base.w;
}

//static void page_icon_cb(struct page_icon_cb_p *p)
//{
//    gui_pagebar_t *bar = p->bar;
//    bar->current_page = p->current_page;
//    //gui_free(p);
//    gui_app_exec(gui_current_app());
//}
void gui_page_ctor(gui_page_t *this, gui_obj_t *parent, const char *name, int16_t x,
                   int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    gui_obj_t *root = (gui_obj_t *)this;
    root->type = PAGE;
    root->obj_update_att = page_update;

    // if (parent->type == PAGEBAR)
    // {
    //     gui_pagebar_t *bar = (gui_pagebar_t *)parent;
    //     struct page_icon_cb_p *p = gui_malloc(sizeof(struct page_icon_cb_p));
    //     p->bar = (void *)parent;
    //     p->current_page = bar->page_count;
    //     /// gui_icon_t *ic = gui_icon_create_theme4(bar->ic, "_page_ic", 0,0,65,65, bar->page_count++, "app/system/resource/switchOn.bin");
    //     // GUI_ASSERT(NULL != NULL);(ic, page_icon_cb, SHORT_TOUCH_CB, p);
    //     this->base.x = bar->page_x;
    //     this->base.y = bar->page_y;
    //     this->start_x = bar->page_x;
    //     this->start_y = bar->page_y;
    // }
}

gui_page_t *gui_page_create(void *parent, const char *name, int16_t x, int16_t y,
                            int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_PAGE";
    }
    gui_page_t *this = gui_malloc(sizeof(gui_page_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0, sizeof(gui_page_t));

    gui_page_ctor(this, parent, name, x, y, w, h);
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = 1;
    return this;
}
