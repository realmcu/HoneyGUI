/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_seekbar.h
  * @brief seekbar widget header file
  * @details seekbar widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/11/06
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_SEEKBAR_H__
#define __GUI_SEEKBAR_H__
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_progressbar.h"
#include "gui_img.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */
typedef struct gui_seekbar
{
    gui_progressbar_t base;

    gui_img_t *slider_img;
    bool hit_slider;
    bool press_flag;
    void *press_cb;
    void *release_cb;
    void *release_cb_p;
    void *press_cb_p;
    int deltaX_old;
    uint16_t arcx;
    uint16_t arcy;
    uint16_t arc_r;
    uint16_t arc_w;
    float arc_start;
    float arc_end;
    gui_animate_t *animate;
} gui_seekbar_t;

_GUI_API_DEFINE(gui_seekbar_t)
void (*animate)(gui_seekbar_t *o, uint32_t dur, int repeat_count, void *callback, void *p);
float (*get_progress)(gui_seekbar_t *_this);
void (*set_progress)(gui_seekbar_t *_this, float progress);
void (*on_change)(gui_seekbar_t *_this, gui_event_cb_t function, void *param);
_GUI_API_DECLARE(gui_seekbar_t)

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
  * @brief  create a vertical based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  filename _this seekbar widget's name
  * @param  x   the X-axis coordinate relative to parent widget
  * @param  y   the y-axis coordinate relative to parent widget
  * @return return the widget object pointer
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *parent)
  * {
  *  parent = (void *)gui_seekbar_create_img_v(parent, gui_get_file_address(picture), x, y);
  *
  *    return;
  * }
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_img_v(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y);
/**
  * @brief  create a horizontal based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  filename _this seekbar widget's name
  * @param  x   the X-axis coordinate relative to parent widget
  * @param  y   the y-axis coordinate relative to parent widget
  * @return return the widget object pointer
  * <b>Example usage</b>
  *
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_img_h(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y);
/**
  * @brief  create a horizontal movie based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  picture_array image array address
  * @param  array_length image array length
  * @param  x the X-axis coordinate relative to parent widget
  * @param  y the y-axis coordinate relative to parent widget
  * @return return the widget object pointer
  * <b>Example usage</b>
  *
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_movie_h(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y);
/**
  * @brief  create a vertical movie based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  picture_array image array address
  * @param  array_length image array length
  * @param  x   the X-axis coordinate relative to parent widget
  * @param  y   the y-axis coordinate relative to parent widget
  * @return return the widget object pointer
  * <b>Example usage</b>
  *
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_movie_v(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y);
/**
  * @brief  create a arc movie based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  picture_array image array address
  * @param  array_length image array length
  * @param  x the X-axis coordinate relative to parent widget
  * @param  y the y-axis coordinate relative to parent widget
  * @param  arc_x arc x
  * @param  arc_y arc y
  * @param  arc_w arc width
  * @param  arc_start arc start
  * @param  arc_end arc end
  * @return return the widget object pointer
  * <b>Example usage</b>
  *
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_movie_arc(void      *parent,
                                            void     **picture_array,
                                            uint16_t   array_length,
                                            int16_t    x,
                                            int16_t    y,
                                            uint16_t   arc_x,
                                            uint16_t   arc_y,
                                            uint16_t   arc_r,
                                            uint16_t   arc_w,
                                            float      arc_start,
                                            float      arc_end);
/**
  * @brief  create a horizontal movie based on picture seekbar
  * @param  parent the father widget it nested in
  * @param  picture_array image array address
  * @param  array_length image array length
  * @param  x the X-axis coordinate relative to parent widget
  * @param  y the y-axis coordinate relative to parent widget
  * @return return the widget object pointer
  * <b>Example usage</b>
  *
  * \endcode
  */
gui_seekbar_t *gui_seekbar_create_movie_h_double(void      *parent,
                                                 void     **picture_array,
                                                 uint16_t   array_length,
                                                 int16_t    x,
                                                 int16_t    y);



#ifdef __cplusplus
}
#endif

#endif
