/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_custom.h
  * @brief custom font engine header file
  * @details custom font engine header file
  * @author luke_sun@realsil.com.cn
  * @date 2025/11/18
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __FONT_CUSTOM_H__
#define __FONT_CUSTOM_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <draw_font.h>
#include "gui_text.h"
#include "font_rendering_utils.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

typedef void (*gui_font_custom_load_cb_t)(gui_text_t *text, gui_text_rect_t *rect);
typedef void (*gui_font_custom_draw_cb_t)(gui_text_t *text, gui_text_rect_t *rect);
typedef void (*gui_font_custom_unload_cb_t)(gui_text_t *text);
typedef void (*gui_font_custom_destroy_cb_t)(gui_text_t *text);

/*============================================================================*
 *                         Variables
 *============================================================================*/

extern gui_font_custom_load_cb_t gui_font_custom_load_cb;
extern gui_font_custom_draw_cb_t gui_font_custom_draw_cb;
extern gui_font_custom_unload_cb_t gui_font_custom_unload_cb;
extern gui_font_custom_destroy_cb_t gui_font_custom_destroy_cb;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Set custom font callbacks
 *
 * @param load_cb Load callback function
 * @param draw_cb Draw callback function
 * @param unload_cb Unload callback function
 * @param destroy_cb Destroy callback function
 */
void gui_font_custom_set_callbacks(gui_font_custom_load_cb_t load_cb,
                                   gui_font_custom_draw_cb_t draw_cb,
                                   gui_font_custom_unload_cb_t unload_cb,
                                   gui_font_custom_destroy_cb_t destroy_cb);

/**
 * @brief Preprocessing of ttf font drawing with ttf
 *
 * @param text Widget pointer
 */
void gui_font_custom_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of custom fonts using custom-bin
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_custom_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing custom fonts using custom
 *
 * @param text Widget pointer
 */
void gui_font_custom_unload(gui_text_t *text);

/**
 * @brief Destroy this custom font type in font list
 *
 * @param text Widget pointer
 */
void gui_font_custom_destroy(gui_text_t *text);

#ifdef __cplusplus
}
#endif

#endif