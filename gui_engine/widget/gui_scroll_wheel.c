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

#include <guidef.h>
#include <gui_scroll_wheel.h>
#include <tp_algo.h>



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



void scroll_wheel_update_att(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    int child_count = 0;
    int child_gap = ((gui_scroll_wheel_t *)obj)->child_gap;
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        //obj->x = 0;
        obj->y = (child_count++) * child_gap;
    }
    page_update(obj);
    int current_row = 0;
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

    //gui_log("tp->type:%d\n", tp->type);
    if (tp->type == TOUCH_INVALIDE)
    {
        ((gui_scroll_wheel_t *)obj)->press_flag = true;
    }
    else if (((gui_scroll_wheel_t *)obj)->press_flag && tp->type == 0)
    {
        ((gui_scroll_wheel_t *)obj)->press_flag = false;
        //obj->y = obj->y + (((gui_page_t *)obj)->start_y - obj->y) % (((gui_scroll_wheel_t *)obj)->child_gap/2);
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
        gui_img_set_attribute(((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic->base.name,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic_hl_addr,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic->base.x,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 1].pic->base.y);
    }
    if ((current_row >= 0) && (((gui_scroll_wheel_t *)obj)->picture[current_row].pic))
    {
        gui_img_set_attribute(((gui_scroll_wheel_t *)obj)->picture[current_row].pic,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row].pic->base.name,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row].pic_addr,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row].pic->base.x,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row].pic->base.y);
    }
    if ((current_row + 2 >= 0) && (current_row + 2 < ((gui_scroll_wheel_t *)obj)->row_count) &&
        (((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic))
    {
        gui_img_set_attribute(((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic->base.name,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic_addr,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic->base.x,
                              ((gui_scroll_wheel_t *)obj)->picture[current_row + 2].pic->base.y);
    }
    // gui_log("current_row:%d\n",current_row);
    ((gui_scroll_wheel_t *)obj)->index = current_row + 1;
}

void gui_scrollwheel_append(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl)
{
    this->picture[this->row_count].pic = gui_img_create_from_mem(this, "num_pic", num_pic, 0, 0, 0, 0);
    rtgui_image_load_scale(&this->picture[this->row_count].pic->draw_img);
    gui_img_set_attribute(this->picture[this->row_count].pic,
                          this->picture[this->row_count].pic->base.name, this->picture[this->row_count].pic->draw_img.data,
                          (this->base.base.w - this->picture[this->row_count].pic->draw_img.img_h) / 2,
                          this->picture[this->row_count].pic->base.y);
    this->picture[this->row_count].pic_addr = num_pic;
    this->picture[this->row_count].pic_hl_addr = num_pic_hl;
    //gui_log("num:%d\n", (this->base.base.w - this->picture[this->row_count].pic->w) / 2);
    this->row_count++;
}

static uint32_t (get_index)(struct gui_scroll_wheel *this)
{
    return this->index;
}

void gui_scroll_wheel_ctor(struct gui_scroll_wheel *this, gui_obj_t *parent, int16_t x, int16_t y,
                           int16_t w, int16_t h)
{
    //for base class
    gui_page_t *base = (gui_page_t *)this;
    gui_page_ctor(base, parent, "SCROLL_WHEEL", x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->obj_prepare = scroll_wheel_update_att;
    root->type = SCROLL_WHEEL;

    //for self
    this->child_gap = 200 / 3;
    // this->highlight_start = (w - this->child_gap) / 2;
    this->highlight_start = (w + this->child_gap) / 2;
    this->get_index = get_index;
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_scroll_wheel_t *gui_scroll_wheel_create(void *parent, void *addr, int16_t x, int16_t y,
                                            int16_t w,
                                            int16_t h)
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
    // gui_img_create_from_mem(parent, "scroll_bg", addr, x, y, w, h);
    GET_BASE(this)->create_done = true;
    return this;
}


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */




