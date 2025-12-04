/**
\internal
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_round_rect.h
  * @brief Image canvas rect widget header file.
  * @details Image widget is used to show image on the screen.
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_CANVAS_ARC_H__
#define __GUI_CANVAS_ARC_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_obj.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum t_canvas_arc_cap
{
    CANVAS_ARC_BUTT,
    CANVAS_ARC_ROUND,
    CANVAS_ARC_SQUARE,
    CANVAS_ARC_BEVEL,
    CANVAS_ARC_MITER,
} T_CANVAS_ARC_CAP;

typedef struct gui_canvas_arc
{
    gui_obj_t base;
    uint8_t *arc_data;
    uint8_t *circle_data;

    draw_img_t *arc_img[12];
    draw_img_t *circle_img_01;
    draw_img_t *circle_img_02;
    float  from;
    float  to;
    int  r;
    int  stroke_width;
    int  cx;
    int  cy;
    T_CANVAS_ARC_CAP cap;
    gui_color_t color;
    bool use_external_picture;
    void **data;
} gui_canvas_arc_t;

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
 * @brief
 *
 * @param parent
 * @param name
 * @param addr
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_canvas_arc_t*
 */
gui_canvas_arc_t *gui_canvas_arc_create(void       *parent,
                                        const char *name,
                                        int          cx,
                                        int          cy,
                                        int          r,
                                        float        from,
                                        float        to,
                                        int  stroke_width,
                                        gui_color_t  color);


/**
 * @brief
 *
 * @param _this
 * @param cap
 */
void gui_canvas_arc_set_cap(gui_canvas_arc_t *_this, T_CANVAS_ARC_CAP cap);

/**
 * @brief
 *
 */
void gui_canvas_arc_set_external_picture(gui_canvas_arc_t *_this, \
                                         void *data[]
                                        );
/**
 * @brief Creates and initializes a new arc.
 *
 * This function creates an arc graphical object on a specified parent GUI container.
 * The arc is defined by its center, radius, start and end angles, stroke width, and color.
 *
 * @param parent Pointer to the parent GUI object that will contain the arc.
 * @param name A string representing the name of the arc object. This name can be used
 *             for identification and reference within the GUI system.
 * @param cx The x-coordinate of the center of the arc within the parent component's
 *           coordinate system.
 * @param cy The y-coordinate of the center of the arc within the parent component's
 *           coordinate system.
 * @param r The radius of the arc.
 * @param from The starting angle of the arc, in degrees.
 * @param to The ending angle of the arc, in degrees, defining the portion of the arc
 *           to be drawn.
 * @param stroke_width The width of the arc's stroke, in pixels.
 * @param color The color used to draw the arc.
 *
 * @return A pointer to the newly created `gui_canvas_arc_t` instance, or NULL if the
 *         creation fails due to memory allocation issues or invalid parameters.
 */
gui_canvas_arc_t *gui_canvas_arc_create_ftl(void       *parent,
                                            const char *name,
                                            int          cx,
                                            int          cy,
                                            int          r,
                                            float        from,
                                            float        to,
                                            int  stroke_width,
                                            gui_color_t  color);

#ifdef __cplusplus
}
#endif

#endif
