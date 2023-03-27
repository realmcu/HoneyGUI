/*
 * File      : image_stb.c
 */

#include <draw_img.h>
#include <stdio.h>
#include <gui_stb_img.h>
#include "stb_image.h"

static bool rtgui_image_stb_check(draw_img_t *image);

static bool rtgui_image_stb_load(draw_img_t *image, bool load);

static void rtgui_image_stb_unload(draw_img_t *image);

static void rtgui_image_stb_blit(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect);


/* Private variables ---------------------------------------------------------*/
struct rtgui_image_engine rtgui_image_stb_engine =
{
    "stb",
    {NULL},
    rtgui_image_stb_check,
    rtgui_image_stb_load,
    rtgui_image_stb_unload,
    rtgui_image_stb_blit
};
static bool rtgui_image_stb_check(draw_img_t *image)
{
    return true;
}
static bool rtgui_image_stb_load(draw_img_t *image, bool load)
{
    return true;
}
static void rtgui_image_stb_unload(draw_img_t *image)
{
    return;
}
static void rtgui_image_stb_blit(draw_img_t *image, struct gui_dispdev *dc,
                                 struct rtgui_rect *rect)
{
    gui_stb_img_t *buf_image = (gui_stb_img_t *)((gui_obj_t *)image - 1);
    int num_components;
    uint8_t *stb_data = NULL;
    int stb_length = 0;
    stb_data = (uint8_t *)(image->data) + sizeof(gui_img_file_head_t);
    stb_length = ((gui_img_file_head_t *)(image->data))->size.vector_size;

    switch (((gui_img_file_head_t *)(image->data))->type)
    {
    case 7:
    case 9:
    case 10:
        {
            if (dc->section.y1 == 0)
            {
                if (buf_image->src_changed)
                {
                    if (buf_image->buffer != 0)
                    {
                        gui_free(buf_image->buffer);
                        buf_image->buffer = NULL;
                    }
                    buf_image->buffer = stbi_load_from_memory(stb_data, stb_length, (int *) & (image->img_w),
                                                              (int *) & (image->img_h), &num_components, 0);
                }
            }

            int image_x = rect->x1;
            int image_y = rect->y1;
            int image_w = image->img_w;
            int image_h = image->img_h;

            int x_start = _UI_MAX(image_x, 0);
            int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
            int y_start = _UI_MAX(dc->section.y1, image_y);
            int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

            if ((x_start >= x_end) || (y_start >= y_end))
            {
                return;
            }

            uint32_t read_x_off = (uint32_t)(buf_image->buffer) - _UI_MIN(image_x, 0) * num_components;

            gui_dispdev_t *dc = gui_get_dc();
            uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

            if (dc_bytes_per_pixel == 4)
            {
                uint32_t *writebuf = (uint32_t *)dc->frame_buf;
                uint8_t *pixel;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = (i - dc->section.y1) * dc->fb_width ;
                    uint32_t read_off = read_x_off + ((i - image_y) * image_w) * num_components -
                                        num_components * x_start;

                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        pixel = (uint8_t *)(read_off + j * num_components);
                        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                        {
                            writebuf[write_off + j] = (pixel[0] << 16) + (pixel[1] << 8) + (pixel[2] << 0);
                        }
                    }
                }
            }
            else if (dc_bytes_per_pixel == 2)
            {
                uint16_t *writebuf = (uint16_t *)dc->frame_buf;
                uint8_t *pixel;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = (i - dc->section.y1) * dc->fb_width ;
                    uint32_t read_off = read_x_off + ((i - image_y) * image_w) * num_components -
                                        num_components * x_start;
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        pixel = (uint8_t *)(read_off + j * num_components);
                        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                        {
#if 1
                            uint16_t r = pixel[0] * 0x1f / 0xff << 11;
                            uint16_t g = pixel[1] * 0x3f / 0xff << 5;
                            uint16_t b = pixel[2] * 0x1f / 0xff;
                            writebuf[write_off + j] = r + g + b; //R
#else
                            uint16_t r = pixel[0] * 0x1f / 0xff << 3;
                            uint16_t g = pixel[1] * 0x3f / 0xff;
                            uint16_t b = pixel[2] * 0x1f / 0xff << 8;
                            uint16_t g1 = g >> 3;
                            uint16_t g2 = (g & 0x38) << 13;
                            writebuf[write_off + j] = r + g1 + g2 + b; //R
#endif
                        }
                    }
                }
            }
        }
        break;
    case 11:
        {
            if (dc->section.y1 == 0)
            {
                if (buf_image->src_changed)
                {
                    if (buf_image->buffer != 0)
                    {
                        gui_free(buf_image->buffer);
                        buf_image->buffer = NULL;
                    }
                    int *delays = NULL;
                    delays = (int *)malloc(sizeof(int) * buf_image->gif_info->total_frame);
                    buf_image->buffer = stbi_load_gif_from_memory(stb_data, stb_length, &delays,
                                                                  (int *) & (image->img_w),
                                                                  (int *) & (image->img_h), (int *) & (buf_image->gif_info->total_frame), &num_components, 3);
                    buf_image->gif_info->delay_ms = (uint32_t *)delays;
                    for (int i = 0; i < buf_image->gif_info->total_frame; i++)
                    {
                        buf_image->gif_info->delay_ms_sum += delays[i];
                    }
                    buf_image->src_changed = false;
                }
            }

            int image_x = rect->x1;
            int image_y = rect->y1;
            int image_w = image->img_w;
            int image_h = image->img_h;

            int x_start = _UI_MAX(image_x, 0);
            int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
            int y_start = _UI_MAX(dc->section.y1, image_y);
            int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

            if ((x_start >= x_end) || (y_start >= y_end))
            {
                return;
            }

            num_components = 3;
            uint32_t read_x_off = (uint32_t)((uint8_t *)buf_image->buffer + image->img_h * image->img_w *
                                             buf_image->gif_info->current_frame * 3) - _UI_MIN(image_x, 0) * num_components;

            gui_dispdev_t *dc = gui_get_dc();
            uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

            if (dc_bytes_per_pixel == 4)
            {
                uint32_t *writebuf = (uint32_t *)dc->frame_buf;
                uint8_t *pixel;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = (i - dc->section.y1) * dc->fb_width ;
                    uint32_t read_off = read_x_off + ((i - image_y) * image_w) * num_components -
                                        num_components * x_start;

                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        pixel = (uint8_t *)(read_off + j * num_components);
                        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                        {
                            writebuf[write_off + j] = (pixel[0] << 16) + (pixel[1] << 8) + (pixel[2] << 0);
                        }
                    }
                }
            }
            else if (dc_bytes_per_pixel == 2)
            {
                uint16_t *writebuf = (uint16_t *)dc->frame_buf;
                uint8_t *pixel;
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    int write_off = (i - dc->section.y1) * dc->fb_width ;
                    uint32_t read_off = read_x_off + ((i - image_y) * image_w) * num_components -
                                        num_components * x_start;
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        pixel = (uint8_t *)(read_off + j * num_components);
                        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                        {
#if 1
                            uint16_t r = pixel[0] * 0x1f / 0xff << 11;
                            uint16_t g = pixel[1] * 0x3f / 0xff << 5;
                            uint16_t b = pixel[2] * 0x1f / 0xff;
                            writebuf[write_off + j] = r + g + b; //R
#else
                            uint16_t r = pixel[0] * 0x1f / 0xff << 3;
                            uint16_t g = pixel[1] * 0x3f / 0xff;
                            uint16_t b = pixel[2] * 0x1f / 0xff << 8;
                            uint16_t g1 = g >> 3;
                            uint16_t g2 = (g & 0x38) << 13;
                            writebuf[write_off + j] = r + g1 + g2 + b; //R
#endif
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}
void rtgui_image_stb_init(void)
{
    /* register rgb565 on image system */
    rtgui_image_register_engine(&rtgui_image_stb_engine);
}













