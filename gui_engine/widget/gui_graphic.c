/*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author
* @date
* @version
*********************************************************************************************************
*/

#include <guidef.h>
#include <gui_api.h>
#include <gui_graphic.h>
#include <gui_obj.h>
#include <math.h>
int abs(int t)
{
    if (t > 0)
    {
        return t;
    }
    else
    {
        return -t;
    }
}


void set_point(gui_point_t *point, int16_t x, int16_t y)
{
    point->x = (float)x;
    point->y = (float)y;
    return;
}
void set_circle(gui_circleshape_t *circle, int16_t x, int16_t y, float r)
{
    circle->center.x = (float)x;
    circle->center.y = (float)y;
    circle->radius = r;
    return;
}
float calc_dis(gui_point_t *p1, gui_point_t *p2)
{
    return sqrtf((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y));
}
float calc_dis_square(gui_point_t *p1, gui_point_t *p2)
{
    return ((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y));
}
float calc_degree(gui_point_t *o, gui_point_t *p1, gui_point_t *p2)
{
    return acosf((calc_dis_square(o, p1) + calc_dis_square(o, p2) - calc_dis_square(p2,
                                                                                    p1)) / (2 * calc_dis(o, p1) * calc_dis(o, p2)));
}
void get_line_from_point(gui_point_t *point1, gui_point_t *point2, gui_line_t *line)
{
    if (point1->x == point2->x)
    {
        line->x = point1->x;
        line->vertical = true;
    }
    else
    {
        line->a = (point1->y - point2->y) / (point1->x - point2->x);
        line->b = (point2->y * point1->x - point1->y * point2->x) / (point1->x - point2->x);
        line->vertical = false;
    }
}
bool judge_point_above_line(gui_line_t *line, gui_point_t *point)
{
    if (line->vertical)
    {
        if (point->x <= line->x)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        float y = line->a * point->x + line->b; //gui_log("y =%f\n", y);
        if (point->y <= y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
void calc_equation_result(float a, float b, float c, float *x1, float *x2)
{
    float p = b * b - 4.0f * a * c;
    *x1 = (-b + sqrtf(p)) / (2.0f * a);
    *x2 = (-b - sqrtf(p)) / (2.0f * a);
}
void get_first_rect_point(gui_rect_t *rect, gui_point_t *point1, gui_point_t *point2, float width)
{
    //DBG_DIRECT("$$$$line89$$$$ point1.x = %f,point1.y =%f,point2.x = %f,point2.y =%f ",point1->x,point1->y,point2->x,point2->y);
    float hypotenuse;
    hypotenuse = sqrtf((point1->y - point2->y) * (point1->y - point2->y) + (point1->x - point2->x) *
                       (point1->x - point2->x));
    //DBG_DIRECT("$$$$line92$$$$ hypotenuse = %f",hypotenuse);
    float y = width / 2 * (point1->x - point2->x) / (hypotenuse);
    float x = (point1->y - point2->y) / (point1->x - point2->x) * y;
    //DBG_DIRECT("$$$$line95$$$$ y = %f,x = %f,",y,x);
    rect->p1.x = point2->x - x;
    rect->p1.y = point2->y + y;
    rect->p2.x = point2->x + x;
    rect->p2.y = point2->y - y;
    rect->p3.x = point1->x + x;
    rect->p3.y = point1->y - y;
    rect->p4.x = point1->x - x;
    rect->p4.y = point1->y + y;
    // DBG_DIRECT("$$$$line104$$$$ rect->p1.x = %f,rect->p1.y =%f,rect->p2.x= %f,rect->p2.y =%f ",rect->p1.x,rect->p1.y,rect->p2.x,rect->p2.y);
}
void get_rect_point(gui_rect_t *rect, gui_rect_t *rect_before, gui_point_t *point1,
                    gui_point_t *point2, float width)
{
    float hypotenuse;
    hypotenuse = sqrtf((point1->y - point2->y) * (point1->y - point2->y) + (point1->x - point2->x) *
                       (point1->x - point2->x));
    float y = width / 2 * (point1->x - point2->x) / (hypotenuse);
    float x = (point1->y - point2->y) / (point1->x - point2->x) * y;
    rect->p1.x = rect_before->p4.x - 1.0f;
    rect->p1.y = rect_before->p4.y;
    rect->p2.x = rect_before->p3.x - 1.0f;
    rect->p2.y = rect_before->p3.y;
    rect->p3.x = point1->x + x;
    rect->p3.y = point1->y - y;
    rect->p4.x = point1->x - x;
    rect->p4.y = point1->y + y;
}
void get_rect(gui_rect_t *rect, gui_rect_t *rect_before, gui_point_t *point1, gui_point_t *point2,
              float width, bool first)
{
    if (first)
    {
        get_first_rect_point(rect, point1, point2, width);
    }
    else
    {
        get_rect_point(rect, rect_before, point1, point2, width);
    }
    get_line_from_point(&rect->p1, &rect->p2, &rect->l1);
    get_line_from_point(&rect->p2, &rect->p3, &rect->l2);
    get_line_from_point(&rect->p3, &rect->p4, &rect->l3);
    get_line_from_point(&rect->p4, &rect->p1, &rect->l4);
    memcpy(rect_before, rect, sizeof(gui_rect_t));
}
void get_scale(gui_rect_t *rect, gui_scale_t *scale)
{
    float maxx = rect->p1.x;
    if (maxx < rect->p1.x)
    {
        maxx = rect->p1.x;
    }
    if (maxx < rect->p2.x)
    {
        maxx = rect->p2.x;
    }
    if (maxx < rect->p3.x)
    {
        maxx = rect->p3.x;
    }
    if (maxx < rect->p4.x)
    {
        maxx = rect->p4.x;
    }

    float minx = rect->p1.x;
    if (minx > rect->p1.x)
    {
        minx = rect->p1.x;
    }
    if (minx > rect->p2.x)
    {
        minx = rect->p2.x;
    }
    if (minx > rect->p3.x)
    {
        minx = rect->p3.x;
    }
    if (minx > rect->p4.x)
    {
        minx = rect->p4.x;
    }
    float maxy = rect->p1.y;
    if (maxy < rect->p1.y)
    {
        maxy = rect->p1.y;
    }
    if (maxy < rect->p2.y)
    {
        maxy = rect->p2.y;
    }
    if (maxy < rect->p3.y)
    {
        maxy = rect->p3.y;
    }
    if (maxy < rect->p4.y)
    {
        maxy = rect->p4.y;
    }
    float miny = rect->p1.y;
    if (miny > rect->p1.y)
    {
        miny = rect->p1.y;
    }
    if (miny > rect->p2.y)
    {
        miny = rect->p2.y;
    }
    if (miny > rect->p3.y)
    {
        miny = rect->p3.y;
    }
    if (miny > rect->p4.y)
    {
        miny = rect->p4.y;
    }
    scale->maxx = (int)ceilf(maxx);
    scale->maxy = (int)ceilf(maxy);
    scale->minx = (int)floor(minx);
    scale->miny = (int)floor(miny);
    if (scale->maxx < 0)
    {
        scale->maxx = 0;
    }
    if (scale->maxy < 0)
    {
        scale->maxy = 0;
    }
    if (scale->minx < 0)
    {
        scale->minx = 0;
    }
    if (scale->miny < 0)
    {
        scale->miny = 0;
    }
}
bool judge_point_in_rect(gui_rect_t *rect, gui_point_t *point)
{
    if (rect->l1.vertical || rect->l3.vertical)
    {
        //l1 right
        //DBG_DIRECT("$$$$judge point->y=%f,rect->l1.b = %f,rect->l1.a= %f",point->y ,rect->l1.b,rect->l1.a);
        float x = rect->l1.x;
        //DBG_DIRECT("$$$$judge ,x=%f,point->x = %f",x,point->x);
        if ((int)floorf(x) > (int)ceilf(point->x))
        {
            return false;
        }
        //l2 up
        float y = rect->l2.a * point->x + rect->l2.b;
        if ((int)floorf(y) > (int)ceilf(point->y))
        {
            return false;
        }
        //l3 left
        x = rect->l3.x;
        if ((int)ceilf(x) < (int)floorf(point->x))
        {
            return false;
        }
        //l4 down
        y = rect->l4.a * point->x + rect->l4.b;
        if ((int)ceilf(y) < (int)floorf(point->y))
        {
            return false;
        }
        return true;
    }
    else
    {
        //l1 right
        //DBG_DIRECT("$$$$judge point->y=%f,rect->l1.b = %f,rect->l1.a= %f",point->y ,rect->l1.b,rect->l1.a);
        float x = (point->y - rect->l1.b) / rect->l1.a;
        //DBG_DIRECT("$$$$judge ,x=%f,point->x = %f",x,point->x);
        if ((int)floorf(x) > (int)ceilf(point->x))
        {
            return false;
        }
        //l2 up
        float y = rect->l2.a * point->x + rect->l2.b;
        if ((int)floorf(y) > (int)ceilf(point->y))
        {
            return false;
        }
        //l3 left
        x = (point->y - rect->l3.b) / rect->l3.a;
        if ((int)ceilf(x) < (int)floorf(point->x))
        {
            return false;
        }
        //l4 down
        y = rect->l4.a * point->x + rect->l4.b;
        if ((int)ceilf(y) < (int)floorf(point->y))
        {
            return false;
        }
        return true;

    }


}
void get_scale_circle(gui_scale_t *scale, gui_circleshape_t *circle)
{
    scale->maxx = (int)ceil(circle->center.x + circle->radius);
    scale->maxy = (int)ceil(circle->center.y + circle->radius);
    scale->minx = (int)floor(circle->center.x - circle->radius);
    scale->miny = (int)floor(circle->center.y - circle->radius);
}

static void draw_rect(gui_graphic_t *this, int16_t x,
                      int16_t y, int16_t w, int16_t h, uint32_t color)
{
    this->rect_count = 1;
    this->rect.x = x;
    this->rect.y = y;
    this->rect.w = w;
    this->rect.h = h;
    this->rect.color = color;
}

static void graphic_draw(gui_obj_t *obj)
{
}
void gui_graphic_ctor(gui_graphic_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y, int16_t w, int16_t h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    root->obj_draw = graphic_draw;
    this->draw_rect = draw_rect;
}

gui_graphic_t *gui_graphic_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h)
{
#define _GUI_NEW_param_gui_graphic_create_ this, parent, filename, x, y, w, h
    GUI_NEW(gui_graphic_t, gui_graphic_ctor, _GUI_NEW_param_gui_graphic_create_)
}
/*
typedef struct _vector vector;
struct _vector
{
    uint8_t len;
    uint8_t (*at)(vector *this,uint8_t id);
    uint8_t vector[];
};
static uint8_t vector_at(vector *this, uint8_t id)
{
    GUI_ASSERT(id<this->len);
    return vector[id];
}
static void vector_pushback(vector *this, uint8_t src)
{

}
vector *create_vector(uint8_t len)
{
    GUI_ASSERT(len>0);
    vector *buf = (vector *)gui_malloc(sizeof(vector) + len);
    buf->at = vector_at;
    return buf;
}
*/








#define GET_FUNCTION_DRAW_CIRCLE(GPU) (draw_circle_##GPU)
#define GET_FUNCTION_DRAW_RECT(GPU) (draw_rect_##GPU)
#define GET_FUNCTION_DRAW_ROUND_CORNER_RECT(GPU) (draw_round_corner_rect_##GPU)
#define DEFINE_FUNCTION_DRAW_CIRCLE(GPU) static void GET_FUNCTION_DRAW_CIRCLE(GPU)(gui_canvas_t *this, int16_t center_x, int16_t center_y, int16_t radius)
#define DEFINE_FUNCTION_DRAW_RECT(GPU) static void GET_FUNCTION_DRAW_RECT(GPU)(gui_canvas_t *this, int16_t x, int16_t y, int16_t w, int16_t h)

#if RTK_GUI_GPU_NANOVG
#include "agge/nanovg_agge.h"
static void drawGraph(NVGcontext *vg, float x, float y, float w, float h, float t)
{
    NVGpaint bg;
    float samples[6];
    float sx[6], sy[6];
    float dx = w / 5.0f;
    int i;

    samples[0] = (1 + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1 + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1 + sinf(t * 1.1642f + cosf(t * 0.33457) * 1.24f)) * 0.5f;
    samples[3] = (1 + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1 + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1 + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;

    for (i = 0; i < 6; i++)
    {
        sx[i] = x + i * dx;
        sy[i] = y + h * samples[i] * 0.8f;
    }

    // Graph background
    bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(0, 160, 192, 0), nvgRGBA(0, 160, 192, 100));
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 6; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }
    nvgLineTo(vg, x + w, y + h);
    nvgLineTo(vg, x, y + h);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    // Graph line
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0] + 2);
    for (i = 1; i < 6; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i],
                    sy[i] + 2);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 6; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 160, 192, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // Graph sample pos
    for (i = 0; i < 6; i++)
    {
        bg = nvgRadialGradient(vg, sx[i], sy[i] + 2, 3.0f, 8.0f, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 0));
        nvgBeginPath(vg);
        nvgRect(vg, sx[i] - 10, sy[i] - 10 + 2, 20, 20);
        nvgFillPaint(vg, bg);
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    for (i = 0; i < 6; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 4.0f);
    }
    nvgFillColor(vg, nvgRGBA(0, 160, 192, 255));
    nvgFill(vg);
    nvgBeginPath(vg);
    for (i = 0; i < 6; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 2.0f);
    }
    nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
    nvgFill(vg);

    nvgStrokeWidth(vg, 1.0f);
}
float pi_per;
static void canvas_draw(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    size_t w = dc->fb_width;
    size_t h = dc->fb_height;
    char BPP = 2;
    int32_t size = w * h * BPP;
    //uint8_t* data = (uint8_t*)gui_malloc(size);printf("data:%x\n",data);
    uint8_t *data = dc->frame_buf;
    NVGcontext *vg = nvgCreateAGGE(w, h, w * BPP, BPP == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA,
                                   data);
    uint32_t *d1 = data;
    memset(data, 0x0, size);

    nvgBeginFrame(vg, w, h, 1);
    nvgSave(vg);
//    nvgBeginPath(vg);
//    //nvgArc(vg, 320/2,320/2, 100, pi_per*2*M_PI-2.5f, pi_per*2*M_PI, NVG_CW);
//    nvgArc(vg, 320/2,320/2, 100, 0, 1.5f*M_PI, NVG_CW);
//  pi_per += 0.01f;
//    if (pi_per > 1.0f)
//    {
//        pi_per = 0;
//    }
//
//    //nvgCircle(vg, 320/2,320/2, 100);
//    //int offset = 10;
//    //nvgRect(vg, offset,offset,w-offset*2,h-offset*2);
//    //nvgFillColor(vg, nvgRGBA(0xff,0xff, 0x00,0x66));
//    NVGpaint paint;
//  //paint = nvgLinearGradient(vg, 10,10, 310,310, nvgRGBA(255,255,0,0xff), nvgRGBA(0,255,255,0xff));
//  paint = nvgRadialGradient(vg, 320/2,320/2, 80, 150, nvgRGBA(0,255,0,0xff), nvgRGBA(0,255,0,0x1));
//  //paint = nvgBoxGradient(vg, 5,5, 310,310, 115, 20,nvgRGBA(0,255,0,0xff), nvgRGBA(0,255,0,0x33));
//
//  //nvgFillPaint(vg, paint);
//  //nvgFill(vg);
//    nvgStrokeWidth(vg, 50.0f);
//  //nvgStrokeColor(vg, nvgRGBA(0,255,255,255));
//    nvgStrokePaint(vg, paint);
//    nvgLineCap(vg, NVG_ROUND);
//    nvgStroke(vg);

    /*  // Center triangle
        nvgBeginPath(vg);
        nvgMoveTo(vg, 20,20);
        nvgLineTo(vg, 300,300);
        nvgLineTo(vg, 20,300);
        nvgClosePath(vg);
        paint = nvgLinearGradient(vg, 20,20, 300,300, nvgRGBA(255,0,0,60), nvgRGBA(255,0,0,255));


        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgStrokeColor(vg, nvgRGBA(255,255,0,255));
        nvgStrokeWidth(vg, 5.0f);
        nvgStroke(vg);
        */
    /*
        nvgBeginPath(vg);
        nvgCircle(vg, 200,200,100);
        nvgFillColor(vg, nvgRGBA(0xff,0x00, 0x00,0x00));
        nvgFill(vg);
        nvgStrokeWidth(vg, 20.0f);
        nvgStrokeColor(vg, nvgRGBA(0,0,255,100));
        nvgStroke(vg);
    */
    /*
        nvgBeginPath(vg);
    nvgRect(vg, 100,100, 120,30);
    nvgCircle(vg, 115,115, 10);
    nvgPathWinding(vg, NVG_HOLE);   // Mark circle as a hole.
    nvgFillColor(vg, nvgRGBA(255,192,0,255));
    nvgFill(vg);
    */
    /*
        nvgBeginPath(vg);
        nvgLineCap(vg, 0);
        nvgMoveTo(vg, 10, 30);
        nvgLineTo(vg, 100, 30);
        nvgArcTo(vg, 100,30, 200,20, 200);
        nvgStrokeWidth(vg, 20);
        nvgStrokeColor(vg, nvgRGBA(0,0,255,200));
        nvgStroke(vg);
    */
    drawGraph(vg, 0, 0, 320, 320, 1);
    nvgRestore(vg);
    nvgEndFrame(vg);

    nvgDeleteAGGE(vg);


}





DEFINE_FUNCTION_DRAW_CIRCLE(nanovg)
{
    uint8_t *data = NULL;
    int w = 0;
    int h = 0;
    {
        data = (uint8_t *)(this->base.base.data) + sizeof(struct gui_img_file_head);
        w = GET_BASE(this)->w;
        h = GET_BASE(this)->h;
    }
    //this->base.base.color_type = RGBA8888;
    gui_color_t color = {.rgba = this->base.color};
    NVGcontext *vg = nvgCreateAGGE(w, h, w * 2, NVG_TEXTURE_BGR565, data);
    nvgBeginFrame(vg, w, h, 1);
    nvgBeginPath(vg);
    nvgCircle(vg, center_x, center_y, radius);
    nvgFillColor(vg, nvgRGBA(color.red, color.green, color.blue, color.alpha));
    nvgFill(vg);
    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}


static void (draw_rect_nanovg)(gui_canvas_t *this, int16_t center_x, int16_t center_y,
                               int16_t radius)
{
    uint8_t *data = NULL;
    int w = 0;
    int h = 0;
    {
        data = (uint8_t *)(this->base.base.data) + sizeof(struct gui_img_file_head);
        w = GET_BASE(this)->w;
        h = GET_BASE(this)->h;
    }
    //this->base.base.color_type = RGBA8888;
    gui_color_t color = {.rgba = this->base.color};
    NVGcontext *vg = nvgCreateAGGE(w, h, w * 2, NVG_TEXTURE_BGR565, data);
    nvgBeginFrame(vg, w, h, 1);
    nvgBeginPath(vg);
    nvgCircle(vg, center_x, center_y, radius);
    nvgFillColor(vg, nvgRGBA(color.red, color.green, color.blue, color.alpha));
    nvgFill(vg);
    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}
#endif




