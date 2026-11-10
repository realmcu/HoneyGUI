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
static int16_t g_Bar_Width = 0;
static int16_t g_Bar_Height = 0;
static gui_color_t g_Bar_Color = {0};
static int8_t g_offset_increase = 0;
static bool quick_slide = false;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static uint16_t gui_list_argb_to_rgb565(gui_color_t color);
static void gui_list_fill_bar(gui_list_t *_this);

static gui_list_note_t *gui_list_add_note(gui_list_t *list, int16_t index);

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

    if (_this->speed > max_speed)
    {
        _this->speed = max_speed;
    }
    else if (_this->speed < -max_speed)
    {
        _this->speed = -max_speed;
    }
}

static uint16_t gui_list_argb_to_rgb565(gui_color_t color)
{
    uint16_t r5 = (uint16_t)(color.color.rgba.r >> 3);
    uint16_t g6 = (uint16_t)(color.color.rgba.g >> 2);
    uint16_t b5 = (uint16_t)(color.color.rgba.b >> 3);
    return (uint16_t)((r5 << 11) | (g6 << 5) | b5);
}

/* Redraw the scrollbar image in place. The bar is 5px wide and at most one
 * screen edge long, so filling it a pixel at a time costs less than allocating
 * an anti-aliased rasteriser for it. */
static void gui_list_fill_bar(gui_list_t *_this)
{
    uint16_t *buf = (uint16_t *)((uint8_t *)_this->bar_data + sizeof(gui_rgb_data_head_t));
    int16_t bar_w = _this->bar->base.w;
    int16_t bar_h = _this->bar->base.h;
    int16_t rw = (g_Bar_Width < bar_w) ? g_Bar_Width : bar_w;
    int16_t rh = (g_Bar_Height < bar_h) ? g_Bar_Height : bar_h;
    uint16_t color = gui_list_argb_to_rgb565(g_Bar_Color);
    const int16_t radius = 2;

    memset(buf, 0, (size_t)bar_w * bar_h * sizeof(uint16_t));

    for (int16_t y = 0; y < rh; y++)
    {
        for (int16_t x = 0; x < rw; x++)
        {
            /* Skip the four corner squares that fall outside the arc. */
            int16_t cx = -1, cy = -1;
            if (x < radius) { cx = radius - x; }
            else if (x >= rw - radius) { cx = x - (rw - radius - 1); }
            if (y < radius) { cy = radius - y; }
            else if (y >= rh - radius) { cy = y - (rh - radius - 1); }

            if (cx > 0 && cy > 0 && (cx * cx + cy * cy) > radius * radius) { continue; }

            buf[y * bar_w + x] = color;
        }
    }
}

static void gui_list_update_bar(gui_obj_t *obj)
{
    gui_list_t *_this = (gui_list_t *)obj;
    int16_t w = _this->bar->base.w;
    int16_t h = _this->bar->base.h;
    int total_length = _this->total_length + (!_this->area_display) * (_this->dir == HORIZONTAL ?
                                                                       obj->x : obj->y);
    if (_this->need_update_bar)
    {
        _this->need_update_bar = false;
        if (_this->dir == HORIZONTAL)
        {
            g_Bar_Width = w * w / total_length;
            g_Bar_Height = LIST_BAR_WIDTH;
            gui_obj_hidden(GUI_BASE(_this->bar), (total_length == w));
        }
        else
        {
            g_Bar_Width = LIST_BAR_WIDTH;
            g_Bar_Height = h * h / total_length;
            gui_obj_hidden(GUI_BASE(_this->bar), total_length == h);
        }
        g_Bar_Color = _this->bar_color;
        gui_list_fill_bar(_this);
    }

    int32_t offset = _this->offset;
    offset = (offset > 0) ? 0 : offset;
    int32_t t_x = 0;
    int32_t t_y = 0;
    if (_this->dir == HORIZONTAL && total_length > w)
    {
        int32_t range = w - g_Bar_Width;
        t_x = -offset * range / (total_length - w);
        t_x = (t_x > range) ? range : t_x;
    }
    else if (_this->dir == VERTICAL && total_length > h)
    {
        int32_t range = h - g_Bar_Height;
        t_y = -offset * range / (total_length - h);
        t_y = (t_y > range) ? range : t_y;
    }
    _this->bar->base.x = (int16_t)(_this->bar_x + t_x);
    _this->bar->base.y = (int16_t)(_this->bar_y + t_y);
}

static void gui_list_wrap_loop_offset(gui_list_t *_this);

// Add inertia effect while tp released
static void gui_list_inertia_motion(gui_obj_t *obj)
{
    gui_list_t *_this = (gui_list_t *)obj;

    /* Programmatic scroll-to animation: exponential ease toward target */
    if (_this->scroll_to_active)
    {
        int distance = _this->scroll_target - _this->offset;
        if (distance == 0)
        {
            _this->scroll_to_active = false;
            gui_list_wrap_loop_offset(_this);
        }
        else
        {
            int delta = (int)(distance * 0.2f);
            if (delta == 0) { delta = (distance > 0) ? 1 : -1; }
            _this->offset += delta;
            _this->hold = _this->offset;
            gui_fb_change();
        }
        return;
    }

    g_Limit = false;
    int temp = _this->dir == HORIZONTAL ? obj->w : obj->h;
    int soft_offset_min = temp - _this->total_length -
                          _this->out_scope; //Range that can be slightly exceeded when scrolling
    int soft_offset_max = _this->out_scope;
    int hard_offset_min = temp -
                          _this->total_length; //The final range that the list must rebound when it is still
    int hard_offset_max = 0;
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
            int16_t abs_remainder = abs(remainder);
            if (!_this->loop && _this->offset == hard_offset_min)
            {
                return;
            }
            else
            {
                if (quick_slide)
                {
                    distance = g_offset_increase * ((abs_remainder > half_grid) ? (grid_size - abs_remainder) :
                                                    abs_remainder);
                }
                else
                {
                    /* Flip threshold = how far the drag must leave the current note */
                    uint8_t flip_num = 1;
                    uint8_t flip_den = 2;
                    int16_t r0 = remainder;
                    if (r0 < 0) { r0 += grid_size; }
                    int16_t base = _this->offset - r0;
                    int16_t target;
                    if (g_offset_increase > 0)
                    {
                        target = (r0 * flip_den > grid_size * flip_num) ? (base + grid_size) : base;
                    }
                    else
                    {
                        target = (r0 * flip_den < grid_size * (flip_den - flip_num)) ? base :
                                 (base + grid_size);
                    }
                    distance = target - _this->offset;
                }
            }

            float e_factor = 0.2f;
            int delta = (int16_t)(distance * e_factor); //exponential decay
            if (delta == 0) { delta = (distance > 0) ? 1 : -1; }
            _this->offset += delta;
        }
        _this->hold = _this->offset;
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
        _this->hold = _this->offset;
    }

}

// Wrap loop offset to prevent drift from accumulated programmatic scrolls.
// Same logic as released_cb's loop handling.
static void gui_list_wrap_loop_offset(gui_list_t *_this)
{
    if (!_this->loop || _this->note_num == 0) { return; }

    int ring = _this->note_num * (_this->note_length + _this->space);
    if (ring == 0) { return; }
    if (abs(_this->offset) < ring) { return; }

    _this->offset %= ring;
    _this->hold = _this->offset;
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
            temp = obj->x * _this->area_display;
        }
        else
        {
            pos = obj->y + note->start_y + _this->offset;
            range = obj->y + obj->h;
            temp = obj->y * _this->area_display;
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

static void gui_list_input_process(gui_obj_t *obj)
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
    if (!(tp->pressing || tp->released))
    {
        gui_list_inertia_motion(obj);
    }

    if (_this->keep_note_alive == true)
    {
        if (obj->child_list.next == &obj->child_list)
        {
            for (int i = 0; i < _this->note_num; i++)
            {
                gui_list_add_note(_this, i);
            }
        }
        return;
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
        while (pos < range - _this->space)
        {
            if (++index >= _this->note_num * (_this->loop * 2 + 1)) {break;}
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
            temp = obj->x * _this->area_display;
        }
        else
        {
            pos = obj->y + note->start_y + _this->offset;
            temp = obj->y * _this->area_display;
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

    if (_this->enable_scroll)
    {
        if (!g_Limit && (gui_view_t *)(obj->parent) != gui_view_get_next())
        {
            if (_this->dir == HORIZONTAL)
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");
            }
            else
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, "touch");
            }
        }
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    }

    if (_this->bar)
    {
        gui_list_update_bar(obj); // Update the scrollbar
    }

    uint8_t last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, sizeof(gui_list_t));
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
        case OBJ_INPUT_PROCESS:
            {
                gui_list_input_process(obj);
            }
            break;

        case OBJ_PRE_PROCESS:
            {
                gui_list_prepare(obj);
            }
            break;

        case OBJ_POST_PROCESS:
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

static void gui_list_note_input_process(gui_obj_t *obj)
{
    gui_list_note_t *_this = (gui_list_note_t *)obj;
    gui_list_t *list = (gui_list_t *)obj->parent;

    if (list->dir == HORIZONTAL)
    {
        obj->x = _this->start_x + list->offset;
    }
    else
    {
        obj->y = _this->start_y + list->offset;
    }
}

/* Place the note on a circle of the given radius, centred on the list. Only
 * the cross axis moves; the scroll axis keeps its position so hit testing and
 * the fade/scroll logic still see the layout coordinate. */
static void gui_list_note_circle(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;

    int32_t r = list->circle_radius;
    int32_t diff = (list->dir == HORIZONTAL)
                   ? (list->base.w / 2 - (obj->x + list->note_length / 2))
                   : (list->base.h / 2 - (obj->y + list->note_length / 2));
    /* Integer square root: r <= UINT16_MAX, so r*r fits in uint32_t. */
    int32_t coord = r;
    if (abs(diff) < r)
    {
        uint32_t rest = (uint32_t)(r * r - diff * diff);
        uint32_t root = 0;
        uint32_t bit = 1uL << 30;
        while (bit > rest) { bit >>= 2; }
        while (bit != 0)
        {
            if (rest >= root + bit)
            {
                rest -= root + bit;
                root = (root >> 1) + bit;
            }
            else
            {
                root >>= 1;
            }
            bit >>= 2;
        }
        coord = r - (int32_t)root;
    }
    if (list->dir == HORIZONTAL)
    {
        obj->y = coord;
    }
    else
    {
        obj->x = coord;
    }
}

/* Fade the note as it leaves the viewport. The original wrote the list origin
 * straight into the matrix translation column; with integer translation that is
 * just the coordinate itself. */
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
            obj->opacity_value = UINT8_MAX - UINT8_MAX * abs(obj->x) / obj->w;
            obj->x = list->base.x;
        }
        else
        {
            obj->opacity_value = UINT8_MAX - UINT8_MAX * abs(obj->y) / obj->h;
            obj->y = list->base.y;
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
    case LIST_FADE:
        gui_list_note_fade(obj);
        break;
    default:
        break;
    }
}

static void gui_list_note_prepare(gui_obj_t *obj)
{
    gui_list_t *list = (gui_list_t *)obj->parent;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    if (list->dir == VERTICAL)
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");
    }
    else
    {
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_VERTICAL, "touch");
    }

    gui_list_note_transform(obj);
}

static void gui_list_note_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PROCESS:
            {
                gui_list_note_input_process(obj);
            }
            break;

        case OBJ_PRE_PROCESS:
            {
                gui_list_note_prepare(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_list_pressing_cb(void *object, gui_event_t *e)
{
    (void)e;
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)object;
    gui_list_t *_this = (gui_list_t *)object;
    _this->scroll_to_active = false;
    g_Limit = false;

    int offset_min = 0;
    int offset_max = 0;

    switch (_this->dir)
    {
    case HORIZONTAL:
        {
            gui_list_update_speed(_this, tp->deltaX);
            g_offset_increase = tp->deltaX > 0 ? 1 : -1;
            _this->offset = _this->hold + tp->deltaX;
            offset_min = obj->w - _this->total_length - _this->out_scope;
            offset_max = _this->out_scope;
        }
        break;
    case VERTICAL:
        {
            gui_list_update_speed(_this, tp->deltaY);
            g_offset_increase = tp->deltaY > 0 ? 1 : -1;
            _this->offset = _this->hold + tp->deltaY;
            offset_min = obj->h - _this->total_length - _this->out_scope;
            offset_max = _this->out_scope;
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

static void gui_list_released_cb(void *obj, gui_event_t *e)
{
    (void)e;
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
            int16_t index = note->index;
            index %= _this->note_num;
            index += _this->note_num;
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
    if (abs(_this->speed) >= GUI_MIN_SPEED)
    {
        quick_slide = true;
    }
    else
    {
        quick_slide = false;
    }

    memset(_this->record, 0, sizeof(_this->record));
}

static void gui_list_create_bar(gui_list_t *_this,
                                int16_t     x,
                                int16_t     y,
                                int16_t     w,
                                int16_t     h)
{
    gui_dispdev_t *dc = gui_get_dc();
    int bar_x = x;
    int bar_y = y;
    int bar_w = 0;
    int bar_h = 0;
    if (_this->dir == HORIZONTAL)
    {
        bar_x = x * _this->area_display;
        bar_y = y;
        bar_w = _this->area_display ? w : dc->screen_width;
        bar_h = LIST_BAR_WIDTH;
    }
    else
    {
        bar_x = w - LIST_BAR_WIDTH;
        bar_y = y * _this->area_display;
        bar_w = LIST_BAR_WIDTH;
        bar_h = _this->area_display ? h : dc->screen_height;
    }
    size_t buffer_size = (size_t)bar_w * bar_h * sizeof(uint16_t) + sizeof(gui_rgb_data_head_t);
    _this->bar_data = gui_malloc(buffer_size);
    GUI_ASSERT(_this->bar_data != NULL);
    memset(_this->bar_data, 0, buffer_size);
    ((gui_rgb_data_head_t *)_this->bar_data)->type = RGB565;
    ((gui_rgb_data_head_t *)_this->bar_data)->w = bar_w;
    ((gui_rgb_data_head_t *)_this->bar_data)->h = bar_h;
    _this->bar = gui_img_create_from_mem(_this->base.parent, "list_bar", _this->bar_data, bar_x, bar_y,
                                         bar_w, bar_h);
    gui_img_set_mode(_this->bar, IMG_SRC_OVER_MODE);
    _this->bar->base.w = bar_w;
    _this->bar->base.h = bar_h;
    _this->bar_x = bar_x;
    _this->bar_y = bar_y;
    _this->need_update_bar = true;
    _this->bar_color = APP_COLOR_WHITE;
}

static gui_list_note_t *gui_list_add_note(gui_list_t *list, int16_t index)
{
    list->last_created_note_index = index;
    // gui_log("add note %d\n", index);

    gui_list_note_t *_this = gui_malloc(sizeof(gui_list_note_t));
    GUI_ASSERT(_this != NULL);
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
    GET_BASE(_this)->has_input_process_cb = true;
    GET_BASE(_this)->has_pre_process_cb = true;
    GET_BASE(_this)->has_post_process_cb = false;
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
    GUI_ASSERT(_this != NULL);
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
    _this->enable_scroll = true;
    if (dir == HORIZONTAL)
    {
        _this->total_length = w;
        _this->circle_radius = _this->base.h;
    }
    else
    {
        _this->total_length = h;
        _this->circle_radius = _this->base.w;
    }
    GET_BASE(_this)->obj_cb = gui_list_cb;
    GET_BASE(_this)->has_input_process_cb = true;
    GET_BASE(_this)->has_pre_process_cb = true;
    GET_BASE(_this)->has_post_process_cb = true;
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

    gui_obj_t *obj = GET_BASE(list);
    if (obj->child_list.next != &obj->child_list)
    {
        gui_obj_child_free(obj);
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
    if (!list->loop)
    {
        if (offset > offset_max)
        {
            offset = offset_max;
        }
        else if (offset < offset_min)
        {
            offset = offset_min;
        }
    }

    list->offset = offset;
    list->hold = offset;
}

void gui_list_set_bar_color(gui_list_t *list, gui_color_t color)
{
    list->bar_color = color;
}

void gui_list_set_circle_radius(gui_list_t *list, uint16_t radius)
{
    list->circle_radius = radius;
}

void gui_list_enable_loop(gui_list_t *list, bool loop)
{
    int16_t temp = list->dir == HORIZONTAL ? list->base.w : list->base.h;
    if (list->total_length <= temp || list->keep_note_alive)
    {
        gui_log("warning: list loop is disabled!!!\n");
        list->loop = false;
        return;
    }
    if (list->out_scope > 0)
    {
        list->out_scope = 0;
    }
    list->loop = loop;
}

void gui_list_enable_area_display(gui_list_t *list, bool enable)
{
    list->area_display = enable;
    if (list->bar)
    {
        gui_free(list->bar_data);
        gui_obj_tree_free(list->bar);
        gui_list_create_bar(list, list->base.x, list->base.y, list->base.w, list->base.h);
    }
}

void gui_list_keep_note_alive(gui_list_t *list, bool enable)
{
    if (list->loop)
    {
        gui_log("warning: keep note alive is disabled when list enables loop!!!\n");
        list->keep_note_alive = false;
        return;
    }
    list->keep_note_alive = enable;
}

/**
 * @brief Calculate the target offset for a given note index, considering style and boundaries.
 * @return The scroll distance to reach the target, or 0 if already at target.
 */
static int gui_list_calc_scroll_distance(gui_list_t *list, uint16_t note_index)
{
    int16_t grid_size = list->note_length + list->space;
    if (grid_size == 0) { return 0; }

    int16_t view_size = (list->dir == HORIZONTAL) ? list->base.w : list->base.h;

    /* Calculate target offset to bring the note into view */
    int target_offset = -(note_index * grid_size);

    if (list->loop)
    {
        /* Loop mode: pick the shortest path around the ring.
         * Ring circumference = note_num * grid_size (includes consistent spacing
         * between all adjacent notes, including the last-to-first gap).
         * This differs from total_length which omits the trailing space. */
        int ring = list->note_num * grid_size;
        int direct = target_offset - list->offset;
        int alt = (direct > 0) ? (direct - ring) : (direct + ring);
        return (abs(alt) < abs(direct)) ? alt : direct;
    }
    else
    {
        /* Non-loop: clamp target to valid scroll boundaries */
        int hard_min = view_size - list->total_length;
        int hard_max = 0;
        if (target_offset > hard_max) { target_offset = hard_max; }
        if (target_offset < hard_min) { target_offset = hard_min; }
        return target_offset - list->offset;
    }
}

void gui_list_scroll_to_note(gui_list_t *list, uint16_t note_index)
{
    if (note_index >= list->note_num) { return; }

    /* Normalize loop offset before distance calculation to prevent drift */
    gui_list_wrap_loop_offset(list);

    int distance = gui_list_calc_scroll_distance(list, note_index);
    if (distance == 0) { return; }

    /* Timer approach: animate directly toward exact target offset.*/
    /* No inertia or auto_align needed: exponential ease in inertia_motion*/
    /* drives offset to scroll_target pixel-perfectly. */
    list->scroll_target = list->offset + distance;
    list->scroll_to_active = true;
    list->inertia = false;
    list->speed = 0;
    gui_fb_change();
}

void gui_list_jump_to_note(gui_list_t *list, uint16_t note_index)
{
    if (note_index >= list->note_num) { return; }

    /* Normalize loop offset before calculation to prevent drift */
    gui_list_wrap_loop_offset(list);

    int distance = gui_list_calc_scroll_distance(list, note_index);
    if (distance == 0) { return; }

    /* Set offset directly: no animation */
    list->offset += distance;
    list->hold = list->offset;
    list->scroll_to_active = false;
    list->inertia = false;
    list->speed = 0;

    /* Normalize loop offset after jump (also updates hold) */
    gui_list_wrap_loop_offset(list);

    gui_fb_change();
}

uint16_t gui_list_get_current_note_index(gui_list_t *list)
{
    int16_t grid_size = list->note_length + list->space;
    if (grid_size == 0) { return 0; }

    /* Reverse the target_offset formula: offset = -index * grid_size
     * => index = -offset / grid_size. Round to the nearest note using floor
     * division; C truncates toward zero and would round the wrong way. */
    int shifted = -list->offset + grid_size / 2;
    int index = shifted / grid_size;
    if (shifted < 0 && (shifted % grid_size) != 0)
    {
        index--;
    }

    if (list->loop)
    {
        index = index % list->note_num;
        if (index < 0) { index += list->note_num; }
    }
    else
    {
        if (index < 0) { index = 0; }
        if (index >= list->note_num) { index = list->note_num - 1; }
    }

    return (uint16_t)index;
}

void gui_list_enable_scroll(gui_list_t *list, bool enable)
{
    list->enable_scroll = enable;
}
