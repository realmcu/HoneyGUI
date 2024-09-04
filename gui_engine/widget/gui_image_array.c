/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_image_array.c
  * @brief image array widget
  * @details show a image array
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/05
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
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <acc_init.h>
#include <gui_matrix.h>
#include "gui_image_array.h"
#include "tp_algo.h"
#include <math.h>
#include "gui_fb.h"
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




static void prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_image_array_t *this = (gui_image_array_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    this->draw_img = gui_malloc(this->array_count * sizeof(draw_img_t *));
    memset(this->draw_img, 0, this->array_count * sizeof(draw_img_t *));

    int w = 0;
    if (!this->scope_flag)
    {
        this->scope = gui_malloc(this->array_count * sizeof(struct gui_image_array_scope *));
        memset(this->scope, 0, this->array_count * sizeof(struct gui_image_array_scope *));
        this->scope_flag = 1;
        gui_obj_t *o = obj;
        while (o->parent != NULL)
        {
            o = o->parent;
            if (o->type == WINDOW && GUI_TYPE(gui_win_t, o)->scope)
            {
                this->parent_scope = 1;
                break;
            }
        }
        for (int i = 0; i < this->array_count; i++)
        {
            this->scope[i] = gui_malloc(sizeof(struct gui_image_array_scope));
            memset(this->scope[i], 0, sizeof(struct gui_image_array_scope));
        }

    }

    for (int i = 0; i < this->array_count; i++)
    {
        this->draw_img[i] = gui_malloc(sizeof(draw_img_t));
        memset(this->draw_img[i], 0, sizeof(draw_img_t));
        this->draw_img[i]->data = this->image_data[i];
        this->draw_img[i]->blend_mode = IMG_SRC_OVER_MODE;
        this->draw_img[i]->opacity_value = obj->parent->opacity_value;
        memcpy(&this->draw_img[i]->matrix, obj->matrix, sizeof(struct gui_matrix));
        matrix_translate(w, 0, &this->draw_img[i]->matrix);
        memcpy(&this->draw_img[i]->inverse, &this->draw_img[i]->matrix, sizeof(struct gui_matrix));

        matrix_inverse(&this->draw_img[i]->inverse);
        draw_img_load_scale(this->draw_img[i], IMG_SRC_MEMADDR);
        if (this->parent_scope)
        {

            gui_obj_t *o = obj;
            gui_win_t *win_scope = 0;
            this->scope[i]->ax = obj->x + w;
            this->scope[i]->ay = obj->y;
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
                this->scope[i]->ax += o->x;
                this->scope[i]->ay += o->y;
            }
            if (win_scope)
            {
                int ax = win_scope->base.x ;
                int ay = win_scope->base.y;
                o = GUI_BASE(win_scope);
                while (o->parent != NULL)
                {
                    o = o->parent;
                    ax += o->x;
                    ay += o->y;
                }
                int w_w = win_scope->base.w;
                int w_h = win_scope->base.h;
                int img_x = this->scope[i]->ax;
                int img_y = this->scope[i]->ay;
                int img_w = this->draw_img[i]->img_w;
                int img_h = this->draw_img[i]->img_h;
                //gui_log("ax,ay:%d,%d,%d,%d,%d,%d,%d,%d\n",ax,ay,img_x,img_y,img_w ,img_h, w_w, w_h);
                this->scope[i]->scope_x1 = 0;
                this->scope[i]->scope_x2 = img_w;
                this->scope[i]->scope_y1 = 0;
                this->scope[i]->scope_y2 = img_h;
                obj->not_show = 0;
                if (ax > img_x)
                {
                    this->scope[i]->scope_x1 = ax - img_x;
                }
                if (ay > img_y)
                {
                    this->scope[i]->scope_y1 = ay - img_y;
                }
                if (ax + w_w < img_x + img_w)
                {
                    this->scope[i]->scope_x2 = -(img_x - (ax + w_w));
                }
                if (ay + w_h < img_y + img_h)
                {
                    this->scope[i]->scope_y2 = -(img_y - (ay + w_h));
                }
                if (ay + w_h < img_y)
                {
                    //obj->not_show = 1;
                    this->scope[i]->scope_y2 = this->scope[i]->scope_y1 - 1;
                }
                if (ay > img_y + img_h * this->scope[i]->scope_y2)
                {
                    //obj->not_show = 1;
                    this->scope[i]->scope_y2 = this->scope[i]->scope_y1 - 1;
                }
            }
        }


        draw_img_new_area(this->draw_img[i], NULL);
        w += this->draw_img[i]->img_w + 1;

    }
    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_image_array_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void draw(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_dispdev_t *dc = gui_get_dc();
    gui_image_array_t *this = (gui_image_array_t *)obj;
    for (int i = 0; i < this->array_count; i++)
    {

        if (this->parent_scope)
        {
            gui_rect_t rect =
            {
                .x1 = this->scope[i]->scope_x1,
                .x2 = this->scope[i]->scope_x2,
                .y1 = this->scope[i]->scope_y1,
                .y2 = this->scope[i]->scope_y2,
            };
            gui_acc_blit_to_dc(this->draw_img[i], dc, &rect);
        }
        else
        {
            gui_acc_blit_to_dc(this->draw_img[i], dc, NULL);

        }
    }
}

static void end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_image_array_t *this = (gui_image_array_t *)obj;

    if (this->draw_img != NULL)
    {
        for (int i = 0; i < this->array_count; i++)
        {
            gui_free(this->draw_img[i]);
        }
        gui_free(this->draw_img);
        this->draw_img = NULL;
    }

}

static void destory(gui_obj_t *obj)
{
    gui_image_array_t *this = (gui_image_array_t *)obj;
    if (this->image_data)
    {
        gui_free(this->image_data);
        this->image_data = 0;
    }
    if (this->scope != NULL)
    {
        for (int i = 0; i < this->array_count; i++)
        {
            gui_free(this->scope[i]);
        }
        gui_free(this->scope);
        this->scope = NULL;
    }

}

static void cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare(obj);
            break;

        case OBJ_DRAW:
            draw(obj);
            break;

        case OBJ_END:
            end(obj);
            break;

        case OBJ_DESTORY:
            destory(obj);
            break;

        default:
            break;
        }
    }
}

static void ctor(gui_image_array_t *this, void *parent,
                 int16_t x,
                 int16_t y,
                 void **image_data_array,
                 int array_lenght,
                 const char *widget_name)
{
    GUI_ASSERT(this);
    GUI_ASSERT(parent);
    GUI_ASSERT(image_data_array);
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, widget_name, x, y, 0, 0);
    base->type = IMAGE_FROM_MEM;
    base->obj_cb = cb;
    base->has_prepare_cb = true;
    base->has_draw_cb = true;
    base->has_end_cb = true;
    base->has_destroy_cb = true;
    base->w = ((gui_rgb_data_head_t *)image_data_array[0])->w * array_lenght;
    base->h = ((gui_rgb_data_head_t *)image_data_array[0])->h;
    this->image_data = gui_malloc(array_lenght * sizeof(void *));
    memcpy(this->image_data, image_data_array, array_lenght * sizeof(void *));
    this->array_count = array_lenght;


}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_image_array_write(gui_image_array_t *obj, void **image_data_array, int array_length)
{
    if (obj->image_data)
    {
        gui_free(obj->image_data);
        obj->image_data = 0;
    }
    obj->image_data = gui_malloc(array_length * sizeof(image_data_array));
    memcpy(obj->image_data, image_data_array, array_length * sizeof(void *));
    obj->array_count = array_length;

}
gui_image_array_t *gui_image_array_create(void *parent,
                                          int16_t x,
                                          int16_t y,
                                          void **image_data_array,
                                          int array_lenght,
                                          const char *widget_name)
{
#define _GUI_IMG_ARRAR_WIDGET_CTOR_PARAMETER_ this, parent, x, y, image_data_array, array_lenght, widget_name
    GUI_CREATE_HELPER(gui_image_array_t, ctor, _GUI_IMG_ARRAR_WIDGET_CTOR_PARAMETER_)
    return this;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
