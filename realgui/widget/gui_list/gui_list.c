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


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_list_update_speed(gui_list_widget_t *this, int16_t tp_delta)
{
    int record_num = 4;

    for (size_t i = 0; i < record_num; i++)
    {
        this->record[i] = this->record[i + 1];
    }

    this->record[record_num] = tp_delta;
    this->speed = this->record[record_num] - this->record[0];
    int max_speed = GUI_MAX_SPEED;
    int min_speed = GUI_MIN_SPEED;

    if (this->speed > max_speed)
    {
        this->speed = max_speed;
    }
    else if (this->speed < -max_speed)
    {
        this->speed = -max_speed;
    }

    if ((this->speed > 0) && (this->speed < min_speed))
    {
        this->speed = min_speed;
    }
    else if ((this->speed < 0) && (this->speed > -min_speed))
    {
        this->speed = -min_speed;
    }
}


static void gui_list_input_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_widget_t *this = (gui_list_widget_t *)obj;
    GUI_UNUSED(dc);

    if (!tp->pressing)
    {
        int temp = this->dir == HORIZONTAL ? obj->w : obj->h;
        int offset_min = temp - this->total_length;
        int offset_max = OUT_SCOPE;
        if (this->style == LIST_CARD)
        {
            offset_max = temp - this->tab_length;
            offset_min -= temp / 3;
        }

        // restore list offset
        if (this->speed == 0)
        {
            if (this->offset >= GUI_FRAME_STEP)
            {
                this->offset -= GUI_FRAME_STEP;
            }
            else if (this->offset > 0)
            {
                this->offset = 0;
            }
            else if (this->offset <= offset_min - GUI_FRAME_STEP)
            {
                this->offset += GUI_FRAME_STEP;
            }
            else if (this->offset < offset_min)
            {
                this->offset = offset_min;
            }
        }
        else
        {
            this->offset += this->speed;
            this->speed >= 0 ? this->speed-- : this->speed++;
            if (this->offset > offset_max)
            {
                this->offset = offset_max;
                this->speed = 0;
            }
            else if (this->offset < offset_min - OUT_SCOPE)
            {
                this->offset = offset_min - OUT_SCOPE;
                this->speed = 0;
            }
        }
        this->hold = this->offset;
    }
}

static void gui_list_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_list_widget_t *this = (gui_list_widget_t *)obj;
    GUI_UNUSED(dc);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);


    // animation update
    // animate_frame_update(this->animate, obj);

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, (uint8_t)sizeof(gui_list_widget_t));
    if (last != this->checksum)
    {
        gui_fb_change();
        // gui_log("fb_change, obj->y = %d, m[0][2] = %f\n", obj->y, obj->matrix->m[0][2]);
    }
}

static void gui_list_destroy(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_widget_t *this = (gui_list_widget_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_list_end(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_widget_t *this = (gui_list_widget_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
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

static void gui_list_tab_input_prepare(gui_obj_t *obj)
{
    gui_list_tab_t *this = (gui_list_tab_t *)obj;
    gui_list_widget_t *list = (gui_list_widget_t *)obj->parent;

    if (list->dir == HORIZONTAL)
    {
        obj->x = this->start_x + list->offset;
    }
    else
    {
        obj->y = this->start_y + list->offset;
    }
}

static void gui_list_tab_prepare(gui_obj_t *obj)
{
    gui_list_tab_t *this = (gui_list_tab_t *)obj;
    gui_list_widget_t *list = (gui_list_widget_t *)obj->parent;

    if (list->style == LIST_CIRCLE)
    {
        int32_t r = (list->dir == HORIZONTAL) ? list->base.h : list->base.w;
        int32_t diff = (list->dir == HORIZONTAL)
                       ? (list->base.w / 2 + list->base.x) - (obj->x + list->tab_length / 2)
                       : (list->base.h / 2 + list->base.y) - (obj->y + list->tab_length / 2);

        int32_t coord = diff >= r ? r : r - (int)sqrt(r * r - diff * diff);

        if (list->dir == HORIZONTAL)
        {
            obj->y = coord;
        }
        else
        {
            obj->x = coord;
        }
    }
    else if (list->style == LIST_ZOOM)
    {
        float scale_range = 0.5f;
        int32_t scope = (list->dir == HORIZONTAL) ? list->base.w : list->base.h;

        int32_t diff = (list->dir == HORIZONTAL)
                       ? (list->base.w / 2 + list->base.x) - (obj->x + list->tab_length / 2)
                       : (list->base.h / 2 + list->base.y) - (obj->y + list->tab_length / 2);
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

        if (list->dir == HORIZONTAL)
        {
            int16_t limit = (int16_t)(list->base.x + list->base.w - list->tab_length);
            if (obj->x > limit)
            {
                obj->x = limit;
            }
        }
        else
        {
            int16_t limit = (int16_t)(list->base.y + list->base.h - list->tab_length);
            if (obj->y > limit)
            {
                obj->y = limit;
            }
        }

        float scale_0 = 1.0f;
        float scale_1 = 0.9f;

        int obj_location = (list->dir == HORIZONTAL) ? obj->x : obj->y;
        int list_length = (list->dir == HORIZONTAL) ? list->base.w : list->base.h;
        int list_location = (list->dir == HORIZONTAL) ? list->base.x : list->base.y;
        int location_1 = list_location + list_length - list->tab_length;
        int location_0 = location_1 - list->tab_length / 3;

        if (obj_location >= location_0 && obj_location <= location_1)
        {
            float scale = scale_0 - (scale_0 - scale_1) * (obj_location - location_0) /
                          (location_1 - location_0);
            matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
            matrix_scale(scale, scale, obj->matrix);
            matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
        }
        matrix_translate(obj->x, obj->y, obj->matrix);
    }

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, (uint8_t)sizeof(gui_list_tab_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_list_tab_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_list_tab_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_list_tab_prepare(obj);
            }
            break;

        // case OBJ_END:
        //     {
        //         gui_list_end(obj);
        //     }
        //     break;

        // case OBJ_DESTROY:
        //     {
        //         gui_list_destroy(obj);
        //     }
        //     break;

        default:
            break;
        }
    }
}

static void gui_list_pressing_cb(void *object, gui_event_t e, void *param)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)object;
    gui_list_widget_t *this = (gui_list_widget_t *)object;

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if (this->dir == VERTICAL) { break; }
            gui_list_update_speed(this, tp->deltaX);

            this->offset = this->hold + tp->deltaX;
            int offset_min = obj->w - this->total_length;
            int offset_max = OUT_SCOPE;
            if (this->style == LIST_CARD)
            {
                offset_max = obj->w - this->tab_length;
                offset_min -= obj->w / 3;
            }
            // Limiting the range of movement
            if (this->offset > offset_max)
            {
                this->offset = offset_max;
            }
            else if (this->offset < offset_min - OUT_SCOPE)
            {
                this->offset = offset_min - OUT_SCOPE;
            }
            // gui_log("this->offset = %d\n", this->offset);
        }
        break;
    case TOUCH_HOLD_Y:
        {
            if (this->dir == HORIZONTAL) { break; }
            gui_list_update_speed(this, tp->deltaY);

            this->offset = this->hold + tp->deltaY;
            int offset_min = obj->h - this->total_length;
            int offset_max = OUT_SCOPE;
            if (this->style == LIST_CARD)
            {
                offset_max = obj->h - this->tab_length;
                offset_min -= obj->h / 3;
            }
            // Limiting the range of movement
            if (this->offset > offset_max)
            {
                this->offset = offset_max;
            }
            else if (this->offset < offset_min - OUT_SCOPE)
            {
                this->offset = offset_min - OUT_SCOPE;
            }
            // gui_log("this->offset = %d\n", this->offset);
        }
        break;

    default:
        break;
    }
}

static void gui_list_released_cb(void *obj, gui_event_t e, void *param)
{
    gui_list_widget_t *this = (gui_list_widget_t *)obj;
    this->hold = this->offset;

    memset(this->record, 0, 10);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_list_widget_t *gui_list_create(void       *parent,
                                   const char *name,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h,
                                   uint16_t    tab_length,
                                   uint8_t     space,
                                   LIST_DIR    dir)
{
    gui_list_widget_t *this = gui_malloc(sizeof(gui_list_widget_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_list_widget_t));
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }
    gui_obj_ctor(&this->base, parent, name, x, y, w, h);

    this->tab_length = tab_length;
    this->space = space;
    this->dir = dir;
    if (dir == HORIZONTAL)
    {
        this->total_length = w;
    }
    else
    {
        this->total_length = h;
    }
    GET_BASE(this)->obj_cb = gui_list_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->has_destroy_cb  = true;
    GET_BASE(this)->type = LIST;
    GET_BASE(this)->create_done = true;
    gui_list_init(&(GET_BASE(this)->child_list));

    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }

    gui_obj_add_event_cb(this, gui_list_pressing_cb, GUI_EVENT_TOUCH_PRESSING, NULL);
    gui_obj_add_event_cb(this, gui_list_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    return this;
}

gui_list_tab_t *gui_list_add_tab(gui_list_widget_t *list)
{
    gui_list_tab_t *this = gui_malloc(sizeof(gui_list_tab_t));
    memset(this, 0, sizeof(gui_list_tab_t));

    int16_t x, y, w, h;
    if (list->dir == HORIZONTAL)
    {
        x = list->base.x + list->widget_num * (list->tab_length + list->space);
        y = 0;
        w = list->tab_length;
        h = list->base.h;
        if (list->total_length < x + w)
        {
            list->total_length = x + w;
        }
    }
    else
    {
        x = 0;
        y = list->base.y + list->widget_num * (list->tab_length + list->space);
        w = list->base.w;
        h = list->tab_length;
        if (list->total_length < y + h)
        {
            list->total_length = y + h;
        }
    }
    this->start_x = x;
    this->start_y = y;
    list->widget_num++;

    gui_obj_ctor(&this->base, &list->base, "list_tab", x, y, w, h);
    GET_BASE(this)->obj_cb = gui_list_tab_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = false;
    GET_BASE(this)->has_destroy_cb  = false;
    GET_BASE(this)->type = LIST;
    GET_BASE(this)->create_done = true;
    gui_list_init(&(GET_BASE(this)->child_list));

    if ((GET_BASE(this)->parent) != NULL)
    {
        // be carefull here, not same as others
        gui_list_insert(&((GET_BASE(this)->parent)->child_list),
                        &(GET_BASE(this)->brother_list));
    }
    return this;
}

void gui_list_set_style(gui_list_widget_t *list, LIST_STYLE style)
{
    list->style = style;
}
