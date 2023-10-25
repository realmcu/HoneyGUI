/**
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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

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






/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif


