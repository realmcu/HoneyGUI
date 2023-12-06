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
#include <guidef.h>
#include <gui_api.h>

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


typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} gui_card_id_t;
/** @brief  cardview structure */
typedef struct gui_cardview
{
    gui_obj_t base;
    gui_list_t  tab_list;
    gui_card_id_t id;
    uint16_t tab_cnt;
    int8_t tab_cnt_left;
    int8_t tab_cnt_right;
    int8_t tab_cnt_up;
    int8_t tab_cnt_down;
    gui_card_id_t cur_id;
    SLIDE_STYLE style;
    int16_t release_y;
    int16_t remain_y;
    bool mute;
    void (*status_cb)(struct gui_cardview *this);
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
gui_cardview_t *gui_cardview_create(void *parent,  const char *name,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief set cardview style
 *
 * @param this widget pointer
 * @param style refer to SLIDE_STYLE
 */
void gui_cardview_set_style(gui_cardview_t *this, SLIDE_STYLE style);

/**
 * @brief disable cardview
 *
 * @param this widget pointer
 */
void gui_cardview_mute(gui_cardview_t *this);

/**
 * @brief listen to cardview's event
 *
 * @param this widget pointer
 * @param cb callback to be triggered
 */
void gui_cardview_status_cb(gui_cardview_t *this, void (*cb)(gui_cardview_t *this));



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





