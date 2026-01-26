/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_svg.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_vfs.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#include "nanovg.h"

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride,
                                 enum NVGtexture format, uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

static void render_svg_shape(NVGcontext *vg, NSVGshape *shape, float scale, uint8_t opacity)
{
    for (NSVGpath *path = shape->paths; path != NULL; path = path->next)
    {
        nvgBeginPath(vg);

        if (path->npts > 0 && path->nsegs > 0)
        {
            nvgMoveTo(vg, path->pts[0] * scale, path->pts[1] * scale);

            int idx = 1;  // Start from first point after moveTo
            for (int i = 0; i < path->nsegs; i++)
            {
                if (path->types[i] == 0)  // Line
                {
                    float *p = &path->pts[idx * 2];
                    nvgLineTo(vg, p[0] * scale, p[1] * scale);
                    idx += 1;
                }
                else  // Bezier
                {
                    float *p = &path->pts[idx * 2];
                    nvgBezierTo(vg, p[0] * scale, p[1] * scale,
                                p[2] * scale, p[3] * scale,
                                p[4] * scale, p[5] * scale);
                    idx += 3;
                }
            }
        }

        if (path->closed)
        {
            nvgClosePath(vg);
        }

        if (shape->fill.type == NSVG_PAINT_COLOR)
        {
            unsigned int c = shape->fill.d.color;
            NVGcolor color = nvgRGBA(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF,
                                     (((c >> 24) & 0xFF) * opacity) / 255);
            nvgFillColor(vg, color);
            nvgFill(vg);
        }
        else if (shape->fill.type == NSVG_PAINT_LINEAR_GRADIENT)
        {
            NSVGgradient *grad = shape->fill.d.gradient;
            if (grad && grad->nstops >= 2)
            {
                unsigned int c0 = grad->stops[0].color;
                unsigned int c1 = grad->stops[grad->nstops - 1].color;
                NVGcolor icol = nvgRGBA(c0 & 0xFF, (c0 >> 8) & 0xFF, (c0 >> 16) & 0xFF,
                                        (((c0 >> 24) & 0xFF) * opacity) / 255);
                NVGcolor ocol = nvgRGBA(c1 & 0xFF, (c1 >> 8) & 0xFF, (c1 >> 16) & 0xFF,
                                        (((c1 >> 24) & 0xFF) * opacity) / 255);
                NVGpaint paint = nvgLinearGradient(vg,
                                                   shape->bounds[0] * scale, shape->bounds[1] * scale,
                                                   shape->bounds[2] * scale, shape->bounds[3] * scale,
                                                   icol, ocol);
                nvgFillPaint(vg, paint);
                nvgFill(vg);
            }
        }
        else if (shape->fill.type == NSVG_PAINT_RADIAL_GRADIENT)
        {
            NSVGgradient *grad = shape->fill.d.gradient;
            if (grad && grad->nstops >= 2)
            {
                unsigned int c0 = grad->stops[0].color;
                unsigned int c1 = grad->stops[grad->nstops - 1].color;
                NVGcolor icol = nvgRGBA(c0 & 0xFF, (c0 >> 8) & 0xFF, (c0 >> 16) & 0xFF,
                                        (((c0 >> 24) & 0xFF) * opacity) / 255);
                NVGcolor ocol = nvgRGBA(c1 & 0xFF, (c1 >> 8) & 0xFF, (c1 >> 16) & 0xFF,
                                        (((c1 >> 24) & 0xFF) * opacity) / 255);
                float cx = (shape->bounds[0] + shape->bounds[2]) * 0.5f * scale;
                float cy = (shape->bounds[1] + shape->bounds[3]) * 0.5f * scale;
                float r = (shape->bounds[2] - shape->bounds[0]) * 0.5f * scale;
                NVGpaint paint = nvgRadialGradient(vg, cx, cy, 0, r, icol, ocol);
                nvgFillPaint(vg, paint);
                nvgFill(vg);
            }
        }

        if (shape->stroke.type == NSVG_PAINT_COLOR)
        {
            unsigned int c = shape->stroke.d.color;
            NVGcolor color = nvgRGBA(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF,
                                     (((c >> 24) & 0xFF) * opacity) / 255);
            nvgStrokeColor(vg, color);
            nvgStrokeWidth(vg, shape->strokeWidth * scale);
            nvgStroke(vg);
        }
    }
}

static void gui_svg_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
}

static void gui_svg_draw(gui_obj_t *obj)
{
    gui_svg_t *_this = (gui_svg_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    if (_this->svg_data == NULL)
    {
        return;
    }

    NSVGimage *svg = (NSVGimage *)_this->svg_data;
    NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height,
                                   dc->fb_width * (dc->bit_depth >> 3),
                                   (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA,
                                   dc->frame_buf);

    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

    nvgScissor(vg, 0, 0, dc->fb_width, dc->fb_height);
    nvgResetTransform(vg);
    nvgTranslate(vg, 0, -(float)dc->fb_height * (float)dc->section_count);
    nvgScale(vg, _this->scale, _this->scale);
    nvgTransformxyz(vg,
                    obj->matrix->m[0][0], obj->matrix->m[1][0], obj->matrix->m[0][1],
                    obj->matrix->m[1][1], obj->matrix->m[0][2], obj->matrix->m[1][2],
                    obj->matrix->m[2][0], obj->matrix->m[2][1], obj->matrix->m[2][2]);

    for (NSVGshape *shape = svg->shapes; shape != NULL; shape = shape->next)
    {
        render_svg_shape(vg, shape, 1.0f, _this->opacity_value);
    }

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}

static void gui_svg_end(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
}

static void gui_svg_destroy(gui_obj_t *obj)
{
    gui_svg_t *_this = (gui_svg_t *)obj;

    if (_this->svg_data)
    {
        nsvgDelete((NSVGimage *)_this->svg_data);
        _this->svg_data = NULL;
    }
}

static void gui_svg_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj == NULL)
    {
        return;
    }

    switch (cb_type)
    {
    case OBJ_PREPARE:
        gui_svg_prepare(obj);
        break;
    case OBJ_DRAW:
        gui_svg_draw(obj);
        break;
    case OBJ_END:
        gui_svg_end(obj);
        break;
    case OBJ_DESTROY:
        gui_svg_destroy(obj);
        break;
    default:
        break;
    }
}

static void gui_svg_ctor(gui_svg_t  *_this,
                         gui_obj_t  *parent,
                         const char *name,
                         int16_t     x,
                         int16_t     y,
                         int16_t     w,
                         int16_t     h)
{
    gui_obj_ctor(&_this->base, parent, name, x, y, w, h);
    GET_BASE(_this)->type = IMAGE;
    GET_BASE(_this)->obj_cb = gui_svg_cb;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_draw_cb = true;
    GET_BASE(_this)->has_end_cb = true;
    GET_BASE(_this)->has_destroy_cb = true;

    _this->scale = 1.0f;
    _this->opacity_value = 255;
}

gui_svg_t *gui_svg_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   uint32_t    size,
                                   int16_t     x,
                                   int16_t     y)
{
    gui_svg_t *_this = gui_malloc(sizeof(gui_svg_t));
    memset(_this, 0, sizeof(gui_svg_t));

    gui_svg_ctor(_this, parent, name, x, y, 0, 0);

    char *svg_copy = gui_malloc(size + 1);
    memcpy(svg_copy, addr, size);
    svg_copy[size] = '\0';

    _this->svg_data = nsvgParse(svg_copy, "px", 96);
    gui_free(svg_copy);

    if (_this->svg_data)
    {
        NSVGimage *svg = (NSVGimage *)_this->svg_data;
        GET_BASE(_this)->w = (int16_t)svg->width;
        GET_BASE(_this)->h = (int16_t)svg->height;
    }

    gui_list_init(&GET_BASE(_this)->child_list);
    if (parent)
    {
        gui_list_insert_before(&((gui_obj_t *)parent)->child_list, &GET_BASE(_this)->brother_list);
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

gui_svg_t *gui_svg_create_from_file(void       *parent,
                                    const char *name,
                                    const char *filename,
                                    int16_t     x,
                                    int16_t     y)
{
    gui_svg_t *_this = gui_malloc(sizeof(gui_svg_t));
    memset(_this, 0, sizeof(gui_svg_t));

    gui_svg_ctor(_this, parent, name, x, y, 0, 0);
    _this->from_file = 1;

    gui_vfs_file_t *fd = gui_vfs_open(filename, GUI_VFS_READ);
    if (fd)
    {
        gui_vfs_stat_t stat;
        if (gui_vfs_stat(filename, &stat) == 0)
        {
            char *svg_buf = gui_malloc(stat.size + 1);
            if (svg_buf)
            {
                int read_size = gui_vfs_read(fd, svg_buf, stat.size);
                if (read_size > 0)
                {
                    svg_buf[read_size] = '\0';
                    _this->svg_data = nsvgParse(svg_buf, "px", 96);
                }
                gui_free(svg_buf);
            }
        }
        gui_vfs_close(fd);
    }

    if (_this->svg_data)
    {
        NSVGimage *svg = (NSVGimage *)_this->svg_data;
        GET_BASE(_this)->w = (int16_t)svg->width;
        GET_BASE(_this)->h = (int16_t)svg->height;
    }

    gui_list_init(&GET_BASE(_this)->child_list);
    if (parent)
    {
        gui_list_insert_before(&((gui_obj_t *)parent)->child_list, &GET_BASE(_this)->brother_list);
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

void gui_svg_set_scale(gui_svg_t *_this, float scale)
{
    if (_this && scale > 0)
    {
        _this->scale = scale;
    }
}

void gui_svg_set_opacity(gui_svg_t *_this, uint8_t opacity)
{
    if (_this)
    {
        _this->opacity_value = opacity;
    }
}

uint16_t gui_svg_get_width(gui_svg_t *_this)
{
    if (_this && _this->svg_data)
    {
        return (uint16_t)(((NSVGimage *)_this->svg_data)->width * _this->scale);
    }
    return 0;
}

uint16_t gui_svg_get_height(gui_svg_t *_this)
{
    if (_this && _this->svg_data)
    {
        return (uint16_t)(((NSVGimage *)_this->svg_data)->height * _this->scale);
    }
    return 0;
}
