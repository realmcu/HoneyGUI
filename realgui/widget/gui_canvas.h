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
 * @brief Create a canvas widget used to draw graphics in NanoVG.
 * @param parent The father widget nested in.
 * @param name This widget's canvas name.
 * @param addr Address.
 * @param x The X-axis coordinate relative to parent widget.
 * @param y The Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return Return the canvas widget pointer.
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
 * @param this_widget This widget pointer.
 * @param cb The callback function for drawing specific shapes.
 */
void gui_canvas_set_canvas_cb(gui_canvas_t *this_widget, void (*cb)(gui_canvas_t *this_widget));
#define GUI_CANVAS_OUTPUT_PNG 1
#define GUI_CANVAS_OUTPUT_JPG 2
#define GUI_CANVAS_OUTPUT_RGBA 3
#define GUI_CANVAS_OUTPUT_RGB565 4

typedef void (*gui_canvas_render_function)(NVGcontext *vg);

/**
 * @brief Render a canvas and output it in the specified format.
 * @param output_format The format of the output image.
 * @param compression Whether to apply compression to the output.
 * @param image_width The width of the output image.
 * @param image_height The height of the output image.
 * @param renderer A function pointer to the rendering function.
 * @return Return a pointer to the buffer containing the output image data.
 */
const uint8_t *gui_canvas_output(int output_format, bool compression, int image_width,
                                 int image_height, gui_canvas_render_function renderer);

/**
 * @brief Render a canvas and output the result to a target buffer in the specified format.
 * @param format The format of the output image.
 * @param compression Whether to apply compression to the output.
 * @param image_width The width of the output image.
 * @param image_height The height of the output image.
 * @param renderer A function pointer to the rendering function.
 * @param target_buffer A pointer to the buffer where the rendered image data will be written.
 */
void gui_canvas_output_buffer(int format, bool compression, int image_width, int image_height,
                              gui_canvas_render_function renderer, uint8_t *target_buffer);


#include "draw_img.h"
/**
 * @brief Initialize a blank output buffer for the canvas.
 * @param format The format of the output buffer.
 * @param compression Boolean flag indicating whether compression should be used.
 * @param image_width The width of the image.
 * @param image_height The height of the image.
 * @param target_buffer Pointer to the target buffer where the output will be stored.
 * @return Return a pointer to the initialized NanoVG context.
 */
NVGcontext *gui_canvas_output_buffer_blank(int format, bool compression, int image_width,
                                           int image_height,
                                           uint8_t *target_buffer);

/**
 * @brief Close the canvas and free resources.
 * @param vg Pointer to the NanoVG context to be closed.
 */
void gui_canvas_output_buffer_blank_close(NVGcontext *vg);


#ifdef __cplusplus
}
#endif

#endif
