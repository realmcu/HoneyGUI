/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wave.h
  * @brief wave widget
  * @details wave widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/11/07
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
#ifndef __GUI_WAVE_H__
#define __GUI_WAVE_H__



#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  WAVE widget structure */
typedef struct gui_wave
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_wave_t;


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
 * @brief create a wave widget.
 *
 * @param parent the father widget the wave nested in.
 * @param filename the wave widget name.
 * @param x X-axis coordinate
 * @param x Y-axis coordinate
 * @param w width
 * @param h hight
 * @return return the widget object pointer
 *
 * <b>Example usage</b>
 * \code{.c}
 *{
 *     gui_wave_t *wave = gui_wave_create(parent, "test_svg_widget", 0, 0, 454, 454);
 *}
 * \endcode
 */
gui_wave_t *gui_wave_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                            int16_t h);

/**
 * @brief set wave size
 *
 * @param this widget pointer
 * @param x  X-axis coordinate
 * @param y  Y-axis coordinate
 * @param w  width
 * @param h  hight
 * @param t  one frame time
 */
void gui_wave_set(gui_wave_t *this, float x, float y, float w, float h, float t);


#ifdef __cplusplus
}
#endif

#endif

