#include "gui_bitmap.h"
#include "gui_rectangle.h"
static void set_rectangle(gui_rectangle_t *this, int16_t x, int16_t y, int16_t w, int16_t h,
                          uint32_t color)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;
    if (draw_img->data)
    {
        gui_free(draw_img->data);
    }
    uint8_t *bitmap = gui_malloc(h * w / 8 + 1 + sizeof(gui_img_file_head_t));
    memset(bitmap, 0, h * w / 8 + 1 + sizeof(gui_img_file_head_t));
    gui_img_file_head_t *head = (gui_img_file_head_t *)bitmap;
    head->type = 5;
    head->size.img_size.img_w = w;
    head->size.img_size.img_h = h;
    memset(bitmap + sizeof(gui_img_file_head_t), 255, h * w / 8 + 1);
    gui_img_set_attribute(&(this->base.base), this->base.base.base.name, bitmap, x, y);
    this->base.color = color;
}
void gui_rectangle_ctor(gui_rectangle_t *this, gui_obj_t *parent, const char *name,
                        int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    this->base.ctor = gui_bitmap_ctor;
    this->base.ctor(&(this->base), parent, name, NULL, x, y, w, h, color);
    this->set = set_rectangle;
    uint8_t *bitmap = gui_malloc(h * w / 8 + 1 + sizeof(gui_img_file_head_t));
    memset(bitmap, 0, sizeof(gui_img_file_head_t));
    gui_img_file_head_t *head = (gui_img_file_head_t *)bitmap;
    head->type = 5;
    head->size.img_size.img_w = w;
    head->size.img_size.img_h = h;
    uint8_t *raw_bitmap = bitmap + sizeof(gui_img_file_head_t);
    memset(raw_bitmap, 255, h * w / 8 + 1);
    draw_img->data = bitmap;
    this->base.base.base.type = RECTANGLE;
}
gui_rectangle_t *gui_rectangle_create(void *parent, const char *name, int16_t x, int16_t y,
                                      int16_t w, int16_t h, uint32_t color)
{
#define _GUI_NEW_param_gui_rectangle_create_ this, parent, name, x, y, w, h, color
    GUI_NEW(gui_rectangle_t, gui_rectangle_ctor, _GUI_NEW_param_gui_rectangle_create_)
}
