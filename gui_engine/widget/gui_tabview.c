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

static void gui_tabview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *this = (gui_tabview_t *)obj;
    GUI_UNUSED(tp);
    GUI_UNUSED(this);
    // if (this->tab_cnt_left < this->cur_id.x)
    // {
    //     gui_obj_skip_other_right_hold(obj);
    // }
    // if (this->tab_cnt_right > this->cur_id.x)
    // {
    //     gui_obj_skip_other_left_hold(obj);
    // }

    // if (this->tab_cnt_up < this->cur_id.y)
    // {
    //     gui_obj_skip_other_down_hold(obj);
    // }

    // if (this->tab_cnt_down > this->cur_id.y)
    // {
    //     gui_obj_skip_other_up_hold(obj);
    // }
    obj->skip_tp_left_hold = false;
    obj->skip_tp_right_hold = false;
}

static void gui_tabview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    gui_tabview_t *this = (gui_tabview_t *)obj;
    gui_tabview_tab_id_t last_cur_id = this->cur_id;
    uint8_t last = this->checksum;

    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0))
    {
        gui_obj_event_set(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }
    if (tabview->jump.jump_flag)
    {
        tabview->cur_id.y = tabview->jump.jump_id.y;
        tabview->cur_id.x = tabview->jump.jump_id.x;
        tabview->jump.jump_flag = false;

        return;
    }

    if (tabview->tp_disable)
    {
        return;
    }

    if (tabview->cur_id.x != 0)
    {
        if ((tp->type == TOUCH_HOLD_Y) || (tp->type == TOUCH_DOWN_SLIDE) || (tp->type == TOUCH_UP_SLIDE))
        {
            return;
        }
    }

    if (tabview->cur_id.y != 0)
    {
        if ((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_LEFT_SLIDE) || (tp->type == TOUCH_RIGHT_SLIDE))
        {
            return;
        }
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
    {
        switch (tp->type)
        {
        case TOUCH_HOLD_X:
            {
                if (!this->loop_x)
                {
                    if ((obj->skip_tp_left_hold) && (tp->deltaX  < 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_right_hold) && (tp->deltaX  > 0))
                    {
                        break;
                    }

                    if ((tabview->tab_cnt_left == 0) && (tabview->tab_cnt_right == 0))
                    {
                        break;
                    }

                    this->release_x = tp->deltaX;

                    if ((tabview->cur_id.x == 0) && (tabview->tab_cnt_right == 0))
                    {
                        if (this->release_x < 0)
                        {
                            break;
                        }
                    }

                    if ((tabview->cur_id.x == 0) && (tabview->tab_cnt_left == 0))
                    {
                        if (this->release_x > 0)
                        {
                            this->release_x = 0;
                            break;
                        }
                    }
                }
                else
                {
                    this->release_x = tp->deltaX;
                }
            }
            break;

        case TOUCH_HOLD_Y:
            {
                if (!this->loop_y)
                {
                    if ((obj->skip_tp_up_hold) && (tp->deltaY  < 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_up_hold) && (obj->skip_tp_down_hold))
                    {
                        break;
                    }

                    if ((tabview->tab_cnt_down == 0) && (tabview->tab_cnt_up == 0))
                    {
                        break;
                    }

                    gui_obj_event_set(obj, GUI_EVENT_8);
                    this->release_y = tp->deltaY;

                    if ((tabview->cur_id.y == 0) && (tabview->tab_cnt_down == 0))
                    {
                        if (this->release_y < 0)
                        {
                            break;
                        }
                    }

                    if ((tabview->cur_id.y == 0) && (tabview->tab_cnt_up == 0))
                    {
                        if (this->release_y > 0)
                        {
                            this->release_y = 0;
                            break;
                        }
                    }
                }
                else
                {
                    this->release_y = tp->deltaY;
                }
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                gui_log("[TV]TOUCH_LEFT_SLIDE\n");

                if (!this->loop_x)
                {
                    if ((tabview->tab_cnt_right == 0) && (tabview->cur_id.x == 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_left_hold) && (tp->deltaX  < 0))
                    {
                        break;
                    }
                }

                //when current tab is the end,come back to current tab if sliding.
                if ((tabview->cur_id.x == tabview->tab_cnt_right))
                {
                    if (tabview->loop_x)
                    {
                        if (tabview->tab_cnt_left != 0)
                        {
                            tabview->cur_id.x = tabview->tab_cnt_left;
                        }
                        else
                        {
                            tabview->cur_id.x = 0;
                        }
                        this->release_x = this->release_x + tabview->base.w;
                    }
                }
                else
                {
                    tabview->cur_id.x = tabview->cur_id.x + 1;
                    this->release_x = this->release_x + tabview->base.w;
                }
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                gui_log("[TV]TOUCH_RIGHT_SLIDE\n");

                if (!this->loop_x)
                {
                    if ((tabview->tab_cnt_left == 0) && (tabview->cur_id.x == 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_right_hold) && (tp->deltaX  > 0))
                    {
                        break;
                    }
                }

                //when current tab is the end,come back to current tab if sliding.
                if (tabview->cur_id.x == tabview->tab_cnt_left)
                {
                    if (tabview->loop_x)
                    {
                        if (tabview->tab_cnt_right != 0)
                        {
                            tabview->cur_id.x = tabview->tab_cnt_right;
                        }
                        else
                        {
                            tabview->cur_id.x = 0;
                        }
                        this->release_x = this->release_x - tabview->base.w;
                    }
                }
                else
                {
                    tabview->cur_id.x = tabview->cur_id.x - 1;
                    this->release_x = this->release_x - tabview->base.w;
                }
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                gui_log("[TV]TOUCH_DOWN_SLIDE\n");

                if (!this->loop_y)
                {
                    if ((tabview->tab_cnt_up == 0) && (tabview->cur_id.y == 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_down_hold) && (tp->deltaY  > 0))
                    {
                        break;
                    }
                }

                //when current tab is the end,come back to current tab if sliding.
                if (tabview->cur_id.y == tabview->tab_cnt_up)
                {
                    if (tabview->loop_y)
                    {
                        if (tabview->tab_cnt_down != 0)
                        {
                            tabview->cur_id.y = tabview->tab_cnt_down;
                        }
                        else
                        {
                            tabview->cur_id.y = 0;
                        }
                        this->release_y = this->release_y - tabview->base.h;
                    }
                }
                else
                {
                    tabview->cur_id.y = tabview->cur_id.y - 1;
                    this->release_y = this->release_y - tabview->base.h;
                }
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                gui_log("[TV]TOUCH_UP_SLIDE\n");

                if (!this->loop_y)
                {
                    if ((tabview->tab_cnt_down == 0) && (tabview->cur_id.y == 0))
                    {
                        break;
                    }

                    if ((obj->skip_tp_up_hold) && (tp->deltaY < 0))
                    {
                        break;
                    }
                }

                //when current tab is the end,come back to current tab if sliding.
                if (tabview->cur_id.y == tabview->tab_cnt_down)
                {
                    if (tabview->loop_y)
                    {
                        if (tabview->tab_cnt_up != 0)
                        {
                            tabview->cur_id.y = tabview->tab_cnt_up;
                        }
                        else
                        {
                            tabview->cur_id.y = 0;
                        }
                        this->release_y = this->release_y + tabview->base.h;
                    }
                }
                else
                {
                    tabview->cur_id.y = tabview->cur_id.y + 1;
                    this->release_y = this->release_y + tabview->base.h;
                }
            }
            break;

        case TOUCH_ORIGIN_FROM_X:
            {

            }
            break;

        case TOUCH_ORIGIN_FROM_Y:
            {

            }
            break;

        default:
            break;
        }
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
        gui_obj_event_set(obj, (gui_event_t)TABVIEW_EVENT_TAB_CHANGE);
        this->tab_change_ready = false;
        this->initial = false;
        this->tab_need_pre_load = true;

        gui_log("do cache right and left \n");
    }
    else
    {
        this->tab_need_pre_load = false;
    }
}

void gui_tabview_destory(gui_obj_t *obj)
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

        case OBJ_DESTORY:
            {
                gui_tabview_destory(obj);
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

    if (this->style == TAB_CUBE)
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
    gui_obj_add_event_cb(tabview, (gui_event_cb_t)callback, (gui_event_t)TABVIEW_EVENT_TAB_CHANGE,
                         parameter);
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
