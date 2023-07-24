#include "gui_canvas.h"
#include <math.h>
void (set_color)(void *o, uint32_t color)
{
    gui_canvas_t *this = o;
    this->base.color = color;
}
static void set_bit_to_bitmap(uint8_t *bitmap, int row_start, int col_start, int w, int h)
{
    int length = row_start * w + col_start;
    if (length < 0)
    {
        return;
    }
    if (length >= w * h)
    {
        return;
    }


    (bitmap[length / 8]) = (1 << (length % 8)) | (bitmap[length / 8]);
    //gui_log("%x ", bitmap[length / 8]);
}

uint32_t get_color(gui_canvas_t *this)
{
    return this->base.color;
}

bool judge_point_in_circle(gui_circleshape_t *circle, gui_point_t *point)
{
    float center_to_point;
    center_to_point = sqrtf((circle->center.x - point->x) * (circle->center.x - point->x) +
                            (circle->center.y - point->y) * (circle->center.y - point->y));
    if ((center_to_point + 0.4f) < (circle->radius))
    {
        return true;
    }
    else
    {
        return false;
    }
}
#include "acc_engine.h"
void draw_circle(gui_canvas_t *this, int16_t center_x, int16_t center_y, int16_t radius)
{

}


static void clear_bit_to_bitmap(uint8_t *bitmap, int row_start, int col_start, int w, int h)
{
    int length = row_start * w + col_start;
    (bitmap[length / 8]) = (~(1 << (length % 8))) & (bitmap[length / 8]);
}

void erase_circle(gui_canvas_t *this, int16_t center_x, int16_t center_y, int16_t radius)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    uint8_t *bitmap = (uint8_t *)(draw_img->data) + sizeof(gui_img_file_head_t);
    gui_circleshape_t circle;
    circle.center.x = (float)center_x;
    circle.center.y = (float)center_y;
    circle.radius = (float)radius;
    for (int i = (int16_t)(center_y - radius); i < radius * 2 + (int16_t)(center_y - radius); i++)
    {
        for (int j = (int16_t)(center_x - radius); j < radius * 2 + (int16_t)(center_x - radius); j++)
        {
            gui_point_t point = {.x = (float)j, .y = (float)i};
            if (judge_point_in_circle(&(circle), &point))
            {
                clear_bit_to_bitmap(bitmap, i, j, this->base.base.base.w, this->base.base.base.h);
            }
        }
    }
}
void draw_rectangle(gui_canvas_t *this, int16_t x, int16_t y, int16_t w, int16_t h)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    uint8_t *bitmap = (uint8_t *)(draw_img->data) + sizeof(gui_img_file_head_t);
    for (size_t i = y; i < y + h; i++)
    {
        for (size_t j = x; j < x + w; j++)
        {
            set_bit_to_bitmap(bitmap, i, j, this->base.base.base.w, this->base.base.base.h);
        }

    }

}
void draw_bar_v(gui_canvas_t *this, int16_t x, int16_t y, int16_t w, int16_t h)
{
    int rect_x, recty, rectw, recth, c1_x, c1_y, c2_x, c2_y, cr;
    rect_x = 0; recty = 0; rectw = 0; recth = 0; c1_x = 0; c1_y = 0; c2_x = 0; c2_y = 0; cr = 0;
    cr = w / 2 + w % 2;
    c1_x = w / 2 + x;
    c2_x = w / 2 + x;
    c1_y = w / 2 + y;
    c2_y = h - w / 2 + y;
    rectw = w;
    recth = h - w / 2 * 2 + 1;
    rect_x = x;
    recty = c1_y;
    draw_rectangle(this, rect_x, recty, rectw, recth);
    draw_circle(this, c1_x, c1_y, cr);
    draw_circle(this, c2_x, c2_y, cr);
}
void (draw_round_rect)(gui_canvas_t *this, int16_t x, int16_t y, int16_t w, int16_t h)
{
    int rect_x, recty, rectw, recth, c1_x, c1_y, c2_x, c2_y, cr;
    rect_x = 0; recty = 0; rectw = 0; recth = 0; c1_x = 0; c1_y = 0; c2_x = 0; c2_y = 0; cr = 0;
    if (w > h)
    {
        cr = h / 2;
        c1_y = cr;
        c2_y = cr;
        c1_x = cr;
        c2_x = w - cr;
        recth = h;
        rectw = w - h;
        rect_x = c1_x;
        recty = 0;
    }
    else if (w < h)
    {
        cr = w / 2;
        c1_x = cr;
        c2_x = cr;
        c1_y = cr;
        c2_y = h - cr;
        rectw = w;
        recth = h - w;
        rect_x = 0;
        recty = c1_y;
    }
    else if (w == h)
    {
        cr = w / 2;
        c1_x = cr;
        c2_x = cr;
        c1_y = cr;
        c2_y = h - cr;
        rectw = w;
        recth = h - w + 1;
        rect_x = 0;
        recty = c1_y;
    }
    draw_rectangle(this, rect_x, recty, rectw, recth);
    draw_circle(this, c1_x, c1_y, cr);
    draw_circle(this, c2_x, c2_y, cr);
}
void erase(gui_canvas_t *this)
{
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    memset((uint8_t *)(draw_img->data) + sizeof(gui_img_file_head_t), 0,
           this->base.base.base.w * this->base.base.base.h / 8 + 1);
}
void draw_ring(gui_canvas_t *this, int16_t center_x, int16_t center_y, int16_t radius,
               uint16_t width)
{
    draw_circle(this, center_x, center_y, radius + width);
    erase_circle(this, center_x, center_y, radius);
}
void draw_round_corner_rect(gui_canvas_t *this, int16_t x, int16_t y, int16_t w, int16_t h,
                            int16_t corner_radius)
{
    int16_t corner1_x, corner1_y, corner2_x, corner2_y, corner3_x, corner3_y, corner4_x, corner4_y;
    int16_t rect1_x, rect1_y, rect2_x, rect2_y, rect3_x, rect3_y, rect1_w, rect1_h, rect2_w, rect2_h,
            rect3_w, rect3_h;
    corner1_x = corner_radius;
    corner1_y = corner_radius;
    corner2_x = w - corner_radius - 1;
    corner2_y = corner1_y;
    corner3_x = corner1_x;
    corner3_y = h - corner_radius;
    corner4_x = corner2_x;
    corner4_y = corner3_y;
    rect1_x = corner1_x;
    rect1_y = 0;
    rect1_w = w - 2 * corner_radius;
    rect1_h = corner_radius;
    rect2_x = 0;
    rect2_y = corner_radius;
    rect2_w = w;
    rect2_h = h - 2 * corner_radius;
    rect3_x = rect1_x;
    rect3_y = corner3_y;
    rect3_w = rect1_w;
    rect3_h = corner_radius;
    draw_circle(this, corner1_x, corner1_y, corner_radius + 1);
    draw_circle(this, corner2_x, corner2_y, corner_radius + 1);
    draw_circle(this, corner3_x, corner3_y - 1, corner_radius + 1);
    draw_circle(this, corner4_x, corner4_y - 1, corner_radius + 1);
    draw_rectangle(this, rect1_x, rect1_y, rect1_w, rect1_h);
    draw_rectangle(this, rect2_x, rect2_y, rect2_w, rect2_h);
    draw_rectangle(this, rect3_x, rect3_y, rect3_w, rect3_h);
}

void get_line_from_point_and_degree(gui_point_t *point1, float degree, gui_line_t *line)
{
    if (degree == 0 || degree == 180 || degree == 360)
    {
        line->x = point1->x;
        line->vertical = true;
    }
    else
    {
        line->a = tan(-(90.0f - degree) / 180.f * 3.141592654f);
        line->b = point1->y - line->a * point1->x;
        line->vertical = false;
    }
}

void get_piont_line_cross_circle(int cx, int cy, int r, gui_line_t *l, gui_point_t *result,
                                 bool if_left)
{
    if (l->vertical)
    {
        if (if_left)
        {
            result->x = cx;
            result->y = cy - r;
        }
        else
        {
            result->x = cx;
            result->y = cy + r;
        }
    }
    else
    {
        float k = l->a;
        float b = l->b;
        /*
            (1 + k^2)*x^2 - x*(2*cx -2*k*(b -cy) ) + cx*cx + ( b - cy)*(b - cy) - r*r = 0
        */

        float c = cx * cx + (b - cy) * (b - cy) - r * r;
        float a = (1 + k * k);
        float b1 = (2 * cx - 2 * k * (b - cy));
        // a*x^2 - b1*x + c = 0;

        float tmp = sqrt(b1 * b1 - 4 * a * c);
        if (if_left)
        {
            result->x = (b1 - tmp) / (2 * a);
            result->y = k * result->x + b;
        }
        else
        {
            result->x = (b1 + tmp) / (2 * a);
            result->y = k * result->x + b;
        }
    }
}
void (draw_arc)(gui_canvas_t *this, int16_t center_x, int16_t center_y, int16_t radius,
                uint16_t width, int16_t start_degreee, int16_t end_degreee, bool round)
{
    int16_t start_degree = start_degreee % 360;
    int16_t end_degree = end_degreee % 360;
    if (start_degreee == 360)
    {
        start_degree = 360;
    }
    if (end_degreee == 360)
    {
        end_degree = 360;
    }
    if (end_degree != start_degree)
    {


        draw_ring(this, center_x, center_y, radius, width);
        gui_point_t center = {.x = center_x, .y = center_y};
        gui_line_t cut_line, cut_line2;


        get_line_from_point_and_degree(&center, start_degree, &cut_line);
        get_line_from_point_and_degree(&center, end_degree, &cut_line2);
        bool start_judge_show = false;
        bool end_judge_show = false;

        gui_img_t *img = (gui_img_t *)this;
        draw_img_t *draw_img = &img->draw_img;

        uint8_t *bitmap = (uint8_t *)(draw_img->data) + sizeof(gui_img_file_head_t);

        uint32_t bitmapw = this->base.base.base.w;
        uint32_t bitmaph = this->base.base.base.h;
        for (size_t i = center_y - radius - width; i < center_y + radius + width; i++)
        {
            for (size_t j = center_x - radius - width; j < center_x + radius + width; j++)
            {
                if (get_bit_from_bitmap(bitmap, i, j, bitmapw, bitmaph))
                {
                    gui_point_t bitpoint = {.x = j, .y = i};
                    bool start_up = judge_point_above_line(&cut_line, &bitpoint);
                    bool end_up = judge_point_above_line(&cut_line2, &bitpoint);
                    if (start_degree < 180)
                    {
                        if (j < center_x)
                        {
                            start_judge_show = true;
                        }
                        else
                        {
                            start_judge_show = !start_up;
                        }
                    }
                    else
                    {
                        if (j < center_x)
                        {
                            start_judge_show = start_up;
                        }
                        else
                        {
                            start_judge_show = false;
                        }

                    }
                    if (start_degree == 0)
                    {
                        start_judge_show = true;
                    }
                    if (start_degree == 360)
                    {
                        start_judge_show = false;
                    }


                    if (end_degree < 180)
                    {
                        if (j >= center_x)
                        {
                            end_judge_show = end_up;
                        }
                        else
                        {
                            end_judge_show = false;
                        }

                    }
                    else
                    {
                        if (j >= center_x)
                        {
                            end_judge_show = true;
                        }
                        else
                        {
                            end_judge_show = !end_up;
                        }

                    }
                    if (end_degree == 360)
                    {
                        end_judge_show = true;
                    }
                    if (end_degree == 0)
                    {
                        end_judge_show = false;
                    }

                    if (end_degree > start_degree)
                    {
                        if (!(end_judge_show && start_judge_show))
                        {
                            clear_bit_to_bitmap(bitmap, i, j, this->base.base.base.w, this->base.base.base.h);
                        }
                    }
                    else if (end_degree < start_degree)
                    {
                        if (!(end_judge_show || start_judge_show))
                        {
                            clear_bit_to_bitmap(bitmap, i, j, this->base.base.base.w, this->base.base.base.h);
                        }
                    }
                }

            }

        }
        if (round)
        {
            gui_point_t cstart; bool start_left = true;
            gui_point_t cend; bool end_left = true;
            if (start_degree > 0 && start_degree <= 180)
            {
                start_left = false;
            }
            if (end_degree > 0 && end_degree <= 180)
            {
                end_left = false;
            }
            get_piont_line_cross_circle(center_x, center_y, radius + width / 2, &cut_line, &cstart, start_left);
            get_piont_line_cross_circle(center_x, center_y, radius + width / 2, &cut_line2, &cend, end_left);
            draw_circle(this, (int)(cstart.x), (int)(cstart.y), width / 2);
            draw_circle(this, (int)(cend.x), (int)(cend.y), width / 2);

        }


    }

}
#include "gui_server.h"
static void (obj_update_att)(struct _gui_obj_t *o)
{
    gui_canvas_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * (1000 / 15) / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

    }
}
void set_animate(gui_canvas_t *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_canvas_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_canvas_t *)o)->animate = animate;
}
static void (circle)(gui_canvas_t *this, canvas_circle_t *circle_data)
{

//    struct gui_dispdev dc = {.frame_buf = (uint8_t *)(this->base.base.draw_img.data) + sizeof(gui_img_file_head_t), .fb_height = GET_BASE(this)->h, .fb_width = GET_BASE(this)->w};
    circle_data->cx += GET_BASE(this)->dx;
    circle_data->cy += GET_BASE(this)->dy;
    gui_get_acc()->draw_circle(circle_data, gui_get_dc());
}
static void (rectangle)(gui_canvas_t *this, canvas_rectangle_t *rect_data)
{
    rect_data->x += GET_BASE(this)->dx;
    rect_data->y += GET_BASE(this)->dy;
    gui_get_acc()->draw_rectangle(rect_data, gui_get_dc());
}
static void (arc)(gui_canvas_t *this, const canvas_arc_t *data)
{
    canvas_arc_t rect;
    canvas_arc_t *rect_data = &rect;
    memcpy(rect_data, data, sizeof(canvas_arc_t));
    rect_data->cx += (GET_BASE(this)->dx);
    rect_data->cy += (GET_BASE(this)->dy);
    rect_data->r = rect_data->r * this->sx;
    rect_data->stroke.stroke_width = rect_data->stroke.stroke_width * (this->sx);
    rect_data->stroke.fill.color_data.rgba = rgba_mask_alpha(rect_data->stroke.fill.color_data.rgba,
                                                             this->opacity_value);
    gui_get_acc()->draw_arc(rect_data, gui_get_dc());
}
static void (bezier_curve)(gui_canvas_t *this, const canvas_bezier_curve_t *data)
{
    canvas_bezier_curve_t rect;
    canvas_bezier_curve_t *rect_data = &rect;
    memcpy(rect_data, data, sizeof(canvas_bezier_curve_t));
    rect_data->end_x += (GET_BASE(this)->dx);
    rect_data->control_x_1 += (GET_BASE(this)->dx);
    rect_data->start_x += (GET_BASE(this)->dx);
    rect_data->control_x_2 += (GET_BASE(this)->dx);
    rect_data->end_y += (GET_BASE(this)->dy);
    rect_data->control_y_1 += (GET_BASE(this)->dy);
    rect_data->start_y += (GET_BASE(this)->dy);
    rect_data->control_y_2 += (GET_BASE(this)->dy);
    rect_data->stroke.stroke_width = rect_data->stroke.stroke_width * (this->sx);
    rect_data->stroke.fill.color_data.rgba = rgba_mask_alpha(rect_data->stroke.fill.color_data.rgba,
                                                             this->opacity_value);
    gui_get_acc()->draw_bezier_curve(rect_data, gui_get_dc());
}
static void (line)(gui_canvas_t *this, canvas_line_t *line_data)
{
    line_data->x1 += GET_BASE(this)->dx;
    line_data->y1 += GET_BASE(this)->dy;
    line_data->x2 += GET_BASE(this)->dx;
    line_data->y2 += GET_BASE(this)->dy;
    gui_get_acc()->draw_line(line_data, gui_get_dc());
}
static void (obj_draw)(struct _gui_obj_t *obj)
{
    gui_canvas_t *this = (void *)obj;
    this->draw(this);
}
void (gui_canvas_ctor)(gui_canvas_t *this, gui_obj_t *parent, const char *name, int16_t x,
                       int16_t y,
                       int16_t w, int16_t h, uint32_t color)
{
    this->base.ctor = gui_bitmap_ctor;
    this->base.ctor(&(this->base), parent, name, NULL, x, y, w, h, color);

    GET_BASE(this)->obj_prepare = NULL;
    GET_BASE(this)->obj_end = NULL;
    GET_BASE(this)->obj_draw = obj_draw;
    GET_BASE(this)->obj_update_att = obj_update_att;
    GET_BASE(this)->type = CANVAS;
    gui_canvas_api.scale(this, 1, 1);
    this->opacity_value = UINT8_MAX;


}
gui_canvas_t *gui_canvas_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w,
                                int16_t h, uint32_t color)
{
#define _GUI_NEW_param_gui_canvas_create_ this, parent, name, x, y, w, h, color
    GUI_NEW(gui_canvas_t, gui_canvas_ctor, _GUI_NEW_param_gui_canvas_create_)
}
static void scale(gui_canvas_t *this, float sx, float sy)
{
    this->sx = sx;
    this->sy = sy;
}
static void translate(gui_canvas_t *this, float tx, float ty)
{
    this->tx = tx;
    this->ty = ty;
}
static void (svg)(gui_canvas_t *this, void *svg, uint32_t data_length, int x, int y, float scale)
{
    x += GET_BASE(this)->dx;
    y += GET_BASE(this)->dy;
    scale *= scale * this->sx;
    gui_get_acc()->draw_svg(svg, data_length, gui_get_dc(), x, y, scale, 0, 0, 0);
}
static void (palette_wheel)(gui_canvas_t *this, canvas_palette_wheel_t *pw)
{
    pw->x += GET_BASE(this)->dx + this->tx;
    pw->y += GET_BASE(this)->dy + this->ty;
    pw->w = pw->w;
    pw->h = pw->h;
    pw->scale = this->sx;
    gui_get_acc()->draw_palette_wheel(pw, gui_get_dc());
}
static float get_scale_offset_x(float x, float scale_x)
{
    /**
     * @note (1-scale)(center-x)
     *
     */
    return (1.0f - scale_x) * (((float)(gui_get_screen_width() / 2)) - (float)(x));
}
static void (wave)(gui_canvas_t *this, canvas_wave_t *wave)
{
    wave->x += GET_BASE(this)->dx + this->tx;
    wave->y += GET_BASE(this)->dy + this->ty; wave->w = wave->w * this->sx;
    wave->h = wave->h * this->sx;
    for (size_t i = 0; i < wave->point_count; i++)
    {
        wave->point_x[i] += GET_BASE(this)->dx + get_scale_offset_x(wave->point_x[i], this->sx);
        wave->point_y[i] += GET_BASE(this)->dy + get_scale_offset_x(wave->point_y[i], this->sy);
    }

    gui_get_acc()->draw_wave(wave, gui_get_dc());
}
gui_api_canvas_t gui_canvas_api =
{
    .circle = circle,
    .rectangle = rectangle,
    .arc = arc,
    .line = line,
    .scale = scale,
    .translate = translate,
    .svg = svg,
    .set_animate = set_animate,
    .wave = wave,
    .palette_wheel = palette_wheel,
    .bezier_curve = bezier_curve
};
