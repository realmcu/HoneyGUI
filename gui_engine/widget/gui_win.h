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
#include <guidef.h>
#include <gui_fb.h>
#include "gui_curtainview.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

/** @brief  window structure */
typedef struct gui_win
{
    gui_obj_t base;
    void (*ctor)(struct gui_win *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
    gui_animate_t *animate;
    bool press_flag;
    bool long_flag;
    bool release_flag;
} gui_win_t;
/** End of WIDGET_Exported_Types
  * @}
  */
typedef struct gui_api_win
{
    void (*set_animate)(gui_win_t *b, uint32_t dur, int repeatCount, void *callback, void *p);
    void (*onPress)(gui_win_t *b, void *callback, void *parameter);
    void (*onRelease)(gui_win_t *b, void *callback, void *parameter);
    void (*onLong)(gui_win_t *b, void *callback, void *parameter);
    void (*onClick)(gui_win_t *b, void *callback, void *parameter);
    void (*onLeft)(gui_win_t *b, void *callback, void *parameter);
    void (*onRight)(gui_win_t *b, void *callback, void *parameter);
    void (*onUp)(gui_win_t *b, void *callback, void *parameter);
    void (*onDown)(gui_win_t *b, void *callback, void *parameter);
} gui_api_win_t;
extern gui_api_win_t gui_win_api;
/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */





/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

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
gui_win_t *gui_win_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h);

/**
 * @brief
 *
 * @param o widget object pointer
 * @param dur Animation duration
 * @param repeatCount Repeat play times, -1 means play on repeat forever
 * @param callback animate frame callback
 * @param p parameter
 */
void gui_win_set_animate(gui_win_t *o, uint32_t dur, int repeatCount, void *callback, void *p);



/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif



