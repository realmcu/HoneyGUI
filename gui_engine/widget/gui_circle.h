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
typedef struct gui_circle
{
    gui_obj_t base;
    gui_color_t fill;
    uint8_t checksum;
    uint8_t render;
} gui_circle_t;
typedef struct gui_sector
{
    gui_circle_t base;
    float start_angle;
    float end_angle;
} gui_sector_t;
typedef struct gui_rounded_rect
{
    gui_circle_t base;
    int32_t radius;
} gui_rounded_rect_t;
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
 * @brief Create a circle GUI object.
 *
 * This function creates a circle GUI object as a child of the specified parent object.
 *
 * @param parent Pointer to the parent GUI object.
 * @param r Radius of the circle.
 * @param cx X-coordinate of the circle center.
 * @param cy Y-coordinate of the circle center.
 * @param fill_color Fill color of the circle.
 * @return Pointer to the created circle GUI object.
 */
gui_circle_t *gui_circle_create(gui_obj_t *parent, int r, int cx, int cy, gui_color_t fill_color);

/**
 * @brief Create a sector GUI object.
 *
 * This function creates a sector (pie slice) GUI object as a child of the specified parent object.
 *
 * @param parent Pointer to the parent GUI object.
 * @param r Radius of the sector.
 * @param cx X-coordinate of the sector center.
 * @param cy Y-coordinate of the sector center.
 * @param fill_color Fill color of the sector.
 * @param start_angle Starting angle of the sector in radians.
 * @param end_angle End angle of the sector in radians.
 * @return Pointer to the created sector GUI object.
 */
gui_sector_t *gui_sector_create(gui_obj_t *parent, int r, int cx, int cy, gui_color_t fill_color,
                                float start_angle, float end_angle);

/**
 * @brief Create a rounded rectangle GUI object.
 *
 * This function creates a rounded rectangle GUI object as a child of the specified parent object.
 *
 * @param parent Pointer to the parent GUI object.
 * @param x X-coordinate of the top-left corner of the rectangle.
 * @param y Y-coordinate of the top-left corner of the rectangle.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param radius Radius of the rounded corners.
 * @param fill_color Fill color of the rounded rectangle.
 * @return Pointer to the created rounded rectangle GUI object.
 */
gui_rounded_rect_t *gui_rounded_rect_create(gui_obj_t *parent, int x, int y, int w, int h,
                                            int radius, gui_color_t fill_color);

#ifdef __cplusplus
}
#endif

#endif
