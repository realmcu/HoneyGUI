/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_scope.c
  * @brief  create a picture with scope
  * @details only display pixels in scope
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img_scope.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "acc_init.h"
#include "gui_win.h"
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
static void gui_img_scope_prepare(gui_obj_t *obj)
{
    uint8_t last;
    touch_info_t *tp;
    gui_img_t *this;

    GUI_ASSERT(obj != NULL);
    this = (gui_img_t *)obj;
    tp = tp_get_info();

    matrix_translate(this->t_x, this->t_y, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    matrix_translate(-this->c_x, -this->c_y, obj->matrix);

    float m00 = obj->matrix->m[0][0];
    float m01 = obj->matrix->m[0][1];
    float m02 = obj->matrix->m[0][2];
    float m10 = obj->matrix->m[1][0];
    float m11 = obj->matrix->m[1][1];
    float m12 = obj->matrix->m[1][2];
    float m20 = obj->matrix->m[2][0];
    float m21 = obj->matrix->m[2][1];
    float m22 = obj->matrix->m[2][2];

    if ((m01 == 0)
        && (m10 == 0)
        && (m20 == 0)
        && (m21 == 0)
        && (m22 == 1)) //scale and translate, no rotate
    {
        float x_min = m02;
        float x_max = m02 + m00 * obj->w;
        float y_min = m12;
        float y_max = m12 + m11 * obj->h;

        if ((x_min > (int)gui_get_screen_width())
            || (x_max < 0)
            || (y_min > (int)gui_get_screen_height())
            || (y_max < 0))
        {
            return;
        }
    }

    this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(this->draw_img, 0x00, sizeof(draw_img_t));

    if (this->src_mode == IMG_SRC_FILESYS)
    {
        this->data = this->file_path;
    }
    this->draw_img->data = this->data;
    this->draw_img->blend_mode = this->blend_mode;
    this->draw_img->high_quality = this->high_quality;
    this->draw_img->opacity_value = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_img_t *b = (void *)obj;

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);
                b->press_flag = true;
            }
        }

        if (b->release_flag)
        {
            b->press_flag = false;
            b->release_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
        }

        if (tp->released && b->press_flag)
        {
            b->release_flag = true;
        }
    }



    matrix_translate(this->t_x, this->t_y, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    matrix_translate(-this->c_x, -this->c_y, obj->matrix);

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&this->draw_img->inverse);
    gui_image_load_scale(this->draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode);
    gui_obj_t *o = obj;
    gui_win_t *win_scope = 0;
    GUI_TYPE(gui_img_scope_t, obj)->ax = obj->x;
    GUI_TYPE(gui_img_scope_t, obj)->ay = obj->y;
    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == WINDOW && GUI_TYPE(gui_win_t, o)->scope)
        {
            win_scope = (void *)o;
            break;
        }
    }
    o = obj;
    while (o->parent != NULL)
    {
        o = o->parent;
        GUI_TYPE(gui_img_scope_t, obj)->ax += o->x;
        GUI_TYPE(gui_img_scope_t, obj)->ay += o->y;
    }
    if (win_scope)
    {
        int ax = o->matrix->m[0][2] ;
        int ay = o->matrix->m[1][2];

        int w_w = o->w;
        int w_h = o->h;
        int img_x = GUI_TYPE(gui_img_scope_t, obj)->ax;
        int img_y = GUI_TYPE(gui_img_scope_t, obj)->ay;
        int img_w = this->draw_img->img_w;
        int img_h = this->draw_img->img_h;
        //gui_log("ax,ay:%d,%d,%d,%d,%d,%d,%d,%d\n",ax,ay,img_x,img_y,img_w ,img_h, w_w, w_h);
        GUI_TYPE(gui_img_scope_t, obj)->scope_x1 = 0;
        GUI_TYPE(gui_img_scope_t, obj)->scope_x2 = img_w;
        GUI_TYPE(gui_img_scope_t, obj)->scope_y1 = 0;
        GUI_TYPE(gui_img_scope_t, obj)->scope_y2 = img_h;
        obj->not_show = 0;
        if (ax > img_x)
        {
            GUI_TYPE(gui_img_scope_t, obj)->scope_x1 = ax - img_x;
        }
        if (ay > img_y)
        {
            GUI_TYPE(gui_img_scope_t, obj)->scope_y1 = ay - img_y;
        }
        if (ax + w_w < img_x + img_w)
        {
            GUI_TYPE(gui_img_scope_t, obj)->scope_x2 = -(img_x - (ax + w_w));
        }
        if (ay + w_h < img_y + img_h)
        {
            GUI_TYPE(gui_img_scope_t, obj)->scope_y2 = -(img_y - (ay + w_h));
        }
        if (ay + w_h < img_y)
        {
            obj->not_show = 1;
        }

    }
    gui_image_new_area(this->draw_img, NULL);



    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->type == TOUCH_SHORT)
            && (obj->event_dsc_cnt > 0))
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
        }
    }

    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_img_scope_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *this = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();

    gui_rect_t rect =
    {
        .x1 = GUI_TYPE(gui_img_scope_t, obj)->scope_x1,
        .x2 = GUI_TYPE(gui_img_scope_t, obj)->scope_x2,
        .y1 = GUI_TYPE(gui_img_scope_t, obj)->scope_y1,
        .y2 = GUI_TYPE(gui_img_scope_t, obj)->scope_y2,
    };

    // cache img to buffer
    uint8_t *img_buff = NULL;

    extern void *gui_draw_cache_img(draw_img_t *image, IMG_SOURCE_MODE_TYPE src_mode);
    img_buff = gui_draw_cache_img(this->draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode);
    if (dc->type == DC_RAMLESS && this->src_mode == IMG_SRC_FILESYS)
    {
        if (dc->section_count == 0)
        {
            this->data = img_buff;
        }
        else
        {
            img_buff = this->data;
        }
    }

    // blit
    gui_acc_blit_to_dc(this->draw_img, dc, &rect);

    // release img if cached
    extern void gui_draw_free_img(draw_img_t *draw_img, void *img_buff, IMG_SOURCE_MODE_TYPE src_mode);
    gui_draw_free_img(this->draw_img, img_buff, (IMG_SOURCE_MODE_TYPE)this->src_mode);
}

static void gui_img_scope_img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    if (img->draw_img != NULL)
    {
        if (gui_image_acc_end_cb != NULL)
        {
            gui_image_acc_end_cb(img->draw_img);
        }
        gui_free(img->draw_img);
        img->draw_img = NULL;
    }
}

static void gui_img_scope_img_destory(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *this = (gui_img_t *)obj;

    gui_log("do obj %s free\n", obj->name);
    if (this->src_mode == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        // free path transforming memory on win
        gui_free(this->file_path);
        this->file_path = NULL;
#endif
    }
}

static void gui_img_scope_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_img_scope_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_img_scope_draw_cb(obj);
            break;

        case OBJ_END:
            gui_img_scope_img_end(obj);
            break;

        case OBJ_DESTORY:
            gui_img_scope_img_destory(obj);
            break;

        default:
            break;
        }
    }
}

void gui_img_scope_ctor(gui_img_t  *this,
                        gui_obj_t  *parent,
                        const char *name,
                        IMG_SOURCE_MODE_TYPE  src_mode,
                        void       *addr,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    //for base class
    gui_obj_ctor(GET_BASE(this), parent, name, x, y, w, h);

    //for root class
    GET_BASE(this)->type = IMAGE_SCOPE;
    GET_BASE(this)->obj_cb = gui_img_scope_cb;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_draw_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    //for self

    this->blend_mode = IMG_FILTER_BLACK;
    this->opacity_value = UINT8_MAX;

    this->scale_x = 1.0f;
    this->scale_y = 1.0f;

    this->src_mode = src_mode;
    if (this->src_mode == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        addr = gui_filepath_transforming(addr);
#endif
        this->data = (void *)addr;
        this->file_path = (void *)addr;
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        this->data = (void *)addr;
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_img_scope_set_range(gui_img_scope_t *this, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    this->scope_x1 = x1;
    this->scope_x2 = x2;
    this->scope_y1 = y1;
    this->scope_y2 = y2;
}
gui_img_scope_t *gui_img_scope_create(void    *parent,
                                      const char *name,
                                      void    *addr,
                                      int16_t  x,
                                      int16_t  y)
{
    GUI_ASSERT(parent != NULL);

    gui_img_scope_t *img = gui_malloc(sizeof(gui_img_scope_t));
    GUI_ASSERT(img != NULL);

    memset(img, 0x00, sizeof(gui_img_scope_t));
    gui_img_scope_ctor((void *)img, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, 0, 0);
    img->scope_x1 = 0;
    img->scope_y1 = 0;
    img->scope_x2 = gui_img_get_width(&img->base);
    img->scope_y2 = gui_img_get_height(&img->base);

    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }

    GET_BASE(img)->create_done = true;

    return img;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
