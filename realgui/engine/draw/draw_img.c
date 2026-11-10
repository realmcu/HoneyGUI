/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "draw_img.h"
#include <string.h>
#include "gui_vfs.h"

void (* draw_img_acc_prepare_cb)(struct draw_img *image, gui_rect_t *rect) = NULL;
void (* draw_img_acc_end_cb)(struct draw_img *image) = NULL;

bool draw_img_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                          int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end)
{
    (void)rect;
    int16_t image_x = image->rel_x;
    int16_t image_y = image->rel_y;

    int16_t image_w = (int16_t)image->img_w;
    int16_t image_h = (int16_t)image->img_h;

    *x_start = _UI_MAX(image_x, dc->section.x1);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->section.x2);

    *y_start = _UI_MAX(image_y, dc->section.y1);
    *y_end = _UI_MIN(image_y + image_h - 1, dc->section.y2);

    if (rect)
    {
        *y_start = _UI_MAX(*y_start, rect->y1);
        *y_end   = _UI_MIN(*y_end,   rect->y2);
        *x_start = _UI_MAX(*x_start, rect->x1);
        *x_end   = _UI_MIN(*x_end,   rect->x2);
    }

    if ((*x_start > *x_end) || (*y_start > *y_end))
    {
        return false;
    }
    return true;
}


gui_rgb_data_head_t draw_img_get_header(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};

    if (src_mode == IMG_SRC_FILESYS)
    {
        gui_vfs_file_t *file = gui_vfs_open(img->data, GUI_VFS_READ);
        if (!file)
        {
            gui_log("open file fail:%s !\n", (char *)img->data);
            return head;
        }
        gui_vfs_read(file, &head, sizeof(head));
        gui_vfs_close(file);
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, img->data, sizeof(head));
    }
    else
    {
        gui_log("Unsupported image source mode: %d\n", (int)src_mode);
    }

    return head;
}
void draw_img_load_scale(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};

    head = draw_img_get_header(img, src_mode);
    img->img_w = head.w;
    img->img_h = head.h;
}
bool draw_img_new_area(draw_img_t *img, gui_rect_t *rect)
{
    (void)rect;

    if (draw_img_acc_prepare_cb != NULL)
    {
        draw_img_acc_prepare_cb(img, rect);
    }

    return true;
}


void draw_img_cache(draw_img_t *image, IMG_SOURCE_MODE_TYPE src_mode, const char *path)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (dc->section_count != 0)
    {
        return;
    }
    switch (src_mode)
    {
    case IMG_SRC_FILESYS:
        {
            /* Only directly mappable files are supported; anything else has to
             * be read into RAM, which nano does not do. */
            const void *data = gui_vfs_get_file_address(path);
            if (data == NULL)
            {
                gui_log("Image file is not memory-mappable: %s\n", path);
                image->data = NULL;
                break;
            }
            image->data = (uint8_t *)data;
            break;
        }
    case IMG_SRC_MEMADDR:
        {
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
            if (head->type == JPEG || head->type == PNG)
            {
                gui_log("Unsupported image format: JPEG/PNG has no decoder in nano\n");
                image->data = NULL;
            }
            /* RLE images are used in place; the compressed data is decoded per
             * pixel during blit. */
            break;
        }
    default:
        gui_log("Unsupported image source mode: %d\n", (int)src_mode);
        break;
    }
}

void draw_img_free(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode, const char *path)
{
    (void)img;
    (void)src_mode;
    (void)path;
}

