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
#include "acc_engine.h"
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



static void set_arc_60_img(gui_canvas_arc_t *this, void *data, draw_img_t **input_img,
                           float rotate_degree, float image_angle, int img_w, int img_h)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = NULL;
    img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    img->data = data;

    memset(img->data, 0x0, img_w * img_h * 4 + sizeof(gui_rgb_data_head_t));

    NVGcontext *vg = nvgCreateAGGE(img_w, img_h, img_w * 4 /*ARGB 4 byte*/, NVG_TEXTURE_BGRA,
                                   (uint8_t *)img->data + sizeof(gui_rgb_data_head_t));
    nvgBeginFrame(vg, img_w, img_h, 1);

    // Draw 1/4 circle
    nvgArc(vg, img_w / 2, this->r + this->stroke_width / 2, this->r, -NVG_PI / 2 - image_angle,
           -NVG_PI / 2 + image_angle, NVG_CW);

    nvgStrokeWidth(vg, this->stroke_width);

    nvgStrokeColor(vg, nvgRGBA(this->color.color.rgba.r, this->color.color.rgba.g,
                               this->color.color.rgba.b, this->color.color.rgba.a));

    nvgStroke(vg);


    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img->data;

    set_arc_w_and_h(head, img_w, img_h);


    img->blend_mode = IMG_FILTER_BLACK;
    img->opacity_value = UINT8_MAX;
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));

    matrix_translate(-(this->cx - this->r), -(this->cy - this->r), &img->matrix);


    matrix_translate(this->cx, this->cy, &img->matrix);
    matrix_rotate(rotate_degree, &img->matrix);
    matrix_translate(-img_w / 2, -(this->r + this->stroke_width / 2), &img->matrix);



    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));



    matrix_inverse(&img->inverse);
    gui_image_load_scale(img);
    gui_image_new_area(img, NULL);
    *input_img = img;
}

static void gui_canvas_arc_prepare(gui_canvas_arc_t *this)
{

    float img_60_angle = (70 / 2) / 180.0f * 3.1415926f;
    int img_60_w = 2 * this->r * sin(img_60_angle);
    int img_60_h = this->stroke_width + this->r - this->r * cos(img_60_angle);
    this->arc_data_60 = gui_malloc(img_60_w * img_60_h * 4 + sizeof(gui_rgb_data_head_t));

    float img_10_angle = (10 / 2) / 180.0f * 3.1415926f;
    int img_10_w = 2 * this->r * sin(img_10_angle);
    int img_10_h = this->stroke_width + this->r - this->r * cos(img_10_angle);
    this->arc_data_10 = gui_malloc(img_10_w * img_10_h * 4 + sizeof(gui_rgb_data_head_t));


    set_arc_60_img(this, this->arc_data_60, &this->arc_60_01, 0,  img_60_angle, img_60_w, img_60_h);
    set_arc_60_img(this, this->arc_data_60, &this->arc_60_02, 60, img_60_angle, img_60_w, img_60_h);
    set_arc_60_img(this, this->arc_data_60, &this->arc_60_03, 120, img_60_angle, img_60_w, img_60_h);
    set_arc_60_img(this, this->arc_data_60, &this->arc_60_04, 180, img_60_angle, img_60_w, img_60_h);
    set_arc_60_img(this, this->arc_data_60, &this->arc_60_05, 240, img_60_angle, img_60_w, img_60_h);
    set_arc_60_img(this, this->arc_data_60, &this->arc_60_06, 300, img_60_angle, img_60_w, img_60_h);
}

static void gui_canvas_arc_draw(gui_canvas_arc_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();


    gui_acc_blit_to_dc(this->arc_60_01, dc, NULL);
    gui_acc_blit_to_dc(this->arc_60_02, dc, NULL);
    gui_acc_blit_to_dc(this->arc_60_03, dc, NULL);
    // gui_acc_blit_to_dc(this->arc_60_04, dc, NULL);
    // gui_acc_blit_to_dc(this->arc_60_05, dc, NULL);
    // gui_acc_blit_to_dc(this->arc_60_06, dc, NULL);
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
    gui_free(this->arc_data_60);
    gui_free(this->arc_data_10);

    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_01); this->arc_60_01 = NULL;}
    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_02); this->arc_60_02 = NULL;}
    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_03); this->arc_60_03 = NULL;}
    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_04); this->arc_60_04 = NULL;}
    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_05); this->arc_60_05 = NULL;}
    if (this->arc_60_01 != NULL) {gui_free(this->arc_60_06); this->arc_60_06 = NULL;}
    if (this->arc_10 != NULL) {gui_free(this->arc_10); this->arc_10 = NULL;}
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
                                        float        degree,
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
    this->degree = degree;
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


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
