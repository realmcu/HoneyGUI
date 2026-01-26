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
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_view.h"
#include "gui_view_transition.h"


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
    // unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    // GUI_BASE(view)->opacity_value = opacity_value;

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;
}

static void view_transition_animation_from_left(float pro, gui_view_t *view, float *scale_x,
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
    int start_x = -screen_width; // Right side of the screen
    int end_x = 0; // Left side position

    // Get view's Y-coordinate to keep it unchanged
    int y_position = GUI_BASE(view)->y;

    // Calculate current position
    int new_x_position = start_x - (int)((start_x - end_x) * eased_pro);

    // Move view
    GUI_BASE(view)->x = new_x_position;
    GUI_BASE(view)->y = y_position;
    // Calculate and set opacity
    // unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    // GUI_BASE(view)->opacity_value = opacity_value;

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
    // unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    // GUI_BASE(view)->opacity_value = opacity_value;

    // Optionally add scale animation, assume no scale animation here
    float scale_rate_horizontal = 1.0f;
    float scale_rate_vertical = 1.0f;
    *scale_x = scale_rate_horizontal;
    *scale_y = scale_rate_vertical;
}

static void view_transition_animation_to_left(float pro, gui_view_t *view, float *scale_x,
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
    int end_x = -screen_width; // Right side of the screen

    // Get view's Y-coordinate to keep it unchanged
    int y_position = GUI_BASE(view)->y;

    // Calculate current position
    int new_x_position = start_x + (int)((end_x - start_x) * eased_pro);

    // Move view
    GUI_BASE(view)->x = new_x_position;
    GUI_BASE(view)->y = y_position;

    // Calculate and set opacity
    // unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    // GUI_BASE(view)->opacity_value = opacity_value;

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
static void view_transition_animation_zoom_from_top_left(float pro, gui_view_t *view,
                                                         float *scale_x,
                                                         float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function
    float eased_pro = ease_in_out(pro);

    // Calculate scaling factor (scale from 0.0 to 1.0)
    float scale_rate = 0.3f + 0.7f * eased_pro;
    *scale_x = scale_rate;
    *scale_y = scale_rate;

    // Calculate and set opacity (from 0 to 255)
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;

    GUI_BASE(view)->x = 0;
    GUI_BASE(view)->y = 0;
}
static void view_transition_animation_zoom_from_top_right(float pro, gui_view_t *view,
                                                          float *scale_x,
                                                          float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function
    float eased_pro = ease_in_out(pro);

    // Calculate scaling factor (scale from 0.0 to 1.0)
    float scale_rate = 0.3f + 0.7f * eased_pro;
    *scale_x = scale_rate;
    *scale_y = scale_rate;

    // Calculate and set opacity (from 0 to 255)
    unsigned char opacity_value = (unsigned char)(255 * eased_pro);
    GUI_BASE(view)->opacity_value = opacity_value;

    uint32_t screen_width = gui_get_screen_width();
    GUI_BASE(view)->x = screen_width - GUI_BASE(view)->w;
    GUI_BASE(view)->y = 0;
}
static void view_transition_animation_zoom_to_top_left(float pro, gui_view_t *view, float *scale_x,
                                                       float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function
    float eased_pro = ease_in_out(pro);

    // Calculate scaling factor (scale from 1.0 to 0.0)
    float scale_rate = 1.0f - eased_pro;
    *scale_x = scale_rate;
    *scale_y = scale_rate;

    // Calculate and set opacity (from 255 to 0)
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;

    // Move from current position to top left corner while scaling down
    GUI_BASE(view)->x = 0;
    GUI_BASE(view)->y = 0;
}
static void view_transition_animation_zoom_to_top_right(float pro, gui_view_t *view, float *scale_x,
                                                        float *scale_y)
{
    // Ensure progress is between 0.0f and 1.0f
    if (pro < 0.0f) { pro = 0.0f; }
    if (pro > 1.0f) { pro = 1.0f; }

    // Use easing function
    float eased_pro = ease_in_out(pro);

    // Calculate scaling factor (scale from 1.0 to 0.0)
    float scale_rate = 1.0f - 0.7f * eased_pro;
    *scale_x = scale_rate;
    *scale_y = scale_rate;

    // Calculate and set opacity (from 255 to 0)
    unsigned char opacity_value = (unsigned char)(255 * (1.0f - eased_pro));
    GUI_BASE(view)->opacity_value = opacity_value;

    uint32_t screen_width = gui_get_screen_width();
    GUI_BASE(view)->x = screen_width - GUI_BASE(view)->w;
    GUI_BASE(view)->y = 0;
}

/*============================================================================*
 *                           Animation Case Handler
 *============================================================================*/

static void animation_case(gui_view_t *this, float pro)
{
    gui_obj_t *obj = GUI_BASE(this);
    VIEW_SWITCH_STYLE style = this->current_transition_style;
    float scale_x = 0;
    float scale_y = 0;

    switch (style)
    {
    case SWITCH_OUT_NONE_ANIMATION:
        gui_obj_hidden(obj, true);
        break;
    case SWITCH_IN_NONE_ANIMATION:
        break;
    case SWITCH_IN_ANIMATION_ZOOM:
        {
            view_transition_animation_zoom_center(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_MOVE_FADE:
        {
            view_transition_animation_fade_in(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT:
        {
            view_transition_animation_from_right(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_MOVE_FROM_LEFT:
        {
            view_transition_animation_from_left(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_BOUNCE_FROM_RIGHT:
        {
            view_transition_animation_from_right(bounce_ease_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_FADE:
        {
            view_transition_animation_from_opacity0(ease_in_out(pro), this);
        }
        break;
    case SWITCH_OUT_ANIMATION_ZOOM:
        {
            view_transition_animation_collapse_to_center(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT:
        {
            view_transition_animation_to_right(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_OUT_ANIMATION_MOVE_TO_LEFT:
        {
            view_transition_animation_to_left(ease_in_out(pro), this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_OUT_ANIMATION_FADE:
        {
            view_transition_animation_to_opacity0(ease_in_out(pro), this);
        }
        break;
    case SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_LEFT:
        {
            view_transition_animation_zoom_from_top_left(pro, this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_RIGHT:
        {
            view_transition_animation_zoom_from_top_right(pro, this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_LEFT:
        {
            view_transition_animation_zoom_to_top_left(pro, this, &scale_x, &scale_y);
        }
        break;
    case SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_RIGHT:
        {
            view_transition_animation_zoom_to_top_right(pro, this, &scale_x, &scale_y);
        }
        break;

    default:
        break;
    }
    // Apply scale transformation with different pivot points
    if (scale_x != 0 || scale_y != 0)
    {
        switch (style)
        {
        case SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_LEFT:
        case SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_LEFT:
            // left top scale:(0, 0)
            matrix_scale(scale_x, scale_y, obj->matrix);
            break;

        case SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_RIGHT:
        case SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_RIGHT:
            // Top-right corner zoom: First pan to the top-right corner, then zoom, then pan back.
            matrix_translate(obj->w, 0, obj->matrix);
            matrix_scale(scale_x, scale_y, obj->matrix);
            matrix_translate(-obj->w, 0, obj->matrix);
            break;

        default:
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
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void view_transition_animation(void *obj, float pro)
{
    animation_case((gui_view_t *)obj, pro);
}
