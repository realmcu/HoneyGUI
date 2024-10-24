/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_calendar.h
  * @brief calendar widget
  * @details A calendar container widget
  * @author triton_yu@realsil.com.cn
  * @date 2024/06/03
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
#ifndef __GUI_CALENDAR_H__
#define __GUI_CALENDAR_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_tab.h"
/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  calendar structure */
typedef struct gui_calendar
{
    gui_obj_t base;
    uint16_t cellWidth ;
    uint16_t cellHeight;
    int currentDay, currentMonth, currentYear;
    char buffer[32];
    uint16_t margin;
    uint16_t headerHeight;
    gui_text_t *title;
    void *font_source;
    uint16_t font_size;
    gui_color_t color;
    gui_color_t font_color_highlight;
    gui_color_t item_color;
    gui_color_t item_color_highlight;
    gui_color_t title_color;
    gui_color_t title_background_color;
    gui_tab_t *tabs[3];
    int months_show;
    int years_show;
} gui_calendar_t;
_GUI_API_DEFINE(gui_calendar_t)
_GUI_API_DECLARE(gui_calendar_t)
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
 * @brief Create a calendar GUI component.
 *
 * This function initializes a calendar component within the specified parent GUI object.
 *
 * @param parent The parent GUI object that will contain the calendar.
 * @param x The x coordinate of the top left corner of the calendar.
 * @param y The y coordinate of the top left corner of the calendar.
 * @param w The width of the calendar.
 * @param h The height of the calendar.
 * @param font_source_p A pointer to the font to be used in the calendar.
 * @param font_size_p The size of the font to be used in the calendar.
 * @param color_p The date color of the calendar.
 * @param font_color_highlight The color to use for highlighting date text in the calendar.
 * @param item_color The color of the calendar items (dates background).
 * @param item_color_highlight The color to use for highlighting calendar items.
 * @param title_color The color of the calendar title text.
 * @param title_background_color The background color of the calendar title.
 */
void gui_calender_create(gui_obj_t *parent, int x, int y, int w, int h, void *font_source_p,
                         int font_size_p, gui_color_t color_p, gui_color_t font_color_highlight,
                         gui_color_t item_color,
                         gui_color_t item_color_highlight,
                         gui_color_t title_color,
                         gui_color_t title_background_color);




#ifdef __cplusplus
}
#endif

#endif
