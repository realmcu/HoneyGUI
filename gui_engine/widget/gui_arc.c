/*
 * File      : gui_arc.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "gui_arc.h"


static void arc_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
}

void gui_arc_set(gui_arc_t *this, float cx, float cy, float r, float a0_degree, float a1_degree,
                 int dir)
{
    this->cx = cx;
    this->cy = cy;
    this->r = r;
    this->a0 = nvgDegToRad(a0_degree);
    this->a1 = nvgDegToRad(a1_degree);
    this->dir = dir;

}

void gui_arc_set_stroke(gui_arc_t *this, gui_color_t color, float w)
{
    this->color = nvgRGBA(color.channel.red, color.channel.green, color.channel.blue,
                          color.channel.alpha);
    this->width = w;

}

static void draw_arc(gui_arc_t *this, NVGcontext *vg)
{
    gui_dispdev_t *dc = gui_get_dc();
    nvgBeginPath(vg);
    nvgResetTransform(vg);
    nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);

    nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
    nvgScale(vg, this->base.sx, this->base.sy);
    nvgTranslate(vg, -dc->screen_width / 2, -dc->screen_height / 2);

    nvgLineCap(vg, NVG_ROUND);
    nvgArc(vg, this->cx, this->cy, this->r, this->a0, this->a1, NVG_CW);

    nvgStrokeColor(vg, this->color);
    nvgStrokeWidth(vg, this->width);
    nvgStroke(vg);
}

static void arc_draw_cb(gui_obj_t *obj)
{
    gui_arc_t *this = (gui_arc_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                     uint8_t *data);
    extern void nvgDeleteAGGE(NVGcontext * ctx);
    NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                   (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
    draw_arc(this, vg);
    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);

}
static void arc_end(gui_obj_t *obj)
{

}
static void arc_destory(gui_obj_t *obj)
{

}



static void arc_ctor(gui_arc_t *this, gui_obj_t *parent, const char *name,
                     int16_t x,
                     int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_prepare = arc_prepare;
    root->obj_draw = arc_draw_cb;
    root->obj_end = arc_end;
    root->obj_destory = arc_destory;

    //for self
    this->color = nvgRGBA(0xFF, 0, 0, 128);
    this->width = 20.0;

}



gui_arc_t *gui_arc_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                          int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "arc";
    }
    gui_arc_t *this = gui_malloc(sizeof(gui_arc_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_arc_t));

    arc_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }


    GET_BASE(this)->create_done = true;
    return this;
}



