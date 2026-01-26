/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <stdlib.h>
#include "gui_lottie.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_vfs.h"

/* Simple JSON value extraction helpers */
static float json_get_float(const char *json, const char *key)
{
    char search[64];
    snprintf(search, sizeof(search), "\"%s\"", key);
    const char *pos = strstr(json, search);
    if (!pos)
    {
        return 0;
    }
    pos = strchr(pos, ':');
    if (!pos)
    {
        return 0;
    }
    return (float)atof(pos + 1);
}

static void gui_lottie_prepare(gui_obj_t *obj)
{
    gui_lottie_t *_this = (gui_lottie_t *)obj;

    if (!_this->playing || !_this->lottie_data)
    {
        return;
    }

    uint32_t current_time = gui_ms_get();
    uint32_t elapsed = current_time - _this->last_time;

    if (elapsed > 0 && _this->frame_rate > 0)
    {
        float frames_to_advance = (elapsed / 1000.0f) * _this->frame_rate;
        _this->current_frame += frames_to_advance;

        if (_this->current_frame >= _this->total_frames)
        {
            if (_this->loop)
            {
                while (_this->current_frame >= _this->total_frames)
                {
                    _this->current_frame -= _this->total_frames;
                }
            }
            else
            {
                _this->current_frame = _this->total_frames - 1;
                _this->playing = 0;
            }
        }
        _this->last_time = current_time;
    }
}

static void gui_lottie_draw(gui_obj_t *obj)
{
    gui_lottie_t *_this = (gui_lottie_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    if (!_this->lottie_data)
    {
        return;
    }

    /* Placeholder: Draw a simple progress indicator based on current frame */
    int x_start = obj->x + obj->parent->x;
    int y_start = obj->y + obj->parent->y;
    int w = obj->w;
    int h = obj->h;

    float progress = _this->current_frame / (_this->total_frames > 0 ? _this->total_frames : 1);
    int fill_w = (int)(w * progress);

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

            int fb_offset = (screen_y - dc->section.y1) * dc->fb_width + screen_x;

            /* Draw border or filled area based on progress */
            bool is_border = (y == 0 || y == h - 1 || x == 0 || x == w - 1);
            bool is_filled = (x < fill_w);

            uint8_t r, g, b;
            if (is_border)
            {
                r = 128; g = 128; b = 128;
            }
            else if (is_filled)
            {
                r = 0; g = 150; b = 255;
            }
            else
            {
                continue;
            }

            uint8_t a = _this->opacity_value;

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

static void gui_lottie_end(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
}

static void gui_lottie_destroy(gui_obj_t *obj)
{
    gui_lottie_t *_this = (gui_lottie_t *)obj;

    if (_this->lottie_data)
    {
        gui_free(_this->lottie_data);
        _this->lottie_data = NULL;
    }
    if (_this->image_buf)
    {
        gui_free(_this->image_buf);
        _this->image_buf = NULL;
    }
}

static void gui_lottie_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj == NULL)
    {
        return;
    }

    switch (cb_type)
    {
    case OBJ_PREPARE:
        gui_lottie_prepare(obj);
        break;
    case OBJ_DRAW:
        gui_lottie_draw(obj);
        break;
    case OBJ_END:
        gui_lottie_end(obj);
        break;
    case OBJ_DESTROY:
        gui_lottie_destroy(obj);
        break;
    default:
        break;
    }
}

static void gui_lottie_ctor(gui_lottie_t *_this,
                            gui_obj_t    *parent,
                            const char   *name,
                            int16_t       x,
                            int16_t       y,
                            int16_t       w,
                            int16_t       h)
{
    gui_obj_ctor(&_this->base, parent, name, x, y, w, h);
    GET_BASE(_this)->type = MOVIE;
    GET_BASE(_this)->obj_cb = gui_lottie_cb;
    GET_BASE(_this)->has_prepare_cb = true;
    GET_BASE(_this)->has_draw_cb = true;
    GET_BASE(_this)->has_end_cb = true;
    GET_BASE(_this)->has_destroy_cb = true;

    _this->opacity_value = 255;
    _this->loop = 1;
    _this->frame_rate = 30.0f;
    _this->total_frames = 60.0f;
}

static void gui_lottie_parse(gui_lottie_t *_this, const char *json_data)
{
    _this->frame_rate = json_get_float(json_data, "fr");
    if (_this->frame_rate <= 0)
    {
        _this->frame_rate = 30.0f;
    }

    float ip = json_get_float(json_data, "ip");
    float op = json_get_float(json_data, "op");
    _this->total_frames = op - ip;
    if (_this->total_frames <= 0)
    {
        _this->total_frames = 60.0f;
    }

    float w = json_get_float(json_data, "w");
    float h = json_get_float(json_data, "h");
    if (w > 0)
    {
        _this->img_w = (uint16_t)w;
    }
    if (h > 0)
    {
        _this->img_h = (uint16_t)h;
    }
}

gui_lottie_t *gui_lottie_create_from_mem(void       *parent,
                                         const char *name,
                                         void       *addr,
                                         uint32_t    size,
                                         int16_t     x,
                                         int16_t     y,
                                         int16_t     w,
                                         int16_t     h)
{
    gui_lottie_t *_this = gui_malloc(sizeof(gui_lottie_t));
    memset(_this, 0, sizeof(gui_lottie_t));

    gui_lottie_ctor(_this, parent, name, x, y, w, h);

    _this->lottie_data = gui_malloc(size + 1);
    memcpy(_this->lottie_data, addr, size);
    ((char *)_this->lottie_data)[size] = '\0';

    gui_lottie_parse(_this, (const char *)_this->lottie_data);

    _this->last_time = gui_ms_get();

    gui_list_init(&GET_BASE(_this)->child_list);
    if (parent)
    {
        gui_list_insert_before(&((gui_obj_t *)parent)->child_list, &GET_BASE(_this)->brother_list);
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

gui_lottie_t *gui_lottie_create_from_file(void       *parent,
                                          const char *name,
                                          const char *filename,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h)
{
    gui_lottie_t *_this = gui_malloc(sizeof(gui_lottie_t));
    memset(_this, 0, sizeof(gui_lottie_t));

    gui_lottie_ctor(_this, parent, name, x, y, w, h);
    _this->from_file = 1;

    const void *addr = gui_vfs_get_file_address(filename);
    if (addr)
    {
        /* XIP: use directly, add null terminator copy */
        gui_vfs_stat_t st;
        gui_vfs_stat(filename, &st);
        _this->lottie_data = gui_malloc(st.size + 1);
        memcpy(_this->lottie_data, addr, st.size);
        ((char *)_this->lottie_data)[st.size] = '\0';
    }
    else
    {
        /* Non-XIP: read file */
        gui_vfs_file_t *f = gui_vfs_open(filename, GUI_VFS_READ);
        if (f)
        {
            gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
            int fsize = gui_vfs_tell(f);
            gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);
            if (fsize > 0)
            {
                _this->lottie_data = gui_malloc(fsize + 1);
                gui_vfs_read(f, _this->lottie_data, fsize);
                ((char *)_this->lottie_data)[fsize] = '\0';
            }
            gui_vfs_close(f);
        }
    }

    if (_this->lottie_data)
    {
        gui_lottie_parse(_this, (const char *)_this->lottie_data);
    }

    _this->last_time = gui_ms_get();

    gui_list_init(&GET_BASE(_this)->child_list);
    if (parent)
    {
        gui_list_insert_before(&((gui_obj_t *)parent)->child_list, &GET_BASE(_this)->brother_list);
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

void gui_lottie_play(gui_lottie_t *_this)
{
    if (_this)
    {
        _this->playing = 1;
        _this->last_time = gui_ms_get();
    }
}

void gui_lottie_pause(gui_lottie_t *_this)
{
    if (_this)
    {
        _this->playing = 0;
    }
}

void gui_lottie_stop(gui_lottie_t *_this)
{
    if (_this)
    {
        _this->playing = 0;
        _this->current_frame = 0;
    }
}

void gui_lottie_set_loop(gui_lottie_t *_this, bool loop)
{
    if (_this)
    {
        _this->loop = loop ? 1 : 0;
    }
}

void gui_lottie_set_frame(gui_lottie_t *_this, float frame)
{
    if (_this)
    {
        _this->current_frame = frame;
        if (_this->current_frame < 0)
        {
            _this->current_frame = 0;
        }
        if (_this->current_frame >= _this->total_frames)
        {
            _this->current_frame = _this->total_frames - 1;
        }
    }
}

float gui_lottie_get_frame(gui_lottie_t *_this)
{
    return _this ? _this->current_frame : 0;
}

float gui_lottie_get_total_frames(gui_lottie_t *_this)
{
    return _this ? _this->total_frames : 0;
}

void gui_lottie_set_opacity(gui_lottie_t *_this, uint8_t opacity)
{
    if (_this)
    {
        _this->opacity_value = opacity;
    }
}
