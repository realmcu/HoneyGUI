/**
 * @file acc_sw_jpeg.c
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief JPEG decoder implementation
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdio.h>
#include <string.h>
#include "acc_sw_jpeg.h"
#include "draw_img.h"
#include "gui_api.h"

#define GUI_STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_NO_PNG
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_THREAD_LOCALS

#include "gui_api.h"
#define STBI_MALLOC(sz)           gui_malloc(sz)
#define STBI_REALLOC(p,newsz)     gui_realloc(p, newsz)
#define STBI_FREE(p)              gui_free(p)
#include "gui_stb_image.h"

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel)
{
    if (gui_get_acc()->jpeg_load != NULL)
    {
        return gui_get_acc()->jpeg_load(input, len, w, h, channel);
    }
    else
    {
        // Fallback to software decoding if no hardware acceleration is available
        unsigned char *img_data = gui_stbi_load_from_memory(input, len, w, h, channel, 0);
        if (img_data == NULL)
        {
            gui_log("Failed to load JPEG image from memory!\n");
            return NULL;
        }

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img_data;
        memset(head, 0x00, sizeof(gui_rgb_data_head_t));
        head->type = RGB888; // Assuming RGB888 format for JPEG
        head->w = *w;
        head->h = *h;
        head->compress = false; // Not compressed
        head->jpeg = true;

        return img_data;
    }
}

void gui_acc_jpeg_free(void *decode_image)
{
    if (gui_get_acc()->jpeg_free != NULL)
    {
        gui_get_acc()->jpeg_free(decode_image);
    }
    else
    {
        // Fallback to software memory deallocation
        if (decode_image != NULL)
        {
            gui_stbi_image_free(decode_image);
        }
    }
}
