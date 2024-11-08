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

/** @brief  canvas structure */
typedef struct gui_canvas
{
    gui_obj_t base;
    NVGcontext *vg;
    void (*nanovg_canvas_cb)(struct gui_canvas *this_widget);
    uint8_t checksum;
    uint8_t render;
} gui_canvas_t;

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
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

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
