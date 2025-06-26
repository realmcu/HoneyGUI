/**
 * @file acc_init.c
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdio.h>
#include "acc_api.h"
#include "acc_sw.h"


/**
 * Represents the absolute bounds of an image.
 * The `rect` parameter defines the rectangular area that specifies
 * the absolute range of the image in terms of its position and size.
 *
 * @param rect A structure or object that defines the absolute range
 *             of the image, typically including properties such as
 *             x, y, width, and height.
 */
void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    if (gui_get_acc()->blit != NULL)
    {
        gui_get_acc()->blit(image, dc, rect);
    }
    else
    {
        // Fallback to software rendering if no hardware acceleration is available
        sw_acc_blit(image, dc, rect);
    }
}


#include "stb_image.h"



void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel)
{
    if (gui_get_acc()->jpeg_load != NULL)
    {
        return gui_get_acc()->jpeg_load(input, len, w, h, channel);
    }
    else
    {
        // Fallback to software decoding if no hardware acceleration is available
        unsigned char *img_data = stbi_load_from_memory(input, len, w, h, channel, 0);
        if (img_data == NULL)
        {
            gui_log("Failed to load JPEG image from memory!\n");
            return NULL;
        }

        // Allocate memory for the image data header
        void *decode_image = gui_malloc((*w) * (*h) * (*channel) + sizeof(gui_rgb_data_head_t));
        if (decode_image == NULL)
        {
            stbi_image_free(img_data);
            return NULL;
        }

        // Copy the image data into the allocated memory
        memcpy((uint8_t *)decode_image + sizeof(gui_rgb_data_head_t), img_data, (*w) * (*h) * (*channel));
        stbi_image_free(img_data);

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)decode_image;
        memset(head, 0x00, sizeof(gui_rgb_data_head_t));
        head->type = RGB888; // Assuming RGB888 format for JPEG
        head->w = *w;
        head->h = *h;
        head->compress = false; // Not compressed

        return decode_image;
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
            gui_free(decode_image);
        }
    }

}
