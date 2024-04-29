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
#include "gui_matrix.h"
#include "gui_tab.h"

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

    if (this->tab_cnt_left < this->cur_id.x)
    {
        gui_obj_skip_other_right_hold(obj);
    }

    if (this->tab_cnt_right > this->cur_id.x)
    {
        gui_obj_skip_other_left_hold(obj);
    }

    if (this->tab_cnt_up < this->cur_id.y)
    {
        gui_obj_skip_other_down_hold(obj);
    }

    if (this->tab_cnt_down > this->cur_id.y)
    {
        gui_obj_skip_other_up_hold(obj);
    }
}

static void gui_tabview_loop_unpadding(gui_tabview_t *tabview)
{
    gui_obj_t *object = (gui_obj_t *)tabview;
    gui_list_t *node = NULL;

    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        // left unpadding
        if (GUI_TYPE(gui_tab_t, obj)->id.x == -tabview->tab_cnt_left - 1)
        {
            GUI_TYPE(gui_tab_t, obj)->id.x = tabview->tab_cnt_right;
            tabview->loop_status = 0;
            break;
        }
        // right unpadding
        else if (GUI_TYPE(gui_tab_t, obj)->id.x == tabview->tab_cnt_right + 1)
        {
            GUI_TYPE(gui_tab_t, obj)->id.x = -tabview->tab_cnt_left;
            tabview->loop_status2 = 0;
            break;
        }
    }
}

static void gui_tabview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    gui_tabview_t *this = (gui_tabview_t *)obj;
    gui_tabview_tab_id_t last_cur_id = this->cur_id;
    uint8_t last = this->checksum;

    if (tabview->jump.jump_flag)
    {
        tabview->cur_id.y = tabview->jump.jump_id.y;
        tabview->cur_id.x = tabview->jump.jump_id.x;
        tabview->jump.jump_flag = false;

        // handle jump from one edge(hold) to the other edge.
        if ((tabview->cur_id.x == tabview->tab_cnt_left) || (tabview->cur_id.x == tabview->tab_cnt_right))
        {
            if ((tabview->loop) && (tabview->loop_status || tabview->loop_status2))
            {
                gui_tabview_loop_unpadding(tabview);
            }
        }
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

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if (!this->loop)
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
        break;

    case TOUCH_LEFT_SLIDE:
        {
            gui_log("[TV]TOUCH_LEFT_SLIDE\n");

            if (!this->loop)
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
                if (tabview->loop)
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

            if (!this->loop)
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
                if (tabview->loop)
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

            if ((tabview->tab_cnt_up == 0) && (tabview->cur_id.y == 0))
            {
                break;
            }

            //when current tab is the end,come back to current tab if sliding.
            if (tabview->cur_id.y == tabview->tab_cnt_up)
            {
                return;
            }

            tabview->cur_id.y = tabview->cur_id.y - 1;
            this->release_y = this->release_y - tabview->base.h;
            gui_obj_event_set(obj, GUI_EVENT_1);
        }
        break;

    case TOUCH_UP_SLIDE:
        {
            gui_log("[TV]TOUCH_UP_SLIDE\n");

            if ((tabview->tab_cnt_down == 0) && (tabview->cur_id.y == 0))
            {
                break;
            }

            //when current tab is the end,come back to current tab if sliding.
            if (tabview->cur_id.y == tabview->tab_cnt_down)
            {
                return;
            }

            tabview->cur_id.y = tabview->cur_id.y + 1;
            this->release_y = this->release_y + tabview->base.h;
            gui_obj_event_set(obj, GUI_EVENT_2);
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

    this->checksum = 0;
    this->checksum = gui_checksum(0, (uint8_t *)this, sizeof(gui_tabview_t));

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
            gui_tabview_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_tabview_prepare(obj);
            break;

        case OBJ_DESTORY:
            gui_tabview_destory(obj);
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
        this->center_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + w * h * dc->bit_depth /
                                       8);
        this->left_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + w * h * dc->bit_depth / 8);
        this->right_shot = gui_malloc(sizeof(gui_rgb_data_head_t) + w * h * dc->bit_depth /
                                      8);
    }
}

gui_tabview_t *gui_tabview_create(void       *parent,
                                  const char *filename,
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

    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

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

void gui_tabview_loop(gui_tabview_t *tabview, bool loop_or_not)
{
    tabview->loop = loop_or_not;
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
