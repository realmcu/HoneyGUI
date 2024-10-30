/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_combo.h
  * @brief combo widget
  * @details A combo container widget
  * @author triton_yu@realsil.com.cn
  * @date 2024/10/30
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
#ifndef __GUI_COMBO_H__
#define __GUI_COMBO_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  calendar structure */
typedef struct gui_combo
{
    gui_obj_t base;
    bool fold;
    uint8_t current_item;
    gui_win_t *window_select;
    gui_text_t *button_selector_text;
    const char **items;
    gui_event_cb_t on_change_function;
    void *on_change_function_parameter;
} gui_combo_t;
_GUI_API_DEFINE(gui_combo_t)
/**
 * @brief Sets the on-change event callback for the combo box.
 *
 * This function sets a callback function that will be called whenever the selected
 * item of the combo box changes.
 * The callback function will receive the combo box
 * object, the event type, and an additional user parameter.
 *
 * @param combo Pointer to the combo box object.
 * @param on_change_function The callback function to be set, which will be called
 *                           when the selected item changes.
 * @param parameter An additional parameter that will be passed to the callback function.
 *
 * @note Example:
 * @code
 * #include "gui_combo.h"
 *
 * // Define an on-change event callback function
 * static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_change_func)
 * {
 *     int c;
 *     const char *s;
 *     // Get the current selected item's index and text
 *     GUI_API(gui_combo_t).get_current_item((gui_combo_t*)obj, &c, &s);
 *     // Log a message
 *     gui_log("Selected item index: %d, text: %s\n", c, s);
 * }
 *
 * // Function to set up the on-change callback for the combo box
 * void setup_combo_on_change_callback(gui_combo_t *combo, void *param)
 * {
 *     GUI_API(gui_combo_t).on_change(combo, on_change_func, param);
 * }
 * @endcode
 */
void (*on_change)(gui_combo_t *combo, gui_event_cb_t on_change_function, void *parameter);


/**
 * @brief Gets the currently selected item in the combo box.
 *
 * This function retrieves the index and text of the currently selected item in the combo box.
 *
 * @param combo Pointer to the combo box object.
 * @param item_index Pointer to an integer where the index of the current item will be stored.
 * @param item_text Pointer to a string where the text of the current item will be stored.
 */
void (*get_current_item)(gui_combo_t *combo, int *item_index, const char **item_text);


_GUI_API_DECLARE(gui_combo_t)
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
 * @brief Creates a combo box (dropdown) GUI element.
 *
 * This function creates a combo box element with the specified properties, and adds it to the given parent object.
 * The combo box contains multiple items, with same background and highlight images.
 *
 * @param parent Pointer to the parent GUI object to which the combo box will be added.
 * @param x The x-coordinate position of the combo box within the parent object.
 * @param y The y-coordinate position of the combo box within the parent object.
 * @param w The width of the combo box.
 * @param item_count The number of items in the combo box.
 * @param item_height The height of each item in the combo box.
 * @param items An array of strings, where each string represents the text of an item in the combo box.
 * @param item_background_image A pointer to the background image data for the items.
 * @param item_background_highlight_image A pointer to the background highlight image data for the items.
 * @param font_size The font size of the item text.
 * @param font_color The color of the item text.
 * @param font_file_pointer A pointer to the font file data used for rendering the text.
 *
 * @return A pointer to the created combo box object, or NULL if creation failed.
 */
gui_combo_t *gui_combo_create(gui_obj_t *parent, int x, int y, int w, int item_count,
                              int item_height,
                              const char *items[], uint8_t *item_background_image, uint8_t *item_background_highlight_image,
                              int font_size, gui_color_t font_color, uint8_t *font_file_pointer);


#ifdef __cplusplus
}
#endif

#endif
