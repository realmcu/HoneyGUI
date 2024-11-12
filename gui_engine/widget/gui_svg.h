/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_svg.h
  * @brief svg widget
  * @details svg widget
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/16
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
#ifndef __GUI_WIDGET_H__
#define __GUI_WIDGET_H__
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

/** @brief  svg structure */
typedef struct gui_svg
{
    gui_obj_t base;
    const char *filename;
    uint8_t *addr;
    uint32_t size;
    float degrees;
    float c_x;
    float c_y;
    float scale_x;
    float scale_y;
    float t_x;
    float t_y;
    gui_animate_t *animate;
} gui_svg_t;


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
 * @brief create a svg widget from file,which should be display an SVG image.
 *
 * @param parent the father widget nested in
 * @param name this svg widget's name.
 * @param filename this svg image's filename
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return gui_svg_t*
 */
gui_svg_t *gui_svg_create_from_file(void       *parent,
                                    const char *name,
                                    const char *filename,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h);

/**
 * @brief create a svg widget from memory,which should be display an SVG image
 *
 * @param parent the father widget nested in
 * @param name this svg widget's name.
 * @param addr change svg address
 * @param size the size of svg
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return gui_svg_t*
 */
gui_svg_t *gui_svg_create_from_mem(void       *parent,
                                   const char *name,
                                   uint8_t    *addr,
                                   uint32_t    size,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);
/**
 * @brief set svg rotation angle
 *
 * @param svg the svg widget pointer
 * @param degrees rotation angle
 * @param c_x the X-axis coordinate of the rotation center
 * @param c_y the Y-axis coordinate of the rotation center
 */
void gui_svg_rotation(gui_svg_t *svg,
                      float      degrees,
                      float      c_x,
                      float      c_y);

/**
 * @brief scale the svg
 *
 * @param svg the svg widget pointer
 * @param scale_x scale in the X-axis direction
 * @param scale_y scale in the Y-axis direction
 */
void gui_svg_scale(gui_svg_t *svg, float scale_x, float scale_y);

/**
 * @brief translate the svg
 *
 * @param svg the svg widget pointer
 * @param t_x translation in the X-axis direction
 * @param t_y translation in the Y-axis direction
 */
void gui_svg_translate(gui_svg_t *svg, float t_x, float t_y);

/**
 * @brief set the opacity value of svg
 *
 * @param svg the svg widget pointer
 * @param opacity_value the opacity value of svg to be set
 */
void gui_svg_set_opacity(gui_svg_t *svg, unsigned char opacity_value);


#ifdef __cplusplus
}
#endif

#endif
