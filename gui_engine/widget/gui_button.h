/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file  gui_button.h
  * @brief button widget
  * @details click to trigger
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/17
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
#ifndef __GUI_BUTTON_H__
#define __GUI_BUTTON_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_text.h"
#include "gui_img.h"
#include "gui_win.h"

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

/** @brief  press effect style */
typedef enum
{
    WIDGET_CLASSIC,                     //!< press to change picture to the highlighted
    WIDGET_FADE,                        //!< press to decrease picture's opacity
    WIDGET_SCALE_FADE,                  //!< press to decrease picture's opacity and size
    WIDGET_SCALE,                       //!< press to decrease picture's  size
} T_GUI_BUTTON_STYLE;

/** @brief  define button structure */
typedef struct t_gui_button
{
    gui_obj_t base;                     //!< base structure
    gui_img_t *img;                     //!< the image can show press and release effect
    gui_text_t *text;                   //!< the text
    bool press_flag;                    //!< press to change picture to the highlighted
    bool long_flag;
    bool release_flag;
    void *on_pic_addr;
    void *off_pic_addr;
    void *data;
    char style;
    gui_animate_t *animate;             //!< can set animation
    IMG_SOURCE_MODE_TYPE src_mode;
    bool flag;
} T_GUI_BUTTON;

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
#define BUTTON_COLOR 0xffffffff //!< default color
#define BUTTON_HLCOLOR 0x0bffff //!< default highlight color

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
 * @brief Creat a button widget from memory, images are loaded from memory address.
 *
 * @param parent The father widget which the button nested in.
 * @param x The X-axis relative coordinate of the button(left).
 * @param y The Y-axis v coordinate of the button(up).
 * @param w The width of the button(response area).
 * @param h The hight of the button(response area).
 * @param background_pic The image shown when button was not pressed.
 * @param highlight_pic The image shown when button was pressed.
 * @param text The text of the button which always shown.
 * @param image_type shoule be 0.
 * @param count shoule be 0.
 * @param src_mode imgae source form memory address(0) or filesystem(1)
 * @return Return the widget object pointer
 * <b>Example usage</b>
 * \code{.c}
 * T_GUI_BUTTON *button = gui_button_create(parent, 0, 0, 100, 100, img1, img2, text, 0, 0, 0);
 * \endcode
 */
T_GUI_BUTTON *gui_button_create(void                 *parent,
                                int16_t               x,
                                int16_t               y,
                                int16_t               w,
                                int16_t               h,
                                void                 *background_pic,
                                void                 *highlight_pic,
                                char                 *text,
                                T_IMG_SOURCE_IMG_TYPE image_type,
                                int count,
                                IMG_SOURCE_MODE_TYPE  src_mode);

/**
 * @brief please use gui_obj_add_event_cb to set gesture trigger.
 *
 * @param this
 * @param event_cb
 */
void gui_button_click(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief please use gui_obj_add_event_cb to set gesture trigger.
 *
 * @param this
 * @param event_cb
 * @param parameter
 */
void gui_button_press(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief please use gui_obj_add_event_cb to set gesture trigger.
 *
 * @param this
 * @param event_cb
 * @param parameter
 */
void gui_button_long(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief please use gui_obj_add_event_cb to set gesture trigger.
 *
 * @param this
 * @param event_cb
 * @param parameter
 */
void gui_button_release(T_GUI_BUTTON *this, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief Set the color of text which belongs to a button.
 *
 * @param this The pointer of this button widget.
 * @param color The color of text.
 */
void T_GUI_BUTTONext_color(T_GUI_BUTTON *this, gui_color_t color);

/**
 * @brief Move the img of this button.
 *
 * @param this The pointer of this button widget.
 * @param img_x The X value of img.
 * @param img_y The Y value of img.
 */
void gui_button_img_move(T_GUI_BUTTON *this, int16_t img_x, int16_t img_y);

/**
 * @brief
 *
 * @param this Move the text of this button.
 * @param text_x  The X relative coordinates value of text(left).
 * @param text_y The y relative coordinates value of text(up).
 */
void T_GUI_BUTTONext_move(T_GUI_BUTTON *this, int16_t text_x, int16_t text_y);

/**
 * @brief
 *
 * @param o
 * @param dur
 * @param repeatCount
 * @param callback
 * @param p
 */
void gui_button_set_animate(T_GUI_BUTTON *this,
                            uint32_t      dur,
                            int           repeatCount,
                            void         *callback,
                            void         *p);

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */

#endif
