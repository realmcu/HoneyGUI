/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
  * @brief the view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details switch the currently view by sliding or clicking
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
#include "guidef.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "kb_algo.h"
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
#define VIEW_TRANSITION_DURATION_MS 1000

/*============================================================================*
 *                            Variables
 *============================================================================*/
static bool event_flag; // 1: stop setting event
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void pre_load_view(gui_view_t *this);

static void obj_pave_shot_cb(void *p, void *this_widget, gui_animate_t *animate)
{
    gui_img_t *pave_shot = (gui_img_t *)this_widget;
    matrix_identity(GUI_BASE(pave_shot)->matrix);
}

static void gui_view_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    gui_view_t *view = (gui_view_t *)obj;
    gui_view_t *this = (gui_view_t *)obj;
    uint8_t last = this->checksum;
    this->initial = true;
    animate_frame_update(this->animate, obj);

    if (this->view_need_pre_load)
    {
        this->view_need_pre_load = false;
        GET_BASE(this)->has_prepare_cb = false;
        pre_load_view(this);
        GET_BASE(this)->has_prepare_cb = true;
    }
    VIEW_CHANGE_STYLE style = this->style;
    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0))
    {
        gui_obj_event_set(obj, GUI_EVENT_KB_SHORT_CLICKED);
    }

    if (view->tp_disable)
    {
        return;
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y))
    {
        switch (tp->type)
        {
        case TOUCH_HOLD_X:
            {
                this->release_x = tp->deltaX;
                if (event_flag) { break; }
                if (this->view_change_ready && this->release_x < 0)
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_MOVE_LEFT);
                    event_flag = 1;
                }
                else if ((this->view_change_ready && this->release_x > 0))
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_MOVE_RIGHT);
                    event_flag = 1;
                }
            }
            break;

        case TOUCH_HOLD_Y:
            {
                this->release_y = tp->deltaY;
                if (event_flag) { break; }
                if (this->view_change_ready && this->release_y < 0)
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_MOVE_UP);
                    event_flag = 1;
                }
                else if (this->view_change_ready && this->release_y > 0)
                {
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_MOVE_DOWN);
                    event_flag = 1;
                }
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                if (this->view_left || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_LEFT_SLIDE\n");
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE);
                    this->cur_id.x -= 1;
                    this->release_x = this->release_x + view->base.w;
                }
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                if (this->view_right || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_RIGHT_SLIDE\n");
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE);
                    this->cur_id.x += 1;
                    this->release_x = this->release_x - view->base.w;
                }
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                if (this->view_down || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_DOWN_SLIDE\n");
                    this->cur_id.y += 1;
                    this->release_y = this->release_y - view->base.h;
                }
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                if (this->view_up || !this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_UP_SLIDE\n");
                    this->cur_id.y -= 1;
                    this->release_y = this->release_y + view->base.h;
                }
            }
            break;
        case TOUCH_SHORT:
            {
                if (event_flag) { break; }
                if (this->view_click && this->view_change_ready)
                {
                    gui_log("[VIEW]TOUCH_CLICK\n");
                    gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
                    event_flag = 1;
                    style = VIEW_ANIMATION_NULL; // keep event_flag 1
                }
            }
            break;
        default:
            break;
        }
    }

    if (this->scale_x != 0 || this->scale_y != 0)
    {
        matrix_translate(GET_BASE(obj)->w / 2, GET_BASE(obj)->h / 2, GET_BASE(obj)->matrix);
        if (this->scale_x == 0)
        {
            matrix_scale(1, this->scale_y, GET_BASE(obj)->matrix);
        }
        else if (this->scale_y == 0)
        {
            matrix_scale(this->scale_x, 1, GET_BASE(obj)->matrix);
        }
        else
        {
            matrix_scale(this->scale_x, this->scale_y, GET_BASE(obj)->matrix);
        }
        matrix_translate(GET_BASE(obj)->w / -2, GET_BASE(obj)->h / -2, GET_BASE(obj)->matrix);
    }

    if (this->enable_pre_load)
    {
        if (style == VIEW_ROTATE_BOOK)
        {
            if (this->shot_pave_obj == NULL)
            {
                this->shot_pave_obj = (gui_obj_t *)gui_img_create_from_mem(obj, "shot_1", (void *)this->shot, 0, 0,
                                                                           0, 0);
                gui_img_set_mode((gui_img_t *)(this->shot_pave_obj), IMG_BYPASS_MODE);
                gui_img_set_mode((gui_img_t *)(this->shot_obj), IMG_BYPASS_MODE);
                gui_img_set_animate((gui_img_t *)(this->shot_pave_obj), 1000, -1, obj_pave_shot_cb, (void *)obj);
            }
            if (this->release_x != 0 || this->release_y != 0)
            {
                gui_obj_show(this->shot_pave_obj, true);
            }
            else
            {
                gui_obj_show(this->shot_pave_obj, false);
            }
        }
        else if (this->shot_pave_obj)
        {
            gui_obj_show(this->shot_pave_obj, false);
        }

        if ((this->release_x != 0 || this->release_y != 0) && style < VIEW_ANIMATION_NULL)
        {
            gui_obj_show(this->shot_obj, true);
            gui_obj_show(this->rte_obj, false);
        }
        else
        {
            gui_obj_show(this->shot_obj, false);
            gui_obj_show(this->rte_obj, true);
        }
    }

    // restore view offset
    if (tp->pressing == false)
    {
        if (style < VIEW_ANIMATION_NULL)
        {
            event_flag = 0;
        }
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


    gui_list_t *list = &(obj->parent->child_list);
    if (list->next == list->prev || style >= VIEW_ANIMATION_NULL)
    {
        return; // skip slide animation
    }
    if (style == VIEW_TRANSPLATION)
    {
        matrix_translate(this->cur_id.x * (int)this->base.w + this->release_x, \
                         this->cur_id.y * (int)this->base.h + this->release_y, \
                         obj->matrix);
    }
    else if (style == VIEW_CUBE)
    {
        gui_view_cube(obj);
    }
    else if (style == VIEW_ROTATE)
    {
        gui_view_rotate(obj);
    }
    else if (style == VIEW_ROTATE_BOOK)
    {
        gui_view_rotate_book(obj);
    }
    else if (style == VIEW_REDUCTION)
    {
        gui_view_reduction(obj);
    }

    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_view_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void pre_load_view(gui_view_t *this)
{
    gui_obj_t *obj = GUI_BASE(this);
    gui_img_tree_convert_to_img((gui_obj_t *)this->rte_obj, NULL, this->shot);

    if (this->shot_obj == NULL)
    {
        this->shot_obj = (gui_obj_t *)gui_img_create_from_mem(obj, "shot", (void *)this->shot, 0,
                                                              0, 0, 0);
    }
}

static void gui_view_free(void *obj, gui_event_t e, void *param)
{
    gui_obj_tree_free(obj);
    gui_log("[VIEW]free view done!\n");
}

static void gui_view_destory(gui_obj_t *obj)
{
    gui_view_t *this = (gui_view_t *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    if (this->shot != NULL)
    {
        gui_free(this->shot);
        this->shot = NULL;
    }
    for (uint32_t i = 0; i < this->view_dsc_cnt; i++)
    {
        gui_free(this->view_dsc[i]);
    }
}

static void gui_view_end(gui_obj_t *obj)
{
    gui_view_t *this = (gui_view_t *)obj;
    VIEW_CHANGE_STYLE style = this->style;

    if ((this->release_x == 0) && (this->release_y == 0) &&
        style < VIEW_ANIMATION_NULL) // distinguish click event
    {
        if (this->cur_id.x != 0 || this->cur_id.y != 0)
        {
            if (this->initial) // prevent freeing new view when it is caching
            {
                gui_obj_event_set(obj, GUI_EVENT_8);
                obj->active = 1;
            }
        }
        else
        {
            this->view_change_ready = true;
        }
    }
}

static void gui_view_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                // gui_view_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                gui_view_prepare(obj);
            }
            break;

        case OBJ_END:
            {
                gui_view_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                gui_view_destory(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void animation_case(gui_view_t *view, float pro);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(view_transition_animation_function)
{
    gui_view_t *view = this_widget;
    float pro = animate->progress_percent;
    animation_case(view, pro);
    if (animate->end_frame)
    {
        if (view->view_change_ready)
        {
            gui_obj_event_set(GUI_BASE(view), GUI_EVENT_8);
        }
        else
        {
            view->scale_x = 0;//reset scale
            view->scale_y = 0;
            GUI_BASE(view)->opacity_value = UINT8_MAX;
            GUI_BASE(view)->x = 0;
            GUI_BASE(view)->y = 0;
            view->view_change_ready = true;
            event_flag = 0;
        }
    }
}

static void gui_view_set_animate(gui_view_t *this,
                                 uint32_t   dur,
                                 int        repeat_count,
                                 gui_animate_callback_t callback,
                                 void      *p)
{
    GUI_SET_ANIMATE_HELPER
}

static void gui_view_update_view(void *obj, gui_event_t e, void *param)
{
    gui_view_t *view_old = (gui_view_t *)obj;

    gui_view_dsc_t *dsc = (gui_view_dsc_t *)param;
    gui_view_t *view_new = gui_view_create(GUI_BASE(view_old)->parent, "view", 0, 0, 0, 0);
    view_old->style = dsc->style_out;
    view_new->style = dsc->style_in;
    if (dsc->obj_pointer)
    {
        *dsc->obj_pointer = view_new;
    }
    dsc->view_design(view_new->rte_obj);

    int16_t idx = 0;
    int16_t idy = 0;
    switch (e)
    {
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        idx = 1;
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        idx = -1;
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        idy = 1;
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        idy = -1;
        break;
    case GUI_EVENT_TOUCH_CLICKED:
        {
            gui_view_set_animate(view_old, VIEW_TRANSITION_DURATION_MS, 0, view_transition_animation_function,
                                 NULL);
            gui_view_set_animate(view_new, VIEW_TRANSITION_DURATION_MS, 0, view_transition_animation_function,
                                 NULL);
        }
        break;
    default:
        break;
    }
    view_new->cur_id.x = idx;
    view_new->cur_id.y = idy;

    if (dsc->style_out == VIEW_ROTATE_BOOK)
    {
        gui_view_enable_pre_load(view_old, true);
        gui_view_enable_pre_load(view_new, true);
    }
}

static float ease_in_out(float t)
{
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}
static float bounce_ease_out(float t)
{
    if (t < 1 / 2.75f)
    {
        return 7.5625f * t * t;
    }
    else if (t < 2 / 2.75f)
    {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    }
    else if (t < 2.5f / 2.75f)
    {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    }
    else
    {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}
static void view_transition_animation_fade_in(float pro, gui_view_t *view)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Calculate and set scale rate
    float scale_rate_horizontal = 1.0f + (0.5f * (1.0f - eased_pro));  // Scale from 1.5 to 1.0
    float scale_rate_vertical = scale_rate_horizontal;    // Scale from 1.5 to 1.0
    view->scale_x = scale_rate_horizontal;
    view->scale_y = scale_rate_vertical;

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);  // Opacity from 0 to 255
    GUI_BASE(view)->opacity_value = opacity_value;

    // Calculate and set position (assume view moves along the X-axis, height remains the same)
    int x_start = 100;  // Start position
    int x_end = 0;      // End position
    int y_position = GUI_BASE(view)->y;  // Maintain Y position

    int new_x_position = x_start + (int)((x_end - x_start) * eased_pro);
    GUI_BASE(view)->x = new_x_position;
    GUI_BASE(view)->y = y_position;
}

static void view_transition_animation_from_right(float pro, gui_view_t *view)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Get screen width
    uint32_t screen_width = gui_get_screen_width();

    // Set initial and final positions of the view
    int start_x = screen_width; // Right side of the screen
    int end_x = 0; // Left side position

    // Get view's Y-coordinate to keep it unchanged
    int y_position = GUI_BASE(view)->y;

    // Calculate current position
    int new_x_position = start_x - (int)((start_x - end_x) * eased_pro);

    // Move view
    GUI_BASE(view)->x = new_x_position;
    GUI_BASE(view)->y = y_position;

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    view->scale_x = scale_rate_horizontal;
    view->scale_y = scale_rate_vertical;
}
static void view_transition_animation_to_opacity0(float eased_pro, gui_view_t *view)
{
    // Ensure progress is between 0.0 and 1.0
    if (eased_pro < 0.0f) { eased_pro = 0.0f; }
    if (eased_pro > 1.0f) { eased_pro = 1.0f; }
    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;
}
static void view_transition_animation_from_opacity0(float eased_pro, gui_view_t *view)
{
    // Ensure progress is between 0.0 and 1.0
    if (eased_pro < 0.0f) { eased_pro = 0.0f; }
    if (eased_pro > 1.0f) { eased_pro = 1.0f; }
    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;
}
static void view_transition_animation_to_right(float pro, gui_view_t *view)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Get screen width
    uint32_t screen_width = gui_get_screen_width();

    // Set initial and final positions of the view
    int start_x = GUI_BASE(view)->x; // Current view position
    int end_x = screen_width; // Right side of the screen

    // Get view's Y-coordinate to keep it unchanged
    int y_position = GUI_BASE(view)->y;

    // Calculate current position
    int new_x_position = start_x + (int)((end_x - start_x) * eased_pro);

    // Move view
    GUI_BASE(view)->x = new_x_position;
    GUI_BASE(view)->y = y_position;

    // Calculate and set opacity
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    view->scale_x = scale_rate_horizontal;
    view->scale_y = scale_rate_vertical;
}

static void view_transition_animation_zoom_center(float pro, gui_view_t *view)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired

    // Calculate scaling factor (scale from 0.5 to 1.0)
    float scale_rate_horizontal = 0.5f + 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    view->scale_x = scale_rate_horizontal;
    view->scale_y = scale_rate_vertical;

    // Calculate and set opacity (from 0 to 255)
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;
}
static void view_transition_animation_collapse_to_center(float pro, gui_view_t *view)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired

    // Calculate scaling factor (scale from 1.0 to 0.5)
    float scale_rate_horizontal = 1.0f - 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    view->scale_x = scale_rate_horizontal;
    view->scale_y = scale_rate_vertical;

    // Calculate and set opacity (from 255 to 0)
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;
}

static void animation_case(gui_view_t *view, float pro)
{
    VIEW_CHANGE_STYLE style = view->style;
    switch (style)
    {
    case VIEW_ANIMATION_NULL:
        break;
    case VIEW_ANIMATION_1:
        {
            view_transition_animation_zoom_center(ease_in_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_2:
        {
            view_transition_animation_fade_in(ease_in_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_3:
        {
            view_transition_animation_from_right(ease_in_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_4:
        {
            view_transition_animation_from_right(bounce_ease_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_5:
        {
            view_transition_animation_from_opacity0(ease_in_out(pro), view);

        }
        break;
    case VIEW_ANIMATION_6:
        {
            view_transition_animation_collapse_to_center(ease_in_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_7:
        {
            view_transition_animation_to_right(ease_in_out(pro), view);
        }
        break;
    case VIEW_ANIMATION_8:
        {
            view_transition_animation_to_opacity0(ease_in_out(pro), view);
        }
        break;
    default:
        break;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_view_t *gui_view_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    gui_view_t *this = gui_malloc(sizeof(gui_view_t));
    gui_dispdev_t *dc = gui_get_dc();
    memset(this, 0, sizeof(gui_view_t));

    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    gui_obj_ctor(&this->base, parent, name, x, y, w, h);

    GET_BASE(this)->obj_cb = gui_view_cb;
    // GET_BASE(this)->has_input_prepare_cb = true;
    GET_BASE(this)->has_prepare_cb = true;
    GET_BASE(this)->has_end_cb = true;
    GET_BASE(this)->has_destroy_cb = true;
    GET_BASE(this)->type = VIEW;
    GET_BASE(this)->create_done = true;
    this->style = VIEW_TRANSPLATION;
    // this->cur_id.x = 0;
    // this->cur_id.y = 0;
    // this->view_dsc_cnt = 0;
    // this->view_need_pre_load = 0;

    gui_list_init(&(GET_BASE(this)->child_list));
    gui_obj_add_event_cb(GET_BASE(this), gui_view_free, GUI_EVENT_8, NULL);
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    if ((GET_BASE(this)->parent)->child_list.next == (GET_BASE(this)->parent)->child_list.prev)
    {
        this->view_change_ready = true;
    }
    else
    {
        this->view_change_ready = false;
    }
    this->rte_obj = gui_obj_create(this, "view_rte", this->base.x, this->base.y, this->base.w,
                                   this->base.h);
    this->rte_obj->type = IMAGE_FROM_MEM;
    gui_obj_show(this->shot_obj, false);
    return this;
}

void gui_view_add_change_event(gui_view_t *this, void **obj_pointer, VIEW_CHANGE_STYLE style_out,
                               VIEW_CHANGE_STYLE style_in,
                               void (* cb)(void *parent), gui_event_t event)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_view_dsc_t *dsc = gui_malloc(sizeof(gui_view_dsc_t));
    dsc->style_out = style_out;
    dsc->style_in = style_in;
    dsc->obj_pointer = obj_pointer;
    dsc->view_design = cb;
    // gui_log("view_dsc_addr: %0x\n", dsc);
    this->view_dsc_cnt++;
    this->view_dsc = gui_realloc(this->view_dsc, sizeof(gui_view_dsc_t *) * this->view_dsc_cnt);
    this->view_dsc[this->view_dsc_cnt - 1] = dsc;

    gui_event_dsc_t *event_dsc_old = NULL;
    for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event)
        {
            event_dsc_old = event_dsc;
            break;
        }
    }
    if (event_dsc_old) // update same event
    {
        event_dsc_old->event_cb = (gui_event_cb_t)gui_view_update_view;
        event_dsc_old->user_data = dsc;
    }
    else
    {
        gui_obj_add_event_cb(this, (gui_event_cb_t)gui_view_update_view, event,
                             dsc);
    }
    switch (event)
    {
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        {
            this->view_left = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        {
            this->view_right = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        {
            this->view_up = 1;
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        {
            this->view_down = 1;
        }
        break;
    case GUI_EVENT_TOUCH_CLICKED:
        {
            this->view_click = 1;
        }
        break;
    default:
        break;
    }
}

void gui_view_tp_disable(gui_view_t *view, bool disable_tp)
{
    view->tp_disable = disable_tp;
}

void gui_view_enable_pre_load(gui_view_t *this, bool enable)
{
    gui_dispdev_t *dc = gui_get_dc();

    uint16_t w = this->base.w;
    uint16_t h = this->base.h;
    uint16_t bit_depth = dc->bit_depth;

    uint32_t size = (uint32_t) w * h * bit_depth / 8;

    if (!this->shot)
    {
        this->shot = gui_malloc(sizeof(gui_rgb_data_head_t) + size);
        this->enable_pre_load = 1;
        this->view_need_pre_load = 1;
    }
    else
    {
        this->view_need_pre_load = 1;
    }
    if (!this->shot)
    {
        gui_log("[VIEW]Error: Failed to allocate memory for pre-load shot.");
    }
}