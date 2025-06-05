/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtain.c
  * @brief create a curtain effect widget,which should be nested in a curtainview.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
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
#define GUI_MAX_SPEED 30
#define GUI_MIN_SPEED 7
#define IMG_NUM_MAX 30

/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_menu_cellular_update_speed(int16_t *record, int16_t *speed, int16_t tp_delta)
{
    int record_num = 4;

    for (size_t i = 0; i < record_num; i++)
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

    this->hor_offset = this->hor_offset > SCREEN_W / 5 ? SCREEN_W / 5 : this->hor_offset;
    this->ver_offset = this->ver_offset > SCREEN_H / 5 ? SCREEN_H / 5 : this->ver_offset;
    this->hor_offset = this->hor_offset < this->hor_offset_min ? this->hor_offset_min :
                       this->hor_offset;
    this->ver_offset = this->ver_offset < this->ver_offset_min ? this->ver_offset_min :
                       this->ver_offset;

    float dis_max = (sqrtf(SCREEN_W * SCREEN_W + SCREEN_H * SCREEN_H) / 2.0f);
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_img_t *img = GUI_TYPE(gui_img_t, obj);

        float offset_X = SCREEN_W / 2.0f - (float)(obj->x + this->hor_offset);
        float offset_Y = SCREEN_H / 2.0f - (float)(obj->y + this->ver_offset);
        float dis = sqrtf(offset_X * offset_X + offset_Y * offset_Y);
        float ratio = dis / dis_max;
        float scale;
        float w = (float)obj->w;

        if (dis > w * 2)
        {
            scale = 0.8f + 8 * w * 2 / dis_max - ratio * 8;
        }
        else
        {
            scale = 1.f - 0.2f * (w * 2 / dis_max) * ratio;
        }

        if (scale <= 0)
        {
            scale = 0.01f;
        }

        gui_img_scale(img, scale, scale);
        gui_img_translate(img, (float)this->hor_offset, (float)this->ver_offset);
        // gui_log("%s dis = %f, dis_max = %f, scale = %f\n",
        //         obj->name, dis, dis_max, img->scale_x);
    }
}

static void gui_menu_cellular_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_menu_cellular_t *this = GUI_TYPE(gui_menu_cellular_t, obj);

    if (tp->pressing)
    {
        this->ver_offset = this->ver_hold + tp->deltaY;
        this->hor_offset = this->hor_hold + tp->deltaX;
        gui_menu_cellular_adjust_image(obj);
        gui_menu_cellular_update_speed(this->ver_record, &this->ver_speed, tp->deltaY);
        gui_menu_cellular_update_speed(this->hor_record, &this->hor_speed, tp->deltaX);
        // gui_log("ver_s = %d, hor_s = %d\n", this->ver_speed, this->hor_speed);
    }
    else if (tp->released)
    {
        memset(this->ver_record, 0, sizeof(this->ver_record));
        memset(this->hor_record, 0, sizeof(this->hor_record));
        this->ver_hold = this->ver_offset;
        this->hor_hold = this->hor_offset;
    }
    else if (this->ver_speed != 0 || this->hor_speed != 0)
    {
        this->ver_hold += this->ver_speed;
        this->hor_hold += this->hor_speed;
        this->ver_offset = this->ver_hold;
        this->hor_offset = this->hor_hold;
        this->ver_speed = (int16_t)((1 - 0.05) * this->ver_speed);
        this->hor_speed = (int16_t)((1 - 0.05) * this->hor_speed);
        gui_menu_cellular_adjust_image(obj);
        // gui_log("!!!ver_s = %d, hor_s = %d\n", this->ver_speed, this->hor_speed);
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
#define WIDTH_GAP (ICON_SIZE + 5)
#define HEIGHT_GAP (ICON_SIZE)
#define INIT_OFFSET_X (icon_size / 2)
#define INIT_OFFSET_Y (icon_size / 2)
    if (array_size > IMG_NUM_MAX)
    {
        array_size = IMG_NUM_MAX;
    }
    for (size_t i = 0; i < array_size; i++)
    {
        gui_img_t *img = NULL;
        if (i < 3)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i + 1) + (WIDTH_GAP / 2) * 1, INIT_OFFSET_Y,
                                          0, 0);
        }
        else if (i < 7)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 3 + 1) + (WIDTH_GAP / 2) * 0,
                                          INIT_OFFSET_Y + HEIGHT_GAP, 0, 0);
        }
        else if (i < 12)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 7) + (WIDTH_GAP / 2) * 1,
                                          INIT_OFFSET_Y + HEIGHT_GAP * 2, 0, 0);
        }
        else if (i < 18)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 12) + (WIDTH_GAP / 2) * 0,
                                          INIT_OFFSET_Y + HEIGHT_GAP * 3, 0, 0);
        }
        else if (i < 23)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 18) + (WIDTH_GAP / 2) * 1,
                                          INIT_OFFSET_Y + HEIGHT_GAP * 4, 0, 0);
        }
        else if (i < 27)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 23 + 1) + (WIDTH_GAP / 2) * 0,
                                          INIT_OFFSET_Y + HEIGHT_GAP * 5, 0, 0);
        }
        else if (i < 30)
        {
            img = gui_img_create_from_mem(this, "img", icon_array[i],
                                          INIT_OFFSET_X + WIDTH_GAP * (i - 27 + 1) + (WIDTH_GAP / 2) * 1,
                                          INIT_OFFSET_Y + HEIGHT_GAP * 6, 0, 0);
        }
        gui_img_set_focus(img, ICON_SIZE / 2, ICON_SIZE / 2);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        this->hor_offset_min = -(INIT_OFFSET_X + WIDTH_GAP * 6 - SCREEN_W);
        this->ver_offset_min = -(INIT_OFFSET_Y + HEIGHT_GAP * 7 - SCREEN_H);
    }

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
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_obj_t *obj = GUI_BASE(menu_cellular);
    int index = 0;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (para_array && para_array[index].callback_function)
        {
            gui_obj_add_event_cb(obj, para_array[index].callback_function, GUI_EVENT_TOUCH_CLICKED,
                                 para_array[index].parameter);
        }
        index++;
    }
}

