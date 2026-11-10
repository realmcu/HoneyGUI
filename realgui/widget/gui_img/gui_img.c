/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "gui_vfs.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/**
 * @brief Prepares the GUI image associated with the specified object.
 *
 * This function performs necessary initialization and setup for an image
 * within a GUI object. It may involve configuring settings to ensure the
 * image is ready for display.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object for which the image
 *                preparation is conducted. This object must be properly
 *                initialized before passing to this function.
 */
static void gui_img_prepare(gui_obj_t *obj)
{
    uint8_t last;
    gui_img_t *_this;
    draw_img_t *draw;
    touch_info_t *tp;
    GUI_UNUSED(tp);

    GUI_ASSERT(obj != NULL);

    _this = (gui_img_t *)obj;
    draw = &_this->draw;
    tp = tp_get_info();

    if (gui_obj_out_screen(obj))
    {
        return;
    }

    memset(draw, 0x00, sizeof(draw_img_t));
    _this->draw_ready = true;

    draw->data = _this->src_data;
    draw->blend_mode = _this->blend_mode;
    draw->opacity_value = obj->parent->opacity_value * _this->opacity_value / UINT8_MAX;
    draw->fg_color_set = _this->fg_color_set;
    draw->bg_color_fix = _this->bg_color_fix;


    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_TRIPLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT, "touch");

    draw->rel_x = obj->draw_x;
    draw->rel_y = obj->draw_y;

    draw_img_load_scale(draw, (IMG_SOURCE_MODE_TYPE)_this->storage_type);


    draw_img_new_area(draw, NULL);


    last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, sizeof(gui_img_t));

    if (last != _this->checksum)
    {
        gui_fb_change();
    }
}


/**
 * @brief Callback function for drawing the GUI image.
 *
 * This function is called to render the image within the specified GUI object.
 * It handles all the necessary operations to display the image.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object that contains the image
 *                to be drawn. The image should be prepared in advance using
 *                `gui_img_prepare`.
 */
static void gui_img_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_img_t *_this = (gui_img_t *)obj;
    draw_img_t *draw = &_this->draw;
    struct gui_dispdev *dc = gui_get_dc();

    if (!_this->draw_ready)
    {
        return;
    }

    // cache img to buffer

    draw_img_cache(draw, (IMG_SOURCE_MODE_TYPE)_this->storage_type, _this->src_data);

    if (draw->data == NULL)
    {
        return;
    }
    if (_this->need_clip)
    {
        gui_rect_t rect = {0};
        gui_obj_get_clip_rect(obj->parent, &rect);
        gui_acc_blit_to_dc(draw, dc, &rect);
    }
    else
    {
        // gui_rect_t rect = {0};
        // gui_obj_get_clip_rect(obj, &rect);
        // gui_acc_blit_to_dc(_this->draw_img, dc, &rect);
        gui_acc_blit_to_dc(draw, dc, NULL);
    }


    // release img if cached
    draw_img_free(draw, (IMG_SOURCE_MODE_TYPE)_this->storage_type, _this->src_data);

}
/**
 * @brief Cleans up resources associated with the GUI image.
 *
 * This function releases any resources, such as memory or handles, that were
 * allocated for the image within the specified GUI object.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object associated with the image
 *                whose resources are being cleaned up.
 */
static void gui_img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_img_t *_this = (gui_img_t *)obj;

    _this->draw_ready = false;

    if (draw_img_acc_end_cb != NULL)
    {
        draw_img_acc_end_cb(&_this->draw);
    }
}


/**
 * @brief Destroys the GUI image and cleans up associated resources.
 *
 * This function is responsible for destroying the image associated with
 * the specified GUI object. It ensures that any resources allocated for
 * the image, such as memory, handles, or buffers, are properly released
 * to prevent memory leaks.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object whose image is to
 *                be destroyed. The object should have been previously
 *                prepared and possibly displayed using related functions.
 *
 * @note After calling this function, the image data associated with the
 *       object is no longer valid and should not be used. This function
 *       should be called when the image is no longer needed.
 */
static void gui_img_destroy(gui_obj_t *obj)
{
    gui_img_t *_this = (gui_img_t *)obj;
    GUI_UNUSED(_this);

    if (_this->free_on_destroy && _this->src_data != NULL)
    {
        gui_free(_this->src_data);
        _this->src_data = NULL;
        _this->free_on_destroy = false;
    }


}
static gui_rgb_data_head_t gui_img_get_header(gui_img_t *_this)
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
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        memcpy(&head, _this->src_data, sizeof(head));
    }

    return head;
}

static void gui_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PRE_PROCESS:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_PROCESS:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_POST_PROCESS:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_img_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}



static void gui_img_ctor(gui_img_t            *_this,
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
    _this->need_clip = false;
    _this->fg_color_set = GUI_COLOR_ARGB8888(0xFF, 0xFF, 0xFF, 0xFF);
    _this->bg_color_fix = GUI_COLOR_ARGB8888(0xFF, 0xFF, 0xFF, 0xFF);

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_img_cb;
    obj->has_input_process_cb = false;
    obj->has_pre_process_cb = true;
    obj->has_process_cb = true;
    obj->has_post_process_cb = true;
    obj->has_destroy_cb = true;
    obj->type = IMAGE_FROM_MEM;

    _this->src_data = (void *)path;

    _this->opacity_value = 255;
    _this->blend_mode = IMG_FILTER_BLACK;
    _this->opacity_value = UINT8_MAX;


    obj->w = gui_img_get_width(_this);
    obj->h = gui_img_get_height(_this);

    gui_rgb_data_head_t head = gui_img_get_header(_this);
    if (head.resize != 0)
    {
        /* Non-1:1 scaling needs the matrix transform nano dropped. */
        gui_log("Unsupported image resize mode: %d (only 1:1 supported)\n", head.resize);
    }
    if (storage_type == IMG_SRC_FILESYS)
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

        _this->storage_type = IMG_SRC_MEMADDR;  /* XIP is also direct memory access */

        gui_free(_this->src_data);

        _this->src_data = (void *)data;

    }

    /* Auto-detect if clipping is needed */
    {
        gui_obj_t *o = obj;
        while (o->parent != NULL)
        {
            o = o->parent;
            if (o->type == WINDOW || o->type == LIST_NOTE)
            {
                if (x < 0 || y < 0 || (x + obj->w) > o->w || (y + obj->h) > o->h)
                {
                    _this->need_clip = true;
                }
                break;
            }
        }
    }
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_img_t *gui_img_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_img_t *_this = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_img_t));
    gui_img_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}


gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h)
{
    GUI_ASSERT(parent != NULL);
    GUI_ASSERT(file != NULL);

    char *path_copy = gui_strdup((const char *)file);
    GUI_ASSERT(path_copy != NULL);

    gui_img_t *_this = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_img_t));
    gui_img_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, path_copy, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}

uint16_t gui_img_get_width(gui_img_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.w = 0;
        gui_vfs_file_t *fd = gui_vfs_open(_this->src_data, GUI_VFS_READ);

        if (fd == NULL)
        {
            gui_log("open file fail:%s !\n", (char *)_this->src_data);
        }

        gui_vfs_read(fd, &head, sizeof(head));
        gui_vfs_close(fd);
        return head.w;
    }
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)_this->src_data;
        return head->w;
    }

    return 0;
}

uint16_t gui_img_get_height(gui_img_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.h = 0;
        gui_vfs_file_t *fd = gui_vfs_open(_this->src_data, GUI_VFS_READ);

        if (fd == NULL)
        {
            gui_log("open file fail:%s !\n", (char *)_this->src_data);
        }

        gui_vfs_read(fd, &head, sizeof(head));
        gui_vfs_close(fd);
        return head.h;
    }
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)_this->src_data;
        return head->h;
    }

    return 0;
}

void gui_img_refresh_size(gui_img_t *_this)
{
    _this->base.w = gui_img_get_width(_this);
    _this->base.h = gui_img_get_height(_this);
}

void gui_img_refresh_draw_data(gui_img_t  *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(this->src_data != NULL);
    this->draw.data = this->src_data;
}

void gui_img_set_mode(gui_img_t *_this, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(_this != NULL);

    _this->blend_mode = mode;
}

void gui_img_set_attribute(gui_img_t  *_this,
                           const char *name,
                           void       *path,
                           int16_t     x,
                           int16_t     y)
{
    GUI_UNUSED(_this);
    GUI_UNUSED(name);
    GUI_UNUSED(path);
    GUI_UNUSED(x);
    GUI_UNUSED(y);
    gui_log("gui_img_set_attribute: Deprecated! \r\nUse 'void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type)' instead.\n");
    gui_log("Use 'void gui_img_set_pos(gui_img_t  *_this, int16_t x, int16_t y)' instead.\n");
    GUI_ASSERT(0);
}
void gui_img_set_pos(gui_img_t  *_this,
                     int16_t     x,
                     int16_t     y)
{
    GUI_ASSERT(_this != NULL);

    _this->base.x = x;
    _this->base.y = y;
}

void gui_img_set_image_data(gui_img_t  *_this, const uint8_t *file_pointer)
{
    GUI_UNUSED(_this);
    GUI_UNUSED(file_pointer);
    gui_log("gui_img_set_image_data: Deprecated! \r\nUse 'void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type)' instead.\n");
    GUI_ASSERT(0);
}

void gui_img_set_src(gui_img_t  *_this, const uint8_t *file_pointer, uint32_t storage_type)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(file_pointer != NULL);
    GUI_ASSERT(storage_type <= IMG_SRC_FILESYS);

    /* Free old src_data if we own it */
    if (_this->free_on_destroy && _this->src_data != NULL)
    {
        gui_free(_this->src_data);
        _this->src_data = NULL;
    }

    if (storage_type == IMG_SRC_FILESYS)
    {
        const void *data = gui_vfs_get_file_address((const char *)file_pointer);
        if (!data)
        {
            /* Fallback: read file into memory */
            gui_vfs_file_t *f = gui_vfs_open((const char *)file_pointer, GUI_VFS_READ);
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

        _this->storage_type = IMG_SRC_MEMADDR;  /* XIP is also direct memory access */


        _this->src_data = (void *)data;
    }
    else
    {
        _this->src_data = (void *)file_pointer;
        _this->free_on_destroy = false;  /* External pointer */
        _this->storage_type = storage_type;
    }

    /* Keep the draw state in step for a source change that lands between
     * OBJ_PRE_PROCESS and OBJ_PROCESS, e.g. from a timer callback. */
    gui_img_refresh_draw_data(_this);
}

const uint8_t *gui_img_get_image_data(gui_img_t  *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->src_data;
}
void gui_img_set_opacity(gui_img_t *_this, unsigned char opacity_value)
{
    _this->opacity_value = opacity_value;
}


void gui_img_a8_recolor(gui_img_t *_this, uint32_t fg_color_set)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    gui_img_set_mode(_this, IMG_2D_SW_FIX_A8_FG);
    _this->fg_color_set = fg_color_set;
}

void gui_img_a8_fix_bg(gui_img_t *_this, uint32_t bg_color_fix)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    _this->bg_color_fix = bg_color_fix;
    gui_img_set_mode(_this, IMG_2D_SW_FIX_A8_BGFG);
}

void gui_img_a8_mix_alpha(gui_img_t *_this, uint8_t alpha_mix)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    _this->opacity_value = alpha_mix;
}

void gui_img_clip(gui_img_t *_this, bool clip)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    _this->need_clip = clip;
}

