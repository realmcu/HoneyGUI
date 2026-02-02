/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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
#include "gui_view_transition.h"
#include "gui_post_process.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *g_CurrentView = NULL;
static gui_view_t *g_PreView = NULL;
static gui_view_t *g_NextView = NULL;

static bool g_SurpressEvent = false;
static bool g_SurpressTP = true;
static int16_t g_Release = 0;
static int16_t g_Target = 0;

static bool g_TriggerMove = false; // whether trigger move event
static int16_t g_Offset = 0; // offset of the g_Release
static bool g_OffsetNeedUpdate = false;

static bool g_SwitchDone = false;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_view_not_show(gui_view_t *_this);
static void gui_view_adjust_list(gui_obj_t *old, gui_obj_t *new);

/*============================================================================*
 *         next is the private function for on obj timer cb
 *============================================================================*/
static void gui_view_released_view_timer_cb(void *obj)
{
    touch_info_t *tp = tp_get_info();
    GUI_UNUSED(tp);
    // gui_log("released in view timer cb, g_Release = %d!\n", g_Release);

    if (g_Release != g_Target)
    {
        const float factor = 0.4f;
        int16_t distance = g_Target - g_Release;
        int delta = (int16_t)(distance * factor); //exponential decay
        if (delta == 0)
        {
            delta = (distance > 0) ? 1 : -1;
        }
        g_Release += delta;
    }
    else
    {
        if (g_Target != 0) // change view, need reset
        {
            // gui_log("obj name = %s, g_Release = %d!\n", ((gui_obj_t *)obj)->name, g_Release);

            if (g_PreView != g_NextView) {gui_view_not_show(g_PreView);}
            gui_view_not_show(g_CurrentView);
            if (g_CurrentView->descriptor->keep)
            {
                g_PreView = g_CurrentView;
                if (g_NextView->opacity != UINT8_MAX) //decrease time of processing PreView
                {
                    gui_obj_hidden(&g_PreView->base, false);
                }
                else
                {
                    gui_obj_hidden(&g_PreView->base, true);
                }
            }
            else
            {
                g_PreView = NULL;
            }
            g_CurrentView = g_NextView;
        }
        else
        {
            gui_view_not_show(g_NextView);
        }
        g_SwitchDone = true;
        g_Release = 0;
        g_Target = g_Release;
        g_NextView = NULL;
        g_CurrentView->current_transition_style = SWITCH_INIT_STATE;
        // gui_log("current view name = %s\n", g_CurrentView->base.name);

        gui_obj_delete_timer(obj);
    }
}

static void gui_view_animate_timer_cb(void *obj)
{
    gui_view_t *_this = (gui_view_t *)obj;

    g_Release += _this->animate_step;
    if (g_Release >= g_Target)
    {
        g_Release = 0;
        g_Target = g_Release;
        _this->current_transition_style = SWITCH_INIT_STATE;
        gui_obj_delete_timer(obj);

        if (g_PreView != g_NextView) {gui_view_not_show(g_PreView);}
        gui_view_not_show(g_CurrentView);
        if (g_CurrentView->descriptor->keep)
        {
            g_PreView = g_CurrentView;
            if (g_NextView->opacity != UINT8_MAX) //decrease time of processing PreView
            {
                gui_obj_hidden(&g_PreView->base, false);
            }
            else
            {
                gui_obj_hidden(&g_PreView->base, true);
            }
        }
        else
        {
            g_PreView = NULL;
        }
        g_SwitchDone = true;
        g_CurrentView = g_NextView;
        g_CurrentView->base.x = 0;
        g_CurrentView->base.y = 0;
        g_CurrentView->current_transition_style = SWITCH_INIT_STATE;
        g_NextView = NULL;
    }
}


/*============================================================================*
 *         next is the private function for on event cb
 *============================================================================*/

static void gui_view_released_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;

    touch_info_t *tp = tp_get_info();
    gui_view_t *_this = (gui_view_t *)obj;
    gui_obj_t *o = (gui_obj_t *)_this;
    // gui_log("name = %s, detalX = %d, detalY = %d\n", o->name, tp->deltaX, tp->deltaY);

    gui_obj_create_timer(o, 10, true, gui_view_released_view_timer_cb);
    gui_obj_start_timer(o);

    g_SurpressTP = true;
    g_SurpressEvent = true;

    if (tp->type == TOUCH_LEFT_SLIDE)
    {
        g_Target = -o->w;
        if (g_Release <= -o->w / 2)
        {
            g_Target = -o->w;
        }
        else
        {
            g_Target = 0;
        }
    }
    else if (tp->type == TOUCH_RIGHT_SLIDE)
    {
        if (g_Release >= o->w / 2)
        {
            g_Target = o->w;
        }
        else
        {
            g_Target = 0;
        }
    }
    else if (tp->type == TOUCH_UP_SLIDE)
    {
        if (g_Release <= -o->h / 2)
        {
            g_Target = -o->h;
        }
        else
        {
            g_Target = 0;
        }
    }
    else if (tp->type == TOUCH_DOWN_SLIDE)
    {
        if (g_Release >= o->h / 2)
        {
            g_Target = o->h;
        }
        else
        {
            g_Target = 0;
        }
    }
    else if (tp->type == TOUCH_ORIGIN_FROM_X)
    {
        g_Target = 0;
    }
    else if (tp->type == TOUCH_ORIGIN_FROM_Y)
    {
        g_Target = 0;
    }
    else
    {
        // GUI_ASSERT(0);
        g_Target = 0;
    }
    g_Offset = 0;
}

static void gui_view_pressing_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_log("g_Offset %d\n", g_Offset);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_event_t trigger_event = g_CurrentView->current_event;

    if (trigger_event == GUI_EVENT_TOUCH_MOVE_LEFT)
    {
        g_Release = tp->deltaX < 0 ? tp->deltaX : 0;

        if (abs(g_Release) > dc->screen_width)
        {
            g_Release = -dc->screen_width;
        }
    }
    else if (trigger_event == GUI_EVENT_TOUCH_MOVE_RIGHT)
    {
        g_Release = tp->deltaX > 0 ? tp->deltaX : 0;

        if (abs(g_Release) > dc->screen_width)
        {
            g_Release = dc->screen_width;
        }
    }
    else if (trigger_event == GUI_EVENT_TOUCH_MOVE_UP)
    {
        g_Release = tp->deltaY < 0 ? tp->deltaY : 0;

        if (abs(g_Release) > dc->screen_height)
        {
            g_Release = -dc->screen_height;
        }
    }
    else if (trigger_event == GUI_EVENT_TOUCH_MOVE_DOWN)
    {
        g_Release = tp->deltaY > 0 ? tp->deltaY : 0;

        if (abs(g_Release) > dc->screen_height)
        {
            g_Release = dc->screen_height;
        }
    }
    // gui_log("delta = %d\n", g_Release);
    if (abs(g_Release) < abs(g_Offset))
    {
        g_Release = 0;
        return;
    }
    g_Release -= g_Offset;
    g_TriggerMove = false;
    gui_obj_hidden(GUI_BASE(g_NextView), false);

    // gui_log("g_release = %d\n", g_Release);
}

static void gui_view_transition(gui_view_t *_this, int16_t release)
{
    if (_this->current_transition_style >= SWITCH_OUT_TO_LEFT_USE_TRANSLATION &&
        _this->current_transition_style < SWITCH_IN_FROM_LEFT_USE_CUBE)
    {
        gui_view_translation(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_CUBE &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_CUBE)
    {
        gui_view_cube(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_ROTATE &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_ROTATE)
    {
        gui_view_rotate(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_REDUCTION &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_REDUCTION)
    {
        gui_view_reduction(_this, release);
    }
    else if ((_this->current_transition_style == SWITCH_OUT_STILL_USE_BLUR ||
              _this->current_transition_style == SWITCH_IN_STILL_USE_BLUR) && blur_prepare != NULL)
    {
        gui_view_blur(_this, release);
    }
}

static void gui_view_on_event_trigger_move_cb(gui_obj_t *obj, gui_event_t e,
                                              gui_view_on_event_t *on_event)
{
    // gui_log("enter event_trigger_move_cb \n");
    g_SurpressTP = false;
    // if (g_NextView != NULL && *on_event->descriptor->pView == g_NextView)
    // {
    //     return;
    // }

    gui_view_t *next_view_rec = g_NextView;
    g_NextView = gui_view_create(obj->parent, on_event->descriptor->name, 0, 0, 0, 0);
    if (g_NextView != next_view_rec) {gui_view_not_show(next_view_rec);}

    g_NextView->current_transition_style = on_event->switch_in_style;
    g_CurrentView->current_transition_style = on_event->switch_out_style;
    if (on_event->switch_in_style <= SWITCH_IN_STILL_USE_BLUR) // cover CurrentView
    {
        gui_view_adjust_list(GUI_BASE(g_NextView), obj);
    }

    gui_obj_hidden(GUI_BASE(g_NextView), true);

    g_NextView->current_event = e;
    g_CurrentView->current_event = e;

    g_TriggerMove = true;
    g_OffsetNeedUpdate = false;
    g_Offset = 0;
    // gui_log("gui_view_on_event_trigger_move_cb next view name = %s\n", g_NextView->base.name);
}

static void gui_view_on_event_change_cb(gui_obj_t *obj, gui_event_t e,
                                        gui_view_on_event_t *on_event)
{
    gui_view_not_show(g_NextView);

    g_NextView = gui_view_create(obj->parent, on_event->descriptor->name, 0, 0, 0, 0);

    g_NextView->current_transition_style = on_event->switch_in_style;
    g_CurrentView->current_transition_style = on_event->switch_out_style;

    g_NextView->current_event = e;
    g_CurrentView->current_event = e;

    gui_obj_create_timer(obj, 10, true, gui_view_animate_timer_cb);
    gui_obj_start_timer(obj);

    g_Release = g_CurrentView->animate_step; //prevent new view abnormal display of the first frame
    g_Target = obj->h;

    g_SurpressEvent = true;
    g_SurpressTP = true;

    if (e == GUI_EVENT_INVALID)
    {
        g_NextView->base.x = gui_get_screen_width();
    }

    // gui_log("gui_view_on_event_cb\n");
}

static void gui_view_not_show(gui_view_t *_this)
{
    if (_this == NULL || (_this == g_NextView && _this == g_PreView && g_SurpressEvent)) { return; }

    gui_obj_t *obj = GUI_BASE(_this);
    if (!_this->descriptor->keep)
    {
        gui_obj_tree_free_async(obj);
    }
    else
    {
        gui_obj_hidden(obj, true);
        gui_log("%s hide\n", obj->name);
    }
}

static void gui_view_adjust_list(gui_obj_t *old, gui_obj_t *new)
{
    gui_node_list_t *list1 = &old->brother_list;
    gui_node_list_t *list2 = &new->brother_list;

    // Get adjacent pointers
    gui_node_list_t *list1_next = list1->next;
    gui_node_list_t *list2_prev = list2->prev;
    gui_node_list_t *list2_next = list2->next;

    // If node2 is already immediately after node1, no action needed
    if (list1_next == list2)
    {
        return;
    }

    //Remove node2 from its original position
    list2_prev->next = list2_next;
    list2_next->prev = list2_prev;

    //Insert node2 after node1
    list2->next = list1_next;
    list2->prev = list1;
    list1_next->prev = list2;
    list1->next = list2;
}

static void gui_view_scroll_offset(void)
{
    touch_info_t *tp = tp_get_info();
    if (tp->left_moved || tp->right_moved ||
        tp->up_moved || tp->down_moved)
    {
        g_Offset = 0;
        g_Release = 0;
    }

    if (g_TriggerMove)
    {
        if (g_OffsetNeedUpdate)
        {
            if (tp->type == TOUCH_HOLD_X)      {g_Offset = tp->deltaX;}
            else if (tp->type == TOUCH_HOLD_Y) {g_Offset = tp->deltaY;}
        }
        else
        {
            g_OffsetNeedUpdate = true;
        }
    }
}

static void gui_view_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    GUI_UNUSED(dc);
    GUI_UNUSED(tp);

    gui_view_t *_this = (gui_view_t *)obj;
    obj->opacity_value = _this->opacity;

    if (!g_SurpressEvent && _this == g_CurrentView)
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_UP, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_DOWN, NULL);
        g_SwitchDone = false;
    }

    if (!g_SurpressTP && _this == g_CurrentView)
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, NULL);
        gui_view_scroll_offset();
    }

    if (_this->current_transition_style == SWITCH_INIT_STATE) {}
    else if (_this->current_transition_style < SWITCH_OUT_NONE_ANIMATION)
    {
        gui_view_transition(_this, g_Release);
    }
    else if (_this->current_transition_style >= SWITCH_OUT_NONE_ANIMATION)
    {
        view_transition_animation(obj, g_Release / (float)g_Target);
    }

    if (g_Release != g_Target)
    {
        gui_fb_change();
    }
}

static void gui_view_preprocess(gui_obj_t *obj)
{
    gui_view_t *_this = (gui_view_t *)obj;
    post_process_event *blur_param = (post_process_event *)_this->blur_param;
    if (blur_param != NULL)
    {
        pre_process_handle(blur_param);
    }
}

static void gui_view_destroy(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *_this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(_this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    for (uint32_t i = 0; i < _this->on_event_num; i++)
    {
        gui_free(_this->on_event[i]);
        _this->on_event[i] = NULL;
    }

    if (_this->on_event != NULL)
    {
        gui_free(_this->on_event);
    }

    if (_this->descriptor->on_switch_out != NULL)
    {
        _this->descriptor->on_switch_out(_this);
    }
    *_this->descriptor->pView = NULL;

    if (_this == g_CurrentView)
    {
        g_CurrentView = NULL;
    }
    else if (_this == g_NextView)
    {
        g_NextView = NULL;
    }
}

static void gui_view_end(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_view_t *_this = (gui_view_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(_this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (_this == g_CurrentView && g_SwitchDone)
    {
        g_SurpressEvent = false;
    }
    if (obj->need_preprocess)
    {
        gui_view_t *_this = (gui_view_t *)obj;
        post_process_event *blur_param = (post_process_event *)_this->blur_param;
        if (blur_param != NULL)
        {
            post_process_event *event = blur_param;
            if (event->param != NULL)
            {
                if (event->type == POST_PROCESS_BLUR)
                {
                    post_process_blur_param *param = (post_process_blur_param *)event->param;
                    blur_depose(&param->cache_mem);
                }
                gui_free(event->param);
            }
            gui_free(blur_param);
            _this->blur_param = NULL;
        }
    }
    obj->need_preprocess = false;
}

static void gui_view_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_view_prepare(obj);
            }
            break;
        case OBJ_PREPROCESS:
            {
                gui_view_preprocess(obj);
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

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_view_t *gui_view_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    const gui_view_descriptor_t *descriptor = gui_view_descriptor_get(name);

    if (descriptor == NULL)
    {
        gui_log("View '%s' not registered\n", name);
        GUI_ASSERT(0);
        return NULL;
    }

    if (*descriptor->pView) // don't recreate view whether keep set or not
    {
        gui_view_t *_this = *descriptor->pView;

        gui_obj_t *obj = (gui_obj_t *)_this;
        obj->parent = parent;
        obj->not_show = false;
        obj->x = 0;
        obj->y = 0;
        gui_list_remove(&obj->brother_list);
        if (GET_BASE(parent)->child_list.next == &GET_BASE(parent)->child_list)
        {
            gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
            g_CurrentView = _this;
        }
        else
        {
            gui_view_adjust_list(GET_BASE(g_CurrentView), obj);
        }
        gui_log("%s show\n", obj->name);

        return _this;
    }
    gui_view_t *_this = gui_malloc(sizeof(gui_view_t));
    // gui_dispdev_t *dc = gui_get_dc();
    memset(_this, 0, sizeof(gui_view_t));
    _this->descriptor = descriptor;
    _this->animate_step = 40;
    _this->opacity = UINT8_MAX;
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_obj_ctor(&_this->base, parent, descriptor->name, x, y, w, h);

    GET_BASE(_this)->obj_cb = gui_view_cb;
    GET_BASE(_this)->has_input_prepare_cb = true;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_end_cb = true;
    GET_BASE(_this)->has_destroy_cb  = true;
    GET_BASE(_this)->type = VIEW;
    GET_BASE(_this)->create_done = true;
    gui_list_init(&(GET_BASE(_this)->child_list));

    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }

    *descriptor->pView = _this;
    descriptor->on_switch_in(_this);


    gui_log("create %s\n", GET_BASE(_this)->name);
    if (g_CurrentView == NULL && parent != gui_obj_get_fake_root())
    {
        g_CurrentView = _this;
        g_SurpressEvent = false;
        g_SurpressTP = true;
        g_SwitchDone = false;
    }
    return _this;
}

void gui_view_switch_on_event(gui_view_t *_this,
                              const char *target_view_name,
                              VIEW_SWITCH_STYLE switch_out_style,
                              VIEW_SWITCH_STYLE switch_in_style,
                              gui_event_t event)
{
    const gui_view_descriptor_t *descriptor = gui_view_descriptor_get(target_view_name);

    if (descriptor == NULL)
    {
        gui_log("View '%s' not registered\n", target_view_name);
        GUI_ASSERT(0);
        return;
    }

    bool update_event_dec = false; //update same event or new event flag
    bool update_on_event = false; //update same event or new event flag

    gui_obj_t *obj = (gui_obj_t *)_this;
    gui_event_dsc_t *event_dsc = NULL;
    gui_view_on_event_t *on_event = NULL;

    uint32_t i = 0;
    uint32_t j = 0;

    //check for obj event_dsc
    for (i = 0; i < obj->event_dsc_cnt; i++)
    {
        event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event)
        {
            update_event_dec = true;
            break;
        }
    }

    //check for view on_event
    for (j = 0; j < _this->on_event_num; j++)
    {
        on_event = _this->on_event[j];
        if (on_event->event == event)
        {
            update_on_event = true;
            break;
        }
    }

    if ((update_on_event == true) && (update_event_dec == true))
    {
        obj->event_dsc[i].user_data = on_event;
        _this->on_event[j]->switch_out_style = switch_out_style;
        _this->on_event[j]->switch_in_style = switch_in_style;
        _this->on_event[j]->descriptor = descriptor;

        if (event == 0)
        {
            gui_view_on_event_change_cb(obj, event, on_event);
        }
        return;
    }

    on_event = gui_malloc(sizeof(gui_view_on_event_t));
    on_event->descriptor = descriptor;
    on_event->switch_out_style = switch_out_style;
    on_event->switch_in_style = switch_in_style;
    on_event->event = event;
    _this->on_event = gui_realloc(_this->on_event,
                                  sizeof(gui_view_on_event_t *) * (_this->on_event_num + 1));
    _this->on_event[_this->on_event_num] = on_event;
    _this->on_event_num++;
    GUI_ASSERT(_this->on_event_num < EVENT_NUM_MAX);

    if (event == 0)
    {
        gui_obj_add_event_cb(_this, (gui_event_cb_t)gui_view_on_event_change_cb, event, on_event);
        gui_view_on_event_change_cb(obj, event, on_event);
        return;
    }

    if (event == GUI_EVENT_TOUCH_MOVE_DOWN ||
        event == GUI_EVENT_TOUCH_MOVE_UP ||
        event == GUI_EVENT_TOUCH_MOVE_LEFT ||
        event == GUI_EVENT_TOUCH_MOVE_RIGHT)
    {
        gui_obj_add_event_cb(_this, gui_view_pressing_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, NULL);
        gui_obj_add_event_cb(_this, gui_view_pressing_cb, GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
        gui_obj_add_event_cb(_this, gui_view_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
        gui_obj_add_event_cb(_this, (gui_event_cb_t)gui_view_on_event_trigger_move_cb, event, on_event);
    }
    else
    {
        gui_obj_add_event_cb(_this, (gui_event_cb_t)gui_view_on_event_change_cb, event, on_event);
    }
}

void gui_view_switch_direct(gui_view_t *_this, const char *target_view_name,
                            VIEW_SWITCH_STYLE switch_out_style,
                            VIEW_SWITCH_STYLE switch_in_style)
{
    const gui_view_descriptor_t *descriptor = gui_view_descriptor_get(target_view_name);

    if (descriptor == NULL)
    {
        gui_log("View '%s' not registered\n", target_view_name);
        GUI_ASSERT(0);
        return;
    }

    if (g_SurpressEvent || g_SwitchDone || g_CurrentView->descriptor == descriptor) { return; }
    gui_view_switch_on_event(_this, target_view_name, switch_out_style, switch_in_style,
                             GUI_EVENT_INVALID);
}

void gui_view_set_animate_step(gui_view_t *_this, uint16_t step)
{
    if (step > _this->base.h)
    {
        step = _this->base.h;
    }
    _this->animate_step = step;
}

gui_view_t *gui_view_get_current(void)
{
    return g_CurrentView;
}

gui_view_t *gui_view_get_next(void)
{
    return g_NextView;
}

void gui_view_set_opacity(gui_view_t *_this, uint8_t opacity)
{
    _this->opacity = opacity;
}

