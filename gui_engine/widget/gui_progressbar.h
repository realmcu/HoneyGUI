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
#include <guidef.h>
#include <gui_fb.h>
#include "gui_img_scope.h"
#include "gui_canvas.h"

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

/** @brief  ... */
typedef struct gui_progressbar gui_progressbar_t;
struct gui_progressbar
{
    gui_obj_t base;
    gui_canvas_t *c;
    uint32_t color;
    uint32_t color_hl;
    size_t max;
    float per;
    void (*ctor)(gui_progressbar_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h);

};


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
/** @defgroup WIDGET_Exported_TEST_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

void gui_progressbar_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h);
void gui_progressbar_v_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h);
void gui_progressbar_h_ctor(gui_progressbar_t *this, gui_obj_t *parent, const char *filename,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h);
#if 0
/**
 * @brief create a progressbar widget.
 *
 * @param parent the father widget it nested in.
 * @param filename this progressbar widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_progressbar_t *gui_progressbar_create(void *parent, const char *filename, int16_t x, int16_t y,
                                          int16_t w, int16_t h);
gui_progressbar_t *gui_progressbar_v_create(void *parent, const char *filename, int16_t x,
                                            int16_t y,
                                            int16_t w, int16_t h);
#endif

/**
  * @brief  create a progressbar widget
 * @param parent parent widget
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget
 * @param h the hight of the widget
 * @param picture_addr the picture background
 * @return gui_progressbar_t*
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *pvParameters)
  * {
  *    parent = (void *)gui_seekbar_create_img_v(pvParameters, gui_get_file_address(picture), x, y);
  *
  *    return;
  * }
  * \endcode
  */
gui_progressbar_t *gui_progressbar_picture_create(void *parent, int16_t x, int16_t y,
                                                  int16_t w, int16_t h, void *picture_addr);
/**
 * @brief create a horizontal progressbar based on picture
 *
 * @param parent parent widget
 * @param picture the picture background
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_img_h_create(void *parent, void *picture, int16_t x, int16_t y);

/**
 * @brief create a vertical progressbar based on picture
 *
 * @param parent parent widget
 * @param picture the picture background
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y);
/**
 * @brief create a frame progressbar based on pictures
 *
 * @param parent parent widget
 * @param picture_array the pictures frame
 * @param array_length picture_array length
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @return gui_progressbar_t*
 */
gui_progressbar_t *gui_progressbar_movie_create(void *parent, void  **picture_array,
                                                uint16_t array_length, int16_t x, int16_t y);
/**
 * @brief set progress
 *
 * @param this
 * @param progress
 */
void gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress);
/**
 * @brief get_progress
 *
 * @param this
 * @return size_t
 */
size_t gui_progressbar_get_progress(gui_progressbar_t *this);
/**
 * @brief set_percentage
 *
 * @param this
 * @param percentage
 */
void gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage);
/**
 * @brief get_percentage
 *
 * @param this
 * @return float
 */
float gui_progressbar_get_percentage(gui_progressbar_t *this);
/**
 * @brief get_max
 *
 * @param this
 * @return size_t
 */
size_t gui_progressbar_get_max(gui_progressbar_t *this);



/** End of WIDGET_Exported_TEST_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

