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
#include "gui_canvas.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static bool g_Limit = false;
static int16_t g_Bar_Width = 0;
static int16_t g_Bar_Height = 0;
static gui_color_t g_Bar_Color = {0};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static gui_list_note_t *gui_list_add_note(gui_list_t *list, int16_t index);

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
    gui_fb_change();
}

static void gui_list_update_speed(gui_list_t *_this, int16_t tp_delta)
{
    int record_num = 4;

    for (int i = 0; i < record_num; i++)
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

static void gui_list_update_bar_data_cb(NVGcontext *vg)
{
    nvgRoundedRect(vg, 0, 0, g_Bar_Width, g_Bar_Height, 2);
    nvgFillColor(vg, nvgRGBA(g_Bar_Color.color.rgba.r,
                             g_Bar_Color.color.rgba.g,
                             g_Bar_Color.color.rgba.b,
                             g_Bar_Color.color.rgba.a));
    nvgFill(vg);
}

static void gui_list_update_bar(gui_obj_t *obj)
{
    gui_list_t *_this = (gui_list_t *)obj;
    if (_this->need_update_bar)
    {
        _this->need_update_bar = false;
        if (_this->dir == HORIZONTAL)
        {
            int16_t w = obj->w;
            g_Bar_Width = w * w / _this->total_length;
            g_Bar_Height = LIST_BAR_WIDTH;
            gui_obj_hidden(GUI_BASE(_this->bar), (g_Bar_Width == w));
        }
        else
        {
            int16_t h = obj->h;
            g_Bar_Width = LIST_BAR_WIDTH;
            g_Bar_Height = h * h / _this->total_length;
            gui_obj_hidden(GUI_BASE(_this->bar), (g_Bar_Height == h));
        }
        g_Bar_Color = _this->bar_color;
        memset(_this->bar_data, 0, _this->bar->base.w * _this->bar->base.h * 4);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, _this->bar->base.w, _this->bar->base.h,
                                          gui_list_update_bar_data_cb,
                                          _this->bar_data);
    }

    if (_this->bar)
    {
        float t_x = 0;
        float t_y = 0;
        float offset = (float)(_this->offset);
        offset = (offset > 0) ? 0 : offset;
        if (_this->dir == HORIZONTAL && _this->total_length > obj->w)
        {
            float range = (float)(obj->w - obj->x - g_Bar_Width);
            t_x = offset * range / (_this->total_length - obj->w);
            t_x = (t_x > range) ? range : t_x;
        }
        else if (_this->dir == VERTICAL && _this->total_length > obj->h)
        {
            float range = (float)(obj->h - g_Bar_Height);
            t_y = -offset * range / (_this->total_length - obj->h);
            t_y = (t_y > range) ? range : t_y;
        }
        gui_img_translate(_this->bar, t_x, t_y);
    }
}

// Add inertia effect while tp released
static void gui_list_inertia_motion(gui_obj_t *obj)
{
    gui_list_t *_this = (gui_list_t *)obj;

    g_Limit = false;
    int temp = _this->dir == HORIZONTAL ? obj->w : obj->h;
    int soft_offset_min = temp - _this->total_length -
                          _this->out_scope; //Range that can be slightly exceeded when scrolling
    int soft_offset_max = _this->out_scope;
    int hard_offset_min = temp -
                          _this->total_length; //The final range that the list must rebound when it is still
    int hard_offset_max = 0;
    if (_this->style == LIST_CARD)
    {
        soft_offset_min = temp - _this->total_length - _this->card_stack_location;
        soft_offset_max = temp - _this->note_length;
        hard_offset_min = soft_offset_min;
        hard_offset_max = soft_offset_max;
    }
    if (!_this->inertia || _this->speed == 0)
    {
        if (!_this->loop && (_this->offset > hard_offset_max || _this->offset < hard_offset_min))
        {
            float e_factor = 0.2f;
            int16_t target = _this->offset >= 0 ? hard_offset_max : hard_offset_min;
            int16_t distance = target - _this->offset;
            int delta = (int16_t)(distance * e_factor); //exponential decay
            if (delta == 0) { delta = (distance > 0) ? 1 : -1; }
            _this->offset += delta;
        }
        else if (_this->auto_align)
        {
            int16_t grid_size = _this->note_length + _this->space;
            if (grid_size == 0) { return; }

            int16_t remainder = _this->offset % grid_size;
            if (remainder == 0) { return; }

            int16_t distance;
            int16_t half_grid = grid_size / 2;

            if (remainder > half_grid)
            {
                distance = grid_size - remainder;
            }
            else if (remainder < -half_grid)
            {
                distance = -grid_size - remainder;
            }
            else
            {
                distance = -remainder;
            }
            float e_factor = 0.2f;
            int delta = (int16_t)(distance * e_factor); //exponential decay
            if (delta == 0) { delta = (distance > 0) ? 1 : -1; }
            _this->offset += delta;
        }
    }
    else
    {
        _this->offset += _this->speed;
        _this->speed = (int16_t)((1 - _this->factor) * _this->speed);
        if (!_this->loop)
        {
            if (_this->offset > soft_offset_max)
            {
                _this->offset = soft_offset_max;
                _this->speed = 0;
            }
            else if (_this->offset < soft_offset_min)
            {
                _this->offset = soft_offset_min;
                _this->speed = 0;
            }
        }
    }
    _this->hold = _this->offset;
}

// Free notes that are out of the list
static void gui_list_free_notes(gui_obj_t *obj)
{
    gui_list_t *_this = (gui_list_t *)obj;
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &(_this->base.child_list))
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        gui_list_note_t *note = (gui_list_note_t *)o;
        int16_t pos = 0;
        int16_t range = 0;
        int16_t temp = 0;
        if (_this->dir == HORIZONTAL)
        {
            pos = obj->x + note->start_x + _this->offset;
            range = obj->x + obj->w;
            temp = obj->x;
        }
        else
        {
            pos = obj->y + note->start_y + _this->offset;
            range = obj->y + obj->h;
            temp = obj->y;
        }
        if (_this->style == LIST_CARD)
        {
            range += (1.2 * _this->note_length); // scale_1 = 0.8f;
        }

        if (pos + _this->note_length <= temp)
        {
            // gui_log("free note %d\n", note->index);
            _this->last_created_note_index += (_this->last_created_note_index == note->index);
            gui_obj_tree_free(o);
        }
        else if (pos >= range)
        {
            // gui_log("free note %d\n", note->index);
            _this->last_created_note_index -= (_this->last_created_note_index == note->index);
            gui_obj_tree_free(o);
            _this->max_created_note_index -= 1;
        }
    }
}

static void gui_list_input_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    GUI_UNUSED(dc);

    if (_this->note_num == 0)
    {
        g_Limit = true;
        return;
    }

    if (!tp->pressing)
    {
        gui_list_inertia_motion(obj);
    }

    gui_list_free_notes(obj);

    // create the first note when the list is empty
    if (obj->child_list.next == &obj->child_list)
    {
        gui_list_add_note(_this, abs(_this->offset) / (_this->note_length + _this->space));
    }

    // create note from the last note
    {
        gui_obj_t *o = gui_list_entry(_this->base.child_list.next, gui_obj_t, brother_list);
        gui_list_note_t *note = (gui_list_note_t *)o;
        int16_t index = note->index;
        _this->max_created_note_index = index;
        int16_t pos = 0;
        int16_t range = 0;
        if (_this->dir == HORIZONTAL)
        {
            pos = obj->x + note->start_x + _this->offset + _this->note_length;
            range = obj->w + obj->x;
        }
        else
        {
            pos = obj->y + note->start_y + _this->offset + _this->note_length;
            range = obj->h + obj->y;
        }
        if (_this->style == LIST_CARD)
        {
            range += (1.2 * _this->note_length); // scale_1 = 0.8f;
        }
        while (pos < range - _this->space)
        {
            if (++index >= _this->note_num * (_this->loop + 1)) {break;}
            gui_list_add_note(_this, index);
            pos += (_this->note_length + _this->space);
        }
    }
    // create note from the first note
    {
        gui_obj_t *o = gui_list_entry((&(_this->base.child_list))->prev, gui_obj_t, brother_list);
        gui_list_note_t *note = (gui_list_note_t *)o;
        int16_t index = note->index;
        int16_t pos = 0;
        int16_t temp = 0;
        if (_this->dir == HORIZONTAL)
        {
            pos = obj->x + note->start_x + _this->offset;
            temp = obj->x;
        }
        else
        {
            pos = obj->y + note->start_y + _this->offset;
            temp = obj->y;
        }
        while (pos > _this->space + temp)
        {
            if (--index < -((_this->loop ? 2 : 0) * _this->note_num)) {break;}
            gui_list_note_t *note = gui_list_add_note(_this, index);
            if (_this->dir == HORIZONTAL)
            {
                note->base.x = -_this->note_length * 2; // for judgement of creating entrance animation
            }
            else
            {
                note->base.y = -_this->note_length * 2; // for judgement of creating entrance animation
            }
            pos -= (_this->note_length + _this->space);
        }
    }
}

static void gui_list_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_t *_this = (gui_list_t *)obj;
    GUI_UNUSED(dc);

    // ensure slide back when list moves to the limit position
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

    gui_list_update_bar(obj); // Update the scrollbar

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

    if (_this->bar_data)
    {
        gui_free(_this->bar_data);
    }
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

static inline void gui_list_note_start_anim(gui_list_note_t *note, gui_obj_t *o,
                                            bool speed_positive)
{
    note->is_speed_positive = speed_positive;
    gui_obj_create_timer(o, 10, true, gui_list_note_animate_timer_cb);
    gui_obj_start_timer(o);
    note->animate_cnt = 1;
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
                gui_list_note_start_anim(_this, obj, true);
            }
            else if (x_old >= list->base.w && obj->x < list->base.w)
            {
                gui_list_note_start_anim(_this, obj, false);
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
                gui_list_note_start_anim(_this, obj, true);
            }
            else if (y_old >= list->base.h && obj->y < list->base.h)
            {
                gui_list_note_start_anim(_this, obj, false);
            }
        }
    }
}

static void gui_list_note_circle(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;
    matrix_translate(-obj->x, -obj->y, obj->matrix);

    int32_t r = (list->dir == HORIZONTAL) ? list->base.h : list->base.w;
    r *= 2;
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

static void gui_list_note_zoom(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;
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

static void gui_list_note_card(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;
    int16_t index = ((gui_list_note_t *)obj)->index;
    matrix_translate(-obj->x, -obj->y, obj->matrix);

    const float scale_0 = 1.0f;
    const float scale_1 = 0.8f;
    int obj_location;
    int16_t temp = 0;
    if (list->dir == HORIZONTAL)
    {
        temp = (int16_t)(list->base.x + list->base.w - list->card_stack_location);
        int16_t limit = 0;
        if (index <= 1)
        {
            limit = temp - (int16_t)(list->note_length * (2 - scale_1 - 0.2f * index));
            if (obj->x > limit)
            {
                obj->x = limit;
            }
        }
        else
        {
            limit = temp - (int16_t)(list->note_length * scale_1);
            if (obj->x > limit)
            {
                obj->x = limit;
            }
        }
        obj_location = obj->x;
    }
    else
    {
        temp = (int16_t)(list->base.y + list->base.h - list->card_stack_location);
        int16_t limit = 0;
        if (index <= 1)
        {
            limit = temp - (int16_t)(list->note_length * (2 - scale_1 - 0.2f * index));
            if (obj->y > limit)
            {
                obj->y = limit;
            }
        }
        else
        {
            limit = temp - (int16_t)(list->note_length * scale_1);
            if (obj->y > limit)
            {
                obj->y = limit;
            }
        }
        obj_location = obj->y;
    }
    int location_1 = temp - (int16_t)(list->note_length * scale_1);
    int location_0 = temp - (int16_t)(list->note_length * (2 - scale_1));

    if (obj_location >= location_0 && obj_location <= location_1)
    {
        float scale = scale_0 - (scale_0 - scale_1) * (obj_location - location_0) /
                      (location_1 - location_0);
        matrix_translate(obj->x, obj->y, obj->matrix); //prevent bottom space of screen

        int w = 0;
        int h = 0;
        if (list->dir == HORIZONTAL)
        {
            h = obj->h / 2;
        }
        else
        {
            w = obj->w / 2;
        }
        matrix_translate(w, h, obj->matrix);
        matrix_scale(scale, scale, obj->matrix);
        matrix_translate(-w, -h, obj->matrix);
    }
    else
    {
        matrix_translate(obj->x, obj->y, obj->matrix);
    }
}

static void gui_list_note_fan(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;
    gui_list_note_t *_this = (gui_list_note_t *)obj;
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

static void gui_list_note_helix(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    extern void gui_list_rotate_helix(gui_list_note_t *_this, float degree);
    if (obj->timer)
    {
        float start = _this->is_speed_positive ? -180.0f : 180.0f;
        float degree = start * (1.0f - (float)_this->animate_cnt / LIST_TAB_ANIMATE_MAX);
        gui_list_rotate_helix(_this, degree);
    }
}

static void gui_list_note_curl(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    extern void gui_list_rotate_curl(gui_list_note_t *_this, float degree);
    if (obj->timer)
    {
        float degree = 90.0f * (1.0f - (float)_this->animate_cnt / LIST_TAB_ANIMATE_MAX);
        gui_list_rotate_curl(_this, degree);
    }
}

static void gui_list_note_fade(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    gui_list_t *list = (gui_list_t *)obj->parent;
    static bool flag = true;
    if (_this->index == list->last_created_note_index)
    {
        if (list->dir == HORIZONTAL)
        {
            obj->opacity_value = UINT8_MAX - UINT8_MAX * abs(obj->x - list->base.x) / obj->w;
            obj->matrix->m[0][2] = list->base.x;
        }
        else
        {
            obj->opacity_value = UINT8_MAX - UINT8_MAX * abs(obj->y - list->base.y) / obj->h;
            obj->matrix->m[1][2] = list->base.y;

        }
        flag = obj->opacity_value > UINT8_MAX / 2;
    }
    else
    {
        if (tp->pressing)
        {
            obj->opacity_value = 0;
        }
        else
        {
            if (flag)
            {
                obj->opacity_value = 0;
            }
            else
            {
                obj->opacity_value = UINT8_MAX;
                obj->matrix->m[0][2] = list->base.x;
                obj->matrix->m[1][2] = list->base.y;
            }
        }
    }
}

static void gui_list_note_transform(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;
    switch (list->style)
    {
    case LIST_CIRCLE:
        gui_list_note_circle(obj);
        break;
    case LIST_ZOOM:
        gui_list_note_zoom(obj);
        break;
    case LIST_CARD:
        gui_list_note_card(obj);
        break;
    case LIST_FAN:
        gui_list_note_fan(obj);
        break;
    case LIST_HELIX:
        gui_list_note_helix(obj);
        break;
    case LIST_CURL:
        gui_list_note_curl(obj);
        break;
    case LIST_FADE:
        gui_list_note_fade(obj);
        break;
    default:
        break;
    }
}

static void gui_list_note_prepare(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    // touch_info_t *tp = tp_get_info();
    gui_list_t *list = (gui_list_t *)obj->parent;

    // matrix_translate(-list->base.x, -list->base.y,
    //                  obj->matrix); //eliminate list offsets if x\y is not 0

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

    gui_list_note_transform(obj);

    matrix_translate(_this->t_x, _this->t_y,
                     obj->matrix); //_this way to move note in order not to lose tp
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
    (void)e;
    (void)param;
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)object;
    gui_list_t *_this = (gui_list_t *)object;
    g_Limit = false;

    int offset_min = 0;
    int offset_max = 0;
    switch (_this->dir)
    {
    case HORIZONTAL:
        {
            gui_list_update_speed(_this, tp->deltaX);

            _this->offset = _this->hold + tp->deltaX;
            offset_min = obj->w - _this->total_length - _this->out_scope;
            offset_max = _this->out_scope;
            if (_this->style == LIST_CARD)
            {
                offset_max = obj->w - _this->note_length;
                offset_min -= _this->card_stack_location;
            }
        }
        break;
    case VERTICAL:
        {
            gui_list_update_speed(_this, tp->deltaY);

            _this->offset = _this->hold + tp->deltaY;
            offset_min = obj->h - _this->total_length - _this->out_scope;
            offset_max = _this->out_scope;
            if (_this->style == LIST_CARD)
            {
                offset_max = obj->h - _this->note_length;
                offset_min -= _this->card_stack_location;
            }
        }
        break;

    default:
        break;
    }
    // Limiting the range of movement
    if (!_this->loop)
    {
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
    }

    // gui_log("_this->offset = %d\n", _this->offset);
}

static void gui_list_released_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_list_t *_this = (gui_list_t *)obj;
    if (_this->loop && abs(_this->offset) >= _this->total_length)
    {
        // gui_log("loop offset %d\n", _this->offset);
        _this->offset %= _this->total_length;
        _this->max_created_note_index %= _this->note_num;
        _this->last_created_note_index %= _this->note_num;

        gui_node_list_t *node = NULL;
        gui_node_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &(_this->base.child_list))
        {
            gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
            gui_list_note_t *note = (gui_list_note_t *)o;
            uint16_t index = note->index;
            index %= _this->note_num;
            // gui_log("index %d to %d\n", note->index, index);
            note->index = index;
            if (_this->dir == HORIZONTAL)
            {
                note->start_x = index * (_this->note_length + _this->space);
            }
            else
            {
                note->start_y = index * (_this->note_length + _this->space);
            }
        }
    }
    _this->hold = _this->offset;

    memset(_this->record, 0, sizeof(_this->record));
}

static void gui_list_create_bar(gui_list_t *_this,
                                int16_t     x,
                                int16_t     y,
                                int16_t     w,
                                int16_t     h)
{
    (void)x;
    int bar_x = x;
    int bar_y = y;
    int bar_w = 0;
    int bar_h = 0;
    if (_this->dir == HORIZONTAL)
    {
        bar_y = y;
        bar_w = w;
        bar_h = LIST_BAR_WIDTH;
    }
    else
    {
        bar_x = w - LIST_BAR_WIDTH;
        bar_w = LIST_BAR_WIDTH;
        bar_h = h;
    }
    int pixel_bytes = 4;
    size_t buffer_size = bar_h * bar_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    _this->bar_data = gui_lower_malloc(buffer_size);
    memset(_this->bar_data, 0, buffer_size);
    _this->bar = gui_img_create_from_mem(_this->base.parent, "list_bar", _this->bar_data, bar_x, bar_y,
                                         bar_w, bar_h);
    gui_img_set_mode(_this->bar, IMG_SRC_OVER_MODE);
    _this->bar->base.w = bar_w;
    _this->bar->base.h = bar_h;
    _this->need_update_bar = true;
    _this->bar_color = APP_COLOR_WHITE;
}

static gui_list_note_t *gui_list_add_note(gui_list_t *list, int16_t index)
{
    list->last_created_note_index = index;
    // gui_log("add note %d\n", index);

    gui_list_note_t *_this = gui_malloc(sizeof(gui_list_note_t));
    memset(_this, 0, sizeof(gui_list_note_t));

    bool add_at_head = 0;
    if (list->max_created_note_index < index)
    {
        add_at_head = true;
        list->max_created_note_index = index;
    }

    int16_t x, y, w, h;
    if (list->dir == HORIZONTAL)
    {
        x = index * (list->note_length + list->space);
        y = 0;
        w = list->note_length;
        h = list->base.h;
    }
    else
    {
        x = 0;
        y = index * (list->note_length + list->space);
        w = list->base.w;
        h = list->note_length;
    }
    _this->start_x = x;
    _this->start_y = y;
    _this->index = index;

    gui_obj_ctor(&_this->base, &list->base, "list_note", x, y, w, h);
    GET_BASE(_this)->obj_cb = gui_list_note_cb;
    GET_BASE(_this)->has_input_prepare_cb = true;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_end_cb = false;
    GET_BASE(_this)->has_destroy_cb  = false;
    GET_BASE(_this)->type = LIST_NOTE;
    GET_BASE(_this)->create_done = true;
    gui_list_init(&(GET_BASE(_this)->child_list));

    if (add_at_head == false)
    {
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

    if (list->note_design != NULL)
    {
        list->note_design(GUI_BASE(_this), list->design_param);
    }
    return _this;
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
                            LIST_DIR    dir,
                            void (* note_design)(gui_obj_t *obj, void *param),
                            void        *design_param,
                            bool        create_bar)
{
    gui_list_t *_this = gui_malloc(sizeof(gui_list_t));
    // gui_dispdev_t *dc = gui_get_dc();
    memset(_this, 0, sizeof(gui_list_t));
    if (w == 0)
    {
        w = (int)gui_get_screen_width() - x;
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height() - y;
    }
    gui_obj_ctor(&_this->base, parent, name, x, y, w, h);

    _this->note_length = note_length;
    _this->space = space;
    _this->dir = dir;
    _this->factor = 0.05;
    _this->note_design = note_design;
    _this->design_param = design_param;
    _this->inertia = true;
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

    if (create_bar)
    {
        gui_list_create_bar(_this, x, y, w, h);
    }
    return _this;
}

void gui_list_set_note_num(gui_list_t *list, uint16_t num)
{
    list->note_num = num;
    list->max_created_note_index = 0;
    list->last_created_note_index = 0;

    if (list->dir == HORIZONTAL)
    {
        list->total_length = list->note_num * (list->note_length + list->space) - list->space;
        list->total_length = (list->total_length < list->base.w) ? list->base.w : list->total_length;
    }
    else
    {
        list->total_length = list->note_num * (list->note_length + list->space) - list->space;
        list->total_length = (list->total_length < list->base.h) ? list->base.h : list->total_length;
    }

    if (list->bar)
    {
        list->need_update_bar = true;
    }
}

void gui_list_set_style(gui_list_t *list, LIST_STYLE style)
{
    list->style = style;
}

void gui_list_set_out_scope(gui_list_t *list, int16_t out_scope)
{
    list->out_scope = out_scope;
}

void gui_list_set_factor(gui_list_t *list, float factor)
{
    list->factor = factor;
}

void gui_list_set_auto_align(gui_list_t *list, bool auto_align)
{
    list->auto_align = auto_align;
}

void gui_list_set_inertia(gui_list_t *list, bool inertia)
{
    list->inertia = inertia;
}

void gui_list_set_offset(gui_list_t *list, int16_t offset)
{
    int temp = list->dir == HORIZONTAL ? list->base.w : list->base.h;
    int offset_min = temp - list->total_length - list->out_scope;
    int offset_max = list->out_scope;
    if (list->style == LIST_CARD)
    {
        offset_max = temp - list->note_length;
        offset_min -= list->card_stack_location;
    }
    if (offset > offset_max)
    {
        offset = offset_max;
    }
    else if (offset < offset_min)
    {
        offset = offset_min;
    }

    list->offset = offset;
    list->hold = offset;
}

void gui_list_set_bar_color(gui_list_t *list, gui_color_t color)
{
    list->bar_color = color;
}

void gui_list_set_card_stack_location(gui_list_t *list, int16_t location)
{
    list->card_stack_location = location;
}

void gui_list_enable_loop(gui_list_t *list, bool loop)
{
    int16_t temp = list->dir == HORIZONTAL ? list->base.w : list->base.h;
    if (list->total_length <= temp)
    {
        gui_log("warning: list loop is disabled");
        list->loop = false;
        return;
    }
    list->loop = loop;
}
