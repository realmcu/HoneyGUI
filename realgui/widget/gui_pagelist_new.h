/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelist_new.h
  * @brief pagelist widget sample at APP_PAGE_LIST realgui\example\screen_454_454\gui_menu\apps_in_menu.c
  * @details A ram friendly page widget to show large amount of lists.
  * @author triton_yu@realsil.com
  * @date 2024/11/04
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_PAGELIST_NEW_H__
#define __GUI_PAGELIST_NEW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_img.h"
#include "gui_win.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  Pagelist widget structure */


typedef struct gui_pagelist_new
{
    gui_obj_t base;



    const gui_event_cb_t *click_function_array;
    const char **item_text_array;
    const uint8_t *item_image;
    const uint8_t *item_image_hl;
    const uint8_t *font;

    gui_color_t font_color;


    gui_win_t *timer;


    int history_y;
    int touch_y, speed, index_offset;
    uint16_t row_count;
    uint16_t current_item_index;
    uint16_t item_count;
    uint16_t row_space;
    uint16_t font_size;
    unsigned char  alien, render_flag, end_speed;


    bool wheel_take_over;
    int16_t time_array_offset;

    bool event5_flag;
    bool horizontal;
    BLEND_MODE_TYPE blending;
} gui_pagelist_new_t;


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
 * @brief Create a new vertical pagelist.
 *
 * This function initializes and creates a new vertical pagelist with the specified parameters. Sample at APP_PAGE_LIST realgui\example\screen_454_454\gui_menu\apps_in_menu.c
 *
 * @param parent Pointer to the parent GUI object.
 * @param x The X coordinate of the pagelist.
 * @param y The Y coordinate of the pagelist.
 * @param w The width of the pagelist.
 * @param item_space The space between items in the pagelist.
 * @param item_background_image Pointer to the background image data for pagelist items.
 * @param item_background_image_highlight Pointer to the background image data for highlighted items.
 * @param item_background_blending_mode The blending mode for item background images.
 * @param font Pointer to the font data used for item texts.
 * @param font_size The size of the font used for item texts.
 * @param font_color The color of the font used for item texts.
 *
 * @return A pointer to the newly created pagelist object.
 */
gui_pagelist_new_t *gui_pagelist_new_create(void *parent,
                                            int16_t x,
                                            int16_t y,
                                            uint16_t w,
                                            uint16_t item_space,
                                            const uint8_t *item_background_image,
                                            const uint8_t *item_background_image_highlight,
                                            BLEND_MODE_TYPE item_background_blending_mode,
                                            const uint8_t *font,
                                            uint16_t font_size,
                                            gui_color_t font_color);

/**
 * @brief Render the pagelist with the specified items.
 *
 * This function renders the pagelist with the provided items, their click functions, and their text labels. Sample at APP_PAGE_LIST realgui\example\screen_454_454\gui_menu\apps_in_menu.c
 *
 * @param pagelist_new Pointer to the pagelist object to render.
 * @param item_count The number of items in the pagelist.
 * @param item_click_function_array Array of callback functions for item clicks.
 * @param item_text_array Array of text labels for the items.
 *
 * @return A GUI error code indicating the success or failure of the rendering operation.
 */
gui_error_t gui_page_list_new_render(gui_pagelist_new_t *pagelist_new,
                                     const uint16_t item_count,
                                     const gui_event_cb_t *item_click_function_array,
                                     const char **item_text_array);

/**
 * @brief Create a new horizontal pagelist.
 *
 * This function initializes and creates a new horizontal pagelist with the specified parameters. Sample at APP_PAGE_LIST realgui\example\screen_454_454\gui_menu\apps_in_menu.c
 *
 * @param parent Pointer to the parent GUI object.
 * @param x The X coordinate of the pagelist.
 * @param y The Y coordinate of the pagelist.
 * @param item_space The space between items in the pagelist.
 * @param h The height of the pagelist.
 * @param item_background_image Pointer to the background image data for pagelist items.
 * @param item_background_image_highlight Pointer to the background image data for highlighted items.
 * @param item_background_blending_mode The blending mode for item background images.
 * @param font Pointer to the font data used for item texts.
 * @param font_size The size of the font used for item texts.
 * @param font_color The color of the font used for item texts.
 *
 * @return A pointer to the newly created horizontal pagelist object.
 */
gui_pagelist_new_t *gui_pagelist_new_create_horizontal(void *parent,
                                                       int16_t x,
                                                       int16_t y,
                                                       uint16_t item_space,
                                                       uint16_t h,
                                                       const uint8_t *item_background_image,
                                                       const uint8_t *item_background_image_highlight,
                                                       BLEND_MODE_TYPE item_background_blending_mode,
                                                       const uint8_t *font,
                                                       uint16_t font_size,
                                                       gui_color_t font_color);


#ifdef __cplusplus
}
#endif

#endif
