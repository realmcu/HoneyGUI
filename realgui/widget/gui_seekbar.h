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
    uint16_t last_pro;
    float arc_start;
    float arc_end;
    gui_animate_t *animate;
    const uint8_t *thumb_picture_highlight;
    const uint8_t *thumb_picture;

    // knob
    int16_t knob_step;
    uint8_t  knob_en:  1;
    uint8_t animate_array_length;
} gui_seekbar_t;

//_GUI_API_DEFINE(gui_seekbar_t)
typedef struct __gui_api_gui_seekbar_t
{
    void (*animate)(gui_seekbar_t *o, uint32_t dur, int repeat_count, void *callback, void *p);
    float (*get_progress)(gui_seekbar_t *_this);
    void (*set_progress)(gui_seekbar_t *_this, float progress);
    void (*on_change)(gui_seekbar_t *_this, gui_event_cb_t function, void *param);
    void (*append_animate)(gui_seekbar_t              *_this,
                           uint32_t                dur,
                           int                     repeat_count,
                           gui_animate_callback_t  callback,
                           void                   *p,
                           const char             *name);
    //_GUI_API_DECLARE(gui_seekbar_t)
} _gui_api_gui_seekbar_t; extern _gui_api_gui_seekbar_t _gui_api_for_gui_seekbar_t;
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
 * @brief Retrieves the enabled state of the seekbar's knob.
 *
 * This function checks whether the knob of the seekbar is currently enabled.
 * If the knob is enabled, the user can adjust the seekbar value by interacting with the knob.
 *
 * @param this  A pointer to the seekbar.
 * @return      A boolean value indicating whether the knob is enabled.
 *              Returns true if enabled, false if not.
 */
bool gui_seekbar_get_knob_en(gui_seekbar_t *this);

/**
 * @brief Retrieves the step value of the seekbar's knob.
 *
 * This function returns the current step value set for the seekbar's knob.
 * The step value defines the increment by which the seekbar's value changes
 * when the knob is moved.
 *
 * @param this  A pointer to the seekbar.
 * @return      The current step value of the knob, returned as an int16_t.
 */
int16_t gui_seekbar_get_knob_step(gui_seekbar_t *this);

/**
 * @brief Enables or disables the knob of the seekbar.
 *
 * This function is used to enable or disable the knob of the seekbar.
 * When enabled, the knob is interactable by the user, allowing changes
 * to the seekbar's value. When disabled, the knob is not interactable.
 *
 * @param this  A pointer to the seekbar.
 * @param en    A boolean value indicating whether to enable (true) or
 *              disable (false) the knob.
 */
void gui_seekbar_set_knob_en(gui_seekbar_t *this, bool en);

/**
 * @brief Sets the step value for the seekbar's knob.
 *
 * This function assigns a step value for the knob of the seekbar.
 * The step value determines how much the seekbar's value changes
 * when the knob is rolled. By default, the step value is 1.
 *
 * @param this  A pointer to the seekbar.
 * @param step  The step value for the knob, which can be a negative value.
 */
void gui_seekbar_set_knob_step(gui_seekbar_t *this, int16_t step);


/**
  * @brief  Create a vertical seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  filename The name of this seekbar widget.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_img_v(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y);
/**
  * @brief  Create a horizontal seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  filename The name of this seekbar widget.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_img_h(void       *parent,
                                        const char *name,
                                        int16_t     x,
                                        int16_t     y);
/**
  * @brief  Create a horizontal movie seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  picture_array The image array address.
  * @param  array_length The image array length.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_movie_h(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y);
/**
  * @brief  Create a vertical movie seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  picture_array The image array address.
  * @param  array_length The image array length.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_movie_v(void      *parent,
                                          void     **picture_array,
                                          uint16_t   array_length,
                                          int16_t    x,
                                          int16_t    y);
/**
  * @brief  Create an arc movie seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  picture_array The image array address.
  * @param  array_length The image array length.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @param  arc_x The X coordinate of the arc.
  * @param  arc_y The Y coordinate of the arc.
  * @param  arc_w The width of the arc.
  * @param  arc_start The start angle of the arc.
  * @param  arc_end The end angle of the arc.
  * @return Return the widget object pointer.
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
  * @brief  Create a horizontal movie seekbar based on a picture.
  * @param  parent The parent widget it is nested in.
  * @param  picture_array The image array address.
  * @param  array_length The image array length.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_movie_h_double(void      *parent,
                                                 void     **picture_array,
                                                 uint16_t   array_length,
                                                 int16_t    x,
                                                 int16_t    y);
/**
  * @brief  Create a horizontal seekbar with a thumb image.
  * @param  parent The parent widget it is nested in.
  * @param  picture_pointer The thumb image pointer.
  * @param  picture_highlight_pointer The thumb image highlight pointer.
  * @param  x The X-axis coordinate relative to the parent widget.
  * @param  y The Y-axis coordinate relative to the parent widget.
  * @param  bar_width The width of the seekbar.
  * @return Return the widget object pointer.
  */
gui_seekbar_t *gui_seekbar_create_thumb_h(void    *parent,
                                          void    *picture_pointer,
                                          void    *picture_highlight_pointer,
                                          int16_t  x,
                                          int16_t  y,
                                          uint16_t bar_width);



#ifdef __cplusplus
}
#endif

#endif
