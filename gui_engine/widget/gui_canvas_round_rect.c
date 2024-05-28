/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_round_rect.c
  * @brief image rect widget
  * @details image rect widget is used to show rect on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#include "gui_canvas_round_rect.h"
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


static void set_rect_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = XRGB8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = w;
    head->h = h;
    gui_rect_file_head_t *const_color = (gui_rect_file_head_t *)head;
    const_color->color = color;
}

static void set_arc_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
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

static void set_rect_img(gui_canvas_round_rect_t *this, draw_img_t **input_img, uint16_t x,
                         uint16_t y, uint16_t w, uint16_t h)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));
    img->blend_mode = IMG_RECT;
    img->opacity_value = UINT8_MAX;

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->rect_data;
    set_rect_w_and_h(head, w, h, this->color);
    img->data = head;

    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&img->inverse);
    gui_image_load_scale(img);
    gui_image_new_area(img, NULL);
    *input_img = img;

}

static void prepare_arc_img(gui_canvas_round_rect_t *this)
{
    uint32_t *data = (uint32_t *)(this->circle_data + sizeof(gui_rgb_data_head_t));
    float boundary[this->r];
    memset(boundary, 0, this->r * sizeof(float));
    for (int i = 0; i < this->r; i++)
    {
        float y = i + 0.5 - this->r;
        boundary[i] = this->r - sqrtf(this->r * this->r - y * y);

    }
    for (int i = 0; i < this->r; i++)
    {
        uint16_t right = (int)boundary[i];
        uint32_t offset = this->r * i;
        if (right < this->r - 1)
        {
            for (int j = right + 1; j < this->r; j++)
            {
                data[offset + j] = this->color.color.rgba_full;
            }
        }
        float portion = ceil(boundary[i]) - boundary[i];
        gui_color_t color = this->color;
        color.color.rgba.a = round(portion * color.color.rgba.a);
        data[offset + right] = color.color.rgba_full;
        if (i > this->r / 2 - 1)
        {
            data[right * this->r + i] = color.color.rgba_full;
        }
    }
}

static void set_arc_img(gui_canvas_round_rect_t *this, draw_img_t **input_img, uint16_t x,
                        uint16_t y, float degree)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = this->circle_data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = UINT8_MAX;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);

    matrix_translate(this->r, this->r, &img->matrix);
    matrix_rotate(degree, &img->matrix);
    matrix_translate(-this->r, -this->r, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&img->inverse);
    gui_image_load_scale(img);
    gui_image_new_area(img, NULL);
    *input_img = img;
}


static void gui_canvas_round_rect_prepare(gui_canvas_round_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    this->rect_data = gui_malloc(sizeof(gui_rect_file_head_t));


    // set_rect_img(this, &this->rect_0, this->r, 0, this->base.w - 2 * this->r, this->r);
    // set_rect_img(this, &this->rect_1, 0, this->r, this->base.w, this->base.h - 2 * this->r);
    // set_rect_img(this, &this->rect_2, this->r, this->base.h - this->r, this->base.w - 2 * this->r,
    //              this->r);
    set_rect_img(this, &this->rect_0, \
                 this->r - 1,  \
                 0,
                 this->base.w - 2 * this->r + 2, \
                 this->r + 1);

    set_rect_img(this, &this->rect_1, \
                 0, \
                 this->r - 1, \
                 this->base.w, \
                 this->base.h - 2 * this->r + 2);

    set_rect_img(this, &this->rect_2, \
                 this->r - 1,  \
                 this->base.h - this->r - 1,
                 this->base.w - 2 * this->r + 2, \
                 this->r + 1);

    if (this->use_external_picture == false)
    {
        this->circle_data = gui_malloc(this->r * this->r * 4 + sizeof(gui_rgb_data_head_t));
        memset(this->circle_data, 0x00, this->r * this->r * 4 + sizeof(gui_rgb_data_head_t));

        prepare_arc_img(this);
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

        set_arc_w_and_h(head, this->r, this->r, this->color);

    }
    set_arc_img(this, &this->circle_00, 0, 0, 0);
    set_arc_img(this, &this->circle_01, this->base.w - 2 * this->r - 1, 0, 90);
    set_arc_img(this, &this->circle_10, 0, this->base.h - 2 * this->r - 1, 270);
    set_arc_img(this, &this->circle_11, this->base.w - 2 * this->r - 1, this->base.h - 2 * this->r - 1,
                180);
}

static void gui_canvas_round_rect_draw(gui_canvas_round_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    gui_acc_blit_to_dc(this->rect_0, dc, NULL);
    gui_acc_blit_to_dc(this->rect_1, dc, NULL);
    gui_acc_blit_to_dc(this->rect_2, dc, NULL);
    gui_acc_blit_to_dc(this->circle_00, dc, NULL);
    gui_acc_blit_to_dc(this->circle_01, dc, NULL);
    gui_acc_blit_to_dc(this->circle_10, dc, NULL);
    gui_acc_blit_to_dc(this->circle_11, dc, NULL);
}


static void gui_canvas_round_rect_end(gui_canvas_round_rect_t *this)
{
    if (this->rect_data != NULL) {gui_free(this->rect_data); this->rect_data = NULL;}
    if (this->use_external_picture == false) {if (this->circle_data != NULL) {gui_free(this->circle_data); this->circle_data = NULL;}}
    if (this->rect_0 != NULL) {gui_free(this->rect_0); this->rect_0 = NULL;}
    if (this->rect_1 != NULL) {gui_free(this->rect_1); this->rect_1 = NULL;}
    if (this->rect_2 != NULL) {gui_free(this->rect_2); this->rect_2 = NULL;}
    if (this->circle_00 != NULL) {gui_free(this->circle_00); this->circle_00 = NULL;}
    if (this->circle_01 != NULL) {gui_free(this->circle_01); this->circle_01 = NULL;}
    if (this->circle_10 != NULL) {gui_free(this->circle_10); this->circle_10 = NULL;}
    if (this->circle_11 != NULL) {gui_free(this->circle_11); this->circle_11 = NULL;}
}

static void gui_canvas_round_rect_destory(gui_canvas_round_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_canvas_round_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_canvas_round_rect_prepare((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_DRAW:
            gui_canvas_round_rect_draw((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_END:
            gui_canvas_round_rect_end((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_canvas_round_rect_destory((gui_canvas_round_rect_t *)obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_canvas_round_rect_t *gui_canvas_round_rect_create(gui_obj_t   *parent,
                                                      const char *name,
                                                      int          x,
                                                      int          y,
                                                      int          w,
                                                      int          h,
                                                      int          r,
                                                      gui_color_t  color)
{
    GUI_ASSERT(parent != NULL);
    gui_canvas_round_rect_t *canvas_round_rect = gui_malloc(sizeof(gui_canvas_round_rect_t));
    GUI_ASSERT(canvas_round_rect != NULL);
    memset(canvas_round_rect, 0x00, sizeof(gui_canvas_round_rect_t));
    gui_obj_ctor((gui_obj_t *)canvas_round_rect, parent, name, x, y, w, h);
    GET_BASE(canvas_round_rect)->obj_cb = gui_canvas_round_rect_cb;
    GET_BASE(canvas_round_rect)->has_prepare_cb = true;
    GET_BASE(canvas_round_rect)->has_draw_cb = true;
    GET_BASE(canvas_round_rect)->has_end_cb = true;
    GET_BASE(canvas_round_rect)->has_destroy_cb = true;
    gui_list_init(&(GET_BASE(canvas_round_rect)->child_list));
    if ((GET_BASE(canvas_round_rect)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(canvas_round_rect)->parent)->child_list),
                               &(GET_BASE(canvas_round_rect)->brother_list));
    }
    GET_BASE(canvas_round_rect)->create_done = true;
    canvas_round_rect->r = r;
    canvas_round_rect->color = color;
    return canvas_round_rect;
}

void gui_canvas_round_rect_set_external_picture(gui_canvas_round_rect_t *this, void *data)
{
    this->use_external_picture = true;
    this->circle_data = data;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
