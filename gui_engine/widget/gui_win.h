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
 * @brief
 *
 * @param o widget object pointer
 * @param dur Animation duration
 * @param repeat_count Repeat play times, -1 means play on repeat forever
 * @param callback animate frame callback
 * @param p parameter
 */
void gui_win_set_animate(gui_win_t *_this,
                         uint32_t   dur,
                         int        repeat_count,
                         void      *callback,
                         void      *p);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_left(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_right(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_up(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_down(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_press(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_release(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_long(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_click(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param _this
 * @param hold_tp
 */
void gui_win_hole_tp(gui_win_t *_this, bool hold_tp);
/**
 * @brief Get the animation progress percentage.
 *
 * @param win Pointer to the window structure that contains the animation.
 * @return The current animation progress percentage.
 */
float gui_win_get_aniamtion_progress_percent(gui_win_t *win);

/**
 * @brief Set the scale rate for the window both horizontally and vertically.
 *
 * @param win Pointer to the window structure.
 * @param scale_rate_horizontal The horizontal scale rate.
 * @param scale_rate_vertical The vertical scale rate.
 */
void gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical);

/**
 * @brief Enable or disable the scope for the window.
 *
 * @param win Pointer to the window structure.
 * @param enable A boolean value to enable or disable the scope.
 */
void gui_win_set_scope(gui_win_t *win, bool enable);

/**
 * @brief Set the opacity value for the window.
 *
 * @param win Pointer to the window structure.
 * @param opacity_value The desired opacity value to set.
 */
void gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value);

/**
 * @brief Check if the animation is at its end frame.
 *
 * @param win Pointer to the window structure that contains the animation.
 * @return true if the end_frame is not 0, false otherwise.
 */
bool gui_win_is_animation_end_frame(gui_win_t *win);

/**
 * @brief Start the animation by setting the animate field to 1.
 *
 * @param win Pointer to the window structure that contains the animation.
 * If win or win->animate is NULL, the function will log an error message.
 */
void gui_win_start_animation(gui_win_t *win);

/**
 * @brief Stop the animation by setting the animate field to 0.
 *
 * @param win Pointer to the window structure that contains the animation.
 * If win or win->animate is NULL, the function will log an error message.
 */
void gui_win_stop_animation(gui_win_t *win);

#ifdef __cplusplus
}
#endif

#endif
