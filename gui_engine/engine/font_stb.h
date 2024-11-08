/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_stb.h
  * @brief stb font engine header file
  * @details stb font engine header file
  * @author luke_sun@realsil.com.cn
  * @date 2024/11/05
  * @version v1.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __FONT_STB_H__
#define __FONT_STB_H__
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

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

/** @brief  text mode enum start */
typedef struct font_stb_screen
{
    int width;
    int height;
    int offset;
    uint8_t *buf;
} FONT_STB_SCREEN;

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
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Macros
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
 * @brief Initialize a standard ttf file and send the font and
 *        corresponding information to the parser
 *
 * @param font_ttf_addr the address of ttf file
 */
void gui_font_stb_init(void *font_ttf_addr);

/**
 * @brief Preprocessing of ttf font drawing with stb
 *
 * @param text Widget pointer
 */
void gui_font_stb_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of ttf fonts using stb
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_stb_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing ttf fonts using stb
 *
 * @param text Widget pointer
 */
void gui_font_stb_unload(gui_text_t *text);

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
