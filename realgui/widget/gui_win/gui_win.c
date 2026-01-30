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
#include "gui_post_process.h"

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
static void gui_win_input_prepare(gui_obj_t *obj)
{
    (void)obj;
}

static void prepare(gui_obj_t *obj)
{
    // touch_info_t *tp = tp_get_info();
    // kb_info_t *kb = kb_get_info();
    gui_win_t *this = (void *)obj;
    GUI_UNUSED(this);

    matrix_translate(-this->compensate_x, -this->compensate_y, obj->matrix);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_PRESSED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_KB_LONG_PRESSED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, NULL);
    matrix_translate(this->compensate_x, this->compensate_y, obj->matrix);
    if (obj->need_preprocess && blur_prepare != NULL)
    {
        int16_t x1 = (int16_t)obj->matrix->m[0][2];
        int16_t y1 = (int16_t)obj->matrix->m[1][2];
        int16_t x2 = x1 + (int16_t)(obj->w * obj->matrix->m[0][0]);
        int16_t y2 = y1 + (int16_t)(obj->h * obj->matrix->m[1][1]);
        gui_rect_t new_rect = {.x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2};
        uint8_t *blur_param = this->blur_param;
        if (blur_param == NULL)
        {
            blur_param = gui_malloc(sizeof(post_process_event));
            this->blur_param = blur_param;
            memset(blur_param, 0, sizeof(post_process_event));
            post_process_event *event = (post_process_event *)blur_param;
            event->param = gui_malloc(sizeof(post_process_blur_param));
            memset(event->param, 0, sizeof(post_process_blur_param));
            post_process_blur_param *param = (post_process_blur_param *)event->param;
            param->area = new_rect;
            param->blur_degree = this->blur_degree;
            event->type = POST_PROCESS_BLUR;
            if (param->cache_mem == NULL)
            {
                blur_prepare(&param->area, &param->cache_mem);
            }
        }
    }
}

static void gui_win_preprocess(gui_obj_t *obj)
{
    gui_win_t *this = (gui_win_t *)obj;
    post_process_event *blur_param = (post_process_event *)this->blur_param;
    if (blur_param != NULL)
    {
        pre_process_handle(blur_param);
    }
}

static void gui_win_end(gui_obj_t *obj)
{
    if (obj->need_preprocess)
    {
        gui_win_t *this = (gui_win_t *)obj;
        post_process_event *blur_param = (post_process_event *)this->blur_param;
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
            this->blur_param = NULL;
        }
    }
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
        case OBJ_INPUT_PREPARE:
            gui_win_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;
        case OBJ_PREPROCESS:
            gui_win_preprocess(obj);
            break;
        case OBJ_END:
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
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->type = WINDOW;
    this->blur_degree = 225;
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
void gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot set opacity: win is NULL\n");
        return;
    }
    GUI_BASE(win)->opacity_value = opacity_value;
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

void gui_win_enable_blur(gui_win_t *win, bool enable)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot use_blur: win is NULL\n");
        return;
    }
    win->base.need_preprocess = enable;
}

void gui_win_set_blur_degree(gui_win_t *win, uint8_t degree)
{
    if (win == NULL)
    {
        // Handle error: window structure is NULL
        gui_log("Cannot use_blur: win is NULL\n");
        return;
    }
    win->blur_degree = degree;
}

