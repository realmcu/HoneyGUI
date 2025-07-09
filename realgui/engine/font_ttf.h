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
#include "font_rendering_utils.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

#pragma pack(1)
typedef struct
{
    uint8_t head_length;                /*font file head length*/
    uint8_t file_type;                  /*0x2-FONT_FILE_TTF_FLAG is font file*/
    uint8_t version[4];                 /*version*/
    uint8_t font_size;                  /*font size*/
    uint8_t rendor_mode;                /*support 1/2/4/8*/
    uint8_t bold :          1;          /*bold*/
    uint8_t italic :        1;          /*italic*/
    uint8_t scan_mode :     1;          /*scan_mode*/
    uint8_t index_method :  1;          /*0 offset ; 1 address*/
    uint8_t crop :          1;          /*Unavailable*/
    uint8_t rsvd :          3;          /*rsvd*/
    uint32_t index_area_size;           /*length of index area*/
    uint8_t font_name_length;           /*length of font name*/
    int16_t ascent;
    int16_t descent;
    int16_t lineGap;
    uint8_t *font_name;
} GUI_FONT_HEAD_TTF;

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

typedef enum
{
    POS_POS,  // m[0][0] >= 0, m[0][1] >= 0
    POS_NEG,  // m[0][0] >= 0, m[0][1] <  0
    NEG_POS,  // m[0][0] <  0, m[0][1] >= 0
    NEG_NEG   // m[0][0] <  0, m[0][1] <  0
} TransformCase;

typedef enum
{
    FONT_IDENTITY,
    FONT_TRANSFORM,
    FONT_SCALE,
    FONT_HOMOGENEOUS
} FONT_MATRIX_TYPE;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define FONT_FILE_TTF_FLAG  0x02

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

/**
 * @brief calculate rect
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_ttf_adapt_rect(gui_text_t *text, gui_text_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
