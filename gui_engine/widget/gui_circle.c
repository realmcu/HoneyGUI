#include "gui_bitmap.h"
#include "gui_circle.h"
#include "math.h"

static bool judge_point_in_circle(gui_circleshape_t *circle, gui_point_t *point)
{
    float center_to_point;
    center_to_point = sqrtf((circle->center.x - point->x) * (circle->center.x - point->x) +
                            (circle->center.y - point->y) * (circle->center.y - point->y));
    if ((center_to_point + 0.4f) < (circle->radius))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void gui_set_circle(gui_circle_t *this, int16_t center_x, int16_t center_y, int16_t radius,
                           uint32_t color)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    if (draw_img->data)
    {
        gui_free(draw_img->data);
    }
    this->base.base.base.x = (int16_t)(center_x - radius);
    this->base.base.base.y = (int16_t)(center_y - radius);
    this->base.base.base.w = radius * 2;
    this->base.base.base.h = radius * 2;
    this->base.color =  color;
    this->circle.center.x = (float)center_x;
    this->circle.center.y = (float)center_y;
    this->circle.radius = (float)radius;
    this->base.color = color;
    uint8_t *bitmap = gui_malloc(this->base.base.base.h * this->base.base.base.w / 8 + 1 + sizeof(
                                     gui_img_file_head_t));
    memset(bitmap, 0, this->base.base.base.h * this->base.base.base.w / 8 + 1 + sizeof(
               gui_img_file_head_t));
    gui_img_file_head_t *head = (gui_img_file_head_t *)bitmap;
    head->type = 5;
    head->size.img_size.img_h = this->base.base.base.h;
    head->size.img_size.img_w = this->base.base.base.w;

    uint8_t *raw_bitmap = bitmap + sizeof(gui_img_file_head_t);
    size_t bit_length = 0;
    for (int i = this->base.base.base.y; i < this->base.base.base.h + this->base.base.base.y; i++)
    {
        for (int j = this->base.base.base.x; j < this->base.base.base.w + this->base.base.base.x; j++)
        {
            gui_point_t point = {.x = (float)j, .y = (float)i};
            if (judge_point_in_circle(&(this->circle), &point))
            {
                raw_bitmap[bit_length / 8] |= (1 << bit_length % 8);
            }
            bit_length++;
        }
    }
    draw_img->data = bitmap;
    gui_img_set_attribute((gui_img_t *)this, this->base.base.base.name, draw_img->data,
                          center_x - (int16_t)(this->circle.radius), center_y - (int16_t)(this->circle.radius));
    this->circle.center.x = (float)center_x;
    this->circle.center.y = (float)center_y;
}
void gui_circle_ctor(gui_circle_t *this, gui_obj_t *parent, const char *name, int16_t center_x,
                     int16_t center_y, int16_t radius, uint32_t color)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;
    this->base.ctor = gui_bitmap_ctor;
    this->base.ctor(&(this->base), parent, name, NULL, (int16_t)(center_x - radius),
                    (int16_t)(center_y - radius), radius * 2, radius * 2, color);
    this->set = gui_set_circle;
    this->circle.center.x = (float)center_x;
    this->circle.center.y = (float)center_y;
    this->circle.radius = (float)radius;
    uint8_t *bitmap = gui_malloc(this->base.base.base.h * this->base.base.base.w / 8 + 1 + sizeof(
                                     gui_img_file_head_t));
    memset(bitmap, 0, this->base.base.base.h * this->base.base.base.w / 8 + 1 + sizeof(
               gui_img_file_head_t));
    gui_img_file_head_t *head = (gui_img_file_head_t *)bitmap;
    head->type = 5;
    head->size.img_size.img_h = this->base.base.base.h;
    head->size.img_size.img_w = this->base.base.base.w;

    uint8_t *raw_bitmap = bitmap + sizeof(gui_img_file_head_t);
    size_t bit_length = 0;
    for (int i = this->base.base.base.y; i < this->base.base.base.h + this->base.base.base.y; i++)
    {
        for (int j = this->base.base.base.x; j < this->base.base.base.w + this->base.base.base.x; j++)
        {
            gui_point_t point = {.x = (float)j, .y = (float)i};
            if (judge_point_in_circle(&(this->circle), &point))
            {
                raw_bitmap[bit_length / 8] |= (1 << bit_length % 8);
            }
            bit_length++;
        }
    }
    draw_img->data = bitmap;
}

gui_circle_t *gui_circle_create(void *parent, const char *name, int16_t center_x, int16_t center_y,
                                uint16_t radius, uint32_t color)
{
#define _GUI_NEW_param_gui_circle_createe_ this, parent, name, center_x, center_y, (int16_t)radius, color
    GUI_NEW(gui_circle_t, gui_circle_ctor, _GUI_NEW_param_gui_circle_createe_)
}
