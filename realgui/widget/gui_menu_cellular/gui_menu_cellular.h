/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_menu_cellular.h
  * @brief Honeygui menu effect.
  * @details Slide to extend and retract menu_cellulars.
  * @author shel_deng@realsil.com.cn
  * @date 2025/7/2
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_MENU_CELLULAR_H__
#define __GUI_MENU_CELLULAR_H__
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

/** @brief  Menu_cellular structure. */
typedef struct gui_menu_cellular
{
    gui_obj_t base;
    int16_t ver_speed;
    int16_t ver_record[5];
    int16_t ver_hold;
    int16_t hor_hold;
    int16_t ver_offset;     //!< Vertical offset.
    int16_t hor_offset;     //!< Horizontal offset.
    int16_t ver_offset_min; //!< Minimum vertical offset.
    int16_t icon_size;
} gui_menu_cellular_t;

struct gui_menu_cellular_gesture_parameter
{
    gui_event_cb_t callback_function;
    void *parameter;
};

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
 * @brief Create a menu_cellular widget.
 * @param parent The father widget it nested in.
 * @param icon_size Size of icon.
 * @param icon_array The icons' image data address array.
 * @param array_size Size of array.
 * @return The menu_cellular pointer.
 */
gui_menu_cellular_t *gui_menu_cellular_create(void     *parent,
                                              int       icon_size,
                                              uint32_t *icon_array[],
                                              int       array_size);
/**
 * @brief Config offset.
 * @param menu_cellular The menu_cellular pointer.
 * @param offset_x The X-axis offset.
 * @param offset_y The Y-axis offset.
 */
void gui_menu_cellular_offset(gui_menu_cellular_t *menu_cellular, int offset_x, int offset_y);

/**
 * @brief Add click events on images based on the cellular menu.
 * @param menu_cellular The menu_cellular pointer.
 * @param para_array Array of gesture parameters corresponding to the click event.
 * @param array_length Length of the gesture parameters array.
 */
void gui_menu_cellular_on_click(gui_menu_cellular_t *menu_cellular,
                                struct gui_menu_cellular_gesture_parameter *para_array, int array_length);
#ifdef __cplusplus
}
#endif

#endif
