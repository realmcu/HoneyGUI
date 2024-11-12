/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_perspective.c
  * @brief perspective widget
  * @details create 3D widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/20
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
#include <gui_matrix.h>
#include <gui_api.h>
#include <math.h>

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
  * @brief  ...
  * @note   ...
  * @param[IN]  param1 ...
  * @param[IN]  param2 ...
  * @return ...
  * @retval ...
  */

static void gui_perspective_scale_3d(gui_vertex_t *vertex, float scale)
{
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
}

static bool gui_perspective_point_in_rect(draw_img_t *img, int16_t x, int16_t y)
{
    if (x < img->img_target_x)
    {
        return false;
    }
    if (x > img->img_target_x + img->img_target_x)
    {
        return false;
    }
    if (y < img->img_target_y)
    {
        return false;
    }
    if (y > img->img_target_y + img->img_target_h)
    {
        return false;
    }
    return true;
}
void gui_v0_perspective(float release_x, \
                        uint16_t img_w, uint16_t img_h, \
                        gui_matrix_t *m0, \
                        gui_matrix_t *m1, \
                        gui_matrix_t *m2, \
                        gui_matrix_t *m3, \
                        gui_matrix_t *m4, \
                        gui_matrix_t *m5)
{
    struct gui_matrix rotate_3D;
    gui_vertex_t rv0, rv1, rv2, rv3;
    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_dispdev_t *dc = gui_get_dc();

    float w = 300;
    float h = 300;
    float d = w * fix_sin(60);

    gui_vertex_t v0 = {-w / 2 + 10, -h / 2, d};
    gui_vertex_t v1 = {w  / 2 - 10, -h / 2, d};
    gui_vertex_t v2 = {w  / 2 - 10, h  / 2, d};
    gui_vertex_t v3 = {-w / 2 + 10, h  / 2, d};

    float xoff, yoff, zoff, t;
    uint8_t last;


    gui_matrix_t *matrix[6] = {m0, m1, m2, m3, m4, m5};
    for (uint8_t i = 0; i < 6; i++)
    {
        matrix_identity(matrix[i]);
    }


    gui_perspective_scale_3d(&v0, 1.0f);
    gui_perspective_scale_3d(&v1, 1.0f);
    gui_perspective_scale_3d(&v2, 1.0f);
    gui_perspective_scale_3d(&v3, 1.0f);

    int ry[6];
    int temp[6];

    for (uint8_t i = 0; i < 6; i++)
    {
        ry[i] = i * 60;
    }


    for (uint8_t i = 0; i < 6; i++)
    {
        ry[i] += release_x;
        temp[i] = ry[i];
    }

    for (uint8_t i = 0; i < 6 - 1; i++)
    {
        for (uint8_t j = 0; j < 6 - i - 1; j++)
        {
            if (fix_cos(temp[j]) > fix_cos(temp[j + 1]))
            {
                t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        matrix_compute_rotate(0, ry[i], 0, &rotate_3D);

        matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

        matrix_compute_rotate(0, 0, 0, &rotate_3D);//set here for x-axis

        xoff = (float)dc->screen_width / 2;
        yoff = (float)dc->screen_height / 2 + 100;
        zoff = -2 * d;

        matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);


        gui_vertex_t p = {(float)(dc->screen_width) / 2, 0, 2 * d};
        matrix_transfrom_blit(img_w, img_h, &p, &rv0, &rv1, &rv2, &rv3,
                              matrix[i]);

    }

}
