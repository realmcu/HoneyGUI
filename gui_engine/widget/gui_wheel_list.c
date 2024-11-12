/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wheel_list.c
  * @brief  wheel_list design
  * @details Evenly layout vertically and horizontally
  * @author luke_sun@realsil.com.cn
  * @date 2024/4/1
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gui_wheel_list.h>
#include "def_list.h"
#include "tp_algo.h"
#include "gui_img.h"

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

static void wheel_list_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    GUI_UNUSED(tp);
    gui_obj_skip_other_right_hold(obj);
    gui_obj_skip_other_left_hold(obj);
    gui_obj_skip_other_down_hold(obj);
    gui_obj_skip_other_up_hold(obj);
}

static void adjust_radian(float *radian)
{
    while (*radian >= 2 * M_PI)
    {
        *radian -= 2 * M_PI;
    }
    while (*radian <= 0)
    {
        *radian += 2 * M_PI;
    }
}
static void wheel_list_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    gui_wheel_list_t *this = (void *)obj;
    int half_screen_w = (int)(gui_get_screen_width)() / 2;
    int half_screen_h = (int)(gui_get_screen_height)() / 2;
    float cur_angle;
    uint8_t cur_layer = 0;
    if (tp->pressed || tp->pressing || tp->released)
    {
        float x_length_start = (tp->x - half_screen_w);
        float y_length_start = (tp->y - half_screen_h);
        float tp_angle_start = atan2(y_length_start, x_length_start);

#if 1
        float distance_cur = x_length_start / cos(tp_angle_start);
        while ((int)distance_cur > this->distence * cur_layer + this->distence / 2)
        {
            cur_layer++;
        }
#else
        int distance_squ_cur = x_length_start * x_length_start + y_length_start * y_length_start;
        int distance_squ = this->distence * this->distence;
        while (distance_squ_cur > distance_squ * cur_layer * (cur_layer + 1) + distance_squ / 4)
        {
            cur_layer++;
        }
#endif
        float x_length = (tp->x + tp->deltaX - half_screen_w);
        float y_length = (tp->y + tp->deltaY - half_screen_h);
        float tp_angle_cur = atan2(y_length, x_length);

        cur_angle = this->rotation[cur_layer].angle + tp_angle_cur - tp_angle_start;
        // gui_log("tp_angle_start_angle %f,  tp_angle_cur_angle %f\n", (tp_angle_start)/M_PI * 180, (tp_angle_cur)/M_PI * 180);
        if (cur_layer == 0)
        {
            // if (this->icon_list[0][0] != NULL)
            // {
            //     gui_img_translate(this->icon_list[0][0], this->radius, this->radius);
            //     gui_img_rotation(this->icon_list[0][0], (tp_angle_cur)/M_PI * 180, this->radius, this->radius);
            // }
        }
        else if (cur_layer <= this->layer)
        {
            if (this->icon_list[cur_layer] != NULL)
            {
                for (int j = 0; j < 6 * cur_layer; j++)
                {
                    if (this->icon_list[cur_layer][j] == NULL)
                    {
                        continue;
                    }
                    int x = dc->screen_width / 2 + this->distence * cur_layer * cos(RADIAN_60 * j / cur_layer +
                                                                                    cur_angle)  - this->radius;
                    int y = dc->screen_height / 2 + this->distence * cur_layer * sin(RADIAN_60 * j / cur_layer +
                                                                                     cur_angle) - this->radius;
                    gui_img_set_location(this->icon_list[cur_layer][j], x, y);
                }
            }
        }
        if (tp->released)
        {
            this->rotation[cur_layer].angle = cur_angle;
        }
    }
    else
    {
        adjust_radian(&this->rotation[0].angle);
        int base_x = dc->screen_width / 2 - this->radius;
        int base_y = dc->screen_height / 2 - this->radius;
        for (int i = 1; i <= this->layer; i++)
        {
            adjust_radian(&this->rotation[i].angle);
            cur_angle = this->rotation[i].angle;
            if (this->icon_list[i] != NULL)
            {
                for (int j = 0; j < 6 * i; j++)
                {
                    if (this->icon_list[i][j] == NULL)
                    {
                        continue;
                    }
                    int x = base_x + this->distence * i * cos(RADIAN_60 * j / i + cur_angle);
                    int y = base_y + this->distence * i * sin(RADIAN_60 * j / i + cur_angle);
                    gui_img_set_location(this->icon_list[i][j], x, y);
                }
            }
        }
    }
}
static void wheel_list_destory(gui_obj_t *obj)
{
    gui_wheel_list_t *this = (void *)obj;
    gui_free(this->rotation);
    for (int i = 0; i <= this->layer; i++)
    {
        gui_free(this->icon_list[i]);
    }
    gui_free(this->icon_list);
}

static void gui_wheel_list_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            wheel_list_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            wheel_list_prepare(obj);
            break;

        case OBJ_DESTORY:
            wheel_list_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_wheel_list_ctor(gui_wheel_list_t *this,
                                gui_obj_t *parent,
                                uint8_t layer,
                                uint8_t radius,
                                uint8_t distence)
{
    //for base class
    gui_obj_ctor(&this->base, parent, "wheel_list", 0, 0, 0, 0);

    //for root class
    GET_BASE(this)->type = WHEEL_LIST;
    GET_BASE(this)->obj_cb = gui_wheel_list_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;

    this->layer = layer;
    this->distence = distence;
    this->radius = radius;
    this->rotation = gui_malloc(sizeof(WHEEL_ANGLE_T) * (layer + 1));
    this->icon_list = gui_malloc(sizeof(gui_img_t **) * layer);
    memset(this->icon_list, 0, sizeof(gui_img_t **) * layer);
    this->icon_list[0] = gui_malloc(sizeof(gui_img_t *));
    memset(this->icon_list[0], 0, sizeof(gui_img_t *));
    for (int i = 1; i <= layer ; i++)
    {
        this->icon_list[i] = gui_malloc(sizeof(gui_img_t *) * 6 * i);
        memset(this->icon_list[i], 0, sizeof(gui_img_t *) * 6 * i);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_wheel_list_t *gui_wheel_list_create(void *parent,
                                        uint8_t layer,
                                        uint8_t radius,
                                        uint8_t distence)
{
    GUI_ASSERT(parent != NULL);
    gui_wheel_list_t *wheel_list = gui_malloc(sizeof(gui_wheel_list_t));
    GUI_ASSERT(wheel_list != NULL);
    memset(wheel_list, 0x00, sizeof(gui_wheel_list_t));

    gui_wheel_list_ctor(wheel_list, (gui_obj_t *)parent, layer, radius, distence);
    gui_list_init(&(GET_BASE(wheel_list)->child_list));
    if ((GET_BASE(wheel_list)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(wheel_list)->parent)->child_list),
                               &(GET_BASE(wheel_list)->brother_list));
    }
    GET_BASE(wheel_list)->create_done = true;
    return wheel_list;
}

void gui_wheel_list_add_icon_default(gui_wheel_list_t *this,
                                     void *icon_addr,
                                     gui_event_cb_t event_cb)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (this->icon_list[0] != NULL)
    {
        if (this->icon_list[0][0] == NULL)
        {
            int x = dc->screen_width / 2 - this->radius;
            int y = dc->screen_height / 2 - this->radius;
            this->icon_list[0][0] = gui_img_create_from_mem(this, "wl_img_c", icon_addr, x, y, 0, 0);
            if (event_cb != NULL)
            {
                gui_obj_add_event_cb(this->icon_list[0][0], event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
            }
            return;
        }
    }
    for (int i = 1; i <= this->layer; i++)
    {
        if (this->icon_list[i] != NULL)
        {
            for (int j = 0; j < 6 * i; j++)
            {
                if (this->icon_list[i][j] != NULL)
                {
                    continue;
                }
                int x = dc->screen_width / 2 + this->distence * i * cos(RADIAN_60 * j / i) - this->radius;
                int y = dc->screen_height / 2 + this->distence * i * sin(RADIAN_60 * j / i) - this->radius;
                this->icon_list[i][j] = gui_img_create_from_mem(this, "wl_img", icon_addr, x, y, 0, 0);
                if (event_cb != NULL)
                {
                    gui_obj_add_event_cb(this->icon_list[i][j], event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
                }
                return;
            }
        }
    }
}
void gui_wheel_list_set_icon(gui_wheel_list_t *this,
                             void *icon_addr,
                             gui_event_cb_t event_cb,
                             uint8_t layer,
                             uint8_t index)
{
    gui_dispdev_t *dc = gui_get_dc();
    int x = 0;
    int y = 0;
    if (this->icon_list[layer] != NULL)
    {
        if (this->icon_list[layer][index] != NULL)
        {
            gui_obj_tree_free(this->icon_list[layer][index]);
        }
        if (layer == 0 && index == 0)
        {
            x = dc->screen_width / 2 - this->radius;
            y = dc->screen_height / 2 - this->radius;
        }
        else if (layer == 0)
        {
            /* code */
            gui_log("Warning: Layer is zero, using default value 1.0f instead.\n");
            layer = 1;
        }
        else
        {
            x = dc->screen_width / 2 + this->distence * layer * cos(RADIAN_60 * index / layer) - this->radius;
            y = dc->screen_height / 2 + this->distence * layer * sin(RADIAN_60 * index / layer) -
                this->radius;
        }
        this->icon_list[layer][index] = gui_img_create_from_mem(this, "wl_img_c", icon_addr, x, y, 0, 0);
        if (event_cb != NULL)
        {
            gui_obj_add_event_cb(this->icon_list[layer][index], event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
}





