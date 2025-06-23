/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_list.c
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
#include <math.h>
#include "guidef.h"
#include "def_list.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_list.h"
#include "gui_view.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static bool g_Limit = false;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_list_note_animate_timer_cb(void *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    gui_obj_t *o = GUI_BASE(obj);

    _this->animate_cnt++;
    if (_this->animate_cnt > LIST_TAB_ANIMATE_MAX)
    {
        _this->animate_cnt = 0;
        gui_obj_delete_timer(o);
    }
}

static void gui_list_update_speed(gui_list_t *_this, int16_t tp_delta)
{
    int record_num = 4;

    for (size_t i = 0; i < record_num; i++)
    {
        _this->record[i] = _this->record[i + 1];
    }

    _this->record[record_num] = tp_delta;
    _this->speed = _this->record[record_num] - _this->record[0];
    int max_speed = GUI_MAX_SPEED;
    int min_speed = GUI_MIN_SPEED;

    if (_this->speed > max_speed)
    {
        _this->speed = max_speed;
    }
    else if (_this->speed < -max_speed)
    {
        _this->speed = -max_speed;
    }

    if ((_this->speed > 0) && (_this->speed < min_speed))
    {
        _this->speed = min_speed;
    }
    else if ((_this->speed < 0) && (_this->speed > -min_speed))
    {
        _this->speed = -min_speed;
    }
}


static void gui_list_input_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    GUI_UNUSED(dc);

    if (!tp->pressing)
    {
        g_Limit = false;
        int temp = _this->dir == HORIZONTAL ? obj->w : obj->h;

        // restore list offset
        if (_this->speed == 0)
        {
            int offset_min = temp - _this->total_length;
            int offset_max = 0;
            if (_this->style == LIST_CARD)
            {
                offset_max = temp - _this->note_length * 1.8f;
                offset_min -= temp / 3;
            }
            if (_this->offset > offset_max || _this->offset < offset_min)
            {
                float e_factor = 0.2f;
                int16_t target = _this->offset >= 0 ? offset_max : offset_min;
                int16_t distance = target - _this->offset;
                int delta = (int16_t)(distance * e_factor); //exponential decay
                if (delta == 0) { delta = (distance > 0) ? 1 : -1; }
                _this->offset += delta;
            }
        }
        else
        {
            int offset_min = temp - _this->total_length;
            int offset_max = OUT_SCOPE;
            if (_this->style == LIST_CARD)
            {
                offset_max = temp - _this->note_length * 1.8f;
                offset_min -= temp / 3;
            }

            _this->offset += _this->speed;
            _this->speed = (int16_t)((1 - _this->factor) * _this->speed);
            if (_this->offset > offset_max)
            {
                _this->offset = offset_max;
                _this->speed = 0;
            }
            else if (_this->offset < offset_min - OUT_SCOPE)
            {
                _this->offset = offset_min - OUT_SCOPE;
                _this->speed = 0;
            }
        }
        _this->hold = _this->offset;
    }
}

static void gui_list_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    GUI_UNUSED(dc);

    if (tp->left_moved || tp->right_moved ||
        tp->up_moved || tp->down_moved)
    {
        g_Limit = false;
    }

    if (!g_Limit && (gui_view_t *)(obj->parent) != gui_view_get_next())
    {
        if (_this->dir == HORIZONTAL)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL);
        }
        else
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL);
        }
    }
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);

    uint8_t last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, (uint8_t)sizeof(gui_list_t));
    if (last != _this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_list_destroy(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(_this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_list_end(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(_this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_list_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_list_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_list_prepare(obj);
            }
            break;

        case OBJ_END:
            {
                gui_list_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_list_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_list_note_input_prepare(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    gui_list_t *list = (gui_list_t *)obj->parent;

    if (list->dir == HORIZONTAL)
    {
        int16_t x_old = obj->x;
        obj->x = _this->start_x + list->offset;
        if ((list->style >= LIST_FAN) && list->speed)
        {
            if (x_old + obj->w <= 0 && obj->x + obj->w > 0)
            {
                _this->is_speed_positive = true;
                gui_obj_create_timer(obj, 10, true, gui_list_note_animate_timer_cb);
                gui_obj_start_timer(obj);
                _this->animate_cnt = 1;
            }
            else if (x_old >= list->base.w && obj->x < list->base.w)
            {
                _this->is_speed_positive = false;
                gui_obj_create_timer(obj, 10, true, gui_list_note_animate_timer_cb);
                gui_obj_start_timer(obj);
                _this->animate_cnt = 1;
            }
        }
    }
    else
    {
        int16_t y_old = obj->y;
        obj->y = _this->start_y + list->offset;
        if ((list->style >= LIST_FAN) && list->speed)
        {
            if (y_old + obj->h <= 0 && obj->y + obj->h > 0)
            {
                _this->is_speed_positive = true;
                gui_obj_create_timer(obj, 10, true, gui_list_note_animate_timer_cb);
                gui_obj_start_timer(obj);
                _this->animate_cnt = 1;
            }
            else if (y_old >= list->base.h && obj->y < list->base.h)
            {
                _this->is_speed_positive = false;
                gui_obj_create_timer(obj, 10, true, gui_list_note_animate_timer_cb);
                gui_obj_start_timer(obj);
                _this->animate_cnt = 1;
            }
        }
    }
}

static void gui_list_note_prepare(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    touch_info_t *tp = tp_get_info();
    gui_list_t *list = (gui_list_t *)obj->parent;

    matrix_translate(-list->base.x, -list->base.y,
                     obj->matrix); //eliminate list offsets if x\y is not 0

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
    if (list->dir == VERTICAL)
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL);
    }
    else
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL);
    }

    if (list->style == LIST_CIRCLE)
    {
        matrix_translate(-obj->x, -obj->y, obj->matrix);

        int32_t r = (list->dir == HORIZONTAL) ? list->base.h : list->base.w;
        int32_t diff = (list->dir == HORIZONTAL)
                       ? (list->base.w / 2 + list->base.x) - (obj->x + list->note_length / 2)
                       : (list->base.h / 2 + list->base.y) - (obj->y + list->note_length / 2);

        int32_t coord = diff >= r ? r : r - (int)sqrt(r * r - diff * diff);

        if (list->dir == HORIZONTAL)
        {
            obj->y = coord;
        }
        else
        {
            obj->x = coord;
        }

        matrix_translate(obj->x, obj->y, obj->matrix);
    }
    else if (list->style == LIST_ZOOM)
    {
        float scale_range = 0.5f;
        int32_t scope = (list->dir == HORIZONTAL) ? list->base.w : list->base.h;

        int32_t diff = (list->dir == HORIZONTAL)
                       ? (list->base.w / 2 + list->base.x) - (obj->x + list->note_length / 2)
                       : (list->base.h / 2 + list->base.y) - (obj->y + list->note_length / 2);
        diff = abs(diff);
        if (diff > scope / 2) { diff = scope / 2;}
        float scale = 1.0f - scale_range * diff / (scope / 2.0f);

        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        matrix_scale(scale, scale, obj->matrix);
        matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    }
    else if (list->style == LIST_CARD)
    {
        matrix_translate(-obj->x, -obj->y, obj->matrix);

        float scale_0 = 1.0f;
        float scale_1 = 0.9f;
        int obj_location, list_length, list_location;
        if (list->dir == HORIZONTAL)
        {
            int16_t limit = (int16_t)(list->base.x + list->base.w - (int16_t)(list->note_length * scale_1));
            if (obj->x > limit)
            {
                obj->x = limit;
            }
            obj_location = obj->x;
            list_length = list->base.w;
            list_location = list->base.x;
        }
        else
        {
            int16_t limit = (int16_t)(list->base.y + list->base.h - (int16_t)(list->note_length * scale_1));
            if (obj->y > limit)
            {
                obj->y = limit;
            }
            obj_location = obj->y;
            list_length = list->base.h;
            list_location = list->base.y;
        }
        int location_1 = list_location + list_length - (int16_t)(list->note_length * scale_1);
        int location_0 = location_1 - list->note_length / 3;

        if (obj_location >= location_0 && obj_location <= location_1)
        {
            float scale = scale_0 - (scale_0 - scale_1) * (obj_location - location_0) /
                          (location_1 - location_0);
            matrix_translate(obj->x, obj->y, obj->matrix); //prevent bottom space of screen

            matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
            matrix_scale(scale, scale, obj->matrix);
            matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
        }
        else
        {
            matrix_translate(obj->x, obj->y, obj->matrix);
        }
    }
    else if (list->style == LIST_FAN)
    {
        if (obj->timer)
        {
            float start;
            if (list->dir == HORIZONTAL)
            {
                start = _this->is_speed_positive ? 30.0f : -30.0f;
            }
            else
            {
                start = _this->is_speed_positive ? -30.0f : 30.0f;
            }
            float angle = start * (1.0f - (float)_this->animate_cnt / LIST_TAB_ANIMATE_MAX);
            matrix_rotate(angle, obj->matrix);
        }
    }
    else if (list->style == LIST_HELIX)
    {
        extern void gui_list_rotate_helix(gui_list_note_t *_this, float degree);
        if (obj->timer)
        {
            float start = _this->is_speed_positive ? -180.0f : 180.0f;
            float degree = start * (1.0f - (float)_this->animate_cnt / LIST_TAB_ANIMATE_MAX);
            gui_list_rotate_helix(_this, degree);
        }
    }
    else if (list->style == LIST_CURL)
    {
        extern void gui_list_rotate_curl(gui_list_note_t *_this, float degree);
        if (obj->timer)
        {
            float degree = 90.0f * (1.0f - (float)_this->animate_cnt / LIST_TAB_ANIMATE_MAX);
            gui_list_rotate_curl(_this, degree);
        }
    }
    matrix_translate(_this->t_x, _this->t_y,
                     obj->matrix); //_this way to move note in order not to lose tp

    uint8_t last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, (uint8_t)sizeof(gui_list_note_t));
    if (last != _this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_list_note_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_list_note_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_list_note_prepare(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_list_pressing_cb(void *object, gui_event_t e, void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)object;
    gui_list_t *_this = (gui_list_t *)object;
    g_Limit = false;

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if (_this->dir == VERTICAL) { break; }
            gui_list_update_speed(_this, tp->deltaX);

            _this->offset = _this->hold + tp->deltaX;
            int offset_min = obj->w - _this->total_length - OUT_SCOPE;
            int offset_max = OUT_SCOPE;
            if (_this->style == LIST_CARD)
            {
                offset_max = obj->w - _this->note_length * 1.8f;
                offset_min -= obj->w / 3;
            }
            // Limiting the range of movement
            if (_this->offset > offset_max)
            {
                _this->offset = offset_max;
                g_Limit = true;
            }
            else if (_this->offset < offset_min)
            {
                _this->offset = offset_min;
                g_Limit = true;
            }
            // gui_log("_this->offset = %d\n", _this->offset);
        }
        break;
    case TOUCH_HOLD_Y:
        {
            if (_this->dir == HORIZONTAL) { break; }
            gui_list_update_speed(_this, tp->deltaY);

            _this->offset = _this->hold + tp->deltaY;
            int offset_min = obj->h - _this->total_length - OUT_SCOPE;
            int offset_max = OUT_SCOPE;
            if (_this->style == LIST_CARD)
            {
                offset_max = obj->h - _this->note_length * 1.8f;
                offset_min -= obj->h / 3;
            }
            // Limiting the range of movement
            if (_this->offset > offset_max)
            {
                _this->offset = offset_max;
                g_Limit = true;
            }
            else if (_this->offset < offset_min)
            {
                _this->offset = offset_min;
                g_Limit = true;
            }
            // gui_log("_this->offset = %d\n", _this->offset);
        }
        break;

    default:
        break;
    }
}

static void gui_list_released_cb(void *obj, gui_event_t e, void *param)
{
    gui_list_t *_this = (gui_list_t *)obj;
    _this->hold = _this->offset;

    memset(_this->record, 0, sizeof(_this->record));
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_list_t *gui_list_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h,
                            uint16_t    note_length,
                            uint8_t     space,
                            LIST_DIR    dir)
{
    gui_list_t *_this = gui_malloc(sizeof(gui_list_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(_this, 0, sizeof(gui_list_t));
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }
    gui_obj_ctor(&_this->base, parent, name, x, y, w, h);

    _this->note_length = note_length;
    _this->space = space;
    _this->dir = dir;
    _this->factor = 0.05;
    if (dir == HORIZONTAL)
    {
        _this->total_length = w;
    }
    else
    {
        _this->total_length = h;
    }
    GET_BASE(_this)->obj_cb = gui_list_cb;
    GET_BASE(_this)->has_input_prepare_cb = true;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_end_cb = true;
    GET_BASE(_this)->has_destroy_cb  = true;
    GET_BASE(_this)->type = LIST;
    GET_BASE(_this)->create_done = true;
    gui_list_init(&(GET_BASE(_this)->child_list));

    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }

    if (_this->dir == HORIZONTAL)
    {
        gui_obj_add_event_cb(_this, gui_list_pressing_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, NULL);
    }
    else
    {
        gui_obj_add_event_cb(_this, gui_list_pressing_cb, GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
    }
    gui_obj_add_event_cb(_this, gui_list_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    return _this;
}

gui_list_note_t *gui_list_add_note(gui_list_t *list, bool add_at_top)
{
    gui_list_note_t *_this = gui_malloc(sizeof(gui_list_note_t));
    memset(_this, 0, sizeof(gui_list_note_t));

    int16_t x, y, w, h;
    int total_length = 0;
    uint8_t widget_num = add_at_top ? 0 : list->widget_num;
    list->widget_num++;
    if (list->dir == HORIZONTAL)
    {
        x = list->base.x + widget_num * (list->note_length + list->space);
        y = 0;
        w = list->note_length;
        h = list->base.h;
        total_length = list->base.x + list->widget_num * (list->note_length + list->space) - list->space;
    }
    else
    {
        x = 0;
        y = list->base.y + widget_num * (list->note_length + list->space);
        w = list->base.w;
        h = list->note_length;
        total_length = list->base.y + list->widget_num * (list->note_length + list->space) - list->space;
    }
    _this->start_x = x;
    _this->start_y = y;
    list->total_length = list->total_length < total_length ? total_length : list->total_length;

    gui_obj_ctor(&_this->base, &list->base, "list_note", x, y, w, h);
    GET_BASE(_this)->obj_cb = gui_list_note_cb;
    GET_BASE(_this)->has_input_prepare_cb = true;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_end_cb = false;
    GET_BASE(_this)->has_destroy_cb  = false;
    GET_BASE(_this)->type = LIST_TAB;
    GET_BASE(_this)->create_done = true;
    gui_list_init(&(GET_BASE(_this)->child_list));

    if ((GET_BASE(_this)->parent) != NULL)
    {
        if (add_at_top)
        {
            // adjust existed notes position
            gui_node_list_t *node = NULL;
            gui_list_for_each(node, &(list->base.child_list))
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                gui_list_note_t *note = (gui_list_note_t *)obj;
                if (list->dir == HORIZONTAL)
                {
                    note->start_x += (list->note_length + list->space);
                }
                else
                {
                    note->start_y += (list->note_length + list->space);
                }
                gui_obj_move(obj, note->start_x, note->start_y);
            }

            // insert to the end of the list
            gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list),
                                   &(GET_BASE(_this)->brother_list));
        }
        else
        {
            // insert to the begin of the list
            gui_list_insert(&((GET_BASE(_this)->parent)->child_list),
                            &(GET_BASE(_this)->brother_list));
        }
    }
    return _this;
}

void gui_list_set_style(gui_list_t *list, LIST_STYLE style)
{
    list->style = style;
}

void gui_list_set_factor(gui_list_t *list, float factor)
{
    list->factor = factor;
}

void gui_list_set_offset(gui_list_t *list, int16_t offset)
{
    list->offset = offset;
}
