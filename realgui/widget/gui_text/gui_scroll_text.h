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
#include <string.h>
#include "guidef.h"
#include "gui_text.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  card structure */
typedef struct gui_scroll_text
{
    gui_text_t base;
    uint32_t start_value;
    uint32_t end_value;
    uint32_t cnt_value;
    uint32_t init_time_ms;
    uint32_t cur_time_ms;
    uint32_t duration_time_ms;
    uint32_t interval_time_ms;
    bool scrolling;
} gui_scroll_text_t;


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
 * @brief Creat a scroll text widget.
 * @note Scroll text widget can show horizontal and vertical scroll text.
 * @note If the width of the text is less than the width of the text box, the text will stand still.
 * @note Scroll and text properties need to be set by gui_scroll_text_scroll_set and gui_scroll_text_set.
 * @param parent The father widget which the scroll text nested in.
 * @param name The widget's name.
 * @param x The X-axis coordinate of the text box.
 * @param x The Y-axis coordinate of the text box.
 * @param w The width of the text box.
 * @param h The hight of the text box.
 * @return Return the widget object pointer: gui_scroll_text_t*
 *
 */
gui_scroll_text_t *gui_scroll_text_create(void       *parent,
                                          const char *name,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h);

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
void gui_scroll_text_scroll_set(gui_scroll_text_t *_this,
                                TEXT_MODE          mode,
                                uint32_t           start_value,
                                uint32_t           end_value,
                                uint32_t           interval_time_ms,
                                uint32_t           duration_time_ms);

/**
* @brief set the string in a scroll text box widget.
* @note
* @param this the scroll text box widget pointer.
* @param text the scroll text string.
* @param text_type text type.
* @param color the text's color.
* @param length the text string's length.
* @param font_size the text string's font size.
* @return void
*
*/
void gui_scroll_text_set(gui_scroll_text_t *_this,
                         void              *text,
                         FONT_SRC_TYPE      text_type,
                         gui_color_t        color,
                         uint16_t           length,
                         uint8_t            font_size);

/** * @brief set font encoding
 * @note utf-8 or unicode
 * @param this the scroll text box widget pointer
 * @param encoding_type encoding_type
 */
void gui_scroll_text_encoding_set(gui_scroll_text_t *_this, TEXT_CHARSET charset);

/**
 * @brief set font type
 * @note The type must match the font size!
 * @param this the scroll text widget pointer
 * @param font_source the addr of .ttf or .bin
 * @param font_mode font source mode
 */
void gui_scroll_text_type_set(gui_scroll_text_t *_this, void *font_source, FONT_SRC_MODE font_mode);

/**
 * @brief set content of text widget
 *
 * @param this the scroll text widget pointer
 * @param text the text string
 * @param length the text string's length
 */
void gui_scroll_text_content_set(gui_scroll_text_t *_this, void *text, uint16_t length);

/**
 * @brief set scroll text skip frame
 *
 * @param skip_frame The number of frames that need to be skipped.
 * If the value is 2, then scrolling text will skip two frames and scroll one frame.
 */
void gui_scroll_text_skip_frame_set(uint8_t skip_frame);

/**
 * @brief Reset scrolling effects
 *
 * @param this the scroll text widget pointer
 */
void gui_scroll_text_reset(gui_scroll_text_t *_this);

/**
 * @brief Stop scrolling effects
 *
 * @param _this the scroll text widget pointer
 */
void gui_scroll_text_stop(gui_scroll_text_t *_this);

/**
 * @brief Pause scrolling effects
 *
 * @param _this the scroll text widget pointer
 */
void gui_scroll_text_pause(gui_scroll_text_t *_this);

/**
 * @brief Resume scrolling effects
 *
 * @param _this the scroll text widget pointer
 */
void gui_scroll_text_resume(gui_scroll_text_t *_this);

/**
 * @brief Start scrolling effects
 *
 * @param _this the scroll text widget pointer
 */
void gui_scroll_text_start(gui_scroll_text_t *_this);

#ifdef __cplusplus
}
#endif

#endif
