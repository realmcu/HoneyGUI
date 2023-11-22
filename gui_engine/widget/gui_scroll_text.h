/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_text.c
  * @brief scroll text widget
  * @details scroll text widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/11/08
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_SCROLL_TEXT_H__
#define __GUI_SCROLL_TEXT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <draw_font.h>
#include <gui_text.h>

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

/** @brief  card structure */
typedef struct gui_scroll_text
{
    gui_text_t base;
    uint8_t start_value;
    uint8_t end_value;
    uint32_t cnt_value;
    uint32_t init_time_ms;
    uint32_t duration_time_ms;
    uint32_t interval_time_ms;
} gui_scroll_text_t;


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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief Creat a scroll text widget.
 * @note Scroll text widget can show horizontal and vertical scroll text.
 * @note If the width of the text is less than the width of the text box, the text will stand still.
 * @note Scroll and text properties need to be set by gui_scrolltext_scroll_set and gui_scrolltext_text_set.
 * @param parent The father widget which the scroll text nested in.
 * @param name The widget's name.
 * @param x The X-axis coordinate of the text box.
 * @param x The Y-axis coordinate of the text box.
 * @param w The width of the text box.
 * @param h The hight of the text box.
 * @return Return the widget object pointer: gui_scroll_text_t*
 *
 */
gui_scroll_text_t *gui_scrolltext_create(void *parent, const char *name, int16_t x,
                                         int16_t y, int16_t w, int16_t h);

/**
 * @brief Set the text scroll property of the scroll text box.
 *
 * @param this the scroll text widget pointer.
 * @param mode the mode of this scroll text widget, SCROLL_X or SCROLL_Y.
 * @param start_value the number of pixels to the left of the text in the first frame
 * @param end_value the number of pixels to the right of the text at the last frame.
 * @param interval_time_ms the time for one loop.
 * @param duration_time_ms the total time that text scrolling lasts.
 * @return void
 *
 */
void gui_scrolltext_scroll_set(gui_scroll_text_t *this, TEXT_MODE mode, uint8_t start_value,
                               uint8_t end_value,
                               uint32_t interval_time_ms, uint32_t duration_time_ms);

/**
* @brief set the string in a scroll text box widget.
* @note
* @param this the scroll text box widget pointer.
* @param text the scroll text string.
* @param font_type the font's name.
* @param color the text's color.
* @param length the text string's length.
* @param font_size the text string's font size.
* @return void
*
*/
void gui_scrolltext_text_set(gui_scroll_text_t *this, const char *text, char *text_type,
                             uint32_t color,
                             uint16_t length, uint8_t font_size);

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
