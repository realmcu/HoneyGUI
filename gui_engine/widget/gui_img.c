/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.c
  * @brief image widget
  * @details image widget is used to show image on the screen
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
#include <gui_img.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
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
void gui_img_set_animate(gui_img_t *o, uint32_t dur, int repeatCount, void *callback,
                         void *p)
{
    gui_img_t *a = (o);
    GUI_UNUSED(a);

    gui_animate_t *animate = ((gui_img_t *)o)->animate;
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
    ((gui_img_t *)o)->animate = animate;
}
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_img_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * 30 / (1000);
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
static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    obj_update_att(obj);
    gui_img_t *this = (gui_img_t *)obj;
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &this->draw_img;

    rtgui_image_load_scale(draw_img);
    root->w = draw_img->img_w;
    root->h = draw_img->img_h;
    matrix_identity(draw_img->matrix);
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
    rtgui_image_new_area(draw_img);
    if (tp->type == TOUCH_SHORT)
    {
        gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
    }
    gui_img_set_opacity((void *)obj, obj->opacity_value);

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
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

static void magic_img_destory(gui_obj_t *obj)
{
    gui_free(((gui_img_t *)obj)->draw_img.inverse);
    gui_free(((gui_img_t *)obj)->draw_img.matrix);
}

static void gui_img_from_mem_ctor(gui_img_t *this, gui_obj_t *parent, const char *name,
                                  void *addr,
                                  int16_t x,
                                  int16_t y, int16_t w, int16_t h)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)this;
    draw_img_t *draw_img = &this->draw_img;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_prepare = img_prepare;
    root->obj_draw = img_draw_cb;
    root->obj_end = img_end;
    root->obj_destory = magic_img_destory;
    root->type = IMAGE_FROM_MEM;

    //draw_img->blend_mode = IMG_FILTER_BLACK;
    draw_img->data = addr;
    draw_img->opacity_value = 255;
    draw_img->blend_mode = IMG_BYPASS_MODE;
    draw_img->matrix = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->inverse = gui_malloc(sizeof(struct rtgui_matrix));
    draw_img->opacity_value = UINT8_MAX;

    this->scale_x = 1.0f;
    this->scale_y = 1.0f;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

uint16_t gui_img_get_width(gui_img_t *img)
{
    draw_img_t *draw_img = &img->draw_img;
    struct gui_rgb_data_head head;
    memcpy(&head, draw_img->data, sizeof(head));
    return head.w;
}

uint16_t gui_img_get_height(gui_img_t *img)
{
    draw_img_t *draw_img = &img->draw_img;
    struct gui_rgb_data_head head;
    memcpy(&head, draw_img->data, sizeof(head));
    return head.h;
}

void gui_img_set_location(gui_img_t *img, uint16_t x, uint16_t y)
{
    gui_obj_t *root = (gui_obj_t *)img;
    root->x = x;
    root->y = y;
}

void gui_img_set_mode(gui_img_t *img, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(img != NULL);
    struct gui_dispdev *dc = gui_get_dc();
    draw_img_t *draw_img = &img->draw_img;
    draw_img->blend_mode = mode;
}

void gui_img_set_attribute(gui_img_t *img, const char *filename, void *addr, int16_t x,
                           int16_t y)
{
    GUI_ASSERT(img != NULL);
    img->base.x = x;
    img->base.y = y;
    if (addr != NULL)
    {
        draw_img_t *draw_img = &img->draw_img;
        draw_img->blend_mode = IMG_FILTER_MATRIX;
        draw_img->data = addr;
    }
}

void gui_img_rotation(gui_img_t *img, float degrees, float c_x, float c_y)
{
    GUI_ASSERT(img != NULL);
    draw_img_t *draw_img = &img->draw_img;
    draw_img->blend_mode = IMG_FILTER_MATRIX;
    img->degrees = degrees;
    img->c_x = c_x;
    img->c_y = c_y;
}

void gui_img_scale(gui_img_t *img, float scale_x, float scale_y)
{
    GUI_ASSERT(img != NULL);
    draw_img_t *draw_img = &img->draw_img;
    draw_img->blend_mode = IMG_FILTER_MATRIX;
    if (scale_x > 0 && scale_y > 0)
    {
        img->scale_x = scale_x;
        img->scale_y = scale_y;
    }
}

void gui_img_translate(gui_img_t *img, float t_x, float t_y)
{
    GUI_ASSERT(img != NULL);
    draw_img_t *draw_img = &img->draw_img;
    draw_img->blend_mode = IMG_FILTER_MATRIX;
    img->t_x = t_x;
    img->t_y = t_y;
}

void gui_img_skew_x(gui_img_t *img, float degrees)
{

}

void gui_img_skew_y(gui_img_t *img, float degrees)
{

}

void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value)
{
    this->draw_img.opacity_value = opacity_value;
}

gui_img_t *gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                   int16_t x,
                                   int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "MAGIC_IMG";
    }
    gui_img_t *img = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_img_t));

    gui_img_from_mem_ctor(img, (gui_obj_t *)parent, name, addr, x, y, w, h);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}
gui_img_t *gui_img_create_from_fs(void *parent, const char *file, int16_t x, int16_t y)
{
    return gui_img_create_from_mem(parent, "image", gui_get_file_address(file), x, y, 0, 0);
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
