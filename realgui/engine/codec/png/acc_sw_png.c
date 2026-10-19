/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include "acc_sw_png.h"
#include "draw_img.h"
#include "gui_api.h"
#include "stb_image.h"

void *gui_acc_png_load(void *input, int len, int *w, int *h, int *channel)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->png != NULL && acc->png->load != NULL)
    {
        gui_rgb_data_head_t *image = acc->png->load(input, len, w, h, channel);
        if (image != NULL)
        {
            image->png = true;
        }
        return image;
    }

    static const uint8_t png_signature[] = {0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
    if (input == NULL || len < (int)sizeof(png_signature) ||
        memcmp(input, png_signature, sizeof(png_signature)) != 0)
    {
        return NULL;
    }

    int width = 0;
    int height = 0;
    int components = 0;
    uint8_t *pixels = stbi_load_from_memory(input, len, &width, &height, &components, 4);
    if (pixels == NULL)
    {
        gui_log("Failed to load PNG image from memory!\n");
        return NULL;
    }

    if (width <= 0 || width > SHRT_MAX || height <= 0 || height > SHRT_MAX)
    {
        stbi_image_free(pixels);
        return NULL;
    }

    size_t count = (size_t)width * (size_t)height;
    if (count > (SIZE_MAX - sizeof(gui_rgb_data_head_t)) / 4)
    {
        stbi_image_free(pixels);
        return NULL;
    }

    size_t pixel_bytes = count * 4;
    uint8_t *image = gui_realloc(pixels, sizeof(gui_rgb_data_head_t) + pixel_bytes);
    if (image == NULL)
    {
        stbi_image_free(pixels);
        return NULL;
    }

    memmove(image + sizeof(gui_rgb_data_head_t), image, pixel_bytes);
    /* The GUI stores ARGB8888 pixels in BGRA byte order. */
    uint8_t *rgba = image + sizeof(gui_rgb_data_head_t);
    for (size_t i = 0; i < pixel_bytes; i += 4)
    {
        uint8_t red = rgba[i];
        rgba[i] = rgba[i + 2];
        rgba[i + 2] = red;
    }

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image;
    memset(head, 0, sizeof(*head));
    head->png = true;
    head->type = ARGB8888;
    head->w = width;
    head->h = height;

    *w = width;
    *h = height;
    *channel = 4;
    return image;
}

void gui_acc_png_free(void *decode_image)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->png != NULL && acc->png->release != NULL)
    {
        acc->png->release(decode_image);
    }
    else if (decode_image != NULL)
    {
        stbi_image_free(decode_image);
    }
}
