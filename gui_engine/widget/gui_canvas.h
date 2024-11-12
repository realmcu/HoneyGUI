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

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  canvas structure */
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
 * @brief create a canvas widget used to drawing graphics in nanovg.
 *
 * @param parent the father widget nested in.
 * @param name this_widget canvas widget's name.
 * @param addr
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return gui_canvas_t*
 */
gui_canvas_t *gui_canvas_create(void       *parent,
                                const char *name,
                                void       *addr,
                                int16_t     x,
                                int16_t     y,
                                int16_t     w,
                                int16_t     h);

/**
 * @brief set the callback function for drawing specific shapes.
 *
 * @param this_widget this_widget widget pointer
 * @param cb the callback function for drawing specific shapes
 */
void gui_canvas_set_canvas_cb(gui_canvas_t *this_widget, void (*cb)(gui_canvas_t *this_widget));
#define GUI_CANVAS_OUTPUT_PNG 1
#define GUI_CANVAS_OUTPUT_JPG 2
#define GUI_CANVAS_OUTPUT_RGBA 3
#define GUI_CANVAS_OUTPUT_RGB565 4

typedef void (*gui_canvas_render_function)(NVGcontext *vg);

/**
 * @brief Renders a canvas and outputs it in the specified format.
 *
 * This function renders a canvas using the provided renderer function and
 * outputs the result in one of the specified formats. The output can be
 * optionally compressed, depending on the format.
 *
 * @param output_format The format of the output image. Should be one of the following:
 *                      - GUI_CANVAS_OUTPUT_PNG
 *                      - GUI_CANVAS_OUTPUT_JPG
 *                      - GUI_CANVAS_OUTPUT_RGBA
 *                      - GUI_CANVAS_OUTPUT_RGB565
 * @param compression   Whether to apply compression to the output (applicable for RGBA/RGB565 format).
 * @param image_width   The width of the output image.
 * @param image_height  The height of the output image.
 * @param renderer      A function pointer to the rendering function that takes an NVGcontext.
 *                      This function is responsible for all drawing operations.
 *
 * @return A pointer to the buffer containing the output image data. The format of
 *         the data depends on the specified output_format. The user is responsible
 *         for managing the memory associated with this buffer.
 */
const uint8_t *gui_canvas_output(int output_format, bool compression, int image_width,
                                 int image_height, gui_canvas_render_function renderer);

/**
 * @brief Renders a canvas and outputs the result to a target buffer in the specified format.
 *
 * This function uses the provided renderer function to render a canvas, and then
 * writes the output image to the provided target buffer in the specified format.
 * The output can be optionally compressed, depending on the format.
 *
 * @param format        The format of the output image. Should be one of the following:
 *                      - GUI_CANVAS_OUTPUT_PNG
 *                      - GUI_CANVAS_OUTPUT_JPG
 *                      - GUI_CANVAS_OUTPUT_RGBA
 *                      - GUI_CANVAS_OUTPUT_RGB565
 * @param compression   Whether to apply compression to the output (applicable for RGBA/RGB565 format).
 * @param image_width   The width of the output image.
 * @param image_height  The height of the output image.
 * @param renderer      A function pointer to the rendering function that takes an NVGcontext.
 *                      This function is responsible for all drawing operations.
 * @param target_buffer A pointer to the buffer where the rendered image data will be written.
 *                      The buffer must be pre-allocated and large enough to hold the image data
 *                      in the specified format.
 */
void gui_canvas_output_buffer(int format, bool compression, int image_width, int image_height,
                              gui_canvas_render_function renderer, uint8_t *target_buffer);


#include "draw_img.h"
/**
 * @brief Initializes a blank output buffer for the canvas.
 *
 * This function initializes a NanoVG context with a blank output buffer
 * and returns a pointer to the context. The buffer can be configured with
 * the specified format, compression option, width, and height.
 *
 * @param format        The format of the output buffer.
 * @param compression   Boolean flag indicating whether compression should be used.
 * @param image_width   The width of the image.
 * @param image_height  The height of the image.
 * @param target_buffer Pointer to the target buffer where the output will be stored.
 *
 * @return A pointer to the initialized NanoVG context.
 */
NVGcontext *gui_canvas_output_buffer_blank(int format, bool compression, int image_width,
                                           int image_height,
                                           uint8_t *target_buffer);

/**
 * @brief Closes the canvas and frees resources.
 *
 * This function closes the NanoVG context created by gui_canvas_output_buffer_blank
 *
 * @param vg Pointer to the NanoVG context to be closed.
 */
void gui_canvas_output_buffer_blank_close(NVGcontext *vg);


#ifdef __cplusplus
}
#endif

#endif
