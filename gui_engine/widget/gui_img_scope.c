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
#include <guidef.h>
#include <gui_img_scope.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include <kb_algo.h>
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



static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    GUI_TYPE(gui_img_t, obj)->draw_img.opacity_value = obj->parent->opacity_value * GUI_TYPE(gui_img_t,
                                                       obj)->opacity / UINT8_MAX;

    {
        gui_dispdev_t *dc = gui_get_dc();
        touch_info_t *tp = tp_get_info();

        if (((obj->ax + obj->tx) < (int)gui_get_screen_width()) && (((obj->ax + obj->tx) + obj->w) >= 0) &&
            \
            ((obj->ay + obj->ty) < (int)gui_get_screen_height()) && (((obj->ay + obj->ty) + obj->h) >= 0))

        {
            gui_img_t *b = (void *)obj;
            if (tp->pressed)
            {

                if ((tp->x >= obj->ax + obj->tx && tp->x <= (obj->ax + obj->tx  + obj->w)) &&
                    (tp->y >= obj->ay + obj->ty && tp->y <= (obj->ay + obj->ty + obj->h)))
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
    }
    gui_img_t *this = (gui_img_t *)obj;
    gui_obj_t *root = (gui_obj_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    draw_img_t *draw_img = &this->draw_img;

    gui_image_load_scale(draw_img);
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

    memcpy(draw_img->inverse, draw_img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(draw_img->inverse);
    gui_image_new_area(draw_img);

    int sx = obj->dx + obj->ax + obj->tx;
    int sy = obj->dy + obj->ay + obj->ty;
    int ex = sx + obj->w;
    int ey = sy + obj->h;

    if ((tp->x >= sx && tp->x <= ex) && (tp->y >= sy && tp->y <= ey))
    {
        if ((tp->type == TOUCH_SHORT) && (obj->event_dsc_cnt > 0))
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
        }

    }


    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
static void img_scope_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();
    draw_img_t *draw_img = &img->draw_img;

    gui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->ax;
    draw_rect.y1 = obj->ay;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;
    draw_rect.xboundleft = GUI_TYPE(gui_img_scope_t, obj)->scope_x1;
    draw_rect.xboundright = GUI_TYPE(gui_img_scope_t, obj)->scope_x2;
    draw_rect.yboundbottom = GUI_TYPE(gui_img_scope_t, obj)->scope_y2;
    draw_rect.yboundtop = GUI_TYPE(gui_img_scope_t, obj)->scope_y1;

    draw_rect.x2 = _UI_MIN(draw_rect.x2, draw_rect.x1 + draw_rect.xboundright);
    draw_rect.y2 = _UI_MIN(draw_rect.y2, draw_rect.y1 + draw_rect.yboundbottom);

    if ((draw_rect.xboundleft == draw_rect.xboundright) ||
        (draw_rect.yboundbottom == draw_rect.yboundtop))
    {
        return;
    }

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
static void img_destory(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_log("do obj %s free\n", obj->name);
    //gui_free(obj);
}

void gui_img_scope_ctor(gui_img_t *this, gui_obj_t *parent, const char *name, void *addr,
                        int16_t x,
                        int16_t y, int16_t w, int16_t h)
{
    draw_img_t *draw_img = &this->draw_img;
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = IMAGE_SCOPE;
    root->obj_prepare = img_prepare;
    root->obj_draw = img_scope_draw_cb;
    root->obj_end = img_end;
    root->obj_destory = img_destory;
    //for self

    draw_img->data = addr;
    draw_img->blend_mode = IMG_FILTER_BLACK;
    draw_img->matrix = gui_malloc(sizeof(struct gui_matrix));
    draw_img->inverse = gui_malloc(sizeof(struct gui_matrix));
    draw_img->opacity_value = UINT8_MAX;

    this->scale_x = 1.0f;
    this->scale_y = 1.0f;
    this->opacity = draw_img->opacity_value;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_img_scope_t *gui_img_scope_create(void *parent, void *addr, int16_t x, int16_t y)
{
    GUI_ASSERT(parent != NULL);
    char     *name = "DEFAULT_IMG";

    gui_img_scope_t *img = gui_malloc(sizeof(gui_img_scope_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_img_scope_t));

    gui_img_scope_ctor((void *)img, (gui_obj_t *)parent, name, addr, x, y, 0, 0);
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








