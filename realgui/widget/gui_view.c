/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
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
#include "gui_view.h"
#include "kb_algo.h"
#include "gui_img.h"

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
static VIEW_SLIDE_STYLE style;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void pre_load_view(gui_view_t *this);

static void obj_pave_shot_cb(void *p, void *this_widget, gui_animate_t *animate)
{
    gui_img_t *pave_shot = (gui_img_t *)this_widget;
    matrix_identity(GUI_BASE(pave_shot)->matrix);
}

static void gui_view_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_view_t *view = (gui_view_t *)obj;
    gui_view_t *this = (gui_view_t *)obj;
    uint8_t last = this->checksum;

    if (this->initial)
    {
        this->initial = false;
        GET_BASE(this)->has_prepare_cb = false;
        pre_load_view(this);
        GET_BASE(this)->has_prepare_cb = true;
    }

    if (this->view_change_ready)
    {
        style = this->style;
    }
    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0))
    {
        gui_obj_event_set(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }

    if (view->tp_disable)
    {
        return;
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
    {
        switch (tp->type)
        {
        case TOUCH_HOLD_X:
            {
                this->release_x = tp->deltaX;
                if (this->view_change_ready && this->release_x < 0)
                {
                    gui_obj_event_set(obj, (gui_event_t)VIEW_EVENT_MOVE_LEFT);
                }
                else if ((this->view_change_ready && this->release_x > 0))
                {
                    gui_obj_event_set(obj, (gui_event_t)VIEW_EVENT_MOVE_RIGHT);
                }
            }
            break;

        case TOUCH_HOLD_Y:
            {
                this->release_y = tp->deltaY;
                if (this->view_change_ready && this->release_y < 0)
                {
                    gui_obj_event_set(obj, (gui_event_t)VIEW_EVENT_MOVE_UP);
                }
                else if (this->view_change_ready && this->release_y > 0)
                {
                    gui_obj_event_set(obj, (gui_event_t)VIEW_EVENT_MOVE_DOWN);
                }
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                if (this->view_left || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_LEFT_SLIDE\n");
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE);
                    this->cur_id.x -= 1;
                    this->release_x = this->release_x + view->base.w;
                }
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                if (this->view_right || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_RIGHT_SLIDE\n");
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE);
                    this->cur_id.x += 1;
                    this->release_x = this->release_x - view->base.w;
                }
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                if (this->view_down || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_DOWN_SLIDE\n");
                    this->cur_id.y += 1;
                    this->release_y = this->release_y - view->base.h;
                }
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                if (this->view_up || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_UP_SLIDE\n");
                    this->cur_id.y -= 1;
                    this->release_y = this->release_y + view->base.h;
                }
            }
            break;
        default:
            break;
        }
    }

    if (style == VIEW_ROTATE_BOOK)
    {
        if (this->shot_pave_obj == NULL)
        {
            this->shot_pave_obj = (gui_obj_t *)gui_img_create_from_mem(obj, "shot_1", (void *)this->shot, 0, 0,
                                                                       0, 0);
            gui_img_set_mode((gui_img_t *)(this->shot_pave_obj), IMG_BYPASS_MODE);
            gui_img_set_mode((gui_img_t *)(this->shot_obj), IMG_BYPASS_MODE);
            gui_img_set_animate((gui_img_t *)(this->shot_pave_obj), 1000, -1, obj_pave_shot_cb, (void *)obj);
        }
        if (this->release_x != 0)
        {
            gui_obj_show(this->shot_pave_obj, true);
        }
        else
        {
            gui_obj_show(this->shot_pave_obj, false);
        }
    }
    else if (this->shot_pave_obj)
    {
        gui_obj_show(this->shot_pave_obj, false);
    }

    if (this->release_x != 0)
    {
        gui_obj_show(this->shot_obj, true);
        gui_obj_show(this->rte_obj, false);
    }
    else
    {
        gui_obj_show(this->shot_obj, false);
        gui_obj_show(this->rte_obj, true);
    }

    if (tp->pressing == false)
    {
        // this->release_x = 0;
        // this->release_y = 0;
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

    if (style == VIEW_CLASSIC)
    {
        matrix_translate(this->cur_id.x * (int)this->base.w + this->release_x, \
                         this->cur_id.y * (int)this->base.h + this->release_y, \
                         obj->matrix);
    }
    else if (style == VIEW_CUBE)
    {
        gui_view_cube(obj, this->cur_id.x, this->cur_id.y);
    }
    else if (style == VIEW_ROTATE)
    {
        gui_view_rotate(obj, this->cur_id.x, this->cur_id.y);
    }
    else if (style == VIEW_ROTATE_BOOK)
    {
        gui_view_rotate_book(obj, this->cur_id.x, this->cur_id.y);
    }
    else if (style == VIEW_REDUCTION)
    {
        gui_view_reduction(obj, this->cur_id.x, this->cur_id.y);
    }

    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_view_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void pre_load_view(gui_view_t *this)
{
    gui_obj_t *obj = GUI_BASE(this);
    gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, this->shot);

    if (this->shot_obj == NULL)
    {
        this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj, "shot", (void *)this->shot, 0,
                                                              0, 0, 0);
    }
}

static void gui_view_free(void *obj, gui_event_t e, void *param)
{
    gui_obj_tree_free(obj);
    gui_log("[VIEW]free view done!\n");
}

void gui_view_destory(gui_obj_t *obj)
{
    gui_view_t *this = (gui_view_t *)obj;
    if (this->shot != NULL)
    {
        gui_free(this->shot);
        this->shot = NULL;
    }
}

static void gui_view_end(gui_obj_t *obj)
{
    gui_view_t *this = (gui_view_t *)obj;

    if ((this->release_x == 0) && (this->release_y == 0))
    {

        if (this->cur_id.x != 0 || this->cur_id.y != 0)
        {
            if (!this->initial)
            {
                gui_obj_event_set(obj, GUI_EVENT_8);
                obj->active = 1;
            }
        }
        else
        {
            this->view_change_ready = true;
        }
        // if (!this->view_change_ready)
        // {

        // if (this->cur_id.x == 0 && this->cur_id.y == 0)
        // {
        //     gui_obj_t *view_first = gui_list_entry((&(obj->parent->child_list))->next, gui_obj_t, brother_list);
        //     gui_obj_tree_free(view_first);
        //     this->view_change_ready = true;
        //     gui_log("[VIEW]change view done \n");
        // }
        // else
        // {
        //     gui_obj_tree_free(obj);
        // }
        // }
        // else
        // {
        // if (this->cur_id.x == 0 && this->cur_id.y == 0)
        // {
        //     gui_list_t list = GUI_BASE(obj)->parent->child_list;
        //     if (list.prev != list.next)
        //     {
        //         gui_obj_t *view_second = gui_list_entry((&list)->prev, gui_obj_t, brother_list);
        //         gui_obj_tree_free(view_second);
        //     }
        // }
        // }
    }
}

static void gui_view_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                // gui_view_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_view_prepare(obj);
            }
            break;

        case OBJ_END:
            {
                gui_view_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                gui_view_destory(obj);
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
void gui_view_set_style(gui_view_t *this, VIEW_SLIDE_STYLE style)
{
    this->style = style;
}

void gui_view_enable_pre_load(gui_view_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    uint16_t w = this->base.w;
    uint16_t h = this->base.h;
    uint16_t bit_depth = dc->bit_depth;

    uint32_t size = (uint32_t) w * h * bit_depth / 8;

    this->shot = gui_malloc(sizeof(gui_rgb_data_head_t) + size);

    if (!this->shot)
    {
        gui_log("[VIEW]Error: Failed to allocate memory for pre-load shot.");
    }
}

gui_view_t *gui_view_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h,
                            int16_t     idx,
                            int16_t     idy)
{
    // gui_list_t list = GUI_BASE(parent)->child_list;
    // if (list.prev != list.next)
    // {
    //     gui_obj_t *obj = gui_list_entry(list.prev, gui_obj_t, brother_list);
    //     gui_obj_tree_free(obj);
    // }

    gui_view_t *this = gui_malloc(sizeof(gui_view_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_view_t));

    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_obj_ctor(&this->base, parent, name, x, y, w, h);

    GET_BASE(this)->obj_cb = gui_view_cb;
    // GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->type = TABVIEW;
    GET_BASE(this)->create_done = true;
    this->style = VIEW_CLASSIC;
    this->initial = true;
    this->cur_id.x = idx;
    this->cur_id.y = idy;
    gui_view_enable_pre_load(this);
    gui_list_init(&(GET_BASE(this)->child_list));
    gui_obj_add_event_cb(GET_BASE(this), gui_view_free, GUI_EVENT_8, NULL);
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    if ((GET_BASE(this)->parent)->child_list.next == (GET_BASE(this)->parent)->child_list.prev)
    {
        this->view_change_ready = true;
    }
    else
    {
        this->view_change_ready = false;
    }
    this->rte_obj = gui_obj_create(this, "tab_rte", this->base.x, this->base.y, this->base.w,
                                   this->base.h);
    this->rte_obj->type = IMAGE_FROM_MEM;
    return this;
}


void gui_view_tp_disable(gui_view_t *view, bool disable_tp)
{
    view->tp_disable = disable_tp;
}

void gui_view_add_change_event(gui_view_t *view, gui_view_t *target_view, void *callback,
                               T_GUI_VIEW_EVENT event)
{
    gui_obj_add_event_cb(view, (gui_event_cb_t)callback, (gui_event_t)event,
                         target_view);
    switch (event)
    {
    case VIEW_EVENT_MOVE_LEFT:
        view->view_left = 1;
        break;
    case VIEW_EVENT_MOVE_RIGHT:
        view->view_right = 1;
        break;
    case VIEW_EVENT_MOVE_UP:
        view->view_up = 1;
        break;
    case VIEW_EVENT_MOVE_DOWN:
        view->view_down = 1;
        break;
    default:
        break;
    }
}
