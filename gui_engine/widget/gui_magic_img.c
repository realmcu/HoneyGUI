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

void magic_img_get_new_area(gui_obj_t *obj, struct gui_dispdev *dc)
{
    gui_img_t *base = (gui_img_t *)obj;
    GUI_UNUSED(base);
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_magic_img_t *this = (gui_magic_img_t *)obj;
    draw_img_t *draw_img = &base->draw_img;

    matrix_identity(draw_img->matrix);
    matrix_translate(root->dx, root->dy, draw_img->matrix);
    matrix_translate(root->tx, root->ty, draw_img->matrix);


    matrix_translate(dc->screen_width / 2, dc->screen_height / 2, draw_img->matrix);
    matrix_scale(root->sx, root->sy, draw_img->matrix);
    matrix_translate(-dc->screen_width / 2, -dc->screen_height / 2, draw_img->matrix);
    matrix_translate(root->ax, root->ay, draw_img->matrix);

    matrix_translate(this->t_x, this->t_y, draw_img->matrix);
    matrix_rotate(this->degrees, draw_img->matrix);
    matrix_scale(this->scale_x, this->scale_y, draw_img->matrix);

    matrix_translate(-this->c_x, -this->c_y, draw_img->matrix);



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


    pox.p[0] = (float)draw_img->img_w;
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
    pox.p[1] = (float)draw_img->img_h;
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

    pox.p[0] = (float)draw_img->img_w;
    pox.p[1] = (float)draw_img->img_h;
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

    draw_img->img_x = (int16_t)x_min;
    draw_img->img_y = (int16_t)y_min;
    draw_img->target_w = (int16_t)x_max - (int16_t)x_min;
    draw_img->target_h = (int16_t)y_max - (int16_t)y_min;

}

static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &img->draw_img;

    rtgui_image_load_scale(draw_img);
    magic_img_get_new_area(obj, dc);

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
    draw_rect.x1 = draw_img->img_x;
    draw_rect.y1 = draw_img->img_y;
    draw_rect.x2 = draw_rect.x1 + draw_img->target_w;
    draw_rect.y2 = draw_rect.y1 + draw_img->target_h;
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
    if (scale_x > 0 && scale_y > 0)
    {
        img->scale_x = scale_x;
        img->scale_y = scale_y;
    }
}

void gui_img_translate(gui_magic_img_t *img, float t_x, float t_y)
{
    GUI_ASSERT(img != NULL);
    img->t_x = t_x;
    img->t_y = t_y;
}

// Skews the current coordinate system along X axis. Angle is specified in radians.
void gui_img_skew_x(gui_magic_img_t *img, float degrees)
{

}

// Skews the current coordinate system along Y axis. Angle is specified in radians.
void gui_img_skew_y(gui_magic_img_t *img, float degrees)
{

}

void gui_img_set_opacity(gui_magic_img_t *this, unsigned char opacity_value)
{
    this->base.draw_img.opacity_value = opacity_value;
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
    gui_dispdev_t *dc = gui_get_dc();

    //for root class
    gui_obj_t *obj = (gui_obj_t *)this;
    obj->type = IMAGE_FROM_MEM;
    if ((w == 0) || (h == 0))
    {
        obj->w = dc->screen_width;
        obj->h = dc->screen_height;
    }
    else
    {
        obj->w = w;
        obj->h = h;
    }


    obj->obj_prepare = img_prepare;
    obj->obj_draw = img_draw_cb;
    obj->obj_end = img_end;
    obj->obj_update_att = magic_img_update;

    //for self
    this->scale_x = 1.0f;
    this->scale_y = 1.0f;

    draw_img->matrix = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->base.draw_img.opacity_value = UINT8_MAX;

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



void *gui_get_file_address(const char *file)
{
    if (file == NULL)
    {
        return NULL;
    }
#if defined(_WIN32)
    {
        extern char *defaultPath;
        char *path = gui_malloc(strlen(file) + strlen(defaultPath) + 1);
        sprintf(path, "%s%s", defaultPath, file);
        int fd = gui_fs_open(path,  0);
        gui_free(path);
        if (fd == -1)
        {
            return NULL;
        }

        int size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
        void *imgbuf = gui_malloc(size);
        memset(imgbuf, 0, size);
        gui_fs_read(fd, imgbuf, size);
        return imgbuf;
    }
#else
    {
        char *path = gui_malloc(strlen(file) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, file);
        int fd = gui_fs_open(path,  0);
        gui_free(path);
        if (fd == -1)
        {
            return NULL;
        }
        void *rst;
        rst = (void *)gui_fs_ioctl(fd, 0);
        gui_fs_close(fd);
        gui_log("rst:%p\n", rst);
        return rst;

    }
#endif
}
gui_magic_img_t *gui_img_create_from_fs(void *parent, const char *file, int16_t x, int16_t y)
{
    return gui_magic_img_create_from_mem(parent, "image", gui_get_file_address(file), x, y, 0, 0);
}
