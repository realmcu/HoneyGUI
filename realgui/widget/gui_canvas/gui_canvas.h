/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_CANVAS_H__
#define __GUI_CANVAS_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "nanovg.h"
#include "gui_obj.h"
#include "draw_img.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief Canvas structure. */
typedef struct gui_canvas
{
    gui_obj_t base;
    NVGcontext *vg;
    void (*nanovg_canvas_cb)(struct gui_canvas *this_widget);
    uint8_t checksum;
    uint8_t render;
} gui_canvas_t;


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
 * @brief Create a canvas widget used to draw graphics in nanovg.
 *
 * @param parent Father widget nested in.
 * @param name Canvas widget's name.
 * @param addr Address of the canvas.
 * @param x X-axis coordinate relative to parent widget.
 * @param y Y-axis coordinate relative to parent widget.
 * @param w Width of the canvas.
 * @param h Height of the canvas.
 * @return Pointer to the created canvas widget.
 */
gui_canvas_t *gui_canvas_create(void       *parent,
                                const char *name,
                                void       *addr,
                                int16_t     x,
                                int16_t     y,
                                int16_t     w,
                                int16_t     h);

/**
 * @brief Set the callback function for drawing specific shapes.
 *
 * @param this_widget Pointer to the canvas widget.
 * @param cb Callback function for drawing specific shapes.
 */
void gui_canvas_set_canvas_cb(gui_canvas_t *this_widget, void (*cb)(gui_canvas_t *this_widget));
#define GUI_CANVAS_OUTPUT_PNG 1
#define GUI_CANVAS_OUTPUT_JPG 2
#define GUI_CANVAS_OUTPUT_RGBA 3
#define GUI_CANVAS_OUTPUT_RGB565 4
#define GUI_CANVAS_OUTPUT_RGBA_NOMIX 5

typedef void (*gui_canvas_render_function)(NVGcontext *vg);

/**
 * @brief Render a graphical canvas to a specified buffer in a chosen image format.
 *
 * Utilizes the provided renderer function to generate graphical content within
 * a canvas, subsequently outputting the rendered image into the designated target buffer
 * in accordance with the specified format. Optional compression is available based on the format.
 *
 * @param format Format of the output image. It should be specified as one of the following options:
 *               - GUI_CANVAS_OUTPUT_PNG for PNG format.
 *               - GUI_CANVAS_OUTPUT_JPG for JPG format.
 *               - GUI_CANVAS_OUTPUT_RGBA for RGBA format.
 *               - GUI_CANVAS_OUTPUT_RGB565 for RGB565 format.
 * @param compression Boolean flag indicating whether to compress the output image data.
 *                    Note: Compression is only applicable for RGBA/RGB565 formats.
 * @param image_width Width of the output image, specified in pixels.
 * @param image_height Height of the output image, specified in pixels.
 * @param renderer Function pointer to the rendering callback function, responsible for executing
 *                 drawing operations within the rendering context (NVGcontext).
 * @param target_buffer Pointer to the pre-allocated buffer designated for storing the rendered image data.
 *                      Ensure the buffer size is adequate to hold the image data in the specified format.
 * @return Pointer to the rendered image buffer.
 */
void *gui_canvas_render_to_image_buffer(int format, bool compression, int image_width,
                                        int image_height,
                                        gui_canvas_render_function renderer, uint8_t *target_buffer);

#ifdef __cplusplus
}
#endif

#endif
