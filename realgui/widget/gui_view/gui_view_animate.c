/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view_animate.c
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
#include "gui_img.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define VIEW_TRANSITION_DURATION_MS 1000
#define VIEW_FREE_EVENT GUI_EVENT_8
/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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
static void view_transition_animation_fade_in(float pro, gui_view_t *view, float *scale_x,
                                              float *scale_y)
{
    // Ensure progress is between 0.0 and 1.0
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function to transform progress
    float eased_pro = (pro);

    // Calculate and set scale rate
    float scale_rate_horizontal = 1.0f + (0.5f * (1.0f - eased_pro));  // Scale from 1.5 to 1.0
    float scale_rate_vertical = scale_rate_horizontal;    // Scale from 1.5 to 1.0
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;

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

static void view_transition_animation_from_right(float pro, gui_view_t *view, float *scale_x,
                                                 float *scale_y)
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
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;
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
static void view_transition_animation_to_right(float pro, gui_view_t *view, float *scale_x,
                                               float *scale_y)
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
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;
}

static void view_transition_animation_zoom_center(float pro, gui_view_t *view, float *scale_x,
                                                  float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired

    // Calculate scaling factor (scale from 0.5 to 1.0)
    float scale_rate_horizontal = 0.5f + 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;

    // Calculate and set opacity (from 0 to 255)
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;
}
static void view_transition_animation_collapse_to_center(float pro, gui_view_t *view,
                                                         float *scale_x, float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use an easing function to transform progress
    float eased_pro = pro; // Replace this with an actual easing function if desired

    // Calculate scaling factor (scale from 1.0 to 0.5)
    float scale_rate_horizontal = 1.0f - 0.5f * eased_pro;
    float scale_rate_vertical = scale_rate_horizontal;
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;

    // Calculate and set opacity (from 255 to 0)
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;
}

static void animation_case(gui_view_t *this, float pro)
{
    gui_obj_t *obj = GUI_BASE(this);
    VIEW_SWITCH_STYLE style = this->style;
    float scale_x = 0;
    float scale_y = 0;

    switch (style)
    {
    case VIEW_ANIMATION_NULL:
        {
            GUI_BASE(this)->not_show = 1;
        }
        break;
    case VIEW_ANIMATION_1:
        {
            view_transition_animation_zoom_center(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_2:
        {
            view_transition_animation_fade_in(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_3:
        {
            view_transition_animation_from_right(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_4:
        {
            view_transition_animation_from_right(bounce_ease_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_5:
        {
            view_transition_animation_from_opacity0(ease_in_out(pro), this);

        }
        break;
    case VIEW_ANIMATION_6:
        {
            view_transition_animation_collapse_to_center(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_7:
        {
            view_transition_animation_to_right(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case VIEW_ANIMATION_8:
        {
            view_transition_animation_to_opacity0(ease_in_out(pro), this);
        }
        break;
    default:
        break;
    }
    if (scale_x != 0 || scale_y != 0)
    {
        matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
        if (scale_x == 0)
        {
            matrix_scale(1, scale_y, obj->matrix);
        }
        else if (scale_y == 0)
        {
            matrix_scale(scale_x, 1, obj->matrix);
        }
        else
        {
            matrix_scale(scale_x, scale_y, obj->matrix);
        }
        matrix_translate(obj->w / -2, obj->h / -2, obj->matrix);
    }
}

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(view_transition_animation_function)
{
    gui_view_t *this = this_widget;
    this->event = 1;

    float pro = animate->progress_percent;
    animation_case(this, pro);
    if (animate->end_frame)
    {
        if (this->view_switch_ready)
        {
            gui_obj_enable_event(GUI_BASE(this), VIEW_FREE_EVENT);
        }
        else
        {
            GUI_BASE(this)->opacity_value = UINT8_MAX;
            GUI_BASE(this)->x = 0;
            GUI_BASE(this)->y = 0;
            this->view_switch_ready = true;
            this->event = 0;
            this->style = VIEW_STILL;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_set_animate(gui_view_t *this)
{
    uint32_t dur = VIEW_TRANSITION_DURATION_MS;
    int repeat_count = 0;
    gui_animate_callback_t callback = view_transition_animation_function;
    void *p = NULL;
    GUI_SET_ANIMATE_HELPER
}