/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font.h
  * @brief font engine test
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/16
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __TEXT_FONT_H__
#define __TEXT_FONT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "guidef.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_text.h"
#include "gui_view.h"
#include "gui_scroll_text.h"

#include "gui_api_dc.h"
#include "gui_api_os.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*For layout test*/
#define LAYOUT_USE_VECTOR_FONT  0

/*============================================================================*
 *                            Variables
 *============================================================================*/
extern void *font32b1;
extern void *font32b2;
extern void *font32b4;
extern void *font32b8;
extern void *font32vb4;

extern void *fontnoto;
extern void *fontnotoarabic;
extern void *fontnotojp;
extern void *fontnotokr;
extern void *fontnotothai;
extern void *fontnotohebrew;
extern void *fontharmonysc;
/*============================================================================*
 *                           Functions
 *============================================================================*/

void text_font_rendering_test(void);
void text_font_layout_test(void);
void text_multi_language_test(void);
void text_font_scroll_function_test(void);
void text_font_source_mode_test(void);

#ifdef __cplusplus
}
#endif

#endif
