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
#include "gui_list.h"
#include "tp_algo.h"
#include "gui_img.h"
#include "gui_canvas.h"



/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

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

    float cur_angle;
    uint8_t cur_tier = 0;
    if (tp->pressed || tp->pressing || tp->released)
    {
        float x_length_start = (tp->x - dc->fb_width / 2);
        float y_length_start = (tp->y - dc->fb_height / 2);
        float tp_angle_start = atan2(y_length_start, x_length_start);

#if 1
        float distance_cur = x_length_start / cos(tp_angle_start);
        while ((int)distance_cur > this->distence * cur_tier + this->distence / 2)
        {
            cur_tier++;
        }
#else
        int distance_squ_cur = x_length_start * x_length_start + y_length_start * y_length_start;
        int distance_squ = this->distence * this->distence;
        while (distance_squ_cur > distance_squ * cur_tier * (cur_tier + 1) + distance_squ / 4)
        {
            cur_tier++;
        }
#endif
        float x_length = (tp->x + tp->deltaX - dc->fb_width / 2);
        float y_length = (tp->y + tp->deltaY - dc->fb_height / 2);
        float tp_angle_cur = atan2(y_length, x_length);

        cur_angle = this->rotation[cur_tier].angle + tp_angle_cur - tp_angle_start;
        // gui_log("tp_angle_start_angle %f,  tp_angle_cur_angle %f\n", (tp_angle_start)/M_PI * 180, (tp_angle_cur)/M_PI * 180);
        if (cur_tier == 0)
        {
            // if (this->icon_list[0][0] != NULL)
            // {
            //     gui_img_translate(this->icon_list[0][0], this->radius, this->radius);
            //     gui_img_rotation(this->icon_list[0][0], (tp_angle_cur)/M_PI * 180, this->radius, this->radius);
            // }
        }
        else if (cur_tier <= this->tier)
        {
            if (this->icon_list[cur_tier] != NULL)
            {
                for (int j = 0; j < 6 * cur_tier; j++)
                {
                    if (this->icon_list[cur_tier][j] == NULL)
                    {
                        continue;
                    }
                    int x = dc->screen_width / 2 + this->distence * cur_tier * cos(RADIAN_60 * j / cur_tier + cur_angle)
                            - this->radius;
                    int y = dc->screen_height / 2 + this->distence * cur_tier * sin(RADIAN_60 * j / cur_tier +
                                                                                    cur_angle) - this->radius;
                    gui_img_set_location(this->icon_list[cur_tier][j], x, y);
                }
            }
        }
        if (tp->released)
        {
            this->rotation[cur_tier].angle = cur_angle;
        }
    }
    else
    {
        adjust_radian(&this->rotation[0].angle);
        int base_x = dc->screen_width / 2 - this->radius;
        int base_y = dc->screen_height / 2 - this->radius;
        for (int i = 1; i <= this->tier; i++)
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
    for (int i = 0; i <= this->tier; i++)
    {
        gui_free(this->icon_list[i]);
    }
    gui_free(this->icon_list);
}

static void gui_wheel_list_ctor(gui_wheel_list_t *this,
                                gui_obj_t *parent,
                                uint8_t tier,
                                uint8_t radius,
                                uint8_t distence)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(&this->base, parent, "wheel_list", 0, 0, 0, 0);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = WHEEL_LIST;
    root->obj_input_prepare = wheel_list_input_prepare;
    root->obj_prepare = wheel_list_prepare;
    root->obj_destory = wheel_list_destory;

    this->tier = tier;
    this->distence = distence;
    this->radius = radius;
    this->rotation = gui_malloc(sizeof(WHEEL_ANGLE_T) * (tier + 1));
    this->icon_list = gui_malloc(sizeof(gui_img_t **) * tier);
    memset(this->icon_list, 0, sizeof(gui_img_t **) * tier);
    this->icon_list[0] = gui_malloc(sizeof(gui_img_t *));
    memset(this->icon_list[0], 0, sizeof(gui_img_t *));
    for (int i = 1; i <= tier ; i++)
    {
        this->icon_list[i] = gui_malloc(sizeof(gui_img_t *) * 6 * i);
        memset(this->icon_list[i], 0, sizeof(gui_img_t *) * 6 * i);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_wheel_list_t *gui_wheel_list_create(void *parent,
                                        uint8_t tier,
                                        uint8_t radius,
                                        uint8_t distence)
{
    GUI_ASSERT(parent != NULL);
    gui_wheel_list_t *wheel_list = gui_malloc(sizeof(gui_wheel_list_t));
    GUI_ASSERT(wheel_list != NULL);
    memset(wheel_list, 0x00, sizeof(gui_wheel_list_t));

    gui_wheel_list_ctor(wheel_list, (gui_obj_t *)parent, tier, radius, distence);
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
    for (int i = 1; i <= this->tier; i++)
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
                             uint8_t tier,
                             uint8_t index)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (this->icon_list[tier] != NULL)
    {
        if (this->icon_list[tier][index] != NULL)
        {
            gui_tree_free(this->icon_list[tier][index]);
        }
        int x = dc->screen_width / 2 + this->distence * tier * cos(RADIAN_60 * index / tier) - this->radius;
        int y = dc->screen_height / 2 + this->distence * tier * sin(RADIAN_60 * index / tier) -
                this->radius;
        this->icon_list[tier][index] = gui_img_create_from_mem(this, "wl_img_c", icon_addr, x, y, 0, 0);
        if (event_cb != NULL)
        {
            gui_obj_add_event_cb(this->icon_list[tier][index], event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





