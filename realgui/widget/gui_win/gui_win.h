/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  window structure */
typedef struct gui_win
{
    gui_obj_t base;
    float compensate_x;              /* Compensate in X direction for tp event judgment. */
    float compensate_y;              /* Compensate in Y direction for tp event judgment. */
    float scale;
    float scale_y;
    bool press_flag;
    bool long_flag;
    bool release_flag;
    bool enter_auto_scale;
    uint8_t checksum;
    bool scope;
    bool hold_tp;
    void *blur_param;
    uint8_t blur_degree;
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
 * @brief Create a window widget.
 * @param parent Father widget the window nested in.
 * @param name Window widget name.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 * @param w Width.
 * @param h Height.
 * @return Widget object pointer.
 *
 */
gui_win_t *gui_win_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);

/**
 * @brief Register a callback function for the left slide event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_left(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief Register a callback function for the right slide event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_right(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief Register a callback function for the up slide event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_up(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief Register a callback function for the down slide event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_down(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief Register a callback function for the press event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_press(gui_win_t *_this, gui_event_cb_t callback, void *parameter);

/**
 * @brief Register a callback function for the release event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func (void *obj, gui_event_t e, void *param).
 * @param parameter Callback parameter.
 */
void gui_win_release(gui_win_t *_this, gui_event_cb_t callback, void *parameter);

/**
 * @brief Register a callback function for a long press event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func.
 * @param parameter Callback parameter.
 */
void gui_win_long(gui_win_t *_this, void *callback, void *parameter);

/**
 * @brief Register a callback function for a click event of the win widget.
 *
 * @param _this Win pointer.
 * @param callback Callback func (void *obj, gui_event_t e, void *param).
 * @param parameter Callback parameter.
 */
void gui_win_click(gui_win_t *_this, gui_event_cb_t callback, void *parameter);

/**
 * @brief Set the hold_tp state.
 *
 * @param _this Win pointer.
 * @param hold_tp Boolean value to set the state to true or false.
 */
void gui_win_hold_tp(gui_win_t *_this, bool hold_tp);

/**
 * @brief Set the scale rate for the window both horizontally and vertically.
 *
 * @param win Pointer to the window structure.
 * @param scale_rate_horizontal Horizontal scale rate.
 * @param scale_rate_vertical Vertical scale rate.
 */
void gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical);

/**
 * @brief Enable or disable the scope for the window.
 *
 * @param win Pointer to the window structure.
 * @param enable Boolean value to enable or disable the scope.
 */
void gui_win_set_scope(gui_win_t *win, bool enable);

/**
 * @brief Set the opacity value for the window.
 *
 * @param win Pointer to the window structure.
 * @param opacity_value Desired opacity value to set.
 */
void gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value);

/**
 * @brief Window widget prepare.
 *
 * @param obj Pointer.
 */
void gui_win_prepare(gui_obj_t *obj);

/**
 * @brief Move the GUI window to the specified coordinates.
 *
 * This function repositions a GUI window to the specified (x, y) coordinates on the screen.
 *
 * @param win Pointer to the GUI window that needs to be moved.
 * @param x New x-coordinate for the window.
 * @param y New y-coordinate for the window.
 */
void gui_win_move(gui_win_t *win, int x, int y);

/**
 * @brief Get the current x-coordinate of the GUI window.
 *
 * This function returns the current x-coordinate of the specified GUI window.
 *
 * @param win Pointer to the GUI window.
 * @return Current x-coordinate of the window.
 */
int gui_win_get_x(gui_win_t *win);

/**
 * @brief Get the current y-coordinate of the GUI window.
 *
 * This function returns the current y-coordinate of the specified GUI window.
 *
 * @param win Pointer to the GUI window.
 * @return Current y-coordinate of the window.
 */
int gui_win_get_y(gui_win_t *win);

/**
 * @brief Compensate the GUI window's touch event coordinates.
 * @param win Pointer to the GUI window.
 * @param x X-coordinate to compensate.
 * @param y Y-coordinate to compensate.
 */
void gui_win_compensate(gui_win_t *win, int x, int y);

/**
 * @brief Enable GUI window's blur effect.
 * @param win Pointer to the GUI window.
 * @param enable True to enable blur effect, false to disable.
 */
void gui_win_enable_blur(gui_win_t *win, bool enable);

/**
 * @brief Set the blur degree for the GUI window.
 * @param win Pointer to the GUI window.
 * @param degree Blur degree value (0-255) and default value is 225.
 */
void gui_win_set_blur_degree(gui_win_t *win, uint8_t degree);

#ifdef __cplusplus
}
#endif

#endif
