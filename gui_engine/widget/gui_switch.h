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

#include "guidef.h"
#include "gui_fb.h"
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
/* gui_switch_t start*/
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
    void *data;
    gui_animate_t *animate;             //!< can set animation
    IMG_SOURCE_MODE_TYPE src_mode;
    char style;
    uint32_t ifon : 1;
    uint32_t long_touch_state : 1;
    uint32_t long_touch_enable : 1;
    uint32_t press_flag : 1;
    uint32_t long_flag : 1;
    uint32_t release_flag : 1;
    uint32_t touch_disable : 1;
    uint32_t checksum : 8;
};
/* gui_switch_t end*/

_GUI_API_DEFINE(gui_switch_t)
void (*turn_off)(gui_switch_t *sw);

void (*turn_on)(gui_switch_t *sw);
void (*on_turn_on)(gui_switch_t *, gui_event_cb_t cb, void *p);
void (*on_turn_off)(gui_switch_t *, gui_event_cb_t cb, void *p);
void (*on_press)(gui_switch_t *, gui_event_cb_t event_cb, void *parameter);
void (*on_release)(gui_switch_t *, gui_event_cb_t event_cb, void *parameter);
void (*ctor)(gui_switch_t *, gui_obj_t *parent, int16_t x,
             int16_t y, int16_t w, int16_t h, void *off_pic, void *on_pic);
void (*animate)(gui_switch_t *,
                uint32_t      dur,
                int           repeat_count,
                void         *callback,
                void         *p);
void (*turn_off_no_event)(gui_switch_t *sw);
void (*turn_on_no_event)(gui_switch_t *sw);
_GUI_API_DECLARE(gui_switch_t)


typedef enum
{
    SWITCH_EVENT_TURN_ON     = GUI_EVENT_1,
    SWITCH_EVENT_TURN_OFF    = GUI_EVENT_2,
    SWITCH_EVENT_PRESSED     = GUI_EVENT_TOUCH_PRESSED,
    SWITCH_EVENT_RELEASED     = GUI_EVENT_TOUCH_RELEASED,

} T_GUI_SWITCH_EVENT;

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
#define SWITCH_HIGHLIGHT_ARRAY INT8_MAX



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
gui_switch_t *gui_switch_create(void    *parent,
                                int16_t  x,
                                int16_t  y,
                                int16_t  w,
                                int16_t  h,
                                void    *off_pic,
                                void    *on_pic);

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
gui_switch_t *gui_switch_create_from_fs(void    *parent,
                                        int16_t  x,
                                        int16_t  y,
                                        int16_t  w,
                                        int16_t  h,
                                        void    *off_pic,
                                        void    *on_pic);
/**
 * @brief create a switch widget, file source is memory address
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
gui_switch_t *gui_switch_create_from_ftl(void    *parent,
                                         int16_t  x,
                                         int16_t  y,
                                         int16_t  w,
                                         int16_t  h,
                                         void    *off_pic,
                                         void    *on_pic);

/**
 * @brief Turn on the switch, event will be triggered.
 *
 * @param this
 */
void gui_switch_turn_on(gui_switch_t *);

/**
 * @brief Turn off the switch, event will be triggered.
 *
 * @param this
 */
void gui_switch_turn_off(gui_switch_t *);

/**
 * @brief If the switch has been turned on somehow, it can upadte the status to on status.
 *
 * @param this
 */
void gui_switch_is_on(gui_switch_t *);

/**
 * @brief If the switch has been turned off somehow, it can upadte the status to off status.
 *
 * @param this
 */
void gui_switch_is_off(gui_switch_t *);

/**
 * @brief set sw state and change ui, NO event will be trigered.
 *
 * @param this
 * @param ifon set sw state
 */
void gui_switch_change_state(gui_switch_t *, bool ifon);
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
