/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_switch.h
  * @brief on or off status. Click to change.
  * @details listen to on and off gesture
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
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
#ifndef __GUI_SWITCH_H__
#define __GUI_SWITCH_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <guidef.h>
#include <gui_fb.h>
#include "gui_img.h"

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

typedef struct gui_switch gui_switch_t;
struct gui_switch
{
    gui_obj_t base;
    gui_img_t *switch_picture;
    void *on_pic_addr;
    void *off_pic_addr;
    void *on_hl_pic_addr;
    void *off_hl_pic_addr;
    void *long_touch_state_pic_addr;
    void *long_touch_state_hl_pic_addr;
    bool ifon;
    bool long_touch_state;
    bool long_touch_enable;
    bool press_flag;
    bool long_flag;
    bool release_flag;
    bool touch_disable;
    void (*turn_off)(gui_switch_t *sw);
    void (*turn_on)(gui_switch_t *sw);
    void (*onOn)(gui_switch_t *this, void *cb, void *p);
    void (*onOff)(gui_switch_t *this, void *cb, void *p);
    void (*ctor)(gui_switch_t *this, gui_obj_t *parent,
                 int16_t x,
                 int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
    bool flg_fs;
} ;


/** End of WIDGET_Exported_Types
  * @}
  */

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
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */



/**
 * @brief create a switch widget, file source is memory address.
 * @param parent the father widget it nested in.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param off_pic off status image memory address.
 * @param on_pic on status image memory address.
 * @return return the widget object pointer.
 *
 */
gui_switch_t *gui_switch_create(void *parent, int16_t x, int16_t y,
                                int16_t w, int16_t h, void *off_pic, void *on_pic);

/**
 * @brief create a switch widget, file source is filesystem.
 * @param parent the father widget it nested in.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param off_pic off status image filepath.
 * @param on_pic on status image filepath.
 * @return return the widget object pointer.
 *
 */
gui_switch_t *gui_switch_create_from_fs(void *parent, int16_t x, int16_t y,
                                        int16_t w, int16_t h, void *off_pic, void *on_pic);
/**
 * @brief Turn on the switch
 *
 * @param this
 */
void gui_switch_turn_on(gui_switch_t *this);
/**
 * @brief Turn off the switch
 *
 * @param this
 */
void gui_switch_turn_off(gui_switch_t *this);
/**
 * @brief If the switch has been turned on somehow, it can upadte the status to on status.
 *
 * @param this
 */
void gui_switch_is_on(gui_switch_t *this);

/**
 * @brief If the switch has been turned off somehow, it can upadte the status to off status.
 *
 * @param this
 */
void gui_switch_is_off(gui_switch_t *this);


/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif



