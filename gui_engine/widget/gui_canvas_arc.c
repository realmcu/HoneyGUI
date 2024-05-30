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
#include <draw_img.h>
#include "acc_init.h"
#include <tp_algo.h>
#include "nanovg.h"
#include "math.h"



/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

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
    head->type = RGBA8888;
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
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));

    matrix_translate(-(this->cx - this->r), -(this->cy - this->r), &img->matrix);


    matrix_translate(this->cx, this->cy, &img->matrix);
    matrix_rotate(rotate_degree, &img->matrix);
    matrix_translate(-img_w / 2, -(this->r + this->stroke_width / 2), &img->matrix);



    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));



    matrix_inverse(&img->inverse);
    gui_image_load_scale(img, IMG_SRC_MEMADDR);
    gui_image_new_area(img, NULL);
    *input_img = img;
}

static void gui_canvas_arc_prepare(gui_canvas_arc_t *this)
{

    float degree = this->to - this->from;

    if (degree >= 60)
    {
        degree = 60;
    }

    float img_angle = (degree / 2) / 180.0f * 3.1415926f;
    int img_w = this->stroke_width + 2 * this->r * sin(img_angle);
    int img_h = this->stroke_width + this->r - this->r * cos(img_angle) + 1;

    if (this->use_external_picture == false)
    {
        this->arc_data = gui_malloc(img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));
        degree = this->to - this->from;

        memset(this->arc_data, 0x0, img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));

        NVGcontext *vg = nvgCreateAGGE(img_w, img_h, img_w * 4 /*ARGB 4 byte*/, NVG_TEXTURE_BGRA,
                                       (uint8_t *)this->arc_data + sizeof(gui_rgb_data_head_t));
        nvgBeginFrame(vg, img_w, img_h, 1);

        // Draw 1/4 circle
        nvgArc(vg, img_w / 2, this->r + this->stroke_width / 2, this->r, -NVG_PI / 2 - img_angle,
               -NVG_PI / 2 + img_angle, NVG_CW);

        nvgStrokeWidth(vg, this->stroke_width);

        nvgLineCap(vg, this->cap);

        nvgStrokeColor(vg, nvgRGBA(this->color.color.rgba.r, this->color.color.rgba.g,
                                   this->color.color.rgba.b, this->color.color.rgba.a));

        nvgStroke(vg);


        nvgEndFrame(vg);
        nvgDeleteAGGE(vg);

        for (uint32_t i = 0; i < img_h; i++)
        {
            for (uint32_t j = 0; j < img_w; j++)
            {
                gui_color_t *pixel = (gui_color_t *)this->arc_data + i * img_w + j + 2;
                if ((pixel->color.rgba.r != 0) || (pixel->color.rgba.g != 0) || (pixel->color.rgba.b != 0))
                {
                    pixel->color.rgba.a = this->color.color.rgba.a;
                    pixel->color.rgba.r = this->color.color.rgba.r;
                    pixel->color.rgba.g = this->color.color.rgba.g;
                    pixel->color.rgba.b = this->color.color.rgba.b;
                }
            }
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



    if (degree <= 60)
    {
        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + degree / 2, img_angle, img_w,
                    img_h);
    }
    else if (degree <= 120)
    {

        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + 30, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_02, this->from + 30 + (degree - 60), img_angle,
                    img_w, img_h);
    }
    else if (degree <= 180)
    {
        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + 30, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_02, this->from + 30 + 60, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_03, this->from + 30 + 60 + (degree - 120),
                    img_angle, img_w, img_h);
    }
    else if (degree <= 240)
    {
        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + 30, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_02, this->from + 30 + 60, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_03, this->from + 30 + 60 + 60, img_angle, img_w,
                    img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_04, this->from + 30 + 60 + 60 + (degree - 180),
                    img_angle, img_w, img_h);
    }
    else if (degree <= 300)
    {
        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + 30, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_02, this->from + 30 + 60, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_03, this->from + 30 + 60 + 60, img_angle, img_w,
                    img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_04, this->from + 30 + 60 + 60 + 60, img_angle,
                    img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_05,
                    this->from + 30 + 60 + 60 + 60 + (degree - 240), img_angle, img_w, img_h);
    }
    else if (degree <= 360)
    {
        set_arc_img(this, this->arc_data, &this->arc_img_01, this->from + 30, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_02, this->from + 30 + 60, img_angle, img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_03, this->from + 30 + 60 + 60, img_angle, img_w,
                    img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_04, this->from + 30 + 60 + 60 + 60, img_angle,
                    img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_05, this->from + 30 + 60 + 60 + 60 + 60, img_angle,
                    img_w, img_h);
        set_arc_img(this, this->arc_data, &this->arc_img_06,
                    this->from + 30 + 60 + 60 + 60 + 60 + (degree - 300), img_angle, img_w, img_h);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}

static void gui_canvas_arc_draw(gui_canvas_arc_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    float degree = this->to - this->from;
    if (degree <= 60)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
    }
    else if (degree <= 120)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_02, dc, NULL);
    }
    else if (degree <= 180)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_02, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_03, dc, NULL);
    }
    else if (degree <= 240)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_02, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_03, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_04, dc, NULL);
    }
    else if (degree <= 300)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_02, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_03, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_04, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_05, dc, NULL);
    }
    else if (degree <= 360)
    {
        gui_acc_blit_to_dc(this->arc_img_01, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_02, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_03, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_04, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_05, dc, NULL);
        gui_acc_blit_to_dc(this->arc_img_06, dc, NULL);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
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

    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_01); this->arc_img_01 = NULL;}
    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_02); this->arc_img_02 = NULL;}
    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_03); this->arc_img_03 = NULL;}
    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_04); this->arc_img_04 = NULL;}
    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_05); this->arc_img_05 = NULL;}
    if (this->arc_img_01 != NULL) {gui_free(this->arc_img_06); this->arc_img_06 = NULL;}
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

    gui_obj_ctor((gui_obj_t *)this, parent, name, cx - this->r, cy - this->r, cx + this->r,
                 cy + this->r);

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


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
