/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_round_rect.c
  * @brief image rect widget
  * @details image rect widget is used to show rect on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_canvas_round_rect.h"
#include <string.h>
#include "gui_obj.h"
#include "draw_img.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "nanovg.h"
#include <math.h>


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


static void set_rect_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
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
    gui_rect_file_head_t *const_color = (gui_rect_file_head_t *)head;
    const_color->color = color;
}

static void set_arc_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
{
    (void)color;
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

static void set_rect_img(gui_canvas_round_rect_t *this, draw_img_t **input_img, int16_t x,
                         int16_t y, uint16_t w, uint16_t h)
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
    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;

}

static void prepare_arc_img(gui_canvas_round_rect_t *this)
{
    uint32_t *data = (uint32_t *)(this->circle_data + sizeof(gui_rgb_data_head_t));
    uint16_t img_w = this->r + 1;
    float boundary[this->r];
    memset(boundary, 0, this->r * sizeof(float));
    for (int i = 1; i <= this->r; i++)
    {
        float y = i - 0.5 - this->r;
        boundary[i - 1] = this->r - sqrtf(this->r * this->r - y * y);
    }
    for (int i = 1; i <= this->r; i++)
    {
        int k = i - 1;
        uint16_t right = (int)boundary[k];
        uint32_t offset = img_w * i;
        if (right < img_w)
        {
            for (int j = right + 2; j < img_w; j++)
            {
                data[offset + j] = this->color.color.argb_full;
            }
        }
        float portion = ceilf(boundary[k]) - boundary[k];
        gui_color_t color = this->color;
        color.color.rgba.a = roundf(portion * color.color.rgba.a);
        data[offset + right + 1] = color.color.argb_full;
        if (k > this->r / 2 - 1)
        {
            data[(right + 1) * img_w + k] = color.color.argb_full;
        }
    }
}

static void set_arc_img(gui_canvas_round_rect_t *this, draw_img_t **input_img, int16_t x,
                        int16_t y, float degree)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = this->circle_data;
    img->high_quality = 1;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = UINT8_MAX;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);

    matrix_translate(this->r + 1, this->r + 1, &img->matrix);
    matrix_rotate(degree, &img->matrix);
    matrix_translate(-this->r - 1, -this->r - 1, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&img->inverse);
    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}
static void set_arc_img_ftl(gui_canvas_round_rect_t *this, draw_img_t **input_img, int16_t x,
                            int16_t y, float degree)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = this->circle_data;
    img->high_quality = 1;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = UINT8_MAX;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);

    matrix_translate(this->r + 1, this->r + 1, &img->matrix);
    matrix_rotate(degree, &img->matrix);
    matrix_translate(-this->r - 1, -this->r - 1, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&img->inverse);
    draw_img_load_scale(img, IMG_SRC_FTL);
    draw_img_new_area(img, NULL);
    *input_img = img;
}

static void gui_canvas_round_rect_prepare(gui_canvas_round_rect_t *this)
{
    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_CLICKED);
    if (this->color.color.rgba.a == 255)
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
            this->circle_data = gui_malloc((this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));
            memset(this->circle_data, 0x00, (this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));

            prepare_arc_img(this);
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

            set_arc_w_and_h(head, this->r + 1, this->r + 1, this->color);

        }
        set_arc_img(this, &this->circle_00, -1, -1, 0);
        set_arc_img(this, &this->circle_01, this->base.w - 2 * this->r - 2, -1, 90);
        set_arc_img(this, &this->circle_10, -1, this->base.h - 2 * this->r - 2, 270);
        set_arc_img(this, &this->circle_11, this->base.w - 2 * this->r - 2, this->base.h - 2 * this->r - 2,
                    180);
        gui_fb_change();
    }
    else
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
                     this->r,  \
                     0,
                     this->base.w - 2 * this->r, \
                     this->r);

        set_rect_img(this, &this->rect_1, \
                     0, \
                     this->r, \
                     this->base.w, \
                     this->base.h - 2 * this->r);

        set_rect_img(this, &this->rect_2, \
                     this->r,  \
                     this->base.h - this->r,
                     this->base.w - 2 * this->r, \
                     this->r + 1);
        if (this->use_external_picture == false)
        {
            this->circle_data = gui_malloc((this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));
            memset(this->circle_data, 0x00, (this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));

            prepare_arc_img(this);
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

            set_arc_w_and_h(head, this->r + 1, this->r + 1, this->color);

        }
        set_arc_img(this, &this->circle_00, -1, -1, 0);
        set_arc_img(this, &this->circle_01, this->base.w - 2 * this->r - 2, -1, 90);
        set_arc_img(this, &this->circle_10, -1, this->base.h - 2 * this->r - 2, 270);
        set_arc_img(this, &this->circle_11, this->base.w - 2 * this->r - 2, this->base.h - 2 * this->r - 2,
                    180);
        gui_fb_change();
    }



}
static void gui_canvas_round_rect_prepare_ftl(gui_canvas_round_rect_t *this)
{
    if (this->color.color.rgba.a == 255)
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
            this->circle_data = gui_malloc((this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));
            memset(this->circle_data, 0x00, (this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));

            prepare_arc_img(this);
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

            set_arc_w_and_h(head, this->r + 1, this->r + 1, this->color);

        }
        set_arc_img_ftl(this, &this->circle_00, -1, -1, 0);
        set_arc_img_ftl(this, &this->circle_01, this->base.w - 2 * this->r - 2, -1, 90);
        set_arc_img_ftl(this, &this->circle_10, -1, this->base.h - 2 * this->r - 2, 270);
        set_arc_img_ftl(this, &this->circle_11, this->base.w - 2 * this->r - 2,
                        this->base.h - 2 * this->r - 2,
                        180);
        gui_fb_change();
    }
    else
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
                     this->r,  \
                     0,
                     this->base.w - 2 * this->r, \
                     this->r);

        set_rect_img(this, &this->rect_1, \
                     0, \
                     this->r, \
                     this->base.w, \
                     this->base.h - 2 * this->r);

        set_rect_img(this, &this->rect_2, \
                     this->r,  \
                     this->base.h - this->r,
                     this->base.w - 2 * this->r, \
                     this->r + 1);
        if (this->use_external_picture == false)
        {
            this->circle_data = gui_malloc((this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));
            memset(this->circle_data, 0x00, (this->r + 1) * (this->r + 1) * 4 + sizeof(gui_rgb_data_head_t));

            prepare_arc_img(this);
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->circle_data;

            set_arc_w_and_h(head, this->r + 1, this->r + 1, this->color);

        }
        set_arc_img_ftl(this, &this->circle_00, -1, -1, 0);
        set_arc_img_ftl(this, &this->circle_01, this->base.w - 2 * this->r - 2, -1, 90);
        set_arc_img_ftl(this, &this->circle_10, -1, this->base.h - 2 * this->r - 2, 270);
        set_arc_img_ftl(this, &this->circle_11, this->base.w - 2 * this->r - 2,
                        this->base.h - 2 * this->r - 2,
                        180);
        gui_fb_change();
    }



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
static void blit_to_dc_ftl(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    (void)rect;
    if (dc->section_count == 0)
    {


        gui_rgb_data_head_t head;
        uintptr_t base = (uintptr_t)image->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
        uint8_t *data = NULL;
        if (head.compress == true)
        {
            uint32_t end = 0;
            gui_ftl_read(base + sizeof(gui_rgb_data_head_t) + sizeof(imdc_file_header_t) + 4 * (head.h),
                         (uint8_t *)&end, 4);
            uint32_t size = end  + sizeof(gui_rgb_data_head_t);
            data = (uint8_t *)gui_malloc(size);
            GUI_ASSERT(data != NULL);
            gui_ftl_read(base, data, size);
        }
        else
        {
            draw_img_get_header(image, IMG_SRC_FTL);
            uint8_t pixel_byte = draw_img_get_pixel_byte(image, IMG_SRC_FTL);
            uint32_t size = head.w * head.h * pixel_byte;
            data = (uint8_t *)gui_malloc(size);
            GUI_ASSERT(data != NULL);
            gui_ftl_read(base, data, size);
        }
        image->data = data;

    }
    gui_acc_blit_to_dc(image, dc, NULL);
    if (!(dc->section_count != dc->section_total - 1))
    {
        gui_free(image->data);
    }
}
static void gui_canvas_round_rect_draw_ftl(gui_canvas_round_rect_t *this)
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
    if (this->use_external_picture)
    {
        blit_to_dc_ftl(this->circle_00, dc, NULL);
        blit_to_dc_ftl(this->circle_01, dc, NULL);
        blit_to_dc_ftl(this->circle_10, dc, NULL);
        blit_to_dc_ftl(this->circle_11, dc, NULL);
    }
    else
    {
        gui_acc_blit_to_dc(this->circle_00, dc, NULL);
        gui_acc_blit_to_dc(this->circle_01, dc, NULL);
        gui_acc_blit_to_dc(this->circle_10, dc, NULL);
        gui_acc_blit_to_dc(this->circle_11, dc, NULL);
    }


}
#define IMG_DATA_FREE(draw_img ) \
    if(draw_img!=NULL)  \
        draw_img_acc_end_cb(draw_img)

static void gui_canvas_round_rect_end(gui_canvas_round_rect_t *this)
{
    if (this->rect_data != NULL) {gui_free(this->rect_data); this->rect_data = NULL;}
    if (this->use_external_picture == false) {if (this->circle_data != NULL) {gui_free(this->circle_data); this->circle_data = NULL;}}
    if (draw_img_acc_end_cb != NULL)
    {
        IMG_DATA_FREE(this->rect_0);
        IMG_DATA_FREE(this->rect_1);
        IMG_DATA_FREE(this->rect_2);
        IMG_DATA_FREE(this->circle_00);
        IMG_DATA_FREE(this->circle_01);
        IMG_DATA_FREE(this->circle_10);
        IMG_DATA_FREE(this->circle_11);
    }
    if (this->rect_0 != NULL) {gui_free(this->rect_0); this->rect_0 = NULL;}
    if (this->rect_1 != NULL) {gui_free(this->rect_1); this->rect_1 = NULL;}
    if (this->rect_2 != NULL) {gui_free(this->rect_2); this->rect_2 = NULL;}
    if (this->circle_00 != NULL) {gui_free(this->circle_00); this->circle_00 = NULL;}
    if (this->circle_01 != NULL) {gui_free(this->circle_01); this->circle_01 = NULL;}
    if (this->circle_10 != NULL) {gui_free(this->circle_10); this->circle_10 = NULL;}
    if (this->circle_11 != NULL) {gui_free(this->circle_11); this->circle_11 = NULL;}
}

static void gui_canvas_round_rect_destroy(gui_canvas_round_rect_t *this)
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

        case OBJ_DESTROY:
            gui_canvas_round_rect_destroy((gui_canvas_round_rect_t *)obj);
            break;

        default:
            break;
        }
    }
}
static void gui_canvas_round_rect_cb_ftl(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_canvas_round_rect_prepare_ftl((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_DRAW:
            gui_canvas_round_rect_draw_ftl((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_END:
            gui_canvas_round_rect_end((gui_canvas_round_rect_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_canvas_round_rect_destroy((gui_canvas_round_rect_t *)obj);
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
gui_canvas_round_rect_t *gui_canvas_round_rect_create_ftl(gui_obj_t   *parent,
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
    GET_BASE(canvas_round_rect)->obj_cb = gui_canvas_round_rect_cb_ftl;
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

/**
 * @brief Set the opacity of the canvas round rectangle.
 *
 * This function sets the opacity level for a given round rectangle on the canvas.
 * The opacity value ranges from `0` (fully transparent) to `255` (fully opaque).
 *
 * @param _this           Pointer to the gui_canvas_round_rect_t structure.
 * @param opacity_value   The opacity value to be set (0-255).
 */
void gui_canvas_round_rect_set_opacity(gui_canvas_round_rect_t *this, unsigned char opacity_value)
{
    GUI_ASSERT(NULL != NULL);
    this->opacity_value = opacity_value;
}


/**
 * @brief Set the color of the canvas round rectangle.
 *
 * This function assigns a specific color to the round rectangle on the canvas.
 *
 * @param _this  Pointer to the gui_canvas_round_rect_t structure.
 * @param color  The color value to be set, represented as gui_color_t.
 */
void gui_canvas_round_rect_set_color(gui_canvas_round_rect_t *this, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
}


/**
 * @brief Set the location of the canvas round rectangle.
 *
 * This function specifies the starting coordinates (X and Y) for the round rectangle
 * on the canvas.
 *
 * @param _this  Pointer to the gui_canvas_round_rect_t structure.
 * @param x      The start X-coordinate.
 * @param y      The start Y-coordinate.
 */
void gui_canvas_round_rect_set_location(gui_canvas_round_rect_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    gui_obj_t *root = (gui_obj_t *)this;
    root->x = x;
    root->y = y;
}


