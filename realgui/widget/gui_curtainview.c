/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtainview.c
  * @brief curtain effect container widget, which can nest curtains.
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
#include <string.h>
#include "guidef.h"
#include "tp_algo.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_curtain.h"


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

static void gui_curtainview_input_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    GUI_UNUSED(tp);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    switch (this->cur_curtain)
    {
    case CURTAIN_MIDDLE:
        {
            if (this->has_up_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag_up)
                {
                    gui_obj_skip_other_up_hold(obj);
                    gui_obj_skip_other_left_hold(obj);
                    gui_obj_skip_other_right_hold(obj);
                }

                gui_obj_skip_other_down_hold(obj);

                obj->skip_tp_down_hold = false;
            }

            if (this->has_down_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag)
                {
                    gui_obj_skip_other_down_hold(obj);
                    gui_obj_skip_other_left_hold(obj);
                    gui_obj_skip_other_right_hold(obj);
                }

                gui_obj_skip_other_up_hold(obj);

                obj->skip_tp_up_hold = false;
            }

            if (this->has_left_curtain == true)
            {
                if (this->cur_curtain_flag || this->middle_flag_left || this->left_flag)
                {
                    gui_obj_skip_other_left_hold(obj);
                }

                gui_obj_skip_other_right_hold(obj);
            }

            if (this->has_right_curtain == true)
            {
                gui_obj_skip_other_left_hold(obj);
            }
        }
        break;

    case CURTAIN_UP:
        {
            gui_obj_skip_other_down_hold(obj);
            gui_obj_skip_other_up_hold(obj);
            gui_obj_skip_other_right_hold(obj);
            gui_obj_skip_other_left_hold(obj);
        }
        break;

    case CURTAIN_DOWN:
        {
            gui_obj_skip_other_down_hold(obj);
            gui_obj_skip_other_up_hold(obj);
            gui_obj_skip_other_right_hold(obj);
            gui_obj_skip_other_left_hold(obj);
        }
        break;

    case CURTAIN_LEFT:
        {
            gui_obj_skip_other_left_hold(obj);
            gui_obj_skip_other_right_hold(obj);
        }
        break;
    case CURTAIN_RIGHT:
        {
        }
        break;

    default:
        break;
    }
}


// Function prototypes
static void initialize_curtains(gui_obj_t *obj, gui_curtain_t **c_middle, gui_curtain_t **c_up,
                                gui_curtain_t **c_down, gui_curtain_t **c_left, gui_curtain_t **c_right);
static void handle_initial_curtain_state(gui_curtainview_t *this, gui_curtain_t *c_up,
                                         gui_curtain_t *c_down, gui_curtain_t *c_left, gui_curtain_t *c_right);
static void handle_touch_events(gui_curtainview_t *this, touch_info_t *tp, gui_obj_t *obj,
                                gui_curtain_t *c_up, gui_curtain_t *c_down, gui_curtain_t *c_left, gui_curtain_t *c_right);
static void update_positions(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                             gui_curtain_t *c_down, gui_curtain_t *c_left);
static void verify_and_update_checksum(gui_curtainview_t *this);
static void handle_hold_x_event(gui_curtainview_t *this, touch_info_t *tp, gui_obj_t *obj,
                                gui_curtain_t *c_left);
static void handle_hold_y_event(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                gui_curtain_t *c_down);
static void handle_default_touch_event(gui_curtainview_t *this);
static void gui_curtainview_prepare(gui_obj_t *obj)
{
    gui_curtainview_t *this = (gui_curtainview_t *)obj;
    touch_info_t *tp = tp_get_info();

    gui_curtain_t *c_middle = NULL, *c_up = NULL, *c_down = NULL, *c_left = NULL, *c_right = NULL;

    initialize_curtains(obj, &c_middle, &c_up, &c_down, &c_left, &c_right);
    handle_initial_curtain_state(this, c_up, c_down, c_left, c_right);
    handle_touch_events(this, tp, obj, c_up, c_down, c_left, c_right);
    update_positions(this, tp, c_up, c_down, c_left);
    verify_and_update_checksum(this);
}

static void initialize_curtains(gui_obj_t *obj, gui_curtain_t **c_middle, gui_curtain_t **c_up,
                                gui_curtain_t **c_down, gui_curtain_t **c_left, gui_curtain_t **c_right)
{

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (obj->type == CURTAIN)
        {
            gui_curtain_t *c = (void *)obj;
            switch (c->orientation)
            {
            case CURTAIN_MIDDLE: *c_middle = c; break;
            case CURTAIN_UP: *c_up = c; break;
            case CURTAIN_DOWN: *c_down = c; break;
            case CURTAIN_LEFT: *c_left = c; break;
            case CURTAIN_RIGHT: *c_right = c; break;
            default: break;
            }
        }
    }
}

static void handle_initial_curtain_state(gui_curtainview_t *this, gui_curtain_t *c_up,
                                         gui_curtain_t *c_down, gui_curtain_t *c_left, gui_curtain_t *c_right)
{
    if (this->cur_curtain == CURTAIN_MIDDLE && this->init_flag)
    {
        if (c_up) { GET_BASE(c_up)->not_show = true; }
        if (c_down) { GET_BASE(c_down)->not_show = true; }
        if (c_left) { GET_BASE(c_left)->not_show = true; }
        if (c_right) { GET_BASE(c_right)->not_show = true; }

        this->init_flag = false;
    }
}

static void handle_touch_events(gui_curtainview_t *this, touch_info_t *tp, gui_obj_t *obj,
                                gui_curtain_t *c_up, gui_curtain_t *c_down, gui_curtain_t *c_left, gui_curtain_t *c_right)
{
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        handle_hold_x_event(this, tp, obj, c_left);
        break;
    case TOUCH_HOLD_Y:
        handle_hold_y_event(this, tp, c_up, c_down);
        break;
    default:
        handle_default_touch_event(this);
        break;
    }
}

static void handle_hold_x_event(gui_curtainview_t *this, touch_info_t *tp, gui_obj_t *obj,
                                gui_curtain_t *c_left)
{
    if ((obj->skip_tp_left_hold && tp->deltaX  < 0) || (obj->skip_tp_right_hold && tp->deltaX  > 0))
    {
        return;
    }

    if (this->middle_flag || this->middle_flag_up || this->down_flag ||
        this->cur_curtain == CURTAIN_DOWN || this->up_flag || this->cur_curtain == CURTAIN_UP)
    {
        this->release_x = 0;
        this->middle_flag_left = 0;
        return;
    }

    if (this->middle_flag_left)
    {
        return;
    }

    this->release_y = 0;
    if (this->cur_curtain == CURTAIN_LEFT)
    {
        if (tp->deltaX < 0)
        {
            this->release_x = tp->deltaX;
            float scope = c_left ? c_left->scope : 1.0f;

            if (_UI_ABS(tp->deltaX) > (int)((float)gui_get_screen_width() / 2.0f * scope))
            {
                this->left_flag = 1;
            }
        }
        return;
    }

    this->release_x = tp->deltaX;
    float scope = c_left ? c_left->scope : 1.0f;

    if (this->release_x > (int)((float)gui_get_screen_width() * scope))
    {
        this->release_x = (int)((float)gui_get_screen_width() * scope);
    }

    if (this->release_x != 0 && this->cur_curtain == CURTAIN_MIDDLE)
    {
        if (this->release_x > 0 && c_left)
        {
            GET_BASE(c_left)->not_show = false;
        }
    }

    if (this->cur_curtain == CURTAIN_MIDDLE && tp->deltaX > 0 &&
        tp->deltaX > (int)((float)gui_get_screen_width() / 2.0f * scope))
    {
        this->middle_flag_left = 1;
    }
}
static bool should_skip_hold_y(gui_curtainview_t *this, touch_info_t *tp);
static void reset_flags_if_necessary(gui_curtainview_t *this);
static void handle_down_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_down);
static void handle_up_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up);
static void handle_middle_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                  gui_curtain_t *c_down);
static void handle_middle_curtain_down(gui_curtainview_t *this, touch_info_t *tp,
                                       gui_curtain_t *c_down);
static void handle_middle_curtain_up(gui_curtainview_t *this, touch_info_t *tp,
                                     gui_curtain_t *c_up);
static bool should_trigger_action(int delta, float scope);
static void trigger_done_callback(gui_curtainview_t *this);

static void handle_hold_y_event(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                gui_curtain_t *c_down)
{
    if (should_skip_hold_y(this, tp))
    {
        return;
    }

    reset_flags_if_necessary(this);

    this->release_x = 0;

    handle_down_curtain(this, tp, c_down);
    handle_up_curtain(this, tp, c_up);
    handle_middle_curtain(this, tp, c_up, c_down);
}

static bool should_skip_hold_y(gui_curtainview_t *this, touch_info_t *tp)
{
    return (this->base.skip_tp_up_hold && tp->deltaY < 0) ||
           (this->base.skip_tp_down_hold && tp->deltaY > 0);
}

static void reset_flags_if_necessary(gui_curtainview_t *this)
{
    if (this->middle_flag_left || this->left_flag)
    {
        this->release_y = 0;
        this->middle_flag = 0;
        this->middle_flag_up = 0;
    }
}

static void handle_down_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_down)
{
    if (this->cur_curtain == CURTAIN_DOWN && tp->deltaY > 0)
    {
        this->release_y = tp->deltaY;
        float scope = c_down ? c_down->scope : 1.0f;

        if (should_trigger_action(tp->deltaY, scope))
        {
            this->down_flag = 1;
            trigger_done_callback(this);
        }
    }
}

static void handle_up_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up)
{
    if (this->cur_curtain == CURTAIN_UP && tp->deltaY < 0)
    {
        this->release_y = tp->deltaY;
        float scope = c_up ? c_up->scope : 1.0f;

        if (should_trigger_action(tp->deltaY, scope))
        {
            this->up_flag = 1;
            trigger_done_callback(this);
        }
    }
}

static void handle_middle_curtain(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                  gui_curtain_t *c_down)
{
    handle_middle_curtain_down(this, tp, c_down);
    handle_middle_curtain_up(this, tp, c_up);
}

static void handle_middle_curtain_down(gui_curtainview_t *this, touch_info_t *tp,
                                       gui_curtain_t *c_down)
{
    if (this->cur_curtain == CURTAIN_MIDDLE && tp->deltaY < 0 && this->middle_flag_up == 0)
    {
        this->release_y = tp->deltaY;
        if (c_down)
        {
            GET_BASE(c_down)->not_show = false;
        }

        float scope = c_down ? c_down->scope : 1.0f;
        if (should_trigger_action(tp->deltaY, scope))
        {
            this->middle_flag = 1;
            trigger_done_callback(this);
        }
    }
}

static void handle_middle_curtain_up(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up)
{
    if (this->cur_curtain == CURTAIN_MIDDLE && tp->deltaY > 0 && this->middle_flag == 0)
    {
        this->release_y = tp->deltaY;
        if (c_up)
        {
            GET_BASE(c_up)->not_show = false;
        }

        float scope = c_up ? c_up->scope : 1.0f;
        if (should_trigger_action(tp->deltaY, scope))
        {
            this->middle_flag_up = 1;
        }
    }
}

static bool should_trigger_action(int delta, float scope)
{
    return _UI_ABS(delta) > (int)((float)gui_get_screen_height() / 2.0f * scope);
}

static void trigger_done_callback(gui_curtainview_t *this)
{
    if (this->done_cb != NULL)
    {
        this->done_cb(this);
    }
}

static void handle_default_touch_event(gui_curtainview_t *this)
{
    this->cur_curtain_flag = 0;
}

static void update_positions_vertical(gui_curtainview_t *this, touch_info_t *tp,
                                      gui_curtain_t *c_up, gui_curtain_t *c_down);
static void update_middle_to_end(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                 gui_curtain_t *c_down);
static bool finalize_release_y(gui_curtainview_t *this, gui_curtain_t *curtain, float scope,
                               int direction);
static void reset_downward_release(gui_curtainview_t *this, gui_curtain_t *c_down);
static void reset_upward_release(gui_curtainview_t *this, gui_curtain_t *c_up);
static void finalize_down_movement(gui_curtainview_t *this, float scope);
static void finalize_up_movement(gui_curtainview_t *this, float scope);
static void reset_release_y(gui_curtainview_t *this);
static void update_positions_horizontal(gui_curtainview_t *this, touch_info_t *tp,
                                        gui_curtain_t *c_left);
static void finalize_left_movement(gui_curtainview_t *this, gui_curtain_t *c_left, float scope);
static void finalize_to_middle(gui_curtainview_t *this, gui_curtain_t *c_left, float scope);
static void reset_release_x(gui_curtainview_t *this);

static void update_positions(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                             gui_curtain_t *c_down, gui_curtain_t *c_left)
{
    update_positions_vertical(this, tp, c_up, c_down);
    update_positions_horizontal(this, tp, c_left);
}

static void update_positions_vertical(gui_curtainview_t *this, touch_info_t *tp,
                                      gui_curtain_t *c_up, gui_curtain_t *c_down)
{
    float scope;

    if (this->middle_flag || this->middle_flag_up)
    {
        update_middle_to_end(this, tp, c_up, c_down);
    }
    else if (!this->middle_flag && this->release_y < 0 && this->cur_curtain == CURTAIN_MIDDLE &&
             tp->type != TOUCH_HOLD_Y)
    {
        reset_downward_release(this, c_down);
    }
    else if (!this->middle_flag_up && this->release_y > 0 && this->cur_curtain == CURTAIN_MIDDLE &&
             tp->type != TOUCH_HOLD_Y)
    {
        reset_upward_release(this, c_up);
    }
    else if (this->down_flag && tp->type != TOUCH_HOLD_Y)
    {
        scope = c_down ? c_down->scope : 1.0f;
        finalize_down_movement(this, scope);
    }
    else if (this->up_flag && tp->type != TOUCH_HOLD_Y)
    {
        scope = c_up ? c_up->scope : 1.0f;
        finalize_up_movement(this, scope);
    }
    else
    {
        reset_release_y(this);
    }
}

static void update_middle_to_end(gui_curtainview_t *this, touch_info_t *tp, gui_curtain_t *c_up,
                                 gui_curtain_t *c_down)
{
    float scope;

    if (this->middle_flag && tp->type != TOUCH_HOLD_Y)
    {
        scope = c_down ? c_down->scope : 1.0f;
        if (finalize_release_y(this, c_down, scope, -1))
        {
            this->cur_curtain = CURTAIN_DOWN;
        }
    }

    if (this->middle_flag_up && tp->type != TOUCH_HOLD_Y)
    {
        scope = c_up ? c_up->scope : 1.0f;
        if (finalize_release_y(this, c_up, scope, 1))
        {
            this->cur_curtain = CURTAIN_UP;
        }
    }
}

static bool finalize_release_y(gui_curtainview_t *this, gui_curtain_t *curtain, float scope,
                               int direction)
{
    int limit = (int)((float)gui_get_screen_height() * scope - GUI_FRAME_STEP);
    if ((direction < 0 && this->release_y >= -limit) || (direction > 0 && this->release_y <= limit))
    {
        this->release_y += direction * GUI_FRAME_STEP;
        if (curtain)
        {
            GET_BASE(curtain)->not_show = false;
        }
        return false;
    }

    this->release_y = 0;
    if (direction < 0)
    {
        this->middle_flag = 0;
    }
    else
    {
        this->middle_flag_up = 0;
    }

    if (curtain)
    {
        GET_BASE(curtain)->not_show = false;
    }
    return true;
}

static void reset_downward_release(gui_curtainview_t *this, gui_curtain_t *c_down)
{
    if (this->release_y <= 0)
    {
        this->release_y = _UI_ABS(this->release_y + GUI_FRAME_STEP) < GUI_FRAME_STEP ? 0 : this->release_y +
                          GUI_FRAME_STEP;
        if (c_down)
        {
            GET_BASE(c_down)->not_show = true;
        }
    }
}

static void reset_upward_release(gui_curtainview_t *this, gui_curtain_t *c_up)
{
    if (this->release_y >= 0)
    {
        this->release_y = _UI_ABS(this->release_y - GUI_FRAME_STEP) < GUI_FRAME_STEP ? 0 : this->release_y -
                          GUI_FRAME_STEP;
        if (c_up)
        {
            GET_BASE(c_up)->not_show = true;
        }
    }
}

static void finalize_down_movement(gui_curtainview_t *this, float scope)
{
    if (this->release_y <= (float)gui_get_screen_height() * scope - GUI_FRAME_STEP)
    {
        this->release_y += GUI_FRAME_STEP;
    }
    else
    {
        this->release_y = 0;
        this->down_flag = 0;
        this->cur_curtain = CURTAIN_MIDDLE;
    }
}

static void finalize_up_movement(gui_curtainview_t *this, float scope)
{
    if (this->release_y >= -((float)gui_get_screen_height() * scope - GUI_FRAME_STEP))
    {
        this->release_y -= GUI_FRAME_STEP;
    }
    else
    {
        this->release_y = 0;
        this->up_flag = 0;
        this->cur_curtain = CURTAIN_MIDDLE;
    }
}

static void reset_release_y(gui_curtainview_t *this)
{
    if (this->release_y >= GUI_FRAME_STEP)
    {
        this->release_y -= GUI_FRAME_STEP;
    }
    else if (this->release_y <= -GUI_FRAME_STEP)
    {
        this->release_y += GUI_FRAME_STEP;
    }
    else
    {
        this->release_y = 0;
    }
}

static void update_positions_horizontal(gui_curtainview_t *this, touch_info_t *tp,
                                        gui_curtain_t *c_left)
{
    float scope;

    if (this->cur_curtain == CURTAIN_MIDDLE && this->release_x > 0 && !this->middle_flag_left)
    {
        this->release_x -= GUI_FRAME_STEP;
        if (c_left)
        {
            GET_BASE(c_left)->not_show = false;
        }
    }
    else if (this->middle_flag_left)
    {
        scope = c_left ? c_left->scope : 1.0f;
        finalize_left_movement(this, c_left, scope);
    }
    else if (this->left_flag && tp->type != TOUCH_HOLD_X)
    {
        scope = c_left ? c_left->scope : 1.0f;
        finalize_to_middle(this, c_left, scope);
    }
    else
    {
        reset_release_x(this);
    }
}

static void finalize_left_movement(gui_curtainview_t *this, gui_curtain_t *c_left, float scope)
{
    if (this->release_x <= (float)gui_get_screen_width() * scope - GUI_FRAME_STEP)
    {
        this->release_x += GUI_FRAME_STEP;
        if (c_left)
        {
            GET_BASE(c_left)->not_show = false;
        }
    }
    else
    {
        this->release_x = 0;
        this->middle_flag_left = 0;
        if (c_left)
        {
            this->cur_curtain = CURTAIN_LEFT;
            GET_BASE(c_left)->not_show = false;
        }
    }
}

static void finalize_to_middle(gui_curtainview_t *this, gui_curtain_t *c_left, float scope)
{
    if (this->release_x >= -(int)((float)gui_get_screen_width() * scope - GUI_FRAME_STEP))
    {
        this->release_x -= GUI_FRAME_STEP;
    }
    else
    {
        this->release_x = 0;
        this->left_flag = 0;
        this->cur_curtain = CURTAIN_MIDDLE;
        this->cur_curtain_flag = 1;
    }
}

static void reset_release_x(gui_curtainview_t *this)
{
    if (this->release_x >= GUI_FRAME_STEP)
    {
        this->release_x -= GUI_FRAME_STEP;
    }
    else if (this->release_x <= -GUI_FRAME_STEP)
    {
        this->release_x += GUI_FRAME_STEP;
    }
    else
    {
        this->release_x = 0;
    }
}

static void verify_and_update_checksum(gui_curtainview_t *this)
{
    uint8_t last = this->checksum;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_curtainview_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}
static void gui_curtainview_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_curtainview_input_prepare(obj);
            break;

        case OBJ_PREPARE:
            gui_curtainview_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_curtainview_ctor(gui_curtainview_t *this,
                                 gui_obj_t         *parent,
                                 const char        *filename,
                                 int16_t            x,
                                 int16_t            y,
                                 int16_t            w,
                                 int16_t            h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);

    ((gui_obj_t *)this)->type = CURTAINVIEW;
    ((gui_obj_t *)this)->obj_cb = gui_curtainview_cb;
    ((gui_obj_t *)this)->has_input_prepare_cb = true;
    ((gui_obj_t *)this)->has_prepare_cb = true;
    this->cur_curtain = CURTAIN_MIDDLE;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_curtainview_set_done_cb(gui_curtainview_t *this, void (*cb)(gui_curtainview_t *this))
{
    this->done_cb = cb;
}

gui_curtainview_t *gui_curtainview_create(void       *parent,
                                          const char *name,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h)
{
#define _paramgui_curtainview_create_ this, parent, name, x, y, w, h
    GUI_NEW(gui_curtainview_t, gui_curtainview_ctor, _paramgui_curtainview_create_)
}

