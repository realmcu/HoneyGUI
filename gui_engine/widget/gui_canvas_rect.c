/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_rect_create.c
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
#include "gui_canvas_rect.h"
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include "acc_init.h"
#include <tp_algo.h>
#include "acc_engine.h"


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

static void gui_canvas_rect_prepare(gui_canvas_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
    this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(this->draw_img, 0x00, sizeof(draw_img_t));
    this->draw_img->blend_mode = IMG_RECT;
    this->draw_img->opacity_value = UINT8_MAX;
    gui_rgb_data_head_t *head = gui_malloc(sizeof(gui_rect_file_head_t));
    gui_rect_file_head_t *const_color = (gui_rect_file_head_t *)head;
    this->draw_img->data = head;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = RGBA8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = this->base.w;
    head->h = this->base.h;

    const_color->color = this->color;

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&this->draw_img->inverse);
    gui_image_load_scale(this->draw_img);
    gui_image_new_area(this->draw_img, NULL);

}

static void gui_canvas_rect_draw(gui_canvas_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    gui_acc_blit_to_dc(this->draw_img, dc, NULL);
}

static void gui_canvas_rect_end(gui_canvas_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    if (this->draw_img != NULL)
    {
        if (this->draw_img->data != NULL)
        {
            gui_free(this->draw_img->data);
            this->draw_img->data = 0;
        }
        gui_free(this->draw_img);
        this->draw_img = 0;
    }

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_canvas_rect_destory(gui_canvas_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_canvas_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_canvas_rect_prepare((gui_canvas_rect_t *)obj);
            break;

        case OBJ_DRAW:
            gui_canvas_rect_draw((gui_canvas_rect_t *)obj);
            break;

        case OBJ_END:
            gui_canvas_rect_end((gui_canvas_rect_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_canvas_rect_destory((gui_canvas_rect_t *)obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_canvas_rect_t *gui_canvas_rect_create(gui_obj_t   *parent,
                                          const char *name,
                                          int          x,
                                          int          y,
                                          int          w,
                                          int          h,
                                          gui_color_t  color)
{
    GUI_ASSERT(parent != NULL);
    gui_canvas_rect_t *canvas_rect = gui_malloc(sizeof(gui_canvas_rect_t));
    GUI_ASSERT(canvas_rect != NULL);
    memset(canvas_rect, 0x00, sizeof(gui_canvas_rect_t));
    gui_obj_ctor((gui_obj_t *)canvas_rect, parent, name, x, y, w, h);
    GET_BASE(canvas_rect)->obj_cb = gui_canvas_rect_cb;
    GET_BASE(canvas_rect)->has_prepare_cb = true;
    GET_BASE(canvas_rect)->has_draw_cb = true;
    GET_BASE(canvas_rect)->has_end_cb = true;
    GET_BASE(canvas_rect)->has_destroy_cb = true;
    gui_list_init(&(GET_BASE(canvas_rect)->child_list));
    if ((GET_BASE(canvas_rect)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(canvas_rect)->parent)->child_list),
                               &(GET_BASE(canvas_rect)->brother_list));
    }
    GET_BASE(canvas_rect)->create_done = true;
    canvas_rect->color = color;
    return canvas_rect;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
