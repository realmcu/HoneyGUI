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
#include <guidef.h>
#include <gui_fb.h>


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
typedef enum CURTAIN_ORIENTATION
{
    CURTAIN_UNDEFINED,
    CURTAIN_UP,
    CURTAIN_DOWN,
    CURTAIN_LEFT,
    CURTAIN_RIGHT,
    CURTAIN_MIDDLE,
} gui_curtain_enum_t;
/** @brief  curtainview structure */
typedef struct gui_curtainview
{
    gui_obj_t base;
    uint16_t curtain_cnt;
    uint16_t style;
    gui_curtain_enum_t cur_curtain;
    gui_curtain_float_t scopes;
    gui_curtain_bool_t orientations;
    float scopeup;
    float scopedown;
    float scopeleft;
    float scoperight;
    bool down_flag;
    bool left_flag;
    bool init_flag;
    bool release_flag;
    int spring_value;
    void (*done_cb)(struct gui_curtainview *this);
    void (*ctor)(struct gui_curtainview *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
    bool mute;
    int16_t release_y;
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
 * @param scope the scope of every curtain.
 * @return return the widget object pointer.
 *
 */
gui_curtainview_t *gui_curtainview_create(void *parent, const char *filename, int16_t x,
                                          int16_t y,
                                          int16_t w, int16_t h);
/**
 * @brief Curtain expanding completed event
 *
 * @param this widget pointer
 * @param cb event callback
 */
void gui_curtainview_set_done_cb(gui_curtainview_t *this, void (*cb)(gui_curtainview_t *this));



#ifdef __cplusplus
}
#endif

#endif



