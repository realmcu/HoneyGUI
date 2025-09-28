/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_page.h
  * @brief Page widget.
  * @details Page widget.
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

/*============================================================================*
 *                         Types
 *============================================================================*/

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
    float velocity ;
    uint32_t last_ms;
    float y ;
    int target;
    int start_x;
    int start_y;
    int top_offset;
    int bottom_offset;
    int align_hight;
    gui_img_t *scroll_bar;
    int get_yend;
    gui_animate_t *animate;
    void (*ctor)(struct gui_page *, gui_obj_t *parent, const char *name, int16_t x,
                 int16_t y, int16_t w, int16_t h);
    uint8_t release      : 1;
    uint8_t press        : 1;
    uint8_t gesture_flag      : 1;
    uint8_t top_slide_only    : 1;
    uint8_t wheel_take_over   : 1;
    uint8_t wheel_up   : 1;
    uint8_t wheel_pressed   : 1;
    uint8_t status;
    IMG_SOURCE_MODE_TYPE src_mode; // scroll_bar
    uint8_t checksum;
    uint16_t default_h;
} gui_page_t;

typedef enum
{
    PAGE_REBOUND_NOT  = 0,
    PAGE_REBOUND_UP   = 1,
    PAGE_REBOUND_DOWN = 2,
} T_PAGE_REBOUND_TYPE;


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_PAGE_MAX_SPEED 20
#define GUI_PAGE_MIN_SPEED 3


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Update the page widget.
 *
 * @param obj Widget pointer.
 *
 */
void gui_page_update(gui_obj_t *obj);

/**
 * @brief Destroy the page widget.
 *
 * @param obj Widget pointer.
 *
 */
void gui_page_destroy(gui_obj_t *obj);

/**
 * @brief Construct a page widget.
 *
 * @param this Widget pointer.
 * @param parent The father widget the page nested in.
 * @param filename The page widget name.
 * @param x The X-axis coordinate.
 * @param x The Y-axis coordinate.
 * @param w The width.
 * @param h The height.
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
 * @brief Create a page widget.
 *
 * @param parent The father widget the page nested in.
 * @param filename The page widget name.
 * @param x The X-axis coordinate.
 * @param x The Y-axis coordinate.
 * @param w The width.
 * @param h The height.
 * @return Return the widget object pointer.
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
 * @brief Create a page widget horizontal.
 *
 * @param parent The father widget the page nested in.
 * @param filename The page widget name.
 * @param x The X-axis coordinate.
 * @param x The Y-axis coordinate.
 * @param w The width.
 * @param h The height.
 * @return Return the widget object pointer.
 *
 * \endcode
 */
gui_page_t *gui_page_create_horizontal(void       *parent,
                                       const char *name,
                                       int16_t     x,
                                       int16_t     y,
                                       int16_t     w,
                                       int16_t     h);

/**
 * @brief Add a scroll bar to the page widget.
 *
 * @param this Widget object pointer.
 * @param bar_pic Bar picture address.
 * @param src_mode Image source mode, 0 memory and 1 file system.
 */
void gui_page_add_scroll_bar(gui_page_t *, void *bar_pic, IMG_SOURCE_MODE_TYPE src_mode);

/**
 * @brief Set the page offset.
 *
 * @param this Widget object pointer.
 * @param offset Page offset.
 */
void gui_page_set_offset(gui_page_t *, int offset);

/**
 * @brief Get the page offset.
 *
 * @param this Widget object pointer.
 * @return Page offset.
 */
int gui_page_get_offset(gui_page_t *);

/**
 * @brief Set animation for the page widget.
 *
 * @param o Widget object pointer.
 * @param dur Animation duration.
 * @param repeat_count Repeat play times, -1 means play on repeat forever.
 * @param callback Animate frame callback.
 * @param p Parameter.
 */
void gui_page_set_animate(gui_page_t *,
                          uint32_t    dur,
                          int         repeat_count,
                          void       *callback,
                          void       *p);

/**
 * @brief Configure rebound for the page widget.
 *
 * @param this Widget object pointer.
 * @param rebound True: configure rebound; false: not rebound.
 */
void gui_page_rebound(gui_page_t *, bool rebound);
/**
 * @brief Configure horizontal rebound for the page widget.
 *
 * @param this Widget object pointer.
 * @param rebound True: configure rebound; false: not rebound.
 */
void gui_page_rebound_horizontal(gui_page_t *, bool rebound);
/**
 * @brief Perform automatic center alignment for the page widget.
 *
 * @param page Widget pointer.
 * @param align_height Page center aligned height.
 */
void gui_page_center_alignment(gui_page_t *page, int align_height);

/**
 * @brief Set the only top slide flag for the page widget.
 *
 * @param page Widget pointer.
 * @param flag True: only top slide; false: all slide.
 */
void gui_page_set_only_top_slide(gui_page_t *page, bool flag);

#ifdef __cplusplus
}
#endif

#endif
