/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_arc.h
  * @brief image canvas rect widget header file
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2024/05/10
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include "gui_canvas_arc.h"
#include <string.h>
#include <gui_obj.h>
#include "acc_init.h"
#include <tp_algo.h>
#include "nanovg.h"
#include "math.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                 uint32_t h,
                                 uint32_t stride,
                                 enum     NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

static void set_arc_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = w;
    head->h = h;
}


static void set_arc_img(gui_canvas_arc_t *this, void *data, draw_img_t **input_img,
                        float rotate_degree, float image_angle, int img_w, int img_h)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = UINT8_MAX;
    img->high_quality = 1;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(this->r + this->stroke_width / 2, this->r + this->stroke_width / 2, &img->matrix);
    matrix_rotate(rotate_degree + 90, &img->matrix);
    matrix_translate(-(this->r + this->stroke_width / 2 + 1), -img_h / 2, &img->matrix);



    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));



    matrix_inverse(&img->inverse);
    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}

static void set_circle_img(gui_canvas_arc_t *this, void *data, draw_img_t **input_img,
                           bool start, int img_w, int img_h)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = UINT8_MAX;
    img->high_quality = 1;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    if (start)
    {
        float start_x = this->cx + fix_sin((int)this->from) * this->r;
        float start_y = this->cy - fix_cos((int)this->from) * this->r;
        matrix_translate(-(this->cx - this->r - this->stroke_width / 2) + start_x - img_w / 2,
                         -(this->cy - this->r - this->stroke_width / 2) + start_y - img_h / 2, &img->matrix);
    }
    else
    {
        float start_x = this->cx + fix_sin((int)this->to) * this->r;
        float start_y = this->cy - fix_cos((int)this->to) * this->r;
        matrix_translate(-(this->cx - this->r - this->stroke_width / 2) + start_x - img_w / 2,
                         -(this->cy - this->r - this->stroke_width / 2) + start_y - img_h / 2, &img->matrix);
    }



    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));



    matrix_inverse(&img->inverse);
    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}



static void gui_canvas_circle_prepare(gui_canvas_arc_t *this)
{
    int radius = this->stroke_width / 2;
    int img_w = radius * 2;
    int img_h = this->stroke_width + 2;
    this->circle_data = gui_malloc(img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));

    memset(this->circle_data, 0x00, img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));
    uint32_t *data = (uint32_t *)(this->circle_data + sizeof(gui_rgb_data_head_t));
    float boundary[img_h / 2];
    for (int i = 1; i <= radius; i++)
    {
        float y = i - 0.5 - radius;
        boundary[i - 1] = radius - sqrtf(radius * radius - y * y);
    }
    for (int i = 1; i <= radius; i++)
    {
        int k = i - 1;
        uint16_t right = (int)boundary[k];
        uint32_t offset = img_w * i;
        if (right < img_w)
        {
            for (int j = right + 1; j < img_w - right - 1; j++)
            {
                data[offset + j] = this->color.color.argb_full;
            }
        }
        float portion = ceil(boundary[k]) - boundary[k];
        gui_color_t color = this->color;
        color.color.rgba.a = round(portion * color.color.rgba.a);
        data[offset + right] = color.color.argb_full;
        data[offset + img_w - right - 1] = color.color.argb_full;
        if (k > radius / 2 - 1)
        {
            data[(right + 1) * img_w + k] = color.color.argb_full;
            data[(right + 2) * img_w - k - 1] = color.color.argb_full;
        }
    }
    for (int i = 1; i <= radius; i++)
    {
        uint32_t *src = data + i * img_w;
        uint32_t *dst = data + (img_h - i - 1) * img_w;
        memcpy(dst, src, img_w * sizeof(uint32_t));
    }

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

    set_arc_w_and_h(head, img_w, img_h);

    set_circle_img(this, this->circle_data, &this->circle_img_01, true, img_w, img_h);
    set_circle_img(this, this->circle_data, &this->circle_img_02, false, img_w, img_h);
}


static void gui_canvas_arc_prepare(gui_canvas_arc_t *this)
{
    float degree = this->to - this->from;

    if (degree >= 30)
    {
        degree = 30;
    }

    float img_angle = ((degree + 2) / 2) / 180.0f * 3.1415926f;
    int img_h = ceil(2 * (this->r + this->stroke_width / 2) * sin(img_angle)) + 2;
    if (img_h % 2)
    {
        img_h += 1;
    }
    int img_w = ceil(this->stroke_width / 2.0f + this->r - (this->r - this->stroke_width / 2.0f) * cos(
                         img_angle)) + 2;
    degree = this->to - this->from;
    if (this->use_external_picture == false)
    {
        this->arc_data = gui_malloc(img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));

        int32_t h_draw = img_h / 2;
        int r2 = this->r + this->stroke_width / 2;
        int r1 = this->r - this->stroke_width / 2;
        float left_boundary[h_draw];
        float right_boundary[h_draw];
        float right_boundary_sub[h_draw];
        float line_y = 1 / tanf(img_angle);
        float line_c = r2 - line_y * h_draw;
        memset(left_boundary, 0, h_draw * sizeof(float));
        memset(right_boundary, 0, h_draw * sizeof(float));
        memset(right_boundary_sub, 0, h_draw * sizeof(float));
        bool exceed = false;
        for (int i = 1; i <= h_draw; i++)
        {
            int k = i - 1;
            float y = k + 0.5 - h_draw;
            left_boundary[k] = r2 - sqrtf(r2 * r2 - y * y);
            float right = r2 - sqrtf(r1 * r1 - y * y);
            if (!exceed)
            {
                float line_right = line_y * (k + 0.5) + line_c;
                if (line_right > right)
                {
                    exceed = true;
                }
                else
                {
                    right_boundary_sub[k] = line_y * (k + 1) + line_c;
                    right = line_right;
                }
            }
            right_boundary[k] = right;
        }
        memset(this->arc_data, 0x00, img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));
        uint32_t *data = (uint32_t *)(this->arc_data + sizeof(gui_rgb_data_head_t));
        for (int i = 1; i <= h_draw; i++)
        {
            int k = i - 1;
            int16_t left = (int)left_boundary[k];
            int16_t right = (int)right_boundary[k];
            uint32_t offset = img_w * i;
            if (left < right - 1)
            {
                for (int j = left + 1; j < right; j++)
                {
                    data[offset + j + 1] = this->color.color.argb_full;
                }
            }
            float portion = ceil(left_boundary[k]) - left_boundary[k];
            gui_color_t color = this->color;
            color.color.rgba.a = round(portion * color.color.rgba.a);
            data[offset + left + 1] = color.color.argb_full;
            portion = right_boundary[k] - (int)right_boundary[k];
            color.color.rgba.a = round(portion * this->color.color.rgba.a);
            uint8_t right_a = color.color.rgba.a;
            data[offset + right + 1] = color.color.argb_full;
            if (right_boundary_sub[k] > right + 1 && right_boundary_sub[k] < img_w + 1)
            {
                int right_sub = (int)right_boundary_sub[k];
                portion = (right_boundary_sub[k] - right_boundary[k]) / 4 * ((right_boundary_sub[k] - right_sub) /
                                                                             (right_boundary_sub[k] - right_boundary[k]));
                color.color.rgba.a = round(portion * this->color.color.rgba.a);
                data[offset + right + 1] = color.color.argb_full;
                if (right_boundary_sub[k] > right + 2)
                {
                    color.color.rgba.a = (uint8_t)((uint16_t)right_a + (uint16_t)color.color.rgba.a) / 2;
                    for (int j = right + 1; j < right_sub; j++)
                    {
                        data[offset + j + 1] = color.color.argb_full;
                    }
                }
            }
        }
        for (int i = 1; i <= h_draw; i++)
        {
            uint32_t *src = data + i * img_w;
            uint32_t *dst = data + (img_h - i - 1) * img_w;
            memcpy(dst, src, img_w * sizeof(uint32_t));
        }

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->arc_data;

        set_arc_w_and_h(head, img_w, img_h);
    }
    else
    {
        if (degree <= 10)
        {
            this->arc_data = this->data[0];
        }
        else if (degree <= 20)
        {
            this->arc_data = this->data[1];
        }
        else if (degree <= 30)
        {
            this->arc_data = this->data[2];
        }
        else if (degree <= 40)
        {
            this->arc_data = this->data[3];
        }
        else if (degree <= 50)
        {
            this->arc_data = this->data[4];
        }
        else
        {
            this->arc_data = this->data[5];
        }

    }

    int t = ceil(degree / 30);
    if (degree > 30)
    {
        for (int i = 0; i < t; i++)
        {
            if (i == t - 1)
            {
                set_arc_img(this, this->arc_data, &this->arc_img[i], this->from + degree - 15, img_angle, img_w,
                            img_h);
            }
            else
            {
                set_arc_img(this, this->arc_data, &this->arc_img[i], this->from + 15 + 30 * i, img_angle, img_w,
                            img_h);
            }
        }
    }
    else
    {
        set_arc_img(this, this->arc_data, &this->arc_img[0], this->from + degree / 2, img_angle, img_w,
                    img_h);
    }

}

static void gui_canvas_arc_draw(gui_canvas_arc_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    float degree = this->to - this->from;
    if (degree > 360)
    {
        degree = 360;
    }
    if (degree == 0)
    {
        return;
    }
    int num = ceil(degree / 30);
    for (int i = 0; i < num; i++)
    {
        gui_acc_blit_to_dc(this->arc_img[i], dc, NULL);
    }
    if (this->cap == CANVAS_ARC_ROUND)
    {
        gui_acc_blit_to_dc(this->circle_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->circle_img_02, dc, NULL);
    }

}

static void gui_canvas_arc_end(gui_canvas_arc_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
    if (this->arc_data != NULL)    {gui_free(this->arc_data); this->arc_data = NULL;}
    if (this->circle_data != NULL) {gui_free(this->circle_data); this->circle_data = NULL;}

    for (int i = 0; i < 12; i++)
    {
        if (this->arc_img[i] != NULL) {gui_free(this->arc_img[i]); this->arc_img[i] = NULL;}
    }
    if (this->circle_img_01 != NULL) {gui_free(this->circle_img_01); this->circle_img_01 = NULL;}
    if (this->circle_img_02 != NULL) {gui_free(this->circle_img_02); this->circle_img_02 = NULL;}
}

static void gui_canvas_arc_destory(gui_canvas_arc_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_canvas_arc_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_canvas_arc_prepare((gui_canvas_arc_t *)obj);
            gui_canvas_circle_prepare((gui_canvas_arc_t *)obj);
            break;

        case OBJ_DRAW:
            gui_canvas_arc_draw((gui_canvas_arc_t *)obj);
            break;

        case OBJ_END:
            gui_canvas_arc_end((gui_canvas_arc_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_canvas_arc_destory((gui_canvas_arc_t *)obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param data
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_canvas_arc_t*
 */
gui_canvas_arc_t *gui_canvas_arc_create(void       *parent,
                                        const char *name,
                                        int          cx,
                                        int          cy,
                                        int          r,
                                        float        from,
                                        float        to,
                                        int  stroke_width,
                                        gui_color_t  color)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "canvas_arc";
    }

    gui_canvas_arc_t *this = gui_malloc(sizeof(gui_canvas_arc_t));
    gui_obj_t *obj = (gui_obj_t *)this;
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_canvas_arc_t));
    this->r = r;
    this->from = from;
    this->to = to;
    this->cx = cx;
    this->cy = cy;
    this->color = color;
    this->stroke_width = stroke_width;

    gui_obj_ctor((gui_obj_t *)this, parent, name, cx - this->r - this->stroke_width / 2,
                 cy - this->r - this->stroke_width / 2, this->r * 2 + this->stroke_width,
                 this->r * 2 + this->stroke_width);

    obj->obj_cb = gui_canvas_arc_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;


    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;

    return this;
}

void gui_canvas_arc_set_cap(gui_canvas_arc_t *this, T_CANVAS_ARC_CAP cap)
{
    this->cap = cap;
}

void gui_canvas_arc_set_external_picture(gui_canvas_arc_t *this, void **data)
{
    this->use_external_picture = true;
    this->data = data;
}

