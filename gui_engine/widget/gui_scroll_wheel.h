/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_wheel.h
  * @brief scroll wheel effect
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_SCROLL_WHEEL_H__
#define __GUI_SCROLL_WHEEL_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_page.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */


#define SCROLL_WHEEL_ROW 20


/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

typedef struct scroll_wheel_picture
{
    void *pic_addr;
    void *pic_hl_addr;
    gui_img_t *pic;
} scroll_wheel_picture_t;

typedef struct gui_scroll_wheel
{
    gui_page_t base;
    int child_gap;
    int highlight_start;
    int highlight_end;
    uint32_t index;
    uint32_t (*get_index)(struct gui_scroll_wheel *this);
    struct scroll_wheel_picture picture[SCROLL_WHEEL_ROW];
    bool press_flag;
    char row_count;
    IMG_SOURCE_MODE_TYPE src_mode;
} gui_scroll_wheel_t;


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
 * @brief Create a scroll_whell.https://drive.google.com/file/d/1VzPxmv5Cjtlzjs7UGs_dBfI7zKDI_nwV/view?usp=sharing
 *
 * @param parent the father widget it nested in.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_scroll_wheel_t *gui_scroll_wheel_create(void    *parent,
                                            void    *addr,
                                            int16_t  x,
                                            int16_t  y,
                                            int16_t  w,
                                            int16_t  h);
/**
 * @brief Append a picture in the wheel, images are loaded from memory address.
 *
 * @param this  The pointer of scroll_wheel widget.
 * @param num_pic The picture array memory address.
 * @param num_pic_hl The picture(highlight) array memory address.
 */
void gui_scroll_wheel_append(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl);

/**
 * @brief Append a picture in the wheel, images are loaded from filesystem.
 *
 * @param this  The pointer of scroll_wheel widget.
 * @param num_pic The picture filepath.
 * @param num_pic_hl The picture(highlight) filepath.
 */
void gui_scroll_wheel_append_from_fs(gui_scroll_wheel_t *this, void *num_pic, void *num_pic_hl);

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
