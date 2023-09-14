/*
 * File      : gui_dynamic.c
 * This file is part of GUI Widget
 */
#include <guidef.h>
#include <gui_dynamic_img.h>
#include <string.h>
//#include <gui_matrix.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include "acc_engine.h"

static void dynamic_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &img->draw_img;

    struct gui_rgb_data_head head;
    memcpy(&head, draw_img->data, sizeof(head));
    draw_img->img_w = head.w;
    draw_img->img_h = head.h;
    obj->w = draw_img->img_w;
    obj->h = draw_img->img_h;
    magic_img_get_new_area(obj, img, dc);

    gui_dynamic_img_t *this = (gui_dynamic_img_t *)obj;
    gui_img_t *root = (gui_img_t *)obj;

    GUI_UNUSED(root);

    uint32_t cur_time_ms = gui_ms_get();
    uint8_t index = (cur_time_ms - this->init_time_ms) / this->interval_time_ms;
    if ((cur_time_ms < (this->init_time_ms + this->duration_time_ms)) && this->status)
    {
        void **addr = this->addr_entry;
        draw_img->data = addr[index % this->total_cnt];
    }

    if (tp->type == TOUCH_SHORT)
    {
        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
    }
}

static void dynamic_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();
    draw_img_t *draw_img = &img->draw_img;

    rtgui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->dx;
    draw_rect.y1 = obj->dy;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;
    if (gui_get_acc() != NULL)
    {
        gui_get_acc()->blit(draw_img, dc, &draw_rect);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}

static void dynamic_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
}

void gui_dynamic_from_mem_ctor(gui_dynamic_img_t *this, gui_obj_t *parent, const char *name,
                               void *addr_entry,
                               int16_t x, int16_t y, uint8_t total_cnt,
                               uint32_t interval_time_ms, uint32_t duration_time_ms)
{

    void **addr = addr_entry;
    //for base class
    gui_magic_img_t *base = (gui_magic_img_t *)this;
    gui_magic_img_from_mem_ctor(base, (gui_obj_t *)parent, name, addr[0], x, y, 0, 0);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = IMAGE_FROM_MEM;
    root->obj_prepare = dynamic_prepare;
    root->obj_draw = dynamic_draw_cb;
    root->obj_end = dynamic_end;

    //for self
    this->addr_entry = addr_entry;
    this->total_cnt = total_cnt;
    this->interval_time_ms = interval_time_ms;
    this->duration_time_ms = duration_time_ms;
    this->init_time_ms = gui_ms_get();
    this->status = true;
    gui_log("gui tick get = %d \n", this->init_time_ms);


}

gui_dynamic_img_t *gui_dynamic_create_from_mem(gui_obj_t *parent, const char *name,
                                               void *addr_entry, int16_t x, int16_t y,  uint8_t total_cnt, uint32_t interval_time_ms,
                                               uint32_t duration_time_ms)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DYNAMIC_IMG";
    }
    gui_dynamic_img_t *dynamic = gui_malloc(sizeof(gui_dynamic_img_t));
    GUI_ASSERT(dynamic != NULL);
    memset(dynamic, 0x00, sizeof(gui_dynamic_img_t));
    gui_dynamic_from_mem_ctor(dynamic, (gui_obj_t *)parent, name, addr_entry, x, y, total_cnt,
                              interval_time_ms,
                              duration_time_ms);
    gui_list_init(&(GET_BASE(dynamic)->child_list));
    if ((GET_BASE(dynamic)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(dynamic)->parent)->child_list),
                               &(GET_BASE(dynamic)->brother_list));
    }
    GET_BASE(dynamic)->create_done = true;
    return dynamic;
}
void gui_dynamic_img_stop(gui_dynamic_img_t *this)
{
    this->status = false;
}
void gui_dynamic_img_start(gui_dynamic_img_t *this)
{
    this->status = true;
}
void gui_dynamic_img_restart(gui_dynamic_img_t *this)
{
    this->init_time_ms = gui_ms_get();
    this->status = true;
}
