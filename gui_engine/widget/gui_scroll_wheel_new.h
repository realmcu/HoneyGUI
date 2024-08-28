/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_wheel_new.h
  * @brief scroll wheel effect
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_SCROLL_WHEEL_NEW_H__
#define __GUI_SCROLL_WHEEL_NEW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_win.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */
#define GUI_SCROLL_WHEEL_NEW_RENDER_TEXT 1
#define GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY 2


/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */
struct gui_text_image_map
{
    const char *text;
    void *image_data;
};
typedef struct gui_scroll_wheel_new
{
    gui_win_t base;
    unsigned char render_mode, alien, render, end_speed;
    const char **string_array;
    int16_t string_array_size;
    int16_t gap;
    int16_t count;
    int16_t touch_y, speed, text_image_map_length;
    gui_win_t *win;
    int16_t recode[5];
    const struct gui_text_image_map *text_image_map;

} gui_scroll_wheel_new_t;


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
 * @brief Create a new scroll wheel widget.
 *
 * This function initializes a new scroll wheel widget and returns a pointer
 * to the created widget. It sets up the widget with the specified parent,
 * position, row gap, and text strings for each row.
 *
 * @param[in]  parent            Pointer to the parent object.
 * @param[in]  x                 The X coordinate of the widget.
 * @param[in]  y                 The Y coordinate of the widget.
 * @param[in]  w                 The width of the widget.
 * @param[in]  row_gap           The gap between each row in the scroll wheel.
 * @param[in]  row_count         The number of rows in the scroll wheel.
 * @param[in]  string_array      Array of strings to display in the scroll wheel.
 * @param[in]  string_array_size The size of the string array.
 * @return     gui_scroll_wheel_new_t* Pointer to the created scroll wheel widget.
 */
gui_scroll_wheel_new_t *gui_scroll_wheel_new_create(void     *parent,
                                                    int16_t          x,
                                                    int16_t          y,
                                                    int16_t          w,
                                                    int16_t          row_gap,
                                                    int16_t          row_count,
                                                    const char       **string_array,
                                                    int16_t          string_array_size);
/**
 * @brief Render text on the scroll wheel widget.
 *
 * This function renders text for each row of the scroll wheel widget using the
 * specified font and font size.
 *
 * @param[in]  widget             Pointer to the scroll wheel widget.
 * @param[in]  font_file_pointer  Pointer to the font file to use for rendering text.
 * @param[in]  font_size          Size of the font to use for rendering text.
 */
void gui_scroll_wheel_new_render_text(gui_scroll_wheel_new_t *widget, const void *font_file_pointer,
                                      int font_size);
/**
 * @brief Render images on the scroll wheel widget.
 *
 * This function renders a series of images on the provided scroll wheel widget.
 * The images and their corresponding text mappings are provided in a gui_text_image_map array.
 *
 * @param[in]  widget      Pointer to the scroll wheel widget.
 * @param[in]  map         Array of text-image mappings to render.
 * @param[in]  map_length  Length of the text-image mappings array.
 */
void gui_scroll_wheel_new_render_image_array(gui_scroll_wheel_new_t *widget,
                                             const struct gui_text_image_map *map, int map_length);
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
