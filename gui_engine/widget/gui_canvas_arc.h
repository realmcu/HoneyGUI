/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_round_rect.h
  * @brief image canvas rect widget header file
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
 * @param this
 * @param cap
 */
void gui_canvas_arc_set_cap(gui_canvas_arc_t *this, T_CANVAS_ARC_CAP cap);

/**
 * @brief
 *
 */
void gui_canvas_arc_set_external_picture(gui_canvas_arc_t *this, \
                                         void *data[]
                                        );

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
