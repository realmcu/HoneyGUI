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
#include "gui_matrix.h"
#include "gui_obj.h"
#include "gui_win.h"

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
static void gui_win_input_process(gui_obj_t *obj)
{
    (void)obj;
}

static void gui_win_apply_rotation(gui_obj_t *obj, gui_win_t *this)
{
    if (this->degrees == 0.0f)
    {
        return;
    }

    float cx = obj->w / 2.0f;
    float cy = obj->h / 2.0f;

    matrix_translate(cx, cy, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_translate(-cx, -cy, obj->matrix);
}

static void gui_win_apply_scale(gui_obj_t *obj, gui_win_t *this)
{
    if (this->scale == 1.0f && this->scale_y == 1.0f)
    {
        return;
    }

    float cx = obj->w / 2.0f;
    float cy = obj->h / 2.0f;

    matrix_translate(cx, cy, obj->matrix);
    matrix_scale(this->scale, this->scale_y, obj->matrix);
    matrix_translate(-cx, -cy, obj->matrix);
}

static void prepare(gui_obj_t *obj)
{
    // touch_info_t *tp = tp_get_info();
    // kb_info_t *kb = kb_get_info();
    gui_win_t *this = (void *)obj;
    GUI_UNUSED(this);

    gui_win_apply_rotation(obj, this);
    gui_win_apply_scale(obj, this);

    obj->opacity_value = obj->opacity_value * this->opacity_value / UINT8_MAX;

    matrix_translate(-this->compensate_x, -this->compensate_y, obj->matrix);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LEFT_SLIDE_QUICK, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RIGHT_SLIDE_QUICK, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_UP_SLIDE_QUICK, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOWN_SLIDE_QUICK, "touch");
    matrix_translate(this->compensate_x, this->compensate_y, obj->matrix);
}

static void gui_win_end(gui_obj_t *obj)
{
    (void)obj;
}

static void gui_win_destroy(gui_obj_t *obj)
{
    (void)obj;
}

static void gui_win_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PROCESS:
            gui_win_input_process(obj);
            break;
        case OBJ_PRE_PROCESS:
            gui_win_prepare(obj);
            break;
        case OBJ_POST_PROCESS:
            gui_win_end(obj);
            break;
        case OBJ_DESTROY:
            gui_win_destroy(obj);
            break;

        default:
            break;
        }
    }
}

void gui_win_ctor(gui_win_t  *this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h)
{
    gui_obj_ctor(&this->base, parent, name, x, y, w, h);
    GET_BASE(this)->obj_cb = gui_win_cb;
    GET_BASE(this)->has_pre_process_cb = true;
    GET_BASE(this)->has_post_process_cb = true;
    GET_BASE(this)->type = WINDOW;
    this->opacity_value = UINT8_MAX;
    this->scale = 1.0f;
    this->scale_y = 1.0f;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_win_left(gui_win_t *this, void *callback, void *parameter)
{
    (void)this;
    (void)callback;
    (void)parameter;
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
    GUI_ASSERT(0);
}

void gui_win_right(gui_win_t *this, void *callback, void *parameter)
{
    (void)this;
    (void)callback;
    (void)parameter;
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_2, parameter);
    GUI_ASSERT(0);
}

void gui_win_up(gui_win_t *this, void *callback, void *parameter)
{
    (void)this;
    (void)callback;
    (void)parameter;
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_3, parameter);
    GUI_ASSERT(0);
}

void gui_win_down(gui_win_t *this, void *callback, void *parameter)
{
    (void)this;
    (void)callback;
    (void)parameter;
    //gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_4, parameter);
    GUI_ASSERT(0);
}

void gui_win_press(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, callback, GUI_EVENT_TOUCH_PRESSED, parameter);
}

void gui_win_release(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, parameter);
}

void gui_win_long(gui_win_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, parameter);
}

void gui_win_click(gui_win_t *this, gui_event_cb_t callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}

void gui_win_hold_tp(gui_win_t *this, bool hold_tp)
{
    this->hold_tp = hold_tp;
}

gui_win_t *gui_win_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h)
{
    gui_win_t *this = gui_malloc(sizeof(gui_win_t));
    GUI_ASSERT(this != NULL);

    memset(this, 0, sizeof(gui_win_t));
    gui_win_ctor(this, parent, name, x, y, w, h);

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }

    ((gui_obj_t *)this)->create_done = true;

    return this;
}

void gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set_scale_rate: win is NULL\n");
        return;
    }
    win->scale = scale_rate_horizontal;
    win->scale_y = scale_rate_vertical;
}
void gui_win_set_rotation(gui_win_t *win, float degrees)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set rotation: win is NULL\n");
        return;
    }
    win->degrees = degrees;
}
void gui_win_set_scope(gui_win_t *win, bool enable)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set scope: win is NULL\n");
        return;
    }
    win->scope = enable;
}
void gui_win_move(gui_win_t *win, int x, int y)
{
    GUI_ASSERT(win != NULL);
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    GUI_BASE(win)->x = x;
    GUI_BASE(win)->y = y;
}
int gui_win_get_x(gui_win_t *win)
{
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    return GUI_BASE(win)->x;
}
int gui_win_get_y(gui_win_t *win)
{
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    return GUI_BASE(win)->y;
}
void gui_win_set_opacity(gui_win_t *win, uint8_t opacity_value)
{
    GUI_ASSERT(GUI_BASE(win)->type == WINDOW);
    win->opacity_value = opacity_value;
}

void gui_win_prepare_global(gui_obj_t *obj)
{
    prepare(obj);
}
void gui_win_prepare(gui_obj_t *obj)
{
    prepare(obj);
}

void gui_win_compensate(gui_win_t *win, int x, int y)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot compensate: win is NULL\n");
        return;
    }
    win->compensate_x = x;
    win->compensate_y = y;
}
