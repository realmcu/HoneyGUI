/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_progressbar.h
  * @brief progressbar widget header file
  * @details progressbar widget
  * @author luke_sun@realsil.com.cn
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
#ifndef __GUI_progressBAR_H__
#define __GUI_progressBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_img_scope.h"
#include "gui_canvas.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */
/* gui_progressbar_t start*/
typedef struct gui_progressbar
{
    gui_obj_t base;
    gui_canvas_t *c;
    uint32_t color;
    uint32_t color_hl;
    size_t max;
    float per, per_history;
    void (*ctor)(struct gui_progressbar *_this, gui_obj_t *parent, const char *name, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);
    IMG_SOURCE_MODE_TYPE src_mode;   // scroll_bar
    uint8_t image_type;
    uint16_t thumb_width;
} gui_progressbar_t;
/* gui_progressbar_t end*/


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
 * @brief create a horizontal progressbar based on picture.
 *
 * @param parent parent widget.
 * @param picture the picture background.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_img_h_create(void    *parent,
                                                void    *picture,
                                                int16_t  x,
                                                int16_t  y);
/**
 * @brief create a horizontal progressbar based on picture.
 *
 * @param parent parent widget.
 * @param picture the picture background, should be ftl addr.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_img_h_create_from_ftl(void    *parent,
                                                         void    *picture,
                                                         int16_t  x,
                                                         int16_t  y);

/**
 * @brief create a vertical progressbar based on picture.
 *
 * @param parent parent widget.
 * @param picture the picture background.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_img_v_create(void    *parent,
                                                void    *picture,
                                                int16_t  x,
                                                int16_t  y);

/**
 * @brief create a vertical progressbar based on picture.
 *
 * @param parent parent widget.
 * @param picture the picture background, should be ftl addr.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_img_v_create_from_ftl(void    *parent,
                                                         void    *picture,
                                                         int16_t  x,
                                                         int16_t  y);

/**
 * @brief create a frame progressbar based on pictures, file source is memory address.
 *
 * @param parent parent widget.
 * @param picture_array the pictures frame memory address.
 * @param array_length picture_array length.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_movie_create(void      *parent,
                                                void     **picture_array,
                                                uint16_t   array_length,
                                                int16_t    x,
                                                int16_t    y);

/**
 * @brief create a frame progressbar based on pictures, file source is filesystem.
 *
 * @param parent parent widget.
 * @param picture_array the pictures frame filepath.
 * @param array_length picture_array length.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_movie_create_from_fs(void      *parent,
                                                        void     **picture_array,
                                                        uint16_t   array_length,
                                                        int16_t    x,
                                                        int16_t    y);
/**
 * @brief set progress.
 *
 * @param _this widget object pointer.
 * @param progress the progress of widget.
 */
void gui_progressbar_set_progress(gui_progressbar_t *_this, size_t progress);

/**
 * @brief get_progress.
 *
 * @param _this widget object pointer.
 * @return size_t
 */
size_t gui_progressbar_get_progress(gui_progressbar_t *_this);

/**
 * @brief set_percentage.
 *
 * @param _this widget object pointer.
 * @param percentage the percentage of widget.
 */
void gui_progressbar_set_percentage(gui_progressbar_t *_this, float percentage);

/**
 * @brief get_percentage.
 *
 * @param _this widget object pointer.
 * @return float.
 */
float gui_progressbar_get_percentage(gui_progressbar_t *_this);

/**
 * @brief get_max.
 *
 * @param _this widget object pointer.
 * @return size_t.
 */
size_t gui_progressbar_get_max(gui_progressbar_t *_this);

/**
 * @brief create a horizontal progressbar based on picture with thumb.
 *
 * @param parent parent widget.
 * @param picture_pointer the picture background.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param bar_width the bar width.
 * @return gui_progressbar_t*.
 */
gui_progressbar_t *gui_progressbar_thumb_h_create(void    *parent,
                                                  void    *picture_pointer,
                                                  int16_t  x,
                                                  int16_t  y,
                                                  uint16_t bar_width);


#ifdef __cplusplus
}
#endif

#endif
