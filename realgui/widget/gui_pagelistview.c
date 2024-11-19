/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelistview.h
  * @brief pagelistview widget
  * @details Pagelistview widget, to add top and bottom mask for pagelist widget.
*            Please refer to demo "example\screen_454_454_bb2plus_watch\gui_record\pagelist_test.c"
  * @author haonan_shi@realsil.com
  * @date 2023/12/15
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_obj.h"
#include "gui_pagelistview.h"
#include "gui_api.h"

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

static void gui_pagelistview_input_prepare(gui_pagelistview_t *this)
{

}

static void gui_pagelistview_prepare(gui_pagelistview_t *this)
{

}

static void  gui_pagelistview_draw(gui_pagelistview_t *this)
{

}

static void  gui_pagelistview_end(gui_pagelistview_t *this)
{

}

static void  gui_pagelistview_destory(gui_pagelistview_t *this)
{

}

static void gui_pagelistview_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_pagelistview_input_prepare((gui_pagelistview_t *)obj);
            break;

        case OBJ_PREPARE:
            gui_pagelistview_prepare((gui_pagelistview_t *)obj);
            break;

        case OBJ_DRAW:
            gui_pagelistview_draw((gui_pagelistview_t *)obj);
            break;

        case OBJ_END:
            gui_pagelistview_end((gui_pagelistview_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_pagelistview_destory((gui_pagelistview_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void gui_pagelistview_ctor(gui_pagelistview_t *this,
                                  gui_obj_t          *parent,
                                  const char         *name,
                                  int16_t             x,
                                  int16_t             y,
                                  int16_t             w,
                                  int16_t             h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_cb = gui_pagelistview_cb;
    root->has_input_prepare_cb = true;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->base.type = PAGELISTVIEW;
    this->top_mask = NULL;
    this->bottom_mask = NULL;
    this->top_mask_border = 0;
    this->bottom_mask_border = gui_get_screen_height();
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_pagelistview_t *gui_pagelistview_create(void       *parent,
                                            const char *name,
                                            int16_t     x,
                                            int16_t     y,
                                            int16_t     w,
                                            int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "PAGELISTVIEW";
    }

    gui_pagelistview_t *this = gui_malloc(sizeof(gui_pagelistview_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_pagelistview_t));

    gui_pagelistview_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

void gui_pagelistview_add_bottom_mask(gui_pagelistview_t *this, gui_img_t *mask_bottom)
{
    if (mask_bottom != NULL)
    {
        this->bottom_mask = mask_bottom;
        this->bottom_mask_border = (int)gui_get_screen_height() - mask_bottom->base.h;
    }
}

void gui_pagelistview_add_top_mask(gui_pagelistview_t *this, gui_img_t *mask_top)
{
    if (mask_top != NULL)
    {
        this->top_mask = mask_top;
        this->top_mask_border = mask_top->base.h;
    }
}

