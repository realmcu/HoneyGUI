/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_card.h
  * @brief 3d card container
  * @details slide to switch card
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/24
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_CARD_H__
#define __GUI_CARD_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_cardview.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct _gui_card_stacking_t
{
    float scale;
    float location;
    uint8_t opacity;
} gui_card_stacking_t;
/** @brief  card structure */
typedef struct gui_card
{
    gui_obj_t base;
    int32_t id;
} gui_card_t;


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
 * @brief set card style
 *
 * @param this the widget object pointer.
 * @param style
 */
void gui_card_set_style(gui_card_t *this, SLIDE_STYLE style);


/**
 * @brief create a card widget
 * @param parent the father widget it nested in.
 * @param filename this tab widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param idx the X-axis index.
 * @param idy the Y-axis index.
 * @return return the widget object pointer.
 *
 */
gui_card_t *gui_card_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h, int16_t idx, int16_t idy);


#ifdef __cplusplus
}
#endif

#endif


