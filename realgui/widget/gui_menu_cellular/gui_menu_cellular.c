/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_menu_cellular.c
  * @brief honeygui menu effect.
  * @details Slide to extend and retract menu_cellulars
  * @author shel_deng@realsil.com.cn
  * @date 2025/7/2
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <string.h>
#include "gui_menu_cellular.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_img.h"
#include "gui_matrix.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define SCREEN_W        ((float)gui_get_screen_width())
#define SCREEN_H        ((float)gui_get_screen_height())
#define GUI_MAX_SPEED 50
#define GUI_MIN_SPEED 10

/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_menu_cellular_update_speed(int16_t *record, int16_t *speed, int16_t tp_delta)
{
    int record_num = 4;

    for (int i = 0; i < record_num; i++)
    {
        record[i] = record[i + 1];
    }

    record[record_num] = tp_delta;
    *speed = record[record_num] - record[0];
    int max_speed = GUI_MAX_SPEED;
    int min_speed = GUI_MIN_SPEED;

    if (*speed > max_speed)
    {
        *speed = max_speed;
    }
    else if (*speed < -max_speed)
    {
        *speed = -max_speed;
    }

    if ((*speed > 0) && (*speed < min_speed))
    {
        *speed = min_speed;
    }
    else if ((*speed < 0) && (*speed > -min_speed))
    {
        *speed = -min_speed;
    }
}


static void gui_menu_cellular_adjust_image(gui_obj_t *obj)
{
    gui_menu_cellular_t *this = GUI_TYPE(gui_menu_cellular_t, obj);

    this->hor_offset = this->hor_offset > this->icon_size / 2 ? this->icon_size / 2 : this->hor_offset;
    this->ver_offset = this->ver_offset > this->icon_size / 10 ? this->icon_size / 10 :
                       this->ver_offset;
    this->hor_offset = this->hor_offset < -this->icon_size / 2 ? -this->icon_size / 2 :
                       this->hor_offset;
    this->ver_offset = this->ver_offset < this->ver_offset_min ? this->ver_offset_min :
                       this->ver_offset;
    // gui_log("this->hor_offset: %d\n", this->hor_offset);
    float dis_max = (sqrtf(SCREEN_W * SCREEN_W + SCREEN_H * SCREEN_H) / 2.0f);
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_img_t *img = GUI_TYPE(gui_img_t, obj);

        float offset_X = (float)(obj->x + this->hor_offset) - SCREEN_W / 2.0f;
        float offset_Y = (float)(obj->y + this->ver_offset) - SCREEN_H / 2.0f;
        float dis = sqrtf(offset_X * offset_X + offset_Y * offset_Y);
        float ratio = dis / dis_max;
        float scale;
        float radius = (SCREEN_H + SCREEN_W) / 4.0f;

        if (dis >= dis_max)
        {
            scale = 0.01f;
        }
        else if (dis > radius)
        {
            // concave  function, f'(x) increase
            scale = 0.7 * pow((1.0f - ratio) / (1.0f - (radius / dis_max)), 0.7f);
        }
        else
        {
            // convex function, f'(x) decrease
            float a = (float)log(0.7) / (radius / dis_max);
            scale = exp(a * ratio);
        }

        if ((SCREEN_W / 2.0f) - fabsf(offset_X) < 20.0f)
        {
            scale /= 1.5f;
        }

        gui_img_scale(img, scale, scale);
        float t_x = (float)this->hor_offset - (1 - scale) * (offset_X / (SCREEN_W / 2.0f)) *
                    (this->icon_size / (1.5f * SCREEN_H / SCREEN_W));
        float t_y = (float)this->ver_offset - (1 - scale) * (offset_Y / (SCREEN_H / 2.0f)) *
                    (this->icon_size / 1.5f);
        gui_img_translate(img, t_x, t_y);
    }
}

static void _restore_cellular_timer_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_menu_cellular_t *this = (gui_menu_cellular_t *)obj;

    float factor = 0.4f;
    int16_t distance = - this->hor_offset;
    int delta = (int16_t)(distance * factor); //exponential decay
    this->hor_offset += delta;
    if (delta == 0)
    {
        this->hor_offset = 0;
        gui_obj_delete_timer(obj);
    }
    this->hor_hold = this->hor_offset;
    gui_menu_cellular_adjust_image(obj);
}

static void gui_menu_cellular_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_menu_cellular_t *this = GUI_TYPE(gui_menu_cellular_t, obj);

    if (tp->pressing)
    {
        this->hor_offset = this->hor_hold + tp->deltaX;
        this->ver_offset = this->ver_hold + tp->deltaY;
        gui_menu_cellular_update_speed(this->ver_record, &this->ver_speed, tp->deltaY);
        gui_menu_cellular_adjust_image(obj);
    }
    else if (tp->released)
    {
        memset(this->ver_record, 0, sizeof(this->ver_record));
        this->ver_hold = this->ver_offset;
        this->hor_hold = this->hor_offset;
    }
    else if (this->ver_speed != 0)
    {
        this->ver_hold += this->ver_speed;
        this->ver_offset = this->ver_hold;
        this->ver_speed = (int16_t)((1 - 0.05) * this->ver_speed);
        gui_menu_cellular_adjust_image(obj);
    }

    if (!tp->pressing && this->hor_offset != 0 && obj->timer == NULL)
    {
        gui_obj_create_timer(obj, 10, true, _restore_cellular_timer_cb);
    }
}

static void gui_menu_cellular_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_menu_cellular_prepare(obj);
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
gui_menu_cellular_t *gui_menu_cellular_create(void     *parent,
                                              int       icon_size,
                                              uint32_t *icon_array[],
                                              int       array_size)
{

    gui_menu_cellular_t *this = gui_malloc(sizeof(gui_menu_cellular_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_menu_cellular_t));

    gui_obj_ctor(&this->base, parent, "cellular", 0, 0, dc->screen_width, dc->screen_height);

    GET_BASE(this)->obj_cb = gui_menu_cellular_cb;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->type = MENU_CELLULAR;

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list)); \
    }
    GET_BASE(this)->create_done = true;

#define ICON_SIZE (icon_size)
#define WIDTH_GAP (ICON_SIZE + ICON_SIZE / 20)
#define HEIGHT_GAP (ICON_SIZE)
#define INIT_OFFSET_X (icon_size / 2)
#define INIT_OFFSET_Y (0)
#define FOCUS_OFFSET (icon_size / 2)
    uint8_t index = 0;
    uint8_t index_offset = 0;
    for (size_t i = 0; i < (unsigned int)array_size; i++)
    {
        if (index >= 6) {index_offset += 7;}
        index = i - index_offset;
        gui_img_t *img = NULL;
        int16_t start_y = 0;
        if (index < 3)
        {
            start_y = INIT_OFFSET_Y + HEIGHT_GAP * (index_offset / 7 * 2) + FOCUS_OFFSET;
            img = gui_img_create_from_mem(this, 0, icon_array[i],
                                          WIDTH_GAP * index + INIT_OFFSET_X * 1 + FOCUS_OFFSET, start_y, 0, 0);
        }
        else
        {
            start_y = INIT_OFFSET_Y + HEIGHT_GAP * (index_offset / 7 * 2 + 1) + FOCUS_OFFSET;
            img = gui_img_create_from_mem(this, 0, icon_array[i],
                                          WIDTH_GAP * (index - 3) + INIT_OFFSET_X * 0 + FOCUS_OFFSET, start_y, 0, 0);
        }
        gui_img_set_focus(img, FOCUS_OFFSET, FOCUS_OFFSET);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img, true);
        this->ver_offset_min = -(start_y + HEIGHT_GAP - SCREEN_H);
    }
    this->icon_size = icon_size;

    gui_menu_cellular_adjust_image(&this->base);
    return this;
}

void gui_menu_cellular_offset(gui_menu_cellular_t *menu_cellular, int offset_x, int offset_y)
{
    menu_cellular->hor_offset = offset_x;
    menu_cellular->ver_offset = offset_y;
    menu_cellular->hor_hold = offset_x;
    menu_cellular->ver_hold = offset_y;
    gui_menu_cellular_adjust_image(&menu_cellular->base);
}

void gui_menu_cellular_on_click(gui_menu_cellular_t *menu_cellular,
                                struct gui_menu_cellular_gesture_parameter *para_array, int array_length)
{
    (void)array_length;
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_obj_t *obj = GUI_BASE(menu_cellular);
    int index = 0;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (para_array && para_array[index].callback_function)
        {
            gui_obj_add_event_cb(o, para_array[index].callback_function, GUI_EVENT_TOUCH_CLICKED,
                                 para_array[index].parameter);
        }
        index++;
    }
}

