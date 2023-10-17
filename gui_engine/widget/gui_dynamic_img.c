/**
 * @file gui_dynamic_img.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <guidef.h>
#include <gui_dynamic_img.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include "acc_engine.h"

static void dynamic_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    gui_dynamic_img_t *this = (gui_dynamic_img_t *)obj;
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &img->draw_img;

    struct gui_rgb_data_head head;
    memcpy(&head, draw_img->data, sizeof(head));
    draw_img->img_w = head.w;
    draw_img->img_h = head.h;
    obj->w = draw_img->img_w;
    obj->h = draw_img->img_h;

    matrix_identity(draw_img->matrix);
    matrix_translate(root->dx, root->dy, draw_img->matrix);
    matrix_translate(root->tx, root->ty, draw_img->matrix);


    matrix_translate(dc->screen_width / 2, dc->screen_height / 2, draw_img->matrix);
    matrix_scale(root->sx, root->sy, draw_img->matrix);
    matrix_translate(-dc->screen_width / 2, -dc->screen_height / 2, draw_img->matrix);
    matrix_translate(root->ax, root->ay, draw_img->matrix);

    matrix_translate(img->t_x, img->t_y, draw_img->matrix);
    matrix_rotate(img->degrees, draw_img->matrix);
    matrix_scale(img->scale_x, img->scale_y, draw_img->matrix);
    matrix_translate(-img->c_x, -img->c_y, draw_img->matrix);

    memcpy(draw_img->inverse, draw_img->matrix, sizeof(struct rtgui_matrix));
    matrix_inverse(draw_img->inverse);
    rtgui_image_new_area(draw_img);


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
    draw_rect.x1 = obj->ax;
    draw_rect.y1 = obj->ay;
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

static void gui_dynamic_img_mem_ctor(gui_img_t *this, gui_obj_t *parent, const char *name,
                                     void *addr,
                                     int16_t x,
                                     int16_t y, int16_t w, int16_t h)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)this;
    draw_img_t *draw_img = &this->draw_img;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_prepare = dynamic_prepare;
    root->obj_draw = dynamic_draw_cb;
    root->obj_end = dynamic_end;
    root->obj_destory = NULL;
    root->type = IMAGE_FROM_MEM;

    draw_img->blend_mode = IMG_FILTER_BLACK;
    draw_img->data = addr;
    draw_img->opacity_value = 255;
    draw_img->blend_mode = IMG_MAGIC_MATRIX;
    draw_img->matrix = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->inverse = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->opacity_value = UINT8_MAX;

    this->scale_x = 1.0f;
    this->scale_y = 1.0f;

}

void gui_dynamic_from_mem_ctor(gui_dynamic_img_t *this, gui_obj_t *parent, const char *name,
                               void *addr_entry,
                               int16_t x, int16_t y, uint8_t total_cnt,
                               uint32_t interval_time_ms, uint32_t duration_time_ms)
{

    void **addr = addr_entry;
    //for base class
    gui_img_t *base = (gui_img_t *)this;
    gui_dynamic_img_mem_ctor(base, (gui_obj_t *)parent, name, addr[0], x, y, 0, 0);

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
