/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_slider.h
  * @brief slider design
  * @details slider widget
  * @author sienna_shen@realsil.com.cn
  * @date 2024/8/20
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
#ifndef __GUI_SLIDER_H__
#define __GUI_SLIDER_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_text.h"
#include "math.h"

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

typedef struct gui_slider
{
    gui_obj_t base;
    int16_t x, y;
    int16_t deltax, deltay;
    int16_t w, h;
    int16_t slider_size;
    int16_t text_size;
    uint16_t minValue;
    uint16_t maxValue;
    uint16_t preValue;
    uint16_t currentValue;
    bool dragging;
    int16_t sliderX;
    int16_t sliderY;
    gui_img_t *background_image;
    gui_img_t *slider_image;
    void *background_image_addr;
    void *slider_image_addr;
} gui_slider_t;

_GUI_API_DEFINE(gui_slider_t)
void (*on_change)(gui_slider_t *this, gui_event_cb_t event_cb, void *parameter);
uint16_t (*get_currentValue)(gui_slider_t *this);
_GUI_API_DECLARE(gui_slider_t)

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
 * @brief Create a slider widget.
 *
 * @param parent the father widget the page nested in.
 * @param bg_img the background image of the slider widget.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param minValue minimum value of slider range.
 * @param maxValue maximum value of slider range.
 * @param slider_img the slider image of the slider widget.
 * @param currentValue the current value selected by the slider.
 * @param slider_size the size of slider.
 * @return gui_slider_t*
 */
gui_slider_t *gui_slider_create(void          *parent,
                                void          *bg_img,
                                int16_t        x,
                                int16_t        y,
                                int16_t        w,
                                int16_t        h,
                                uint16_t       minValue,
                                uint16_t       maxValue,
                                void          *slider_img,
                                uint16_t       currentValue,
                                int16_t        slider_size);

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
