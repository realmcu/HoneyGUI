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
                          const char *filename,
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
void gui_win_set_animate(gui_win_t *this,
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
void gui_win_left(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_right(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_up(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_down(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_press(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_release(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_long(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param b
 * @param callback
 * @param parameter
 */
void gui_win_click(gui_win_t *this, void *callback, void *parameter);

/**
 * @brief
 *
 * @param this
 * @param hold_tp
 */
void gui_win_hole_tp(gui_win_t *this, bool hold_tp);

#ifdef __cplusplus
}
#endif

#endif
