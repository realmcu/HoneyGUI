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
#include <string.h>
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "kb_algo.h"
#include "gui_img.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
//#define VIEW_FREE_EVENT GUI_EVENT_8

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
void gui_view_free(void *msg);
static void gui_view_moveback_judge(gui_view_t *this);

static void gui_view_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    GUI_UNUSED(tp);
}

static void gui_view_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_view_t *this = (gui_view_t *)obj;
    VIEW_SWITCH_STYLE style = this->style;

    if (this->event && style < VIEW_ANIMATION_NULL) // do not update release during animation
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
    }

    // touch
    if (!this->event && this->view_switch_ready)
    {
        this->event |= (this->view_click       && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED));
        this->event |= (this->view_touch_long  && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG));
        if (this->event)
        {
            this->style = VIEW_ANIMATION_NULL;
            return;
        }
    }
    // keyboard
    if (!this->event && this->view_switch_ready)
    {
        this->event |= (this->view_button      && gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED));
        this->event |= (this->view_button_long && gui_obj_enable_event(obj, GUI_EVENT_KB_LONG_CLICKED));
        if (this->event)
        {
            this->style = VIEW_ANIMATION_NULL;
            return;
        }
    }
    // move
    if (tp->pressing && style < VIEW_ANIMATION_NULL) // do not judge moveback during animation
    {
        gui_view_moveback_judge(this);
    }
    if ((!this->event || this->moveback) && this->view_switch_ready)
    {
        this->event |= (this->view_left  && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT));
        this->event |= (this->view_right && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT));
        this->event |= (this->view_up    && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_UP));
        this->event |= (this->view_down  && gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_DOWN));
    }

    // restore view offset
    if (!this->view_tp)
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

    extern void gui_view_reduction(gui_view_t *this);
    extern void gui_view_cube(gui_view_t *this);
    extern void gui_view_rotate(gui_view_t *this);
    extern void gui_view_rotate_book(gui_view_t *this);

    if (style == VIEW_TRANSPLATION)
    {
        matrix_translate(this->cur_id.x * (int)this->base.w + this->release_x, \
                         this->cur_id.y * (int)this->base.h + this->release_y, \
                         obj->matrix);
    }
    else if (style == VIEW_CUBE)
    {
        gui_view_cube(this);
    }
    else if (style == VIEW_ROTATE)
    {
        gui_view_rotate(this);
    }
    else if (style == VIEW_REDUCTION)
    {
        gui_view_reduction(this);
    }

    // animation update
    animate_frame_update(this->animate, obj);

    uint8_t last = this->checksum;
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

    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    for (uint32_t i = 0; i < this->on_event_num; i++)
    {
        gui_free(this->on_event[i]);
        this->on_event[i] = NULL;
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
            if (!this->descriptor->keep)
            {
                gui_msg_t msg =
                {
                    .event = GUI_EVENT_USER_DEFINE,
                    .cb = gui_view_free,
                    .payload = this,
                };
                gui_send_msg_to_server(&msg);
                // this->descriptor->created = false;
            }
            else
            {
                obj->not_show = true;
                obj->has_prepare_cb = false;
                obj->has_end_cb = false;
            }
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
        if (!this->view_switch_ready)
        {
            if (!this->descriptor->keep)
            {
                gui_msg_t msg =
                {
                    .event = GUI_EVENT_USER_DEFINE,
                    .cb = gui_view_free,
                    .payload = this,
                };
                gui_send_msg_to_server(&msg);
            }
            else
            {
                obj->not_show = true;
                obj->has_prepare_cb = false;
                obj->has_end_cb = false;
            }
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

static void gui_view_adjust_list(gui_obj_t *old, gui_obj_t *new)
{
    gui_list_t *list1 = &old->brother_list;
    gui_list_t *list2 = &new->brother_list;

    // Get adjacent pointers
    gui_list_t *list1_next = list1->next;
    gui_list_t *list2_prev = list2->prev;
    gui_list_t *list2_next = list2->next;

    // If node2 is already immediately after node1, no action needed
    if (list1_next == list2)
    {
        return;
    }

    //Remove node2 from its original position
    list2_prev->next = list2_next;
    list2_next->prev = list2_prev;

    //Insert node2 after node1
    list2->next = list1_next;    // node2->next points to node1's original next
    list2->prev = list1;         // node2->prev points to node1
    list1_next->prev = list2;    // node1's original next's prev points to node2
    list1->next = list2;         // node1->next points to node2
}

static void gui_view_on_event_cb(void *obj, gui_event_t e, void *param)
{
    gui_view_t *this = (gui_view_t *)obj;
    gui_view_on_event_t *on_event = (gui_view_on_event_t *)param;
    this->style = on_event->switch_out_style;
    gui_view_t *target_view = gui_view_create(GUI_BASE(this)->parent, on_event->descriptor, 0, 0, 0, 0);
    target_view->style = on_event->switch_in_style;
    target_view->event = 1;
    gui_view_adjust_list(GUI_BASE(this), GUI_BASE(target_view));
    // change list for transplation cover
    if (target_view->style == VIEW_STILL)
    {
        gui_view_adjust_list(GUI_BASE(target_view), GUI_BASE(this));
    }
    if (target_view->style < VIEW_ANIMATION_NULL)
    {
        target_view->release_x = this->release_x;
        target_view->release_y = this->release_y;
        this->view_tp = 1;
        target_view->view_tp = 1;
    }
    else
    {
        this->view_tp = 0;
        target_view->view_tp = 0;
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

static void gui_view_pressing_cb(void *obj, gui_event_t e, void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *this = (gui_view_t *)obj;

    if (!this->view_tp)
    {
        return;
    }

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            this->release_x = tp->deltaX;
        }
        break;

    case TOUCH_HOLD_Y:
        {
            this->release_y = tp->deltaY;
        }
        break;

    default:
        break;
    }
}

static void gui_view_released_cb(void *obj, gui_event_t e, void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *this = (gui_view_t *)obj;
    VIEW_SWITCH_STYLE style = this->style;

    if (style >= VIEW_ANIMATION_NULL)
    {
        return;
    }
    this->view_tp = 0;
    switch (tp->type)
    {
    case TOUCH_LEFT_SLIDE:
        {
            if (this->release_x < 0 && (this->view_left || !this->view_switch_ready))
            {
                // gui_log("[VIEW]TOUCH_LEFT_SLIDE\n");
                this->event = 1;
                this->cur_id.x -= 1;
                this->release_x = this->release_x + this->base.w;
            }
        }
        break;

    case TOUCH_RIGHT_SLIDE:
        {
            if (this->release_x > 0 && (this->view_right || !this->view_switch_ready))
            {
                // gui_log("[VIEW]TOUCH_RIGHT_SLIDE\n");
                this->event = 1;
                this->cur_id.x += 1;
                this->release_x = this->release_x - this->base.w;
            }
        }
        break;

    case TOUCH_DOWN_SLIDE:
        {
            if (this->release_y > 0 && (this->view_down || !this->view_switch_ready))
            {
                // gui_log("[VIEW]TOUCH_DOWN_SLIDE\n");
                this->event = 1;
                this->cur_id.y += 1;
                this->release_y = this->release_y - this->base.h;
            }
        }
        break;

    case TOUCH_UP_SLIDE:
        {
            if (this->release_y < 0 && (this->view_up || !this->view_switch_ready))
            {
                // gui_log("[VIEW]TOUCH_UP_SLIDE\n");
                this->event = 1;
                this->cur_id.y -= 1;
                this->release_y = this->release_y + this->base.h;
            }
        }
        break;

    default:
        break;
    }
}
static void gui_view_moveback_judge(gui_view_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = GUI_BASE(this);
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if ((this->release_x != 0 && (this->release_x ^ tp->deltaX) < 0) || !tp->deltaX)
            {
                this->moveback = 1;
            }
            // Limiting the range of movement
            if (abs(this->release_x) > obj->w)
            {
                if (this->release_x > 0)
                {
                    this->release_x = obj->w;
                }
                else
                {
                    this->release_x = -obj->w;
                }
            }
        }
        break;
    case TOUCH_HOLD_Y:
        {
            if ((this->release_y != 0 && (this->release_y ^ tp->deltaY) < 0) || !tp->deltaY)
            {
                this->moveback = 1;
            }

            if (abs(this->release_y) > obj->h)
            {
                if (this->release_y > 0)
                {
                    this->release_y = obj->h;
                }
                else
                {
                    this->release_y = -obj->h;
                }
            }
        }
        break;

    default:
        break;
    }
}

static void gui_view_switch_via_msg(void *msg)
{
    gui_view_t *this = (gui_view_t *)((gui_msg_t *)msg)->payload;
    gui_view_on_event_t *on_event = this->on_event[this->on_event_num - 1];
    gui_view_on_event_cb((void *)this, GUI_EVENT_VIEW_SWITCH_DIRECT, (void *)on_event);
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
    if (descriptor->keep && *descriptor->pView)
    {
        gui_view_t *this = *descriptor->pView;
        this->view_switch_ready = false;

        gui_obj_t *obj = (gui_obj_t *)this;
        obj->parent = parent;
        obj->not_show = false;
        obj->has_prepare_cb = true;
        obj->has_end_cb = true;
        obj->x = 0;
        obj->y = 0;
        obj->opacity_value = UINT8_MAX;
        gui_list_remove(&obj->brother_list);
        gui_log("%s show\n", obj->name);

        return this;
    }
    gui_view_t *this = gui_malloc(sizeof(gui_view_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_view_t));
    this->descriptor = descriptor;
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
    }
    else
    {
        this->view_switch_ready = false;
    }
    this->view_tp = 1;

    *descriptor->pView = this;
    descriptor->on_switch_in(this);

    gui_obj_add_event_cb(this, gui_view_pressing_cb, GUI_EVENT_TOUCH_PRESSING, NULL);
    gui_obj_add_event_cb(this, gui_view_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_log("create %s\n", GET_BASE(this)->name);
    return this;
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
    this->on_event = gui_realloc(this->on_event,
                                 sizeof(gui_view_on_event_t *) * (this->on_event_num + 1));
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
        this->event = 1;

        gui_view_on_event_t *on_event = this->on_event[this->on_event_num - 1]; //prevent memory leak
        if (on_event->event != GUI_EVENT_VIEW_SWITCH_DIRECT)
        {
            on_event = gui_malloc(sizeof(gui_view_on_event_t));
            this->on_event = gui_realloc(this->on_event,
                                         sizeof(gui_view_on_event_t *) * (this->on_event_num + 1));
            this->on_event[this->on_event_num] = on_event;
            this->on_event_num++;
            GUI_ASSERT(this->on_event_num < EVENT_NUM_MAX);
        }
        on_event->descriptor = descriptor;
        on_event->switch_out_style = switch_out_style;
        on_event->switch_in_style = switch_in_style;
        on_event->event = GUI_EVENT_VIEW_SWITCH_DIRECT;

        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .cb = gui_view_switch_via_msg,
            .payload = this,
        };
        gui_send_msg_to_server(&msg);
    }
}

gui_view_t *gui_view_get_current_view(void)
{
    return current_view;
}

void gui_view_free(void *msg)
{
    gui_obj_t *obj = (gui_obj_t *)((gui_msg_t *)msg)->payload;
    gui_view_t *this = (gui_view_t *)obj;

    gui_log("%s free!\n", obj->name);
    gui_obj_tree_free(obj);
    if (this->descriptor->on_switch_out)
    {
        this->descriptor->on_switch_out(this);
    }
}
