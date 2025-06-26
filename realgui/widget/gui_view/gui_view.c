/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
  * @brief the view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details switch the current view by sliding or clicking
  * @author shel_deng@realsil.com.cn
  * @date 2025/02/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_view.h"
#include "gui_obj.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define VIEW_FREE_EVENT GUI_EVENT_8

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_view_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    GUI_UNUSED(tp);

    obj->skip_tp_up_hold = 0;
    obj->skip_tp_down_hold = 0;
    obj->skip_tp_left_hold = 0;
    obj->skip_tp_right_hold = 0;
}

static void gui_view_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_view_t *this = (gui_view_t *)obj;
    uint8_t last = this->checksum;

    extern void gui_view_prepare_internal(gui_view_t *this);
    gui_view_prepare_internal(this);

    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, (uint8_t)sizeof(gui_view_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_view_destroy(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (current_view == this)
    {
        current_view = NULL;
    }

    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    for (uint32_t i = 0; i < this->on_event_num; i++)
    {
        gui_free(this->on_event[i]);
    }
}

static void gui_view_end(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (!tp->pressing && (this->release_x == 0) && (this->release_y == 0) &&
        this->style < VIEW_ANIMATION_NULL) // distinguish click event
    {
        this->view_tp = 1;
        if (this->cur_id.x != 0 || this->cur_id.y != 0)
        {
            gui_obj_enable_event(obj, VIEW_FREE_EVENT);
            obj->active = 1;
        }
        else
        {
            this->event = 0;
            this->view_switch_ready = true;
        }
    }
    if (this->moveback)
    {
        this->moveback = 0;
        if (this->view_switch_ready)
        {
            if (tp->pressing)
            {
                this->event = 0;
            }
        }
        else
        {
            gui_obj_enable_event(obj, VIEW_FREE_EVENT);
            obj->active = 1;
        }
    }
    if (this->view_switch_ready)
    {
        current_view = this;
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
                gui_view_input_prepare(obj);
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

        case OBJ_DESTROY:
            {
                gui_view_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_view_free(void *obj, gui_event_t e, void *param)
{
    gui_obj_tree_free(obj);
    // gui_log("[VIEW]free view done!\n");
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_view_t *gui_view_create(void       *parent,
                            const gui_view_descriptor_t *descriptor,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
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

    gui_obj_ctor(&this->base, parent, descriptor->name, x, y, w, h);

    GET_BASE(this)->obj_cb = gui_view_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->has_destroy_cb  = true;
    GET_BASE(this)->type = VIEW;
    GET_BASE(this)->create_done = true;
    gui_list_init(&(GET_BASE(this)->child_list));

    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    if ((GET_BASE(this)->parent)->child_list.next == (GET_BASE(this)->parent)->child_list.prev)
    {
        this->view_switch_ready = true;
        current_view = this;
    }
    else
    {
        this->view_switch_ready = false;
    }
    this->view_tp = 1;

    *descriptor->pView = this;
    descriptor->design_cb(this);
    gui_obj_add_event_cb(GET_BASE(this), gui_view_free, VIEW_FREE_EVENT, NULL);
    return this;
}


static void gui_view_on_event_cb(void *obj, gui_event_t e, void *param)
{
    gui_view_t *this = (gui_view_t *)obj;
    gui_view_on_event_t *on_event = (gui_view_on_event_t *)param;
    this->style = on_event->switch_out_style;
    gui_view_t *target_view = gui_view_create(GUI_BASE(this)->parent, on_event->descriptor, 0, 0, 0, 0);
    target_view->style = on_event->switch_in_style;
    target_view->event = 1;
    // change list for transplation cover
    if (target_view->style == VIEW_STILL)
    {
        gui_obj_t *parent = GUI_BASE(this)->parent;
        gui_obj_t *object = GUI_BASE(obj);
        gui_list_t *next_tab = object->brother_list.next;
        object->brother_list.prev = object->brother_list.next;
        object->brother_list.next = object->brother_list.next->next;
        next_tab->next = &object->brother_list;
        next_tab->prev = object->brother_list.next;
        GUI_BASE(parent)->child_list.next = next_tab;
        GUI_BASE(parent)->child_list.prev = next_tab->next;
    }
    if (target_view->style < VIEW_ANIMATION_NULL)
    {
        target_view->release_x = this->release_x;
        target_view->release_y = this->release_y;
    }
    int16_t idx = 0;
    int16_t idy = 0;
    extern void gui_view_set_animate(gui_view_t *this);
    switch (e)
    {
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        idx = 1;
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        idx = -1;
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        idy = 1;
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        idy = -1;
        break;
    case GUI_EVENT_TOUCH_CLICKED:
        {
            gui_view_set_animate(this);
            gui_view_set_animate(target_view);
        }
        break;
    case GUI_EVENT_TOUCH_LONG:
        {
            gui_view_set_animate(this);
            gui_view_set_animate(target_view);
        }
        break;
    case GUI_EVENT_KB_SHORT_CLICKED:
        {
            gui_view_set_animate(this);
            gui_view_set_animate(target_view);
        }
        break;
    case GUI_EVENT_KB_LONG_CLICKED:
        {
            gui_view_set_animate(this);
            gui_view_set_animate(target_view);
        }
        break;
    case GUI_EVENT_VIEW_SWITCH_DIRECT:
        {
            gui_view_set_animate(this);
            gui_view_set_animate(target_view);
        }
        break;
    default:
        break;
    }
    target_view->cur_id.x = idx;
    target_view->cur_id.y = idy;
}

void gui_view_switch_on_event(gui_view_t *this,
                              const gui_view_descriptor_t *descriptor,
                              VIEW_SWITCH_STYLE switch_out_style,
                              VIEW_SWITCH_STYLE switch_in_style,
                              gui_event_t event)
{
    gui_view_on_event_t *on_event = gui_malloc(sizeof(gui_view_on_event_t));
    on_event->descriptor = descriptor;
    on_event->switch_out_style = switch_out_style;
    on_event->switch_in_style = switch_in_style;
    on_event->event = event;
    this->on_event[this->on_event_num] = on_event;
    this->on_event_num++;
    GUI_ASSERT(this->on_event_num < EVENT_NUM_MAX);

    gui_obj_t *obj = (gui_obj_t *)this;
    gui_event_dsc_t *event_dsc_old = NULL;
    for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event)
        {
            event_dsc_old = event_dsc;
            break;
        }
    }
    if (event_dsc_old) // update same event
    {
        event_dsc_old->user_data = on_event;
    }
    else
    {
        gui_obj_add_event_cb(this, gui_view_on_event_cb, event, on_event);
    }
    switch (event)
    {
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        {
            this->view_left = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        {
            this->view_right = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        {
            this->view_up = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        {
            this->view_down = 1;
        }
        break;
    case GUI_EVENT_TOUCH_CLICKED:
        {
            this->view_click = 1;
        }
        break;
    case GUI_EVENT_TOUCH_LONG:
        {
            this->view_touch_long = 1;
        }
        break;
    case GUI_EVENT_KB_SHORT_CLICKED:
        {
            this->view_button = 1;
        }
        break;
    case GUI_EVENT_KB_LONG_CLICKED:
        {
            this->view_button_long = 1;
        }
        break;
    default:
        break;
    }

}

void gui_view_switch_direct(gui_view_t *this, const gui_view_descriptor_t *descriptor,
                            VIEW_SWITCH_STYLE switch_out_style,
                            VIEW_SWITCH_STYLE switch_in_style)
{
    if (!this->event)
    {
        gui_view_switch_on_event(this, descriptor, switch_out_style, switch_in_style,
                                 GUI_EVENT_VIEW_SWITCH_DIRECT);
        gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_VIEW_SWITCH_DIRECT);
        this->event = 1;
    }
}

gui_view_t *gui_view_get_current_view(void)
{
    return current_view;
}
