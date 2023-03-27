/*
 * File      : acc_engine.h
 */
#ifndef __ACC_ENGINE_H__
#define __ACC_ENGINE_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif
typedef struct canvas_gradient_stop
{
    float offset;
    uint32_t color_rgba;
} canvas_gradient_stop_t;
#define CANVAS_LINEAR_GRADIENT_STOPS_MAXMUN 6
#define CANVAS_RADIAL_GRADIENT_STOPS_MAXMUN 6

/*https://www.w3schools.com/graphics/svg_grad_linear.asp*/
typedef struct canvas_linear_gradient
{
    float x1;
    float y1;
    float x2;
    float y2;
    uint32_t stops_number;//to judge if using linear gradient
    canvas_gradient_stop_t stops[CANVAS_LINEAR_GRADIENT_STOPS_MAXMUN];
} canvas_linear_gradient_t;

/*https://www.w3schools.com/graphics/svg_grad_radial.asp*/
typedef struct canvas_radial_gradient
{
    float cx;
    float cy;
    float r;
    float fx;
    float fy;
    uint32_t stops_number;//to judge if using linear gradient
    canvas_gradient_stop_t stops[CANVAS_RADIAL_GRADIENT_STOPS_MAXMUN];
} canvas_radial_gradient_t;

typedef enum CAP
{
    BUTT,
    ROUND,
    SQUARE,
} STROKE_LINECAP;

typedef enum JOIN
{
    MITER,
    ROUNDER,
    BEVEL,
} STROKE_LINEJOIN;

typedef enum FILL_COLOR
{
    RGBA,
    LINEAR,
    RADIAL,
} FILL_TYPE;

typedef struct canvas_fill
{
    union
    {
        uint32_t rgba;
        canvas_linear_gradient_t linear_gradient;
        canvas_radial_gradient_t radial_gradient;
    } color_data;
    FILL_TYPE type;
} canvas_fill_t;

typedef struct canvas_stroke
{
    STROKE_LINECAP stroke_linecap;
    STROKE_LINEJOIN stroke_linejoin;
    float *dash;
    uint32_t dash_count;
    float dash_phase;
    float stroke_width;
    float miter_limit;
    canvas_fill_t fill;
} canvas_stroke_t;

typedef struct canvas_circle
{
    float cx;
    float cy;
    float r;
    float rotate;
    float scale;
    canvas_fill_t fill;
    canvas_stroke_t stroke;
} canvas_circle_t;

typedef struct canvas_arc
{
    float cx;
    float cy;
    float r;
    float start_angle;
    float end_angle;
    float rotate;
    float scale;
    bool counterclockwise;
    canvas_fill_t fill;
    canvas_stroke_t stroke;
} canvas_arc_t;

typedef struct canvas_rectangle
{
    float x;
    float y;
    float rx;
    float ry;
    float width;
    float height;
    float rotate;
    float scale;
    canvas_fill_t fill;
    canvas_stroke_t stroke;
} canvas_rectangle_t;

/**
 * @link https://www.w3schools.com/graphics/svg_line.asp
 *
 */
typedef struct canvas_line
{
    float x1;
    float y1;
    float x2;
    float y2;
    float rotate;
    float scale;
    canvas_fill_t fill;
    canvas_stroke_t stroke;
} canvas_line_t;


#define CANVAS_POLYLINE_PIONTS_MAXNUM 24

/**
 * @link https://www.w3schools.com/graphics/svg_polyline.asp
 *
 */
typedef struct canvas_polyline
{
    int16_t points[CANVAS_POLYLINE_PIONTS_MAXNUM * 2];
    canvas_fill_t fill;
    canvas_stroke_t stroke;
} canvas_polyline_t;





typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);
    void (*draw_circle)(canvas_circle_t *circle, struct gui_dispdev *dc);
    void (*draw_rectangle)(canvas_rectangle_t *r, struct gui_dispdev *dc);
    void (*draw_arc)(canvas_arc_t *a, struct gui_dispdev *dc);
    void (*draw_line)(canvas_line_t *l, struct gui_dispdev *dc);
    void (*draw_polyline)(canvas_polyline_t *p, struct gui_dispdev *dc);
    void (*draw_path)(draw_path_t *path, struct gui_dispdev *dc);

    //todo
    void (*draw_svg)(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
                     float scale);

} acc_engine_t;


struct acc_engine *gui_get_acc(void);




#ifdef __cplusplus
}
#endif

#endif

