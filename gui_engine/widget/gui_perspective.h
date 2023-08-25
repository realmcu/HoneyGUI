/*
 * File      : gui_vg_lite_clock.h
 */
#ifndef __GUI_PERSPECTIVE_H__
#define __GUI_PERSPECTIVE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include "gui_magic_img.h"


typedef struct VertexRec
{
    float x;
    float y;
    float z;
} Vertex_t;

typedef struct NormalRec
{
    float x;
    float y;
    float z;
} Normal_t;


typedef struct gui_perspective
{
    gui_obj_t base;
    draw_img_t draw_img_front;
    draw_img_t draw_img_back;
    draw_img_t draw_img_up;
    draw_img_t draw_img_down;
    draw_img_t draw_img_left;
    draw_img_t draw_img_right;
    draw_img_t img;
    float c_x;
    float c_y;
    float cbsize;
} gui_perspective_t;

#define RAD(d)        (d*3.1415926f/180.0f)


gui_perspective_t *gui_perspective_create(void *parent,  const char *name, void *addr,
                                          int16_t x, int16_t y, int16_t w, int16_t h);

//void gui_cube_set_center(gui_cube_t *this, float c_x, float c_y);
//void gui_cube_set_size(gui_cube_t *this, float size);

#ifdef __cplusplus
}
#endif

#endif

