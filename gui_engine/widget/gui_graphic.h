/*
 * File      : gui_graphic.h
 */
#ifndef __GUI_GRAPHIC_H__
#define __GUI_GRAPHIC_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_obj.h"
typedef struct gui_line
{
    float a;
    float b;
    float x;
    bool vertical;
} gui_line_t;

typedef struct gui_point
{
    float x;
    float y;
} gui_point_t;
typedef struct gui_scale
{
    int maxx;
    int minx;
    int maxy;
    int miny;
} gui_scale_t;
typedef struct gui_rect
{
    gui_point_t p1;
    gui_point_t p2;
    gui_point_t p3;
    gui_point_t p4;
    gui_line_t l1;
    gui_line_t l2;
    gui_line_t l3;
    gui_line_t l4;
} gui_rect_t;

typedef struct gui_circleshape
{
    //(x-a)^2+(y-b)^2=r^2
    gui_point_t center;
    float radius;
} gui_circleshape_t;
typedef struct gui_rect2
{
    int16_t x, y, w, h;
    uint32_t color;
} gui_rect2_t;
typedef struct gui_graphic gui_graphic_t;
struct gui_graphic
{
    gui_obj_t base;
    gui_rect2_t rect;
    uint8_t rect_count;
    void (*draw_rect)(gui_graphic_t *this, int16_t x,
                      int16_t y, int16_t w, int16_t h, uint32_t color);
    void (*ctor)(gui_graphic_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
};
gui_graphic_t *gui_graphic_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h);








bool get_bit_from_bitmap(uint8_t *bitmap, int row_start, int col_start, int w, int h);

int abs(int t);
void set_point(gui_point_t *point, int16_t x, int16_t y);
void set_circle(gui_circleshape_t *circle, int16_t x, int16_t y, float r);
float calc_dis(gui_point_t *p1, gui_point_t *p2);
float calc_dis_square(gui_point_t *p1, gui_point_t *p2);
float calc_degree(gui_point_t *o, gui_point_t *p1, gui_point_t *p2);
void get_line_from_point(gui_point_t *point1, gui_point_t *point2, gui_line_t *line);
void calc_equation_result(float a, float b, float c, float *x1, float *x2);
bool judge_point_above_line(gui_line_t *line, gui_point_t *point);





#ifdef __cplusplus
}
#endif

#endif
