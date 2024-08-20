/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_tab_rotate.c
  * @brief tab widget
  * @details tab widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_server.h"
#include <gui_matrix.h>



/** @defgroup WIDGET WIDGET
  * @{
  */

/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

void gui_v0_cut_cube(gui_matrix_t *matrix, int32_t x)
{

    gui_dispdev_t *dc = gui_get_dc();
    gui_matrix_t rotate_3D;
    gui_matrix_t temp;

    float w = dc->screen_width;
    float h = dc->screen_height;
    float d = (w + h) / 2;
    float rotate_degree;

    gui_vertex_t v0 = {-w, -h, d};
    gui_vertex_t v1 = {w,  -h, d};
    gui_vertex_t v2 = {w,  h,  d};
    gui_vertex_t v3 = {-w, h,  d};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    matrix_identity(matrix);
    int16_t tab_x_gap = 0;

    if (x < -w)
    {
        tab_x_gap = -1;
    }
    else if (x > w)
    {
        tab_x_gap = 1;
    }

    rotate_degree = 90 * x / (w) + 90.0 * (tab_x_gap);

    matrix_compute_rotate(0, rotate_degree, 0, &rotate_3D);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    float xoff = (float)dc->screen_width / 2;
    float yoff = (float)dc->screen_height / 2 ;
    float zoff = -(xoff + yoff) * 2;

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, xoff + yoff};

    matrix_transfrom_blit(w, h, &p, &rv0, &rv1, &rv2, &rv3, &temp);

    if (rv0.x > rv1.x)
    {
        // obj->not_show = true;
        matrix_translate(4 * w, -1000, matrix);
    }

    if (rotate_degree > 90)
    {
        matrix_translate((tab_x_gap) * w, \
                         0, \
                         matrix);
    }

    if (rotate_degree < -90)
    {
        matrix_translate((tab_x_gap) * w, \
                         0, \
                         matrix);
    }

    matrix_multiply(matrix, &temp);
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
