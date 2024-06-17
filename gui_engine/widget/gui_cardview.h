/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_cardview.h
  * @brief 3d container
  * @details slide to swap
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#ifndef __GUI_CARDVIEW_H__
#define __GUI_CARDVIEW_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_tabview.h"


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

/** @brief  cardview structure */
typedef struct gui_cardview
{
    gui_obj_t base;
    uint16_t height;
    uint16_t card_height;
    uint16_t total_cnt;
    uint16_t cur_id;
    T_SLIDE_STYLE style;
    int16_t hold_y;
    int16_t target_y; //means stop at this location
    int16_t offset_y; //means stop at this location
    int16_t speed;
    int16_t recode[5];
    void (*status_cb)(struct gui_cardview *this);
    uint8_t checksum;
    int yold;
    int target;
    int start_x;
    int start_y;
    int align_hight;
    int get_yend;
    bool release;
    bool press;
    bool gesture_flag;
    bool top_slide_only;
    uint8_t status;
} gui_cardview_t;

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
 * @brief cardview create
 *
 * @param parent the father widget it nested in.
 * @param name this tab widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return gui_cardview_t*
 */
gui_cardview_t *gui_cardview_create(void       *parent,
                                    const char *name,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h);

/**
 * @brief set cardview style
 *
 * @param this widget pointer
 * @param style refer to T_SLIDE_STYLE
 */
void gui_cardview_set_style(gui_cardview_t *this, T_SLIDE_STYLE style);

/**
 * @brief listen to cardview's event
 *
 * @param this widget pointer
 * @param cb callback to be triggered
 */
void gui_cardview_status_cb(gui_cardview_t *this, void (*cb)(gui_cardview_t *this));
/**
 * @brief automatic center alignment
 *
 * @param this widget pointer
 * @param align_hight
 */
void gui_cardview_alignment(gui_cardview_t *this, int align_hight);
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
