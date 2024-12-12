/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_ttf.h
  * @brief ttf font engine header file
  * @details ttf font engine header file
  * @author luke_sun@realsil.com.cn
  * @date 2024/12/09
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __FONT_TTF_H__
#define __FONT_TTF_H__
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


/*============================================================================*
 *                         Types
 *============================================================================*/

#pragma pack(1)
typedef struct
{
    char length;
    char fileFlag;
    char version[4];
    char fontSize;
    char renderMode;
    unsigned char bold : 1;
    unsigned char italic : 1;
    unsigned char scanMode : 1;
    unsigned char indexMethod : 1;
    unsigned char rsvd : 4;
    int indexAreaSize;
    uint8_t fontNameLength;
    short ascent;
    short descent;
    short lineGap;
    char *fontName;
} FontSet;

typedef struct
{
    short x0;
    short y0;
    short x1;
    short y1;
    short advance;
    uint8_t winding_count;
    uint8_t *winding_lengths;
    short *windings;
} FontGlyphData;
#pragma pack()

typedef struct
{
    short x;
    short y;
} FontWindings;

typedef struct
{
    float x;
    float y;
} ttf_point;
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
 * @brief Preprocessing of ttf font drawing with ttf
 *
 * @param text Widget pointer
 */
void gui_font_ttf_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of ttf fonts using ttf-bin
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_ttf_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing ttf fonts using ttf
 *
 * @param text Widget pointer
 */
void gui_font_ttf_unload(gui_text_t *text);


#ifdef __cplusplus
}
#endif

#endif
