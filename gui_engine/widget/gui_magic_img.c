/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <gui_magic_img.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include "acc_engine.h"

void magic_img_get_new_area(gui_obj_t *obj, gui_img_t *img, struct gui_dispdev *dc)
{
    gui_img_t *base = (gui_img_t *)obj;
    GUI_UNUSED(base);
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_magic_img_t *this = (gui_magic_img_t *)obj;
    draw_img_t *draw_img = &img->draw_img;

    matrix_identity(draw_img->matrix);
    matrix_translate(root->dx, root->dy, draw_img->matrix);

    matrix_translate(this->c_x, this->c_y, draw_img->matrix);
    matrix_rotate(this->degrees, draw_img->matrix);
    matrix_translate(-this->c_x, -this->c_y, draw_img->matrix);
    matrix_scale(this->scale_x * this->p_x, this->scale_y * this ->p_y, draw_img->matrix);
    matrix_translate(this->t_x, this->t_y, draw_img->matrix);
    memcpy(draw_img->inverse, draw_img->matrix, sizeof(struct rtgui_matrix));
    matrix_inverse(draw_img->inverse);

    struct rtgui_pox pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];


    pox.p[0] = (float)root->w;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }


    pox.p[0] = 0.0f;
    pox.p[1] = (float)obj->h;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (float)obj->w;
    pox.p[1] = (float)obj->h;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    obj->dx = (int16_t)x_min;
    obj->dy = (int16_t)y_min;
    obj->w = (int16_t)x_max - (int16_t)x_min;
    obj->h = (int16_t)y_max - (int16_t)y_min;
    draw_img->target_w = obj->w;
    draw_img->target_h = obj->h;
}

static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &img->draw_img;

    rtgui_image_load_scale(draw_img);
    obj->w = draw_img->img_w;
    obj->h = draw_img->img_h;
    magic_img_get_new_area(obj, img, dc);

    if (tp->type == TOUCH_SHORT)
    {
        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
    }
}
static void img_draw_cb(gui_obj_t *obj)
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
static void img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

}


void gui_img_rotation(gui_magic_img_t *img, float degrees, float c_x, float c_y)
{
    GUI_ASSERT(img != NULL);

    img->degrees = degrees;
    img->c_x = c_x;
    img->c_y = c_y;
}

void gui_img_scale(gui_magic_img_t *img, float scale_x, float scale_y)
{
    GUI_ASSERT(img != NULL);
    img->scale_x = scale_x;
    img->scale_y = scale_y;
}
void gui_img_scale_add(gui_magic_img_t *img, float scale_x, float scale_y)
{
    GUI_ASSERT(img != NULL);
    img->p_x = scale_x;
    img->p_y = scale_y;
}
void gui_img_translate(gui_magic_img_t *img, float t_x, float t_y)
{
    GUI_ASSERT(img != NULL);
    img->t_x = t_x;
    img->t_y = t_y;
}
static void magic_img_update(gui_obj_t *o)
{
    gui_magic_img_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * 40 / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));
    }
}

void gui_magicimage_set_animate(void *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_magic_img_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_magic_img_t *)o)->animate = animate;
}


void gui_magic_img_from_mem_ctor(gui_magic_img_t *this, gui_obj_t *parent, const char *name,
                                 void *addr,
                                 int16_t x,
                                 int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_img_t *base = (gui_img_t *)this;
    draw_img_t *draw_img = &base->draw_img;
    gui_img_from_mem_ctor(base, parent, name, addr, x, y, w, h);



    draw_img->data = addr;
    draw_img->blend_mode = IMG_MAGIC_MATRIX;

    //for root class
    gui_obj_t *obj = (gui_obj_t *)this;
    obj->type = IMAGE_FROM_MEM;
    obj->obj_prepare = img_prepare;
    obj->obj_draw = img_draw_cb;
    obj->obj_end = img_end;
    obj->obj_update_att = magic_img_update;

    //for self
    this->scale_x = 1.0f;
    this->scale_y = 1.0f;
    this->p_x = 1.0f;
    this->p_y = 1.0f;
    draw_img->matrix = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->base.draw_img.opacity_value = UINT8_MAX;

}
void gui_img_set_opacity(gui_magic_img_t *this, unsigned char opacity_value)
{
    this->base.draw_img.opacity_value = opacity_value;
}
gui_magic_img_t *gui_magic_img_create_from_mem(void *parent,  const char *name, void *addr,
                                               int16_t x,
                                               int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "MAGIC_IMG";
    }
    gui_magic_img_t *img = gui_malloc(sizeof(gui_magic_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_magic_img_t));

    gui_magic_img_from_mem_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}


#if 1
#include "acc_engine.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
static void svg_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_magic_img_t *img = (void *)obj;
    rtgui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->dx;
    draw_rect.y1 = obj->dy;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;
    int x = obj->dx + img->t_x;
    int y = obj->dy + img->t_y;
    float scale = img->scale_x;
    uint32_t svg_len = (uint32_t)img->base.draw_img.engine;
    NSVGshape *shape;
    NSVGimage *image = img->base.draw_img.data;
    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        shape->opacity = ((float)(int)(img->base.draw_img.opacity_value)) / 255.0f;
    }
    gui_get_acc()->draw_svg(img->base.draw_img.data, svg_len, gui_get_dc(), x + img->t_x, y + img->t_y,
                            scale, img->degrees, img->c_x, img->c_y);

}
static void svg_ctor(gui_magic_img_t *this, gui_obj_t *parent,
                     void *addr,
                     int16_t x,
                     int16_t y, int16_t w, int16_t h, uint32_t data_size)
{
    //for base class
    gui_img_t *base = (gui_img_t *)this;
    draw_img_t *draw_img = &base->draw_img;
    gui_img_from_mem_ctor(base, parent, "evo", addr, x, y, w, h);


    draw_img->engine = (void *)data_size;


    //for root class
    gui_obj_t *obj = (gui_obj_t *)this;
    obj->type = IMAGE_FROM_MEM;
    obj->obj_draw = svg_draw_cb;

    obj->obj_prepare = NULL;
    obj->obj_end = NULL;
    //for self
    this->scale_x = 1.0f;
    this->scale_y = 1.0f;
    this->base.draw_img.opacity_value = UINT8_MAX;


    NSVGimage *image = NULL;
    char *data = NULL;
    uint32_t size = data_size;
    data = (char *)gui_malloc(size + 1);
    if (data == NULL) {gui_log("Could not malloc SVG image.\n"); goto error;}
    memcpy(data, addr, size);
    data[size] = '\0';  // Must be null terminated.
    image = nsvgParse(data, "px", 96.0f);
    gui_free(data);
    if (image == NULL)
    {
        gui_log("Could not open SVG image.\n");
        goto error;
    }
    draw_img->data = image;

    if (!w)
    {
        this->scale_x = 1;
    }
    else
    {
        this->scale_x = ((float)w) / image->width;
    }

    if (!h)
    {
        this->scale_y = 1;
    }
    else
    {
        this->scale_y = ((float)h) / image->height;
    }
error:
    //obj->not_show = true;
    return;
}
gui_magic_img_t *gui_svg_create_from_mem(void *parent, void *data, uint32_t data_size,
                                         int16_t x, int16_t y, int16_t w, int16_t h)
{
#define _param_ctor_gui_svg_create_from_mem_  this, parent, data, x,y,w,h,data_size
    GUI_NEW(gui_magic_img_t, svg_ctor, _param_ctor_gui_svg_create_from_mem_)
}
#endif

