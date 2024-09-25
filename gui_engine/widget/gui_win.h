/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_win.h
  * @brief Window widget
  * @details A rectangular container widget which can listen to many gestures
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_WIN_H__
#define __GUI_WIN_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_curtainview.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  window structure */
typedef struct gui_win
{
    gui_obj_t base;
    gui_animate_t *animate;
    gui_animate_t **animate_array;
    float scale;
    float scale_y;
    bool press_flag;
    bool long_flag;
    bool release_flag;
    bool enter_auto_scale;
    bool event5_flag;
    uint8_t checksum;
    bool scope;
    bool hold_tp;
    uint8_t animate_array_length;
} gui_win_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/

/*============================================================================*
 *                         Macros
 *============================================================================*/

/*============================================================================*
 *                         Variables
 *============================================================================*/

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief create a window widget.
 * @param parent the father widget the window nested in.
 * @param filename the window widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 */
gui_win_t *gui_win_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);

/**
 * @brief set animate.
 *
 * @param _this widget object pointer.
 * @param dur animation duration.
 * @param repeat_count repeat play times, -1 means play on repeat forever.
 * @param callback animate frame callback.
 * @param p parameter.
 */
void gui_win_set_animate(gui_win_t *_this,
                         uint32_t   dur,
                         int        repeat_count,
                         void      *callback,
                         void      *p);

/**
 * @brief register a callback function for the left slide event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_left(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for the right slide event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_right(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for the up slide event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_up(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for the down slide event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_down(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for the press event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_press(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for the release event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func (void *obj, gui_event_t e, void *param).
 * @param parameter callback parameter.
 */
void gui_win_release(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for a long press event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func.
 * @param parameter callback parameter.
 */
void gui_win_long(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief register a callback function for a click event of the win widget.
 *
 * @param _this win pointer.
 * @param callback callback func (void *obj, gui_event_t e, void *param).
 * @param parameter callback parameter.
 */
void gui_win_click(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief set the hold_tp state.
 *
 * @param _this win pointer.
 * @param hold_tp a boolean value to set the state to true or false.
 */
void gui_win_hold_tp(gui_win_t *_this, bool hold_tp);

/**
 * @brief get the animation progress percentage.
 *
 * @param win pointer to the window structure that contains the animation.
 * @return the current animation progress percentage.
 */
float gui_win_get_animation_progress_percent(gui_win_t *win);

/**
 * @brief set the scale rate for the window both horizontally and vertically.
 *
 * @param win pointer to the window structure.
 * @param scale_rate_horizontal the horizontal scale rate.
 * @param scale_rate_vertical the vertical scale rate.
 */
void gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical);

/**
 * @brief enable or disable the scope for the window.
 *
 * @param win pointer to the window structure.
 * @param enable a boolean value to enable or disable the scope.
 */
void gui_win_set_scope(gui_win_t *win, bool enable);

/**
 * @brief set the opacity value for the window.
 *
 * @param win pointer to the window structure.
 * @param opacity_value the desired opacity value to set.
 */
void gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value);

/**
 * @brief check if the animation is at its end frame.
 *
 * @param win pointer to the window structure that contains the animation.
 * @return true if the end_frame is not 0, false otherwise.
 */
bool gui_win_is_animation_end_frame(gui_win_t *win);

/**
 * @brief start the animation by setting the animate field to 1.
 *
 * @param win pointer to the window structure that contains the animation.
 * If win or win->animate is NULL, the function will log an error message.
 */
void gui_win_start_animation(gui_win_t *win);

/**
 * @brief stop the animation by setting the animate field to 0.
 *
 * @param win pointer to the window structure that contains the animation.
 * If win or win->animate is NULL, the function will log an error message.
 */
void gui_win_stop_animation(gui_win_t *win);

/**
 * @brief window widget prepare.
 *
 * @param obj pointer.
 */
void gui_win_prepare(gui_obj_t *obj);

/**
 * @brief append an animation to a GUI window.
 *
 * This function appends an animation to the specified GUI window. The animation
 * will run for the specified duration, repeat the specified number of times, and
 * call the provided callback function at each frame.
 *
 * @param win pointer to the GUI window object.
 * @param dur duration of the animation in milliseconds.
 * @param repeat_count number of times the animation should repeat.
 * @param callback function to be called at each frame.
 * @param p user data to be passed to the callback function.
 * @param name aniamte name.
 *
 * @note The `callback` function should match the expected signature for animation
 *       callbacks in the GUI library being used. The `p` parameter allows the
 *       passing of additional data to the callback function.
 */
void gui_win_append_animate(gui_win_t  *win,
                            uint32_t    dur,
                            int         repeat_count,
                            void       *callback,
                            void       *p,
                            const char *name);
#ifdef __cplusplus
}
#endif

#endif
