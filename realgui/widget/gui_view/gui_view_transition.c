/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "acc_api.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_view.h"
#include "gui_view_api.h"


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_view_direction_offset(VIEW_SWITCH_STYLE style, int16_t release,
                                      int16_t w, int16_t h,
                                      int16_t *offset_x, int16_t *offset_y)
{
    int16_t ox = 0, oy = 0;

    switch (style)
    {
    case SWITCH_IN_FROM_LEFT_USE_REDUCTION:
    case SWITCH_IN_FROM_LEFT_USE_CUBE:
    case SWITCH_IN_FROM_LEFT_USE_ROTATE:
        ox = release - w;
        break;
    case SWITCH_IN_FROM_RIGHT_USE_REDUCTION:
    case SWITCH_IN_FROM_RIGHT_USE_CUBE:
    case SWITCH_IN_FROM_RIGHT_USE_ROTATE:
        ox = -release + w;
        break;
    case SWITCH_IN_FROM_TOP_USE_REDUCTION:
    case SWITCH_IN_FROM_TOP_USE_CUBE:
    case SWITCH_IN_FROM_TOP_USE_ROTATE:
        oy = release - h;
        break;
    case SWITCH_IN_FROM_BOTTOM_USE_REDUCTION:
    case SWITCH_IN_FROM_BOTTOM_USE_CUBE:
    case SWITCH_IN_FROM_BOTTOM_USE_ROTATE:
        oy = -release + h;
        break;
    case SWITCH_OUT_TO_LEFT_USE_REDUCTION:
    case SWITCH_OUT_TO_LEFT_USE_CUBE:
    case SWITCH_OUT_TO_LEFT_USE_ROTATE:
        ox = -release;
        break;
    case SWITCH_OUT_TO_RIGHT_USE_REDUCTION:
    case SWITCH_OUT_TO_RIGHT_USE_CUBE:
    case SWITCH_OUT_TO_RIGHT_USE_ROTATE:
        ox = release;
        break;
    case SWITCH_OUT_TO_TOP_USE_REDUCTION:
    case SWITCH_OUT_TO_TOP_USE_CUBE:
    case SWITCH_OUT_TO_TOP_USE_ROTATE:
        oy = -release;
        break;
    case SWITCH_OUT_TO_BOTTOM_USE_REDUCTION:
    case SWITCH_OUT_TO_BOTTOM_USE_CUBE:
    case SWITCH_OUT_TO_BOTTOM_USE_ROTATE:
        oy = release;
        break;

    default:
        break;
    }

    *offset_x = ox;
    *offset_y = oy;
}

/* Slide the view along one axis without any scaling or 3D distortion. */
static void gui_view_translation(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    int16_t w = obj->w;
    int16_t h = obj->h;

    switch (_this->current_transition_style)
    {
    case SWITCH_OUT_TO_RIGHT_USE_TRANSLATION:
        matrix_translate(release, 0, obj->matrix);
        break;
    case SWITCH_OUT_TO_LEFT_USE_TRANSLATION:
        matrix_translate(-release, 0, obj->matrix);
        break;
    case SWITCH_IN_FROM_LEFT_USE_TRANSLATION:
        matrix_translate(release - w, 0, obj->matrix);
        break;
    case SWITCH_IN_FROM_RIGHT_USE_TRANSLATION:
        matrix_translate(-release + w, 0, obj->matrix);
        break;
    case SWITCH_OUT_TO_TOP_USE_TRANSLATION:
        matrix_translate(0, -release, obj->matrix);
        break;
    case SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION:
        matrix_translate(0, release, obj->matrix);
        break;
    case SWITCH_IN_FROM_TOP_USE_TRANSLATION:
        matrix_translate(0, release - h, obj->matrix);
        break;
    case SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION:
        matrix_translate(0, -release + h, obj->matrix);
        break;

    default:
        break;
    }

}

/*
 * Slide the view while shrinking it towards its centre: the further it has
 * travelled, the smaller it gets (clamped to 20%..100%). Gives a "pushed into
 * the distance" look.
 */
static void gui_view_reduction(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = GUI_BASE(_this);
    int16_t w = _this->base.w;
    int16_t h = _this->base.h;
    float s;

    int16_t offset_x = 0, offset_y = 0;
    gui_view_direction_offset(_this->current_transition_style, release, w, h,
                              &offset_x, &offset_y);

    int sx = abs(offset_x);
    int sy = abs(offset_y);

    matrix_translate(offset_x, offset_y, obj->matrix);

    /* Scale factor is proportional to remaining on-screen travel on the active axis. */
    sx = sx % w;
    sy = sy % h;
    if (sx)
    {
        s = 1.0f - (float)sx / w;
    }
    else
    {
        s = 1.0f - (float)sy / h;
    }

    if (s < 0.2f)
    {
        s = 0.2f;
    }

    if (s >= 1.0f)
    {
        s = 1.0f;
    }

    matrix_translate(w / 2, h / 2, obj->matrix);
    matrix_scale(s, s, obj->matrix);
    matrix_translate(-w / 2, -h / 2, obj->matrix);
}

/*
 * Rotate the view in 3D about a screen-edge hinge (like a page turning around
 * a door hinge). The slide distance is converted to a rotation angle, the four
 * corners are projected through the 3D matrix, then a 2D blit matrix is derived.
 */
static void gui_view_rotate(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = GUI_BASE(_this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_matrix_t temp;
    gui_matrix_t rotate_3D;
    float w = _this->base.w;
    float h = _this->base.h;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    float xoff;
    float yoff;
    float zoff;

    gui_vertex_t v0 = {-w, -h, 0};
    gui_vertex_t v1 = {w,  -h, 0};
    gui_vertex_t v2 = {w,  h,  0};
    gui_vertex_t v3 = {-w, h,  0};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    int16_t offset_x = 0, offset_y = 0;
    gui_view_direction_offset(_this->current_transition_style, release,
                              _this->base.w, _this->base.h, &offset_x, &offset_y);

    rotate_degree_x = 90.f * offset_y / (_this->base.h / 2.f);
    rotate_degree_y = 90.f * offset_x / (_this->base.w / 2.f);
    matrix_compute_rotate(-rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    xoff = (float)dc->screen_width / 2;
    yoff = (float)dc->screen_height / 2 ;
    zoff = -(xoff + yoff);

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, -zoff};

    matrix_transfrom_blit(_this->base.w, _this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    if (rotate_degree_x > 90 || rotate_degree_x < -90 ||
        rotate_degree_y > 90 || rotate_degree_y < -90)
    {
        matrix_translate((int)2 * _this->base.w, \
                         (int)2 * _this->base.h, \
                         obj->matrix);
    }

    matrix_multiply(obj->matrix, &temp);
}

/*
 * Rotate the view as one face of a cube: identical to the rotate effect but the
 * corners are pushed out along +Z by half the view size (d), so adjacent views
 * appear to be faces of a revolving cube rather than a flat hinged page.
 */
static void gui_view_cube(gui_view_t *_this, int16_t release)
{
    gui_obj_t *obj = GUI_BASE(_this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_matrix_t temp;
    gui_matrix_t rotate_3D;
    float w = _this->base.w;
    float h = _this->base.h;
    float d = (float)(w + h) / 2;
    float rotate_degree_x = 0;
    float rotate_degree_y = 0;
    float xoff;
    float yoff;
    float zoff;

    gui_vertex_t v0 = {-w, -h, d};
    gui_vertex_t v1 = {w,  -h, d};
    gui_vertex_t v2 = {w,  h,  d};
    gui_vertex_t v3 = {-w, h,  d};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    int16_t offset_x = 0, offset_y = 0;
    gui_view_direction_offset(_this->current_transition_style, release,
                              _this->base.w, _this->base.h, &offset_x, &offset_y);

    rotate_degree_x = 90.f * offset_y / (_this->base.h);
    rotate_degree_y = 90.f * offset_x / (_this->base.w);
    matrix_compute_rotate(-rotate_degree_x, rotate_degree_y, 0, &rotate_3D);

    gui_vertex_t normal = {0, 0, 0};
    matrix_multiply_normal(&rotate_3D, &normal);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    xoff = (float)dc->screen_width / 2;
    yoff = (float)dc->screen_height / 2 ;
    zoff = -(xoff + yoff) * 2;

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(dc->screen_width) / 2, (float)(dc->screen_height) / 2, xoff + yoff};

    matrix_transfrom_blit(_this->base.w, _this->base.h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    if (rv0.x > rv1.x || rv0.y > rv2.y)
    {
        obj->active = false;
    }
    else
    {
        obj->active = true;
    }

    if (rotate_degree_x > 70 || rotate_degree_x < -70 ||
        rotate_degree_y > 70 || rotate_degree_y < -70)
    {
        matrix_translate((int)_this->base.w, \
                         (int)_this->base.h, \
                         obj->matrix);
    }

    matrix_multiply(obj->matrix, &temp);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_transition(gui_view_t *_this, int16_t release) //by
{
    if (_this->current_transition_style >= SWITCH_OUT_TO_LEFT_USE_TRANSLATION &&
        _this->current_transition_style < SWITCH_IN_FROM_LEFT_USE_CUBE)
    {
        gui_view_translation(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_CUBE &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_CUBE)
    {
        gui_view_cube(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_ROTATE &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_ROTATE)
    {
        gui_view_rotate(_this, release);
    }
    else if (_this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_REDUCTION &&
             _this->current_transition_style <= SWITCH_OUT_TO_BOTTOM_USE_REDUCTION)
    {
        gui_view_reduction(_this, release);
    }
}
