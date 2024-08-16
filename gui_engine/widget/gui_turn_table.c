/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_turn_table.c
  * @brief  turn_table design
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2024/5/10
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gui_turn_table.h>
#include "def_list.h"
#include "tp_algo.h"
#include "gui_img.h"



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
#define TURN_TABLE_ROTATE_STEP 6

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

static void turn_table_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_turn_table_t *this = (void *)obj;
    GUI_UNUSED(tp);
    if (this->skip_up_tp)
    {
        gui_obj_skip_other_up_hold(obj);
    }
    if (this->skip_down_tp)
    {
        gui_obj_skip_other_down_hold(obj);
    }
    if (this->skip_left_tp)
    {
        gui_obj_skip_other_left_hold(obj);
    }
    if (this->skip_right_tp)
    {
        gui_obj_skip_other_right_hold(obj);
    }
}
static void gui_turn_table_update_resetting_angle(gui_turn_table_t *this)
{
    float temp = M_PI / this->active_icon_quantity / TURN_TABLE_ROTATE_STEP;
    // gui_log("enter last_angle[0] %f, [1] %f , [2] %f \n",
    //          this->last_angle[0],this->last_angle[1],this->last_angle[2]);
    if (this->last_angle[0] > 0)
    {
        if (this->last_angle[2] > this->last_angle[1] &&
            this->last_angle[1] > this->last_angle[0])
        {
            temp = (this->last_angle[2] - this->last_angle[0]) / 2;
        }
        this->resetting_angle -= temp;
        if (this->resetting_angle <= 0)
        {
            this->resetting_angle = 0;
        }
        this->last_angle[2] -= temp;
        this->last_angle[1] -= temp;
        this->last_angle[0] = this->resetting_angle;
    }
    else if (this->last_angle[0] < 0)
    {
        if (this->last_angle[2] < this->last_angle[1] &&
            this->last_angle[1] < this->last_angle[0])
        {
            temp = (this->last_angle[0] - this->last_angle[2]) / 2;
        }
        this->resetting_angle += temp;

        if (this->resetting_angle >= 0)
        {
            this->resetting_angle = 0;
        }
        this->last_angle[2] += temp;
        this->last_angle[1] += temp;
        this->last_angle[0] = this->resetting_angle;
    }
    if (this->last_angle[0] == 0)
    {
        if (this->last_angle[2] == 0 &&
            this->last_angle[1] == 0)
        {
            return;
        }
        else if (fabs(this->last_angle[2]) < temp ||
                 fabs(this->last_angle[1]) < temp)
        {
            this->last_angle[1] = 0;
            this->last_angle[2] = 0;
        }
    }
    // gui_log("exit last_angle[0] %f, [1] %f , [2] %f \n",
    //         this->last_angle[0],this->last_angle[1],this->last_angle[2]);
}
static void gui_turn_table_refresh_angle_list_release(gui_turn_table_t *this, float angle)
{
    float temp = this->last_angle[0] - angle;
    this->last_angle[0] = angle;
    this->last_angle[1] -= temp;
    this->last_angle[2] -= temp;
}

static void gui_turn_table_refresh_angle_list(gui_turn_table_t *this, float angle)
{
    this->last_angle[2] = this->last_angle[1];
    this->last_angle[1] = this->last_angle[0];
    this->last_angle[0] = angle;
    // gui_log("last_angle[2] %f , last_angle[1] %f , last_angle[0] %f \n",
    //          this->last_angle[2],this->last_angle[1],this->last_angle[0]);

    if (this->last_angle[2] < this->last_angle[1] &&
        this->last_angle[1] < this->last_angle[0])
    {
        this->state = TT_CLOCKWISE;
        return;
    }
    if (this->last_angle[2] > this->last_angle[1] &&
        this->last_angle[1] > this->last_angle[0])
    {
        this->state = TT_ANTICLOCKWISE;
        return;
    }
    if (this->last_angle[2] == this->last_angle[1] &&
        this->last_angle[1] == this->last_angle[0])
    {
        this->state = TT_RESTING;
        return;
    }
    if (this->last_angle[2] <= 0 &&
        this->last_angle[1] < this->last_angle[2] &&
        this->last_angle[0] > 0)
    {
        this->state = TT_ANTICLOCKWISE;
        this->turn_count--;
        return;
    }

    if (this->last_angle[2] >= 0 &&
        this->last_angle[1] > this->last_angle[2] &&
        this->last_angle[0] < 0)
    {
        this->state = TT_CLOCKWISE;
        this->turn_count++;
        return;
    }
}
static gui_turn_table_coordinate_t gui_turn_table_get_image_coordinate(gui_turn_table_t *this,
                                                                       int index, float ratio)
{
    gui_turn_table_coordinate_t input1, input2, output;
    memset(&input1, 0, sizeof(input1));
    memset(&input2, 0, sizeof(input2));
    memset(&output, 0, sizeof(output));

    input1 = this->static_list[index];
    switch (this->state)
    {
    case TT_RESTING:
        input2 = input1;
    case TT_ANTICLOCKWISE:
    case TT_CLOCKWISE:
        if (ratio <= 0)
        {
            if (index + 1 < this->active_icon_quantity)
            {
                input2 = this->static_list[index + 1];
            }
            else if (index + 1 >= this->active_icon_quantity)
            {
                input2 = this->static_list[0];
            }
        }
        else
        {
            if (index - 1 < 0)
            {
                input2 = this->static_list[this->active_icon_quantity - 1];
            }
            else
            {
                input2 = this->static_list[index - 1];
            }
        }
        break;
    default:
        break;
    }
    ratio = fabs(ratio);
    output.x = input1.x * (1.0f - ratio) + input2.x * ratio;
    output.y = input1.y * (1.0f - ratio) + input2.y * ratio;

    return output;
}
static void gui_turn_table_icon_img_set(gui_img_t *this, int x, int y, bool active)
{
    if (this == NULL)
    {
        return;
    }
    if (active)
    {
        gui_img_set_location(this, x, y);
        this->base.gesture = 0;
        gui_obj_show(this, true);
    }
    else
    {
        gui_img_set_location(this, x, y);
        this->base.gesture = 1;
        gui_obj_show(this, false);
    }
}
static void gui_turn_table_update_position(gui_turn_table_t *this, float angle)
{
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    if (this->state == TT_RESTING && tp->pressing == true)
    {
        return;
    }
    int index = this->sector_index;
    float rotation_compensation = (angle >= 0 ? 0.5f : -0.5f);
    rotation_compensation = 0;
    // switch (this->state)
    // {
    // case TT_RESTING:
    //     rotation_compensation = (angle >= 0 ? 0.5f : -0.5f);
    //     break;
    // case TT_ANTICLOCKWISE:
    //     rotation_compensation = -0.5f;
    //     break;
    // case TT_CLOCKWISE:
    //     rotation_compensation = 0.5f;
    //     break;
    // default:
    //     break;
    // }
    index -= (int)(angle / this->sector_angle + rotation_compensation);

    int highlight_icon_x, highlight_icon_y;
    gui_turn_table_coordinate_t out;
    int base_static_x = this->static_list[0].x;
    int base_static_y = this->static_list[0].y;
    float offset_ratio = ((int)(angle / this->sector_angle) * this->sector_angle - angle) /
                         this->sector_angle;

    while (index + this->active_icon_quantity <= 0)
    {
        index += this->icon_quantity;
    }
    while (index > this->icon_quantity - 1)
    {
        index -= this->icon_quantity;
    }

    int min = index;
    int max = index + this->active_icon_quantity;
    if ((min >= 0) && (max <= this->icon_quantity - 1))
    {
        for (int i = 0; i < min; i++)
        {
            gui_turn_table_icon_img_set(this->icon_list[i], base_static_x, base_static_y, false);
        }
        for (int i = min; i <= max; i++)
        {
            out = gui_turn_table_get_image_coordinate(this, i - min, offset_ratio);
            gui_turn_table_icon_img_set(this->icon_list[i], out.x, out.y, true);
        }
        for (int i = max + 1; i < this->icon_quantity; i++)
        {
            gui_turn_table_icon_img_set(this->icon_list[i], base_static_x, base_static_y, false);
        }
        if (offset_ratio > 0)
        {
            gui_turn_table_icon_img_set(this->icon_list[min], base_static_x, base_static_y, false);
        }
    }
    else if ((min < 0) && (max >= 0))
    {
        for (int i = 0; i <= max; i++)
        {
            out = gui_turn_table_get_image_coordinate(this, i - min, offset_ratio);
            gui_turn_table_icon_img_set(this->icon_list[i], out.x, out.y, true);
        }
        for (int i = max + 1; i < this->icon_quantity; i++)
        {
            gui_turn_table_icon_img_set(this->icon_list[i], base_static_x, base_static_y, false);
        }
        for (int i = min; i < 0; i++)
        {
            out = gui_turn_table_get_image_coordinate(this, i - min, offset_ratio);
            gui_turn_table_icon_img_set(this->icon_list[this->icon_quantity + i], out.x, out.y, true);
        }
        if (offset_ratio > 0)
        {
            gui_turn_table_icon_img_set(this->icon_list[this->icon_quantity + min], base_static_x,
                                        base_static_y, false);
        }
    }
    else if ((min <= this->icon_quantity - 1) && (max > this->icon_quantity - 1))
    {
        for (int i = 0; i < min; i++)
        {
            gui_turn_table_icon_img_set(this->icon_list[i], base_static_x, base_static_y, false);
        }
        for (int i = min; i < this->icon_quantity; i++)
        {
            out = gui_turn_table_get_image_coordinate(this, i - min, offset_ratio);
            gui_turn_table_icon_img_set(this->icon_list[i], out.x, out.y, true);
        }
        for (int i = this->icon_quantity; i <= max; i++)
        {
            out = gui_turn_table_get_image_coordinate(this, i - min, offset_ratio);
            gui_turn_table_icon_img_set(this->icon_list[i - this->icon_quantity], out.x, out.y, true);
        }
        if (offset_ratio > 0)
        {
            gui_turn_table_icon_img_set(this->icon_list[min], base_static_x, base_static_y, false);
        }
    }

    //highlight icon set
    int highlight_index = min + this->highlight_index;
    if (highlight_index < 0)
    {
        highlight_index += this->icon_quantity;
    }
    else if (highlight_index >= this->icon_quantity)
    {
        highlight_index -= this->icon_quantity;
    }
    highlight_icon_x = gui_get_screen_width() / 2 - this->icon_radius * this->icon_highlight_scale;
    highlight_icon_y = gui_get_screen_height() / 2 - this->icon_radius * this->icon_highlight_scale;
    gui_img_set_attribute(this->highlight_icon, NULL, this->icon_list[highlight_index]->data,
                          highlight_icon_x, highlight_icon_y);
    if (this->highlight_icon->base.event_dsc == NULL)
    {
        if (this->icon_list[highlight_index]->base.event_dsc != NULL)
        {
            gui_obj_add_event_cb(this->highlight_icon,
                                 this->icon_list[highlight_index]->base.event_dsc->event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else
    {
        if (this->icon_list[highlight_index]->base.event_dsc != NULL)
        {
            this->highlight_icon->base.event_dsc->event_cb =
                this->icon_list[highlight_index]->base.event_dsc->event_cb;
        }
    }
}

static void turn_table_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    gui_turn_table_t *this = (void *)obj;
    uint8_t last;
    int half_screen_w = (int)(gui_get_screen_width)() / 2;
    int half_screen_h = (int)(gui_get_screen_height)() / 2;
    float rotationg_angle = 0;
    if (tp->pressed || tp->pressing || tp->released)
    {
        if (tp->pressed)
        {
            this->turn_count = 0;
            this->need_update = true;
            memset(this->last_angle, 0, sizeof(float) * 3);
        }

        float x_length_start = (tp->x - half_screen_w);
        float y_length_start = (tp->y - half_screen_h);
        float tp_angle_start = atan2(y_length_start, x_length_start);

        float x_length_cur = (tp->x + tp->deltaX - half_screen_w);
        float y_length_cur = (tp->y + tp->deltaY - half_screen_h);
        float tp_angle_cur = atan2(y_length_cur, x_length_cur);

        rotationg_angle = tp_angle_cur - tp_angle_start;

        gui_turn_table_refresh_angle_list(this, rotationg_angle);
        rotationg_angle += this->turn_count * M_PI * 2;
        gui_turn_table_update_position(this, rotationg_angle);

        if (tp->released)
        {
            this->sector_index -= (int)(rotationg_angle / this->sector_angle);
            this->resetting_angle = ((int)(rotationg_angle / this->sector_angle) * this->sector_angle -
                                     rotationg_angle);
            this->turn_count = 0;
            // gui_log("this->resetting_angle %f ,rotationg_angle %f ,this->sector_angle %f \n",
            //          this->resetting_angle,    rotationg_angle,    this->sector_angle);
            gui_turn_table_refresh_angle_list_release(this, this->resetting_angle);
        }
    }
    else
    {
        gui_turn_table_update_resetting_angle(this);
        if (this->last_angle[0] != 0)
        {
            gui_turn_table_update_position(this, this->resetting_angle);
        }
        else
        {
            if (this->last_angle[1] == 0 &&
                this->last_angle[2] == 0 &&
                this->need_update == true)
            {
                gui_turn_table_update_position(this, this->resetting_angle);
                this->need_update = false;
            }
        }
    }

    last = this->checksum;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_turn_table_t));
    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
static void turn_table_destory(gui_obj_t *obj)
{
    gui_turn_table_t *this = (void *)obj;
    gui_free(this->static_list);
    gui_free(this->icon_list);
    gui_free(this->name_list);
}

static void gui_turn_table_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            turn_table_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            turn_table_prepare(obj);
            break;

        case OBJ_DESTORY:
            turn_table_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_turn_table_refresh_icon_list(gui_turn_table_t *this)
{
    if (this->icon_list == NULL)
    {
        this->icon_list = gui_malloc(sizeof(gui_img_t *) * this->icon_quantity);
        memset(this->icon_list, 0, sizeof(gui_img_t *) * this->icon_quantity);
    }
    else
    {
        this->icon_list = gui_realloc(this->icon_list, sizeof(gui_img_t *) * this->icon_quantity);
    }
    if (this->name_list == NULL)
    {
        this->name_list = gui_malloc(TT_IMG_NAME_LENGTH * this->icon_quantity);
        memset(this->name_list, 0, TT_IMG_NAME_LENGTH * this->icon_quantity);
    }
    else
    {
        this->name_list = gui_realloc(this->name_list, TT_IMG_NAME_LENGTH * this->icon_quantity);
    }
}

static void gui_turn_table_ctor(gui_turn_table_t *this,
                                gui_obj_t *parent,
                                uint8_t icon_quantity,
                                uint8_t icon_radius,
                                uint8_t top_bottom_icon_quantity,
                                uint8_t left_right_icon_quantity)
{
    //for base class
    gui_obj_ctor(&this->base, parent, "turn_table", 0, 0, 0, 0);

    //for root class
    GET_BASE(this)->type = TURN_TABLE;
    GET_BASE(this)->obj_cb = gui_turn_table_cb;
    GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->skip_tp_up_hold = true;
    GET_BASE(this)->skip_tp_down_hold = true;
    GET_BASE(this)->skip_tp_left_hold = true;
    GET_BASE(this)->skip_tp_right_hold = true;

    this->icon_quantity = icon_quantity;
    this->icon_radius = icon_radius;
    this->top_bottom_icon_quantity = top_bottom_icon_quantity;
    this->left_right_icon_quantity = left_right_icon_quantity;
    this->active_icon_quantity = (this->top_bottom_icon_quantity + this->left_right_icon_quantity) * 2;
    this->icon_scale = 1.0f;
    this->icon_highlight_scale = 2.0f;
    this->highlight_index = this->active_icon_quantity / 2;
    this->sector_angle = M_PI / this->active_icon_quantity * 2;
    this->need_update = true;
    this->static_list = gui_malloc(sizeof(gui_turn_table_coordinate_t) *
                                   (this->active_icon_quantity + 1));

    gui_turn_table_refresh_icon_list(this);
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_turn_table_t *gui_turn_table_create(void *parent,
                                        uint8_t icon_quantity,
                                        uint8_t icon_radius,
                                        uint8_t top_bottom_icon_quantity,
                                        uint8_t left_right_icon_quantity)
{
    GUI_ASSERT(parent != NULL);
    gui_turn_table_t *turn_table = gui_malloc(sizeof(gui_turn_table_t));
    GUI_ASSERT(turn_table != NULL);
    memset(turn_table, 0x00, sizeof(gui_turn_table_t));

    gui_turn_table_ctor(turn_table, (gui_obj_t *)parent, icon_quantity, icon_radius,
                        top_bottom_icon_quantity, left_right_icon_quantity);
    gui_list_init(&(GET_BASE(turn_table)->child_list));
    if ((GET_BASE(turn_table)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(turn_table)->parent)->child_list),
                               &(GET_BASE(turn_table)->brother_list));
    }
    GET_BASE(turn_table)->create_done = true;
    return turn_table;
}

void gui_turn_table_set_edge_icon_quantity(gui_turn_table_t *this,
                                           uint8_t top_bottom_quantity,
                                           uint8_t left_right_quantity)
{
    this->top_bottom_icon_quantity = top_bottom_quantity;
    this->left_right_icon_quantity = left_right_quantity;
    this->active_icon_quantity = (this->top_bottom_icon_quantity + this->left_right_icon_quantity) * 2;
}

void gui_turn_table_generate_layout_by_calculate(gui_turn_table_t *this,
                                                 uint8_t icon_margin,
                                                 uint8_t icon_distence,
                                                 uint8_t screen_fillet_radius)
{


}

void gui_turn_table_set_tp(gui_turn_table_t *this,
                           bool up,
                           bool down,
                           bool left,
                           bool right)
{
    this->skip_up_tp = !up;
    this->skip_down_tp = !down;
    this->skip_left_tp = !left;
    this->skip_right_tp = !right;
}

void gui_turn_table_generate_layout_by_array(gui_turn_table_t *this,
                                             gui_turn_table_coordinate_t *fixed_point,
                                             uint8_t active_icon_quantity)
{
    if (active_icon_quantity != this->active_icon_quantity)
    {
        gui_log("Please ensure active_icon_quantity! \n");
        return;
    }
    int center_offset = this->icon_radius * this->icon_scale;
    for (int i = 0; i < active_icon_quantity; i++)
    {
        this->static_list[i].x = fixed_point[i].x - center_offset;
        this->static_list[i].y = fixed_point[i].y - center_offset;
    }
    this->static_list[active_icon_quantity].x = fixed_point[0].x - center_offset;
    this->static_list[active_icon_quantity].y = fixed_point[0].y - center_offset;
}



void gui_turn_table_add_icon_default(gui_turn_table_t *this,
                                     void *icon_addr,
                                     gui_event_cb_t event_cb)
{
    gui_dispdev_t *dc = gui_get_dc();
    int x, y;
    for (int i = 0; i < this->icon_quantity; i++)
    {
        if (this->icon_list[i] != NULL)
        {
            continue;
        }
        if (i < this->active_icon_quantity)
        {
            x = this->static_list[i].x;
            y = this->static_list[i].y;
        }
        else
        {
            x = this->static_list[0].x;
            y = this->static_list[0].y;
        }
        sprintf((char *)&this->name_list[i * TT_IMG_NAME_LENGTH], "tt_img_%d", i);
        this->icon_list[i] = gui_img_create_from_mem(this,
                                                     (const char *)&this->name_list[i * TT_IMG_NAME_LENGTH],
                                                     icon_addr, x, y, 0, 0);
        gui_img_scale(this->icon_list[i], this->icon_scale, this->icon_scale);
        gui_img_set_quality(this->icon_list[i], true);
        if (i == this->highlight_index)
        {
            if (this->highlight_icon == NULL)
            {
                x = gui_get_screen_width() / 2 - this->icon_radius * this->icon_highlight_scale;
                y = gui_get_screen_height() / 2 - this->icon_radius * this->icon_highlight_scale;
                this->highlight_icon = gui_img_create_from_mem(this, "tth_img", icon_addr, x, y, 0, 0);
                gui_img_scale(this->highlight_icon, this->icon_highlight_scale, this->icon_highlight_scale);
                gui_img_set_quality(this->highlight_icon, true);
            }
        }

        if (event_cb != NULL)
        {
            gui_obj_add_event_cb(this->icon_list[i], event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
        return;
    }
}

void gui_turn_table_set_highlight_index(gui_turn_table_t *this,
                                        uint8_t highlight_index)
{
    if (highlight_index < this->active_icon_quantity)
    {
        this->highlight_index = highlight_index;
    }
}

void gui_turn_table_set_icon_scale(gui_turn_table_t *this,
                                   float icon_scale,
                                   float icon_highlight_scale)
{
    if (icon_scale > 0)
    {
        this->icon_scale = icon_scale;
    }
    if (icon_scale > 0)
    {
        this->icon_highlight_scale = icon_highlight_scale;
    }
}

void gui_turn_table_add_icon_static(gui_turn_table_t *this,
                                    void *icon_addr)
{
    if (icon_addr == NULL)
    {
        return;
    }
    if (this->static_icon != NULL)
    {
        gui_obj_tree_free(this->static_icon);
    }
    int x = this->static_list[0].x;
    int y = this->static_list[0].y;
    this->static_icon = gui_img_create_from_mem(this, "tts_img", icon_addr, x, y, 0, 0);
    gui_img_scale(this->static_icon, this->icon_scale, this->icon_scale);
    gui_img_set_quality(this->static_icon, true);
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





