#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>

#if defined ( __CC_ARM )
#define __FPU_PRESENT                  1            /* FPU present                                                             */
#include "arm_math.h"
#endif



static void gui_memset16(uint16_t pixel, int16_t *addr, uint32_t len)
{
#if defined ( __CC_ARM )
    arm_fill_q15(pixel, addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}


static bool rtgui_image_compress_mem_check(gui_img_t *image);
static bool rtgui_image_compress_mem_load(gui_img_t *image,
                                          bool load);
static void rtgui_image_compress_mem_unload(gui_img_t *image);
static void rtgui_image_compress_mem_blit(gui_img_t *image, struct gui_dispdev *dc,
                                          rtgui_rect_t *rect);


struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char rsvd : 4;
    unsigned char compress : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
};


struct rtgui_image_engine rtgui_image_compress_mem_engine =
{
    "compress_mem",
    { NULL },
    rtgui_image_compress_mem_check,
    rtgui_image_compress_mem_load,
    rtgui_image_compress_mem_unload,
    rtgui_image_compress_mem_blit,
};

static bool rtgui_image_compress_mem_check(gui_img_t *image)
{
    return true;
}

static bool rtgui_image_compress_mem_load(gui_img_t *image,
                                          bool load)
{
    struct gui_rgb_data_head head;
    memcpy(&head, image->data, sizeof(head));

    /* set image information */
    image->w = head.w;
    image->h = head.h;
    image->color_format = (gui_color_format)(head.type);
    image->engine = &rtgui_image_compress_mem_engine;
    if (!head.compress)
    {
        return false;
    }
    //rt_kprintf("w = %d, h = %d, bit_depth =%d\n", image->w, image->h, image->bit_depth);

    return true;
}

static void rtgui_image_compress_mem_unload(gui_img_t *image)
{

}



static void normal_blit_compress_rgb565_2_rgb565(gui_img_t *image, struct gui_dispdev *dc,
                                                 rtgui_rect_t *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->w;
    int image_h = image->h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t *line_offset = (uint32_t *)((uint8_t *)(image->data) + sizeof(struct gui_rgb_data_head));
    uint8_t *bitfile_line = NULL;
    uint16_t pixel;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    if (image_x < x_start)
    {
        for (size_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width;
            uint16_t read_off = 0;
            bool test_flag = false;

            bitfile_line = (uint8_t *)(line_offset[i - image_y] + sizeof(struct gui_rgb_data_head) +
                                       (uint8_t *)(
                                           image->data));

            //gui_log("bitfile_line = %d\n",bitfile_line);
            uint16_t cur_line_offset = 0;
            for (uint32_t j = x_start; j < x_end;)
            {
                uint8_t len1 = bitfile_line[read_off * 2];
                uint8_t len2 = bitfile_line[read_off * 2 + 1];

                read_off += 1;

                if (test_flag == false)
                {
                    if (len2 == 1)
                    {
                        if ((image_x + cur_line_offset + len1) > x_start)
                        {
                            uint16_t len_sign = ((cur_line_offset + len1) - (x_start - image_x));
                            for (uint16_t loopi = len1 - len_sign; loopi < len_sign; loopi++)
                            {
                                pixel = *((uint16_t *)bitfile_line + read_off + loopi);
                                if (pixel != 0)
                                {
                                    writebuf[write_off + j + loopi - (len1 - len_sign)] = pixel;
                                }
                            }
                            j += len_sign;
                            test_flag = true;
                        }
                        cur_line_offset += len1;
                    }
                    else
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            if (test_flag == false)
                            {
                                uint16_t len_sign = ((cur_line_offset + len2) - (x_start - image_x));
                                if ((image_x + cur_line_offset + len2) > x_start)
                                {
                                    pixel = *((uint16_t *)bitfile_line + read_off + loopi);
                                    if (pixel != 0)
                                    {
                                        gui_memset16(pixel, (int16_t *)(writebuf + write_off + j), len_sign);
                                    }
                                    j += len_sign;
                                    test_flag = true;
                                }
                            }
                            else
                            {
                                pixel = *((uint16_t *)bitfile_line + read_off + loopi);
                                if (pixel != 0)
                                {
                                    gui_memset16(pixel, (int16_t *)(writebuf + write_off + j), len2);
                                }
                                j += len2;
                            }
                            cur_line_offset += len2;
                        }
                    }
                }
                else
                {
                    if (len2 == 1)
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            pixel = *((uint16_t *)bitfile_line + read_off + loopi);
                            if (pixel != 0)
                            {
                                writebuf[write_off + j + loopi] = pixel;
                            }
                        }
                        j += len1;
                    }
                    else
                    {
                        for (uint8_t loopi = 0; loopi < len1; loopi++)
                        {
                            pixel = *((uint16_t *)bitfile_line + read_off + loopi);

                            if (pixel != 0)
                            {
                                gui_memset16(pixel, (int16_t *)(writebuf + write_off + j), len2);
                            }
                            j += len2;
                        }
                    }
                }
                read_off += len1;
            }
#if 0
            if (bitfile_line != NULL)
            {
                free(bitfile_line);
            }
#endif
        }
    }
    else if (image_x >= x_start)
    {
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width;
            uint16_t read_off = 0;
            bool test_flag = false;

            bitfile_line = (uint8_t *)(line_offset[i - image_y] + sizeof(struct gui_rgb_data_head) +
                                       (uint8_t *)(
                                           image->data));
            //gui_log("bitfile_line = %d\n",bitfile_line);
            for (uint32_t j = x_start; j < x_end;)
            {
                uint8_t len1 = bitfile_line[read_off * 2];
                uint8_t len2 = bitfile_line[read_off * 2 + 1];

                read_off += 1;

                if (len2 == 1)
                {
                    if (j + len1 >= x_end)
                    {
                        for (uint16_t loopi = 0; loopi < (x_end - j); loopi++)
                        {
                            pixel = *((uint16_t *)bitfile_line + read_off + loopi);

                            if (pixel != 0)
                            {
                                writebuf[write_off + j + loopi] = pixel;
                            }
                        }
                        break;
                    }
                    else
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            pixel = *((uint16_t *)bitfile_line + read_off + loopi);

                            if (pixel != 0)
                            {
                                writebuf[write_off + j + loopi] = pixel;
                            }
                        }
                    }
                    j += len1;
                }
                else
                {
                    for (uint8_t loopi = 0; loopi < len1; loopi++)
                    {
                        pixel = *((uint16_t *)bitfile_line + read_off + loopi);

                        if (j + len2 >= x_end)
                        {
                            if (pixel != 0)
                            {
                                gui_memset16(pixel, (int16_t *)(writebuf + write_off + j), x_end - j);
                            }
                            test_flag = true;
                            break;
                        }
                        else
                        {
                            if (pixel != 0)
                            {
                                gui_memset16(pixel, (int16_t *)(writebuf + write_off + j), len2);
                            }
                            j += len2;
                        }
                    }
                    if (test_flag == true)
                    {
                        break;
                    }
                }
                read_off += len1;
            }
#if 0
            if (bitfile_line != NULL)
            {
                free(bitfile_line);
            }
#endif
        }
    }

}
static void no_blending_blit_compress_rgb565_2_rgb565(gui_img_t *image, struct gui_dispdev *dc,
                                                      rtgui_rect_t *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->w;
    int image_h = image->h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t *line_offset = (uint32_t *)((uint8_t *)(image->data) + sizeof(struct gui_rgb_data_head));
    uint8_t *bitfile_line = NULL;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    if (image_x < x_start)
    {
        for (size_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width;
            uint16_t read_off = 0;
            bool test_flag = false;

            bitfile_line = (uint8_t *)(line_offset[i - image_y] + sizeof(struct gui_rgb_data_head) +
                                       (uint8_t *)(
                                           image->data));
            uint16_t cur_line_offset = 0;
            for (uint32_t j = x_start; j < x_end;)
            {
                uint8_t len1 = bitfile_line[read_off * 2];
                uint8_t len2 = bitfile_line[read_off * 2 + 1];

                read_off += 1;

                if (test_flag == false)
                {
                    if (len2 == 1)
                    {
                        if ((image_x + cur_line_offset + len1) > x_start)
                        {
                            uint16_t len_sign = ((cur_line_offset + len1) - (x_start - image_x));
                            for (uint16_t loopi = len1 - len_sign; loopi < len_sign; loopi++)
                            {
                                {
                                    writebuf[write_off + j + loopi - (len1 - len_sign)] =
                                        *((uint16_t *)bitfile_line + read_off + loopi);
                                }
                            }
                            j += len_sign;
                            test_flag = true;
                        }
                        cur_line_offset += len1;
                    }
                    else
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            if (test_flag == false)
                            {
                                uint16_t len_sign = ((cur_line_offset + len2) - (x_start - image_x));
                                if ((image_x + cur_line_offset + len2) > x_start)
                                {
                                    {
                                        gui_memset16(*((uint16_t *)bitfile_line + read_off + loopi),
                                                     (int16_t *)(writebuf + write_off + j), len_sign);
                                    }
                                    j += len_sign;
                                    test_flag = true;
                                }
                            }
                            else
                            {
                                {
                                    gui_memset16(*((uint16_t *)bitfile_line + read_off + loopi),
                                                 (int16_t *)(writebuf + write_off + j), len2);
                                }
                                j += len2;
                            }
                            cur_line_offset += len2;
                        }
                    }
                }
                else
                {
                    if (len2 == 1)
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            {
                                writebuf[write_off + j + loopi] = *((uint16_t *)bitfile_line + read_off + loopi);
                            }
                        }
                        j += len1;
                    }
                    else
                    {
                        for (uint8_t loopi = 0; loopi < len1; loopi++)
                        {
                            {
                                gui_memset16(*((uint16_t *)bitfile_line + read_off + loopi),
                                             (int16_t *)(writebuf + write_off + j), len2);
                            }
                            j += len2;
                        }
                    }
                }
                read_off += len1;
            }
        }
    }
    else if (image_x >= x_start)
    {
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width;
            uint16_t read_off = 0;
            bool test_flag = false;

            bitfile_line = (uint8_t *)(line_offset[i - image_y] + sizeof(struct gui_rgb_data_head) +
                                       (uint8_t *)(
                                           image->data));
            for (uint32_t j = x_start; j < x_end;)
            {
                uint8_t len1 = bitfile_line[read_off * 2];
                uint8_t len2 = bitfile_line[read_off * 2 + 1];

                read_off += 1;

                if (len2 == 1)
                {
                    if (j + len1 >= x_end)
                    {
                        for (uint16_t loopi = 0; loopi < (x_end - j); loopi++)
                        {
                            {
                                writebuf[write_off + j + loopi] = *((uint16_t *)bitfile_line + read_off + loopi);
                            }
                        }
                        break;
                    }
                    else
                    {
                        for (uint16_t loopi = 0; loopi < len1; loopi++)
                        {
                            {
                                writebuf[write_off + j + loopi] = *((uint16_t *)bitfile_line + read_off + loopi);
                            }
                        }
                    }
                    j += len1;
                }
                else
                {
                    for (uint8_t loopi = 0; loopi < len1; loopi++)
                    {
                        if (j + len2 >= x_end)
                        {
                            {
                                gui_memset16(*((uint16_t *)bitfile_line + read_off + loopi),
                                             (int16_t *)(writebuf + write_off + j), x_end - j);
                            }
                            test_flag = true;
                            break;
                        }
                        else
                        {
                            {
                                gui_memset16(*((uint16_t *)bitfile_line + read_off + loopi),
                                             (int16_t *)(writebuf + write_off + j), len2);
                            }
                            j += len2;
                        }
                    }
                    if (test_flag == true)
                    {
                        break;
                    }
                }
                read_off += len1;
            }
        }
    }
}

static void rtgui_image_compress_mem_blit(gui_img_t *image, struct gui_dispdev *dc,
                                          rtgui_rect_t *rect)
{
    uint8_t source_bytes_per_pixel;
    switch (image->color_format)
    {
    case RGB565:
        source_bytes_per_pixel = 2;
        break;
    case RGB888:
        source_bytes_per_pixel = 3;
        break;
    case RGBA8888:
        source_bytes_per_pixel = 4;
        break;
    default:
        source_bytes_per_pixel = 0;
        break;
    }
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;//
    if (image->mode == NORMAL_MODE)
    {
        if ((source_bytes_per_pixel == 2) && (dc_bytes_per_pixel == 2))
        {
            normal_blit_compress_rgb565_2_rgb565(image, dc, rect);
        }
    }

    else if (image->mode == NON_BLENDING_MODE)
    {
        if ((source_bytes_per_pixel == 2) && (dc_bytes_per_pixel == 2))
        {
            no_blending_blit_compress_rgb565_2_rgb565(image, dc, rect);
        }
    }

}


void rtgui_image_compress_mem_init(void)
{
    /* register rgb565 on image system */
    rtgui_image_register_engine(&rtgui_image_compress_mem_engine);
}

