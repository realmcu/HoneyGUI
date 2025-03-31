/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wave.h
  * @brief wave widget
  * @details wave widget
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
#ifndef __GUI_WAVE_H__
#define __GUI_WAVE_H__



#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  WAVE widget structure */
typedef struct gui_wave
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_wave_t;

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
 * @brief create a wave widget.
 *
 * @param parent the father widget the wave nested in.
 * @param filename the wave widget name.
 * @param x X-axis coordinate
 * @param x Y-axis coordinate
 * @param w width
 * @param h hight
 * @return return the widget object pointer
 *
 * <b>Example usage</b>
 * \code{.c}
 *{
 *     gui_wave_t *wave = gui_wave_create(parent, "test_svg_widget", 0, 0, 454, 454);
 *}
 * \endcode
 */
gui_wave_t *gui_wave_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h);

/**
 * @brief set wave size
 *
 * @param this widget pointer
 * @param x  X-axis coordinate
 * @param y  Y-axis coordinate
 * @param w  width
 * @param h  hight
 * @param t  one frame time
 */
void gui_wave_set(gui_wave_t *this,
                  float       x,
                  float       y,
                  float       w,
                  float       h,
                  float       t);
#include "nanovg.h"
/**
 * @brief Renders a waveform in a graphical user interface.
 *
 * This function renders a waveform based on the provided samples within the
 * specified area, using the NanoVG context for drawing.
 *
 * @param vg           Pointer to the NanoVG context.
 * @param x            X-coordinate of the top-left corner of the rendering area.
 * @param y            Y-coordinate of the top-left corner of the rendering area.
 * @param w            Width of the rendering area.
 * @param h            Height of the rendering area.
 * @param item_count   Number of samples in the waveform.
 * @param samples      Array of sample values.
 * @param color        Color of the waveform.
 * @param max          Maximum value among the samples.
 * @param min          Minimum value among the samples.
 */
void gui_wave_render(NVGcontext *vg,
                     int16_t x,
                     int16_t y,
                     int16_t w,
                     int16_t h,
                     int16_t item_count,
                     float *samples,
                     gui_color_t color,
                     int16_t max,
                     int16_t min);
/**
 * @brief Renders a waveform in a graphical user interface.
 *
 * This function renders a waveform based on the provided samples within the
 * specified area, using the NanoVG context for drawing.
 *
 * @param vg           Pointer to the NanoVG context.
 * @param x            X-coordinate of the top-left corner of the rendering area.
 * @param y            Y-coordinate of the top-left corner of the rendering area.
 * @param w            Width of the rendering area.
 * @param h            Height of the rendering area.
 * @param item_count   Number of samples in the waveform.
 * @param samples      Array of sample values.
 * @param color        Color of the waveform.
 * @param max          Maximum value among the samples.
 * @param min          Minimum value among the samples.
 * @param width        Width of wave.
 */
void gui_wave_width_render(NVGcontext *vg,
                           int16_t x,
                           int16_t y,
                           int16_t w,
                           int16_t h,
                           int16_t item_count,
                           float *samples,
                           gui_color_t color,
                           int16_t max,
                           int16_t min,
                           int16_t width);
/**
 * @brief Renders a bar graph in a graphical user interface.
 *
 * This function renders a bar graph based on the provided samples within the
 * specified area, using the NanoVG context for drawing. Each bar represents
 * a sample from the array, and the width of each bar can be specified.
 *
 * @param vg          Pointer to the NanoVG context.
 * @param x           X-coordinate of the top-left corner of the rendering area.
 * @param y           Y-coordinate of the top-left corner of the rendering area.
 * @param w           Width of the rendering area.
 * @param h           Height of the rendering area.
 * @param item_count  Number of samples in the bar graph.
 * @param samples     Array of sample values.
 * @param color       Color of the bars.
 * @param max         Maximum value among the samples.
 * @param min         Minimum value among the samples.
 * @param bar_width   Width of each individual bar in the graph.
 */
void gui_bar_render(NVGcontext *vg,
                    int16_t x,
                    int16_t y,
                    int16_t w,
                    int16_t h,
                    int16_t item_count,
                    float *samples,
                    gui_color_t color,
                    int16_t max,
                    int16_t min,
                    int16_t bar_width);

/**
 * @brief Renders a line graph in a graphical user interface.
 *
 * This function renders a line graph based on the provided samples within the
 * specified area, using the NanoVG context for drawing. The function links
 * each sample with a line of specified thickness to form the graph.
 *
 * @param vg          Pointer to the NanoVG context.
 * @param x           X-coordinate of the top-left corner of the rendering area.
 * @param y           Y-coordinate of the top-left corner of the rendering area.
 * @param w           Width of the rendering area.
 * @param h           Height of the rendering area.
 * @param item_count  Number of samples that make up the line graph.
 * @param samples     Array of sample values.
 * @param color       Color of the line.
 * @param max         Maximum value among the samples.
 * @param min         Minimum value among the samples.
 * @param line_width  Width of the line connecting the sample points.
 */
void gui_line_render(NVGcontext *vg,
                     int16_t x,
                     int16_t y,
                     int16_t w,
                     int16_t h,
                     int16_t item_count,
                     float *samples,
                     gui_color_t color,
                     int16_t max,
                     int16_t min,
                     int16_t line_width);

#ifdef __cplusplus
}
#endif

#endif
