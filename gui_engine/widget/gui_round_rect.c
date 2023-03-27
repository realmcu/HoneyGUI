#include "gui_round_rect.h"
static void (rount_rect_set)(gui_round_rect_t *this, int16_t x, int16_t y, int16_t w, int16_t h,
                             uint32_t color)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;
    if (w * h == this->base_canvas.base.base.base.w * this->base_canvas.base.base.base.w)
    {
        this->base_canvas.erase((void *)this);
        this->base_canvas.base.color = color;
        this->base_canvas.base.base.base.w = w;
        this->base_canvas.base.base.base.h = h;
        this->base_canvas.base.base.base.x = x;
        this->base_canvas.base.base.base.y = y;
        gui_img_file_head_t *head = (gui_img_file_head_t *)(draw_img->data);
        head->size.img_size.img_w = w;
        head->size.img_size.img_h = h;
        this->base_canvas.draw_round_rect((void *)this, x, y, w, h);
    }
    else
    {
        this->base_canvas.erase((void *)this);
        this->base_canvas.base.color = color;
        this->base_canvas.base.base.base.w = w;
        this->base_canvas.base.base.base.h = h;
        this->base_canvas.base.base.base.x = x;
        this->base_canvas.base.base.base.y = y;
        gui_free(draw_img->data);
        draw_img->data = gui_malloc(h * w / 8 + 1 + sizeof(gui_img_file_head_t));
        memset(draw_img->data, 0, h * w / 8 + 1 + sizeof(gui_img_file_head_t));
        gui_img_file_head_t *head = (gui_img_file_head_t *)(draw_img->data);
        head->type = 5;
        head->size.img_size.img_w = w;
        head->size.img_size.img_h = h;
        this->base_canvas.draw_round_rect((void *)this, x, y, w, h);
    }
}
static void (rount_rect_ctor)(gui_round_rect_t *this, gui_obj_t *parent, const char *name,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h, uint32_t color)
{
    this->base_canvas.ctor = gui_canvas_ctor;
    this->base_canvas.ctor((void *)this, parent, name, x, y, w, h, color);
    this->base_canvas.draw_round_rect((void *)this, 0, 0, w, h);
    this->set = rount_rect_set;
}
gui_round_rect_t *gui_rount_rect_create(void *parent, const char *name, int16_t x, int16_t y,
                                        int16_t w, int16_t h, uint32_t color)
{
#define _rount_rect_param this, parent, name, x, y, w, h, color
    GUI_NEW(gui_round_rect_t, rount_rect_ctor, _rount_rect_param)
}

