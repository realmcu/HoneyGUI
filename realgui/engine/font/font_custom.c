/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_custom.c
  * @brief realui custom font engine
  * @details realui custom font engine
  * @author luke_sun@realsil.com.cn
  * @date 2025/11/18
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "font_custom.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
gui_font_custom_load_cb_t gui_font_custom_load_cb = NULL;
gui_font_custom_draw_cb_t gui_font_custom_draw_cb = NULL;
gui_font_custom_unload_cb_t gui_font_custom_unload_cb = NULL;
gui_font_custom_destroy_cb_t gui_font_custom_destroy_cb = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_font_custom_set_callbacks(gui_font_custom_load_cb_t load_cb,
                                   gui_font_custom_draw_cb_t draw_cb,
                                   gui_font_custom_unload_cb_t unload_cb,
                                   gui_font_custom_destroy_cb_t destroy_cb)
{
    gui_font_custom_load_cb = load_cb;
    gui_font_custom_draw_cb = draw_cb;
    gui_font_custom_unload_cb = unload_cb;
    gui_font_custom_destroy_cb = destroy_cb;
}

void gui_font_custom_load(gui_text_t *text, gui_text_rect_t *rect)
{
    if (gui_font_custom_load_cb != NULL)
    {
        gui_font_custom_load_cb(text, rect);
        return;
    }

    /* Default implementation - can be used as reference */
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    /* Implementation steps:
     * 1. Load font file and validate format
     * 2. Process font encoding (UTF-8, UTF-16, etc.)
     * 3. Handle multilingual support and character mapping
     * 4. Read glyph information (metrics, bitmaps, etc.)
     * 5. Perform text layout and prepare for rendering
     */
}

void gui_font_custom_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    if (gui_font_custom_draw_cb != NULL)
    {
        gui_font_custom_draw_cb(text, rect);
        return;
    }

    /* Default implementation - can be used as reference */
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    /* Implementation steps:
     * 1. Process font drawing information (position, color, etc.)
     * 2. Perform text rendering with appropriate techniques
     */
}

void gui_font_custom_unload(gui_text_t *text)
{
    if (gui_font_custom_unload_cb != NULL)
    {
        gui_font_custom_unload_cb(text);
        return;
    }

    /* Default implementation - can be used as reference */
    GUI_UNUSED(text);

    /* Implementation steps:
     * 1. Release font file handles and resources
     * 2. Free allocated memory for glyph cache
     * 3. Clean up temporary buffers and reset state
     */
}

void gui_font_custom_destroy(gui_text_t *text)
{
    if (gui_font_custom_destroy_cb != NULL)
    {
        gui_font_custom_destroy_cb(text);
        return;
    }

    /* Default implementation - can be used as reference */
    GUI_UNUSED(text);

    /* Implementation steps:
     * 1. Clear persistent cache required for drawing
     * 2. Release widget custom content cache
     */
}
