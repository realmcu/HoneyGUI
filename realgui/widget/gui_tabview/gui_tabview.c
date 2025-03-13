/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tabview.c
  * @brief Extended in the cross direction
  * @details switch the currently displayed screen by sliding
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/24
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
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_tabview.h"
#include "kb_algo.h"

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

static void gui_tabview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *this = (gui_tabview_t *)obj;
    GUI_UNUSED(tp);
    GUI_UNUSED(this);

}

static void gui_tabview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    gui_tabview_t *this = (gui_tabview_t *)obj;
    GUI_UNUSED(this);
    GUI_UNUSED(tabview);

    gui_tabview_tab_id_t last_cur_id = this->cur_id;
    uint8_t last = this->checksum;


    gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE);


    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_tabview_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }

    if ((last_cur_id.x != this->cur_id.x) || (last_cur_id.y != this->cur_id.y))
    {
        this->tab_change_ready = true;
    }

    if (((this->tab_change_ready == true) && (this->release_x == 0) && (this->release_y == 0)) ||
        (dc->frame_count == 1) || this->initial)
    {
        //gui_obj_enable_event(obj, (gui_event_t)TABVIEW_EVENT_TAB_CHANGE);
        GUI_ASSERT(0);//SEND MESSAGE TO SERVER
        this->tab_change_ready = false;
        this->initial = false;
        this->tab_need_pre_load = true;

        gui_log("do cache right and left \n");
    }
    else
    {
        this->tab_need_pre_load = false;
    }
    if (tp->pressing == false)
    {
        if (this->release_x >= GUI_FRAME_STEP)
        {
            this->release_x -= GUI_FRAME_STEP;
        }
        else if (this->release_x <= -GUI_FRAME_STEP)
        {
            this->release_x += GUI_FRAME_STEP;
        }
        else
        {
            this->release_x = 0;
        }

        if (this->release_y >= GUI_FRAME_STEP)
        {
            this->release_y -= GUI_FRAME_STEP;
        }
        else if (this->release_y <= -GUI_FRAME_STEP)
        {
            this->release_y += GUI_FRAME_STEP;
        }
        else
        {
            this->release_y = 0;
        }
    }
}

void gui_tabview_destroy(gui_obj_t *obj)
{
    gui_tabview_t *this = (gui_tabview_t *)obj;

    if (this->enable_pre_load == true)
    {
        if (this->center_shot != NULL)
        {
            gui_free(this->center_shot);
            this->center_shot = NULL;
        }

        if (this->right_shot != NULL)
        {
            gui_free(this->right_shot);
            this->right_shot = NULL;
        }

        if (this->left_shot != NULL)
        {
            gui_free(this->left_shot);
            this->left_shot = NULL;
        }
    }
}

static void gui_tabview_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_tabview_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_tabview_prepare(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_tabview_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_tabview_set_style(gui_tabview_t *this, T_SLIDE_STYLE style)
{
    this->style = style;
}

void gui_tabview_enable_pre_load(gui_tabview_t *this, bool enable)
{
    gui_dispdev_t *dc = gui_get_dc();

    if (this->style == TAB_CUBE || this->style == TAB_ROTATE_BOOK)
    {
        this->enable_pre_load = enable;
    }

    if (this->enable_pre_load == true)
    {
        uint16_t w = this->base.w;
        uint16_t h = this->base.h;
        uint16_t bit_depth = dc->bit_depth;

        uint32_t size = (uint32_t) w * h * bit_depth / 8;

        this->center_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + size);
        this->left_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + size);
        this->right_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + size);

        if (!this->center_shot || !this->left_shot || !this->right_shot)
        {
            gui_log("Error: Failed to allocate memory for pre-load shots.");
        }
    }

}

gui_tabview_t *gui_tabview_create(void       *parent,
                                  const char *name,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h)
{
    gui_tabview_t *this = gui_malloc(sizeof(gui_tabview_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_tabview_t));

    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_obj_ctor(&this->base, parent, name, x, y, w, h);

    GET_BASE(this)->obj_cb = gui_tabview_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->type = TABVIEW;
    this->initial = true;
    gui_list_init(&(GET_BASE(this)->child_list));

    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

void gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy)
{
    parent_tabview->jump.jump_flag = true;
    parent_tabview->jump.jump_id.x = idx;
    parent_tabview->jump.jump_id.y = idy;
}

void gui_tabview_loop_x(gui_tabview_t *tabview, bool loop)
{
    tabview->loop_x = loop;
}

void gui_tabview_loop_y(gui_tabview_t *tabview, bool loop)
{
    tabview->loop_y = loop;
}

void gui_tabview_tp_disable(gui_tabview_t *tabview, bool disable_tp)
{
    tabview->tp_disable = disable_tp;
}

void gui_tabview_tab_change(gui_tabview_t *tabview, void *callback, void *parameter)
{
    //gui_obj_add_event_cb(tabview, (gui_event_cb_t)callback, (gui_event_t)TABVIEW_EVENT_TAB_CHANGE, parameter);
    GUI_ASSERT(0);//SEND MESSAGE TO SERVER
}
