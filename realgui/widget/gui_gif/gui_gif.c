/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_gif.h"
#include "gui_obj.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "gui_vfs.h"
#include "draw_img.h"
#include "gui_matrix.h"

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Convert GIF frame to palette format for rendering
 */
static void change_gif_to_palette(gui_gif_t *_this, gui_palette_file_t *palette)
{
    if (_this->gif == NULL)
    {
        return;
    }

    palette->img_header.scan = 0; // no scan for palette
    palette->img_header.align = 0;
    palette->img_header.resize = 0; // no resize for palette
    palette->img_header.compress = false; // no compression for palette
    palette->img_header.rsvd = 0; // reserved field
    palette->img_header.jpeg = false; // not a JPEG

    palette->img_header.type = PALETTE;
    palette->img_header.w = _this->draw_img->img_w;
    palette->img_header.h = _this->draw_img->img_h;
    palette->img_header.version = 0; // version number
    palette->img_header.rsvd2 = 0; // reserved field

    palette->palette_data = _this->gif->palette->colors;
    palette->palette_index = _this->gif->frame;
    _this->draw_img->data = palette;
}

/**
 * @brief Timer callback for GIF animation frame updates
 */
static void gif_internal_timer_cb(void *param)
{
    gui_gif_t *_this = (gui_gif_t *)param;
    gui_obj_t *obj = (gui_obj_t *)param;
    GUI_ASSERT(_this != NULL);
    if (gd_get_frame(_this->gif) == 0)
    {
        gd_rewind(_this->gif);
    }
    obj->timer->interval_ms = _this->gif->gce.delay * 10; // 10ms per frame
    obj->timer->expire_time = gui_ms_get() + obj->timer->interval_ms;
    gui_fb_change();
}

/**
 * @brief Read GIF file header based on storage type
 */
static gui_rgb_data_head_t gui_gif_get_header(gui_gif_t *_this)
{
    struct gui_rgb_data_head head = {0};

    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        gui_vfs_file_t *fd = gui_vfs_open(_this->src_data, GUI_VFS_READ);

        if (fd == NULL)
        {
            gui_log("open file fail:%s !\n", (char *)_this->src_data);
        }

        gui_vfs_read(fd, &head, sizeof(head));
        gui_vfs_close(fd);
    }
    else if (_this->storage_type == IMG_SRC_FTL)
    {
        uintptr_t base = (uintptr_t)_this->src_data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
    }
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        memcpy(&head, _this->src_data, sizeof(head));
    }

    return head;
}

/**
 * @brief Prepare the GIF widget for rendering
 */
static void gui_gif_prepare(gui_obj_t *obj)
{
    uint8_t last;
    gui_gif_t *_this;
    touch_info_t *tp;
    GUI_UNUSED(tp);

    GUI_ASSERT(obj != NULL);

    _this = (gui_gif_t *)obj;
    tp = tp_get_info();

    matrix_translate(_this->t_x, _this->t_y, obj->matrix);
    matrix_rotate(_this->degrees, obj->matrix);
    matrix_scale(_this->scale_x, _this->scale_y, obj->matrix);
    matrix_translate(-_this->f_x, -_this->f_y, obj->matrix);

    if (gui_obj_out_screen(obj))
    {
        return;
    }

    _this->draw_img = gui_malloc(sizeof(draw_img_t));
    GUI_ASSERT(_this->draw_img != NULL);
    memset(_this->draw_img, 0x00, sizeof(draw_img_t));

    _this->draw_img->data = _this->src_data;
    _this->draw_img->blend_mode = _this->blend_mode;
    _this->draw_img->high_quality = _this->high_quality;
    _this->draw_img->opacity_value = obj->parent->opacity_value * _this->opacity_value / UINT8_MAX;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_TRIPLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT, "touch");

    memcpy(&_this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&_this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&_this->draw_img->inverse);
    draw_img_load_scale(_this->draw_img, (IMG_SOURCE_MODE_TYPE)_this->storage_type);

    draw_img_new_area(_this->draw_img, NULL);

    last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, sizeof(gui_gif_t));

    if (last != _this->checksum)
    {
        gui_fb_change();
    }
}

/**
 * @brief Draw callback function for rendering the GIF
 */
static void gui_gif_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_gif_t *_this = (gui_gif_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();

    // cache img to buffer
    draw_img_cache(_this->draw_img, (IMG_SOURCE_MODE_TYPE)_this->storage_type, _this->src_data);
    gui_palette_file_t palette = {0};
    change_gif_to_palette(_this, &palette);

    if (_this->need_clip)
    {
        gui_rect_t rect = {0};
        gui_obj_get_clip_rect(obj, &rect);
        gui_acc_blit_to_dc(_this->draw_img, dc, &rect);
    }
    else
    {
        gui_acc_blit_to_dc(_this->draw_img, dc, NULL);
    }

    // release img if cached
    draw_img_free(_this->draw_img, (IMG_SOURCE_MODE_TYPE)_this->storage_type, _this->src_data);
}

/**
 * @brief End callback - clean up temporary resources
 */
static void gui_gif_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_gif_t *_this = (gui_gif_t *)obj;

    if (_this->draw_img != NULL)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(_this->draw_img);
        }
        gui_free(_this->draw_img);
        _this->draw_img = NULL;
    }
}

/**
 * @brief Destroy callback - clean up all resources
 */
static void gui_gif_destroy(gui_obj_t *obj)
{
    gui_gif_t *_this = (gui_gif_t *)obj;
    GUI_UNUSED(_this);

    if (_this->free_on_destroy && _this->src_data != NULL)
    {
        gui_free(_this->src_data);
        _this->src_data = NULL;
        _this->free_on_destroy = false;
    }

    if (_this->gif != NULL)
    {
        gd_close_gif(_this->gif);
        _this->gif = NULL;
    }
}

/**
 * @brief Main callback dispatcher for GIF widget lifecycle
 */
static void gui_gif_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_gif_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_gif_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_gif_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_gif_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Constructor for GIF widget
 */
static void gui_gif_ctor(gui_gif_t            *_this,
                         gui_obj_t            *parent,
                         const char           *name,
                         IMG_SOURCE_MODE_TYPE  storage_type,
                         void                 *path,
                         int16_t               x,
                         int16_t               y,
                         int16_t               w,
                         int16_t               h)
{
    gui_obj_t *obj = (gui_obj_t *)_this;

    _this->storage_type = storage_type;
    _this->free_on_destroy = false;  /* Default: external memory, don't free */
    _this->f_x = 0;
    _this->f_y = 0;
    _this->t_x = 0;
    _this->t_y = 0;
    _this->scale_x = 1;
    _this->scale_y = 1;
    _this->degrees = 0;
    _this->high_quality = false;
    _this->need_clip = false;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_gif_cb;
    obj->has_input_prepare_cb = false;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
    obj->type = IMAGE_FROM_MEM; // GIF uses IMAGE_FROM_MEM type

    _this->src_data = (void *)path;

    _this->opacity_value = 255;
    _this->blend_mode = IMG_FILTER_BLACK;
    _this->opacity_value = UINT8_MAX;

    obj->w = gui_gif_get_width(_this);
    obj->h = gui_gif_get_height(_this);

    // Initialize GIF decoder based on storage type
    if (storage_type == IMG_SRC_MEMADDR)
    {
        gui_gif_file_head_t *gif_head = (gui_gif_file_head_t *)_this->src_data;
        _this->gif = gd_open_gif_from_memory(gif_head->gif, gif_head->size);
        if (gd_get_frame(_this->gif) == 0)
        {
            gd_rewind(_this->gif);
        }
        gui_obj_create_timer(obj, _this->gif->gce.delay * 10, true, gif_internal_timer_cb);
        gui_obj_start_timer(obj);
    }
    else if (storage_type == IMG_SRC_FILESYS)
    {
        const void *data = gui_vfs_get_file_address(path);
        if (!data)
        {
            /* Fallback: read file into memory */
            gui_vfs_file_t *f = gui_vfs_open(path, GUI_VFS_READ);
            GUI_ASSERT(f != NULL);
            gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
            int size = gui_vfs_tell(f);

            if (size <= 0)
            {
                gui_vfs_close(f);
                return;
            }

            gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);
            data = gui_malloc(size);
            GUI_ASSERT(data != NULL);
            gui_vfs_read(f, (void *)data, size);
            gui_vfs_close(f);

            _this->free_on_destroy = true;  /* Allocated memory, must free */
        }
        else
        {
            _this->free_on_destroy = false;  /* XIP memory, don't free */
        }

        if (_this->src_data != NULL && _this->free_on_destroy)
        {
            gui_free(_this->src_data);
        }

        _this->src_data = (void *)data;
        gui_gif_file_head_t *gif_head = (gui_gif_file_head_t *)data;
        _this->gif = gd_open_gif_from_memory(gif_head->gif, gif_head->size);
        if (gd_get_frame(_this->gif) == 0)
        {
            gd_rewind(_this->gif);
        }
        gui_obj_create_timer(obj, _this->gif->gce.delay * 10, true, gif_internal_timer_cb);
        gui_obj_start_timer(obj);
        _this->storage_type = IMG_SRC_MEMADDR;
    }
    else if (storage_type == IMG_SRC_FTL)
    {
        // FTL storage support
        // Note: FTL reading needs special handling similar to FILESYS
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_gif_t *gui_gif_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    GUI_ASSERT(parent != NULL);
    gui_gif_t *_this = gui_malloc(sizeof(gui_gif_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_gif_t));
    gui_gif_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&(GET_BASE(_this)->parent->child_list),
                               &(GET_BASE(_this)->brother_list));
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

gui_gif_t *gui_gif_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h)
{
    GUI_ASSERT(parent != NULL);
    gui_gif_t *_this = gui_malloc(sizeof(gui_gif_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_gif_t));

    // Duplicate the file path string for filesystem storage
    char *path_copy = NULL;
    if (file != NULL)
    {
        size_t len = strlen((const char *)file);
        path_copy = gui_malloc(len + 1);
        GUI_ASSERT(path_copy != NULL);
        memcpy(path_copy, file, len + 1);
    }

    gui_gif_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, path_copy, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&(GET_BASE(_this)->parent->child_list),
                               &(GET_BASE(_this)->brother_list));
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

gui_gif_t *gui_gif_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    GUI_ASSERT(parent != NULL);
    gui_gif_t *_this = gui_malloc(sizeof(gui_gif_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_gif_t));
    gui_gif_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FTL, ftl, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&(GET_BASE(_this)->parent->child_list),
                               &(GET_BASE(_this)->brother_list));
    }

    GET_BASE(_this)->create_done = true;
    return _this;
}

void gui_gif_rotation(gui_gif_t *_this, float degrees)
{
    _this->degrees = degrees;
}

void gui_gif_scale(gui_gif_t *_this, float scale_x, float scale_y)
{
    _this->scale_x = scale_x;
    _this->scale_y = scale_y;
}

void gui_gif_translate(gui_gif_t *_this, float t_x, float t_y)
{
    _this->t_x = t_x;
    _this->t_y = t_y;
}

void gui_gif_set_opacity(gui_gif_t *_this, unsigned char opacity_value)
{
    _this->opacity_value = opacity_value;
}

void gui_gif_set_focus(gui_gif_t *_this, float c_x, float c_y)
{
    _this->f_x = c_x;
    _this->f_y = c_y;
}

void gui_gif_set_pos(gui_gif_t *_this, int16_t x, int16_t y)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    obj->x = x;
    obj->y = y;
}

void gui_gif_set_mode(gui_gif_t *_this, BLEND_MODE_TYPE mode)
{
    _this->blend_mode = mode;
}

void gui_gif_set_quality(gui_gif_t *_this, bool high_quality)
{
    _this->high_quality = high_quality;
}

uint16_t gui_gif_get_width(gui_gif_t *_this)
{
    gui_rgb_data_head_t head = gui_gif_get_header(_this);
    return head.w;
}

uint16_t gui_gif_get_height(gui_gif_t *_this)
{
    gui_rgb_data_head_t head = gui_gif_get_header(_this);
    return head.h;
}
