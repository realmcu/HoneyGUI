/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtainview.h
  * @brief curtain effect container widget, which can nest curtains.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
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
#ifndef __GUI_CURTAINVIEW_H__
#define __GUI_CURTAINVIEW_H__
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

typedef struct
{
    int32_t x;
    int32_t y;
} gui_curtain_ext_id_t;

typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} gui_curtain_bool_t;

typedef struct
{
    float up;
    float down;
    float left;
    float right;
} gui_curtain_float_t;

/* T_GUI_CURTAIN_ENUM start*/
typedef enum
{
    CURTAIN_UNDEFINED,
    CURTAIN_UP,
    CURTAIN_DOWN,
    CURTAIN_LEFT,
    CURTAIN_RIGHT,
    CURTAIN_MIDDLE,
} T_GUI_CURTAIN_ENUM;
/* T_GUI_CURTAIN_ENUM end*/

/** @brief  curtainview structure */
typedef struct gui_curtainview
{
    gui_obj_t base;
    uint16_t curtain_cnt;
    uint16_t style;
    T_GUI_CURTAIN_ENUM cur_curtain;
    gui_curtain_float_t scopes;
    gui_curtain_bool_t orientations;
    float scope_up;
    float scope_down;
    float scope_left;
    float scope_right;
    bool down_flag;
    bool left_flag;
    bool init_flag;
    bool release_flag;
    int spring_value;
    void (*done_cb)(struct gui_curtainview *_this);

    uint16_t has_center_curtain          : 1;
    uint16_t has_left_curtain            : 1;
    uint16_t has_right_curtain           : 1;
    uint16_t has_up_curtain              : 1;
    uint16_t has_down_curtain            : 1;
    uint16_t cur_curtain_flag            : 1;
    uint16_t middle_flag                 : 1;
    uint16_t middle_flag_left            : 1;
    uint16_t middle_flag_up              : 1;
    uint16_t up_flag                     : 1;

    uint8_t checksum;
    int release_y;
    int release_x;
} gui_curtainview_t;

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
 * @brief Create a curtainview widget, which can nest curtains.
 * @param parent the father widget it nested in.
 * @param filename this curtainview widget's name.
 * @param x the X-axis coordinate relative to parent widget.
 * @param y the Y-axis coordinate relative to parent widget.
 * @param w width.
 * @param h height.
 * @return return the widget object pointer.
 *
 */
gui_curtainview_t *gui_curtainview_create(void       *parent,
                                          const char *name,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h);

/**
 * @brief Curtain expanding completed event
 *
 * @param this widget pointer
 * @param cb event callback
 */
void gui_curtainview_set_done_cb(gui_curtainview_t *_this, void (*cb)(gui_curtainview_t *_this));


#ifdef __cplusplus
}
#endif

#endif
