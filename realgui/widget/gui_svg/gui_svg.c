/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_svg.c
  * @brief SVG widget implementation.
  * @details SVG widget is used to display SVG images on the screen.
  * @author
  * @date 2024/12/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <string.h>
#include "gui_svg.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_vfs.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

static void gui_svg_prepare(gui_obj_t *obj)
{
    gui_svg_t *_this = (gui_svg_t *)obj;
    GUI_UNUSED(_this);
}

static void gui_svg_draw(gui_obj_t *obj)
{
    gui_svg_t *_this = (gui_svg_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    if (_this->image_data == NULL || _this->svg_data == NULL)
    {
        return;
    }

    NSVGimage *svg = (NSVGimage *)_this->svg_data;
    NSVGrasterizer *rast = (NSVGrasterizer *)_this->rasterizer;

    if (rast == NULL)
    {
        return;
    }

    int w = (int)(svg->width * _this->scale);
    int h = (int)(svg->height * _this->scale);

    if (w != _this->img_w || h != _this->img_h)
    {
        _this->img_w = w;
        _this->img_h = h;
        if (_this->image_data)
        {
            gui_free(_this->image_data);
        }
        _this->image_data = gui_malloc(w * h * 4);
        if (_this->image_data == NULL)
        {
            return;
        }
        memset(_this->image_data, 0, w * h * 4);
        nsvgRasterize(rast, svg, 0, 0, _this->scale, _this->image_data, w, h, w * 4);
    }

    int x_start = obj->x + obj->parent->x;
    int y_start = obj->y + obj->parent->y;

    uint8_t *src = (uint8_t *)_this->image_data;
    uint8_t dc_bytes = dc->bit_depth >> 3;

    for (int y = 0; y < h; y++)
    {
        int screen_y = y_start + y;
        if (screen_y < 0 || screen_y >= dc->fb_height)
        {
            continue;
        }
        if (screen_y < dc->section.y1 || screen_y > dc->section.y2)
        {
            continue;
        }

        for (int x = 0; x < w; x++)
        {
            int screen_x = x_start + x;
            if (screen_x < 0 || screen_x >= dc->fb_width)
            {
                continue;
            }

            uint8_t *pixel = src + (y * w + x) * 4;
            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];
            uint8_t a = pixel[3];

            if (a == 0)
            {
                continue;
            }

            a = (a * _this->opacity_value) / 255;

            int fb_offset = (screen_y - dc->section.y1) * dc->fb_width + screen_x;

            if (dc_bytes == 2)
            {
                uint16_t *fb = (uint16_t *)dc->frame_buf;
                uint16_t color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                if (a == 255)
                {
                    fb[fb_offset] = color;
                }
                else
                {
                    uint16_t bg = fb[fb_offset];
                    uint8_t bg_r = ((bg >> 11) & 0x1F) << 3;
                    uint8_t bg_g = ((bg >> 5) & 0x3F) << 2;
                    uint8_t bg_b = (bg & 0x1F) << 3;
                    r = (r * a + bg_r * (255 - a)) / 255;
                    g = (g * a + bg_g * (255 - a)) / 255;
                    b = (b * a + bg_b * (255 - a)) / 255;
                    fb[fb_offset] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                }
            }
            else if (dc_bytes == 4)
            {
                uint32_t *fb = (uint32_t *)dc->frame_buf;
                if (a == 255)
                {
                    fb[fb_offset] = (0xFFU << 24) | (r << 16) | (g << 8) | b;
                }
                else
                {
                    uint32_t bg = fb[fb_offset];
                    uint8_t bg_r = (bg >> 16) & 0xFF;
                    uint8_t bg_g = (bg >> 8) & 0xFF;
                    uint8_t bg_b = bg & 0xFF;
                    r = (r * a + bg_r * (255 - a)) / 255;
                    g = (g * a + bg_g * (255 - a)) / 255;
                    b = (b * a + bg_b * (255 - a)) / 255;
                    fb[fb_offset] = (0xFFU << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }
    }
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
    if (_this->rasterizer)
    {
        nsvgDeleteRasterizer((NSVGrasterizer *)_this->rasterizer);
        _this->rasterizer = NULL;
    }
    if (_this->image_data)
    {
        gui_free(_this->image_data);
        _this->image_data = NULL;
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
        _this->rasterizer = nsvgCreateRasterizer();
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

    _this->svg_data = nsvgParseFromFile(filename, "px", 96);

    if (_this->svg_data)
    {
        NSVGimage *svg = (NSVGimage *)_this->svg_data;
        GET_BASE(_this)->w = (int16_t)svg->width;
        GET_BASE(_this)->h = (int16_t)svg->height;
        _this->rasterizer = nsvgCreateRasterizer();
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
