/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_menu_cellular.h
  * @brief honeycomb menu effect.
  * @details Slide to extend and retract menu_cellulars
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

/** @brief  menu_cellular structure */
typedef struct gui_menu_cellular
{
    gui_obj_t base;
    int16_t ver_speed;
    int16_t ver_record[5];
    int16_t ver_hold;
    int16_t hor_hold;
    int16_t ver_offset;     //!< vertical offset
    int16_t hor_offset;     //!< horizontal offset
    int16_t ver_offset_min; //!< minimum vertical offset
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
 * @brief Create a honeycomb effect widget,
 *
 * @param parent the father widget nested in
 * @param icon_size icon's width(px)
 * @param icon_array icons set's file address array
 * @param array_size the array's size
 * @return gui_menu_cellular_t*
 */
gui_menu_cellular_t *gui_menu_cellular_create(void     *parent,
                                              int       icon_size,
                                              uint32_t *icon_array[],
                                              int       array_size);
/**
 * @brief config offset
 *
 * @param menu_cellular the menu_cellular pointer
 * @param offset_x  offset_x
 * @param offset_y  offset_y
 */
void gui_menu_cellular_offset(gui_menu_cellular_t *menu_cellular, int offset_x, int offset_y);

/**
 * @brief Add click events on images based on the cellular menu.
 *
 * @param menu_cellular Pointer to the cellular menu structure.
 * @param para_array Array of gesture parameters corresponding to the click event.
 * @param array_length Length of the gesture parameters array.
 */
void gui_menu_cellular_on_click(gui_menu_cellular_t *menu_cellular,
                                struct gui_menu_cellular_gesture_parameter *para_array, int array_length);
#ifdef __cplusplus
}
#endif

#endif
