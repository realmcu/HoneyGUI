/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_page.h
  * @brief page widget
  * @details page widget
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
#ifndef __GUI_page_H__
#define __GUI_page_H__

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_curtain.h"
#include "gui_img.h"

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

/** @brief  PAGE widget structure */
typedef struct gui_page
{
    gui_obj_t base;
    uint32_t current_id;
    uint32_t widget_count;
    uint32_t width;
    int yold;
    int16_t recode[5];
    int speed;
    int target;
    int start_x;
    int start_y;
    int align_hight;
    gui_img_t *scroll_bar;
    int get_yend;
    gui_animate_t *animate;
    void (*ctor)(struct gui_page *, gui_obj_t *parent, const char *name, int16_t x,
                 int16_t y, int16_t w, int16_t h);
    bool release;
    bool press;
    bool gesture_flag;
    bool top_slide_only;
    uint8_t status;
    IMG_SOURCE_MODE_TYPE src_mode; // scroll_bar
    uint8_t checksum;
} gui_page_t;

typedef enum
{
    PAGE_REBOUND_NOT  = 0,
    PAGE_REBOUND_UP   = 1,
    PAGE_REBOUND_DOWN = 2,
} T_PAGE_REBOUND_TYPE;

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

#define GUI_PAGE_MAX_SPEED 60
#define GUI_PAGE_MIN_SPEED 7

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
 * @brief update the page widget.
 *
 * @param obj widget pointer.
 *
 */
void gui_page_update(gui_obj_t *obj);

/**
 * @brief destory the page widget.
 *
 * @param obj widget pointer.
 *
 */
void gui_page_destory(gui_obj_t *obj);

/**
 * @brief construct a page widget.
 *
 * @param this widget pointer.
 * @param parent the father widget the page nested in.
 * @param filename the page widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 *
 */
void gui_page_ctor(gui_page_t *,
                   gui_obj_t  *parent,
                   const char *name,
                   int16_t     x,
                   int16_t     y,
                   int16_t     w,
                   int16_t     h);

/**
 * @brief create a page widget.
 *
 * @param parent the father widget the page nested in.
 * @param filename the page widget name.
 * @param x the X-axis coordinate.
 * @param x the Y-axis coordinate.
 * @param w the width.
 * @param h the hight.
 * @return return the widget object pointer
 *
 * <b>Example usage</b>
 * \code{.c}
 *{
 *     char* ptxt = "page_name";
 *     parent = (void *)gui_page_create(parent, ptxt, x, y, w, h);
 *}
 * \endcode
 */
gui_page_t *gui_page_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h);

/**
 * @brief
 *
 * @param this widget object pointer
 * @param bar_pic bar picture address
 * @param src_mode image source mode, 0 memory and 1 file system
 */
void gui_page_add_scroll_bar(gui_page_t *, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode);

/**
 * @brief
 *
 * @param this widget object pointer
 * @param offset page offset
 */
void gui_page_set_offset(gui_page_t *, int offset);

/**
 * @brief
 *
 * @param  this widget object pointer
 * @return page offset
 */
int gui_page_get_offset(gui_page_t *);

/**
 * @brief
 *
 * @param o widget object pointer
 * @param dur Animation duration
 * @param repeat_count Repeat play times, -1 means play on repeat forever
 * @param callback animate frame callback
 * @param p parameter
 */
void gui_page_set_animate(gui_page_t *,
                          uint32_t    dur,
                          int         repeat_count,
                          void       *callback,
                          void       *p);

/**
 * @brief config rebound
 *
 * @param this widget object pointer
 * @param rebound true: config rebound; false: not rebound;
 */
void gui_page_rebound(gui_page_t *, bool rebound);

/**
 * @brief automatic center alignment
 *
 * @param page widget pointer
 * @param align_height page center aligned height
 */
void gui_page_center_alignment(gui_page_t *page, int align_height);

/**
 * @brief set only top slide flag
 *
 * @param page widget pointer
 * @param flag true:only top slide; false:all slide
 */
void gui_page_set_only_top_slide(gui_page_t *page, bool flag);

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
