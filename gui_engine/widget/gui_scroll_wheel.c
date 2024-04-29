/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_wheel.c
  * @brief scroll wheel effect
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

#include "gui_scroll_wheel.h"
#include "tp_algo.h"


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

void gui_scroll_wheel_update_att(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_scroll_wheel_t *this = (gui_scroll_wheel_t *)obj;
    gui_list_t *node = NULL;
    scroll_wheel_picture_t *picture;

    int child_count = 0;
    int child_gap = ((gui_scroll_wheel_t *)obj)->child_gap;
    int current_row = 0;

    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        o->y = (child_count++) * child_gap;
    }

    gui_page_update(obj);
    if ((((gui_page_t *)obj)->start_y - obj->y - ((gui_scroll_wheel_t *)obj)->child_gap / 2) <= 0)
    {
        current_row = 0;
    }
    else
    {
        current_row = ((((gui_page_t *)obj)->start_y - obj->y - ((gui_scroll_wheel_t *)obj)->child_gap /
                        2) / (((gui_scroll_wheel_t *)obj)->child_gap) + 1);
    }

    if (current_row + 2 > ((gui_scroll_wheel_t *)obj)->row_count)
    {
        current_row = ((gui_scroll_wheel_t *)obj)->row_count - 2;
    }

    if (tp->type == TOUCH_INVALIDE)
    {
        ((gui_scroll_wheel_t *)obj)->press_flag = true;
    }
    else if (((gui_scroll_wheel_t *)obj)->press_flag && tp->type == 0)
    {
        ((gui_scroll_wheel_t *)obj)->press_flag = false;

        if ((((gui_page_t *)obj)->start_y - obj->y - ((gui_scroll_wheel_t *)obj)->child_gap / 2) <= 0)
        {
            obj->y = ((gui_page_t *)obj)->start_y;
        }
        else
        {
            obj->y = ((gui_page_t *)obj)->start_y - current_row * ((gui_scroll_wheel_t *)obj)->child_gap;
        }
    }

    if ((current_row + 1 >= 0) && (((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic))
    {
        picture = &this->picture[current_row + 1];
        gui_img_set_attribute(picture->pic, picture->pic->base.name, picture->pic_hl_addr,
                              picture->pic->base.x, picture->pic->base.y);
    }

    if ((current_row >= 0) && (((gui_scroll_wheel_t *)obj)->picture[current_row].pic))
    {
        picture = &this->picture[current_row];
        gui_img_set_attribute(picture->pic, picture->pic->base.name, picture->pic_addr,
                              picture->pic->base.x, picture->pic->base.y);
    }

    if ((current_row + 2 >= 0) && (current_row + 2 < ((gui_scroll_wheel_t *)obj)->row_count)
        && (((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic))
    {
        picture = &this->picture[current_row + 2];
        gui_img_set_attribute(picture->pic, picture->pic->base.name, picture->pic_addr,
                              picture->pic->base.x, picture->pic->base.y);
    }

    ((gui_scroll_wheel_t *)obj)->index = current_row + 1;
}

void gui_scroll_wheel_append_core(gui_scroll_wheel_t   *this,
                                  void                 *num_pic,
                                  void                 *num_pic_hl,
                                  IMG_SOURCE_MODE_TYPE  src_mode)
{
    gui_img_t **img = &this->picture[this->row_count].pic;

    this->src_mode = src_mode;
    if (IMG_SRC_FILESYS == src_mode)
    {
        *img = gui_img_create_from_fs(this, "num_pic", num_pic, 0, 0, 0, 0);
    }
    else if (IMG_SRC_MEMADDR == src_mode)
    {
        this->picture[this->row_count].pic = gui_img_create_from_mem(this, "num_pic", num_pic, 0, 0, 0, 0);
    }

    gui_image_load_scale(&this->picture[this->row_count].pic->draw_img);

    this->picture[this->row_count].pic->base.x = (this->base.base.w -
                                                  this->picture[this->row_count].pic->draw_img.img_h) / 2;
    this->picture[this->row_count].pic_addr = num_pic;
    this->picture[this->row_count].pic_hl_addr = num_pic_hl;
    this->row_count++;
}

void gui_scroll_wheel_append(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl)
{
    gui_scroll_wheel_append_core(this, num_pic, num_pic_hl, IMG_SRC_MEMADDR);
}

void gui_scroll_wheel_append_from_fs(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl)
{
    gui_scroll_wheel_append_core(this, num_pic, num_pic_hl, IMG_SRC_FILESYS);
}

static uint32_t gui_scoll_wheel_get_index(struct gui_scroll_wheel *this)
{
    return this->index;
}

static void gui_scroll_wheel_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_scroll_wheel_update_att(obj);
            break;

        default:
            break;
        }
    }
}

void gui_scroll_wheel_ctor(struct gui_scroll_wheel *this,
                           gui_obj_t               *parent,
                           int16_t                  x,
                           int16_t                  y,
                           int16_t                  w,
                           int16_t                  h)
{
    //for base class
    gui_page_t *base = (gui_page_t *)this;
    gui_page_ctor(base, parent, "SCROLL_WHEEL", x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->obj_cb = gui_scroll_wheel_cb;
    root->has_prepare_cb = true;
    root->type = SCROLL_WHEEL;

    //for self
    this->child_gap = 200 / 3;
    this->highlight_start = (w + this->child_gap) / 2;
    this->get_index = gui_scoll_wheel_get_index;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_scroll_wheel_t *gui_scroll_wheel_create(void    *parent,
                                            void    *addr,
                                            int16_t  x,
                                            int16_t  y,
                                            int16_t  w,
                                            int16_t  h)
{
    GUI_ASSERT(parent != NULL);
    gui_scroll_wheel_t *this = gui_malloc(sizeof(gui_scroll_wheel_t));

    memset(this, 0, sizeof(gui_scroll_wheel_t));
    gui_scroll_wheel_ctor(this, parent, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
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
