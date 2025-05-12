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

/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_SCROLL_WHEEL_NEW_RENDER_TEXT 1
#define GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY 2

/*============================================================================*
 *                         Types
 *============================================================================*/

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
    int16_t touch_y, touch_y_old, speed, text_image_map_length, col_offset, index_offset, blank_count,
            time_array_offset;
    gui_win_t *win;
    gui_win_t *win2;
    int16_t recode[5];
    const struct gui_text_image_map *text_image_map;
    bool wheel_take_over;
    bool take_over;
    bool loop;
    int history_y;
    gui_color_t font_color;
    gui_color_t font_color_highlight;
} gui_scroll_wheel_new_t;


//_GUI_API_DEFINE(gui_scroll_wheel_new_t)
typedef struct __gui_api_gui_scroll_wheel_new_t
{
    /**
     * @brief Get the current index of the scroll wheel widget.
     *
     * This function pointer should point to a function that returns the current index
     * of the scroll wheel widget. The index typically represents the currently
     * selected item in the scroll wheel.
     *
     * @param widget Pointer to the scroll wheel widget.
     *
     * @return The current index of the scroll wheel widget.
     */
    int (*get_index)(gui_scroll_wheel_new_t *widget);

    /**
     * @brief Set the current index for the scroll wheel widget.
     *
     * This function pointer should point to a function that sets the current index
     * of the scroll wheel widget. The index typically represents the item to be
     * selected in the scroll wheel.
     *
     * @param this Pointer to the scroll wheel widget.
     * @param index Index to be set as the current index.
     *
     * @return Status code indicating success or failure.
     */
    int (*set_index)(gui_scroll_wheel_new_t *this, int index);
//_GUI_API_DECLARE(gui_scroll_wheel_new_t)
} _gui_api_gui_scroll_wheel_new_t;
extern _gui_api_gui_scroll_wheel_new_t _gui_api_for_gui_scroll_wheel_new_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

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
/**
 * @brief Render images on the scroll wheel widget(ftl address mode).
 *
 * This function renders a series of images on the provided scroll wheel widget.
 * The images and their corresponding text mappings are provided in a gui_text_image_map array.
 *
 * @param[in]  widget      Pointer to the scroll wheel widget.
 * @param[in]  map         Array of text-image mappings to render(ftl address mode).
 * @param[in]  map_length  Length of the text-image mappings array.
 */
void gui_scroll_wheel_new_render_image_array_ftl(gui_scroll_wheel_new_t *widget,
                                                 const struct gui_text_image_map *map, int map_length);
/**
 * @brief Set the column offset for a scroll wheel widget.
 *
 * This function sets the column offset for the specified scroll wheel widget.
 * The offset determines the starting column position for rendering the widget.
 *
 * @param widget Pointer to the scroll wheel widget.
 * @param offset Column offset to be set.
 */
void gui_scroll_wheel_new_set_col_offset(gui_scroll_wheel_new_t *widget, int offset);

/**
 * @brief Get image pointers based on the input text and a mapping.
 *
 * This function parses the input text and retrieves image pointers according to
 * the specified text-image map. It returns an array of pointers to images that
 * correspond to the characters in the input text.
 *
 * @param input Pointer to the input text string.
 * @param num_pointers Pointer to a variable to receive the number of image pointers.
 * @param text_image_map_length Length of the text-image map.
 * @param text_image_map Pointer to the text-image map structure.
 *
 * @return Pointer to an array of image pointers. The array and its content
 *         should be freed by the caller when no longer needed.
 */
void **gui_scroll_wheel_new_get_image_pointers(const char *input, size_t *num_pointers,
                                               int16_t text_image_map_length, const struct gui_text_image_map *text_image_map);

/**
 * @brief Render the scroll wheel based on the input text and mapping.
 *
 * This function renders the scroll wheel using the specified input text and
 * a text-image map. The rendering mode determines how the wheel is displayed.
 *
 * @param text Pointer to the input text string.
 * @param obj Pointer to the text object.
 * @param render_mode Rendering mode to be applied.
 * @param text_image_map_length Length of the text-image map.
 * @param text_image_map Pointer to the text-image map structure.
 */
void gui_scroll_wheel_new_render(const char *text, gui_obj_t *obj, unsigned char render_mode,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map);



#ifdef __cplusplus
}
#endif

#endif
