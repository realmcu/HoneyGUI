/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_engine.h"
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
#define __FPU_PRESENT                  1            /* FPU present                                                             */
#include "arm_math.h"
#endif

/** @defgroup ENGINE ENGINE
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup ENGINE_Exported_Types ENGINE Exported Types
  * @{
  */
typedef struct rtzip_file_header
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} rtzip_file_header_t;

typedef struct rtzip_file
{
    rtzip_file_header_t header;
    uint32_t compressed_addr[1024];

} rtzip_file_t;
#pragma pack(1)

typedef struct rtzip_rgb565_node
{
    uint8_t len;
    uint16_t pixel16;  //rgb565
} rtzip_rgb565_node_t;
typedef struct rtzip_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;    //argb8888
} rtzip_argb8888_node_t;
#pragma pack()

/** End of ENGINE_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup ENGINE_Exported_Constants ENGINE Exported Constants
  * @{
  */


/** End of ENGINE_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup ENGINE_Exported_Macros ENGINE Exported Macros
  * @{
  */


/** End of ENGINE_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup ENGINE_Exported_Variables ENGINE Exported Variables
  * @{
  */


/** End of ENGINE_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup ENGINE_Exported_Functions ENGINE Exported Functions
  * @{
  */

static void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) //rgb565
{
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
    arm_fill_q15(pixel, (int16_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
static void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len)  //argb8888
{
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
    arm_fill_q31(pixel, (int32_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
static void uncompressed_rle_rgb565(rtzip_file_t *file, uint32_t line,  uint8_t *buf)
{
    //rtzip_file_header_t *header = (rtzip_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint16_t *linebuf = (uint16_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        rtzip_rgb565_node_t *node = (rtzip_rgb565_node_t *)addr;
        gui_memset16(linebuf, node->pixel16, node->len);

        addr = addr + sizeof(rtzip_rgb565_node_t);
        linebuf = linebuf + node->len;
    }
}
static void uncompressed_rle_argb8888(rtzip_file_t *file, uint32_t line,  uint8_t *buf)
{
    //rtzip_file_header_t *header = (rtzip_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint32_t *linebuf = (uint32_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        rtzip_argb8888_node_t *node = (rtzip_argb8888_node_t *)addr;
        gui_memset32(linebuf, node->pixel32, node->len);

        addr = addr + sizeof(rtzip_argb8888_node_t);
        linebuf = linebuf + node->len;
    }
}

static void rle_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect)
{
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

    //uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    //uint8_t *head = (uint8_t *)((uint8_t *)(image->data) + sizeof(struct gui_rgb_data_head));
    uint8_t img_type = *((uint8_t *)image_off);
    rtzip_file_t *file = (rtzip_file_t *)image_off;

    if (img_type == 4)
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];

        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;

            uncompressed_rle_rgb565(file, line, line_buf);

            int read_off = (int)line_buf - source_bytes_per_pixel * x_start;

            uint16_t *writebuf = (uint16_t *)dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                if (*((uint16_t *)read_off + j) != 0)
                {
                    writebuf[write_off + j] = (*((uint16_t *)read_off + j));
                }
            }
        }
    }
    else if (img_type == 132) //argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * x_start;

            uint16_t *writebuf = (uint16_t *) dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                if (pixel != 0)
                {
                    writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                              + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                }
            }
        }
    }
}
static void rle_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                struct rtgui_rect *rect)
{
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
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    if (img_type == 132)
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * x_start;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                switch (image->opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel    ] = pixel[0]; //B
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1]; //G
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = pixel[3]; //A
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    else if (img_type == 4) //rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * x_start;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + j * source_bytes_per_pixel));
                if (pixel != 0)
                {
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                    writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                }
            }
        }

    }
    return;
}
static void do_blending_argb8888_2_argb8888(gui_color_t *d, gui_color_t *s)
{
    //gui_log("[GUI] TODO line = %d", __LINE__);

    uint8_t Sa = s->channel.alpha;
    uint8_t Sr = s->channel.red;
    uint8_t Sg = s->channel.green;
    uint8_t Sb = s->channel.blue;

    uint8_t Da = d->channel.alpha;
    uint8_t Dr = d->channel.red;
    uint8_t Dg = d->channel.green;
    uint8_t Db = d->channel.blue;

    d->channel.alpha = ((255 - Sa) * Da + Sa * Sa) / 255;
    d->channel.red = ((255 - Sa) * Dr + Sa * Sr) / 255;
    d->channel.green = ((255 - Sa) * Dg + Sa * Sg) / 255;
    d->channel.blue = ((255 - Sa) * Db + Sa * Sb) / 255;
}
static void do_blending_argb8888_2_rgb565(uint8_t *target, gui_color_t color)
{
    gui_log("[GUI] TODO line = %d", __LINE__);
    while (1);
}
static void normal_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{
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

    uint8_t source_bytes_per_pixel = 2;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            if (*((uint16_t *)read_off + j) != 0)
            {
                writebuf[write_off + j] = (*((uint16_t *)read_off + j));
            }
        }
    }
}
#if 0
static void normal_blit_rgb565_2_rgb565_with_alpha(draw_img_t *image, struct gui_dispdev *dc,
                                                   struct rtgui_rect *rect)
{
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

    uint8_t source_bytes_per_pixel = 2;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        GUI_ASSERT(NULL != NULL);
        GUI_UNUSED(write_off);
        GUI_UNUSED(read_off);
        GUI_UNUSED(writebuf);

        for (uint32_t j = x_start; j < x_end; j++)
        {
            if (*((uint16_t *)read_off + j) != 0)
            {
                //blend_565withalpha_to_screen(writebuf + write_off + j, (*((uint16_t *)read_off + j)),image->alpha_value);
            }
        }
    }
}
#endif

static void normal_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{
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

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//R
                writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
                writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //B
            }
        }
    }
}



static void normal_blit_rgba8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect)
{
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

    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);

            gui_color_t color = {.channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                 .channel.alpha = pixel[3],
                                };
            switch (image->opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            }
        }
    }
}
static void normal_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(_UI_MAX(image_x, image_x + rect->xboundleft), 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    if (rect->xboundright > 0)
    {
        x_end = _UI_MIN(_UI_MIN(image_x + image_w, image_x + rect->xboundright), dc->fb_width);
    }

    int y_start = _UI_MAX(_UI_MAX(dc->section.y1, image_y), image_y + rect->yboundtop);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);
    if (rect->yboundbottom > 0)
    {
        y_end = _UI_MIN(y_end, image_y + rect->yboundbottom);
    }


    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }


    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    uint8_t *writebuf = dc->frame_buf;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
            if (pixel  != 0)
            {
                writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
            }
        }
    }
}





static void no_blending_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                             struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * 2;


    for (uint32_t i = y_start; i < y_end; i++)
    {
        memcpy(writebuf + (i - dc->section.y1) * dc->fb_width + x_start,
               (uint16_t *)(((i - image_y) * image_w) * 2 + read_x_off + image_off - 2 * x_start) + x_start,
               2 * (x_end - x_start));
    }
}

static void no_blending_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                               struct rtgui_rect *rect)
{
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

    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t *writebuf = dc->frame_buf;
    uint16_t pixel;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);

            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
            writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
        }
    }

}

static void cpu_matrix_blit_rgb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                               struct rtgui_rect *rect)
{

    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;//for more data,20220104,howie
    int image_h = image->target_h + 1;//for more data,20220104,howie
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

            gui_color_t color = {.channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                 .channel.alpha = pixel[3],
                                };
            switch (image->opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_argb8888(d, &color);
                }
                break;
            }

        }
    }
}
static void cpu_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect)
{

    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;//for more data,20220104,howie
    int image_h = image->target_h + 1;//for more data,20220104,howie
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            int x = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            int y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
            {
                writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//R
                writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
                writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //B
            }
        }
    }
}

static void cpu_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                              struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;//for more data,20220104,howie
    int image_h = image->target_h + 1;//for more data,20220104,howie
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    uint8_t *writebuf = dc->frame_buf;

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    //gui_log("width = %d, height = %d, image_off = 0x%x;\n", image->img_w, image->img_h, image_off);

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);

            if (pixel  != 0)
            {
                switch (image->opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                    }
                    break;
                default:
                    {
                        if (image->opacity_value < 255)
                        {
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) *
                                                                                  (256 - image->opacity_value)
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * image->opacity_value)) / 256 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                  (256 - image->opacity_value)
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * image->opacity_value)) / 256 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) *
                                                                              (256 - image->opacity_value)
                                                                              + writebuf[(write_off + j) * dc_bytes_per_pixel] * image->opacity_value) / 256 ;
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }

            }

        }

    }
}


static void cpu_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                            struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;//for more data,20220104,howie
    int image_h = image->target_h + 1;//for more data,20220104,howie
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);

            if (pixel  != 0)
            {
                writebuf[write_off + j] = (pixel);
            }
        }
    }
}
static void normal_blit_rgba8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);
    //gui_log("x_start%d,x_end%d, y_start%d, y_end%d\n", x_start,x_end, y_start, y_end);
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * 4  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * 4 + read_x_off -
                       4 * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            uint8_t *pixel = (uint8_t *)read_off;
            gui_color_t color = {.channel.alpha = pixel[3],
                                 .channel.blue = pixel[0],
                                 .channel.green = pixel[1],
                                 .channel.red = pixel[2],
                                };
            do_blending_argb8888_2_rgb565((uint8_t *)(writebuf + write_off + j), color);
        }
    }

}





/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief show image to display
 *
 * @param image image
 * @param dc dispaly
 * @param rect scope
 */
void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    struct gui_rgb_data_head *head = image->data;

    char img_type = head->type;

    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        if ((img_type == RGB888) && (dc_bytes_per_pixel == 4))
        {
            normal_blit_rgb888_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 4))
        {
            no_blending_blit_rgb565_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 2))
        {
            no_blending_blit_rgb565_2_rgb565(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_FILTER_BLACK)
    {
        if ((img_type == RGB888) && (dc_bytes_per_pixel == 4))
        {
            normal_blit_rgb888_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 4))
        {
            normal_blit_rgb565_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 2))
        {
            normal_blit_rgb565_2_rgb565(image, dc, rect);
        }
        else if ((img_type == RGBA8888) && (dc_bytes_per_pixel == 2))
        {
            normal_blit_rgba8888_2_rgb565(image, dc, rect);
        }
        else if ((img_type == RGBA8888) && (dc_bytes_per_pixel == 4))
        {
            normal_blit_rgba8888_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RTZIP_COMPRESS) && (dc_bytes_per_pixel == 2))
        {
            rle_blit_2_rgb565(image, dc, rect);
        }
        else if ((img_type == RTZIP_COMPRESS) && (dc_bytes_per_pixel == 4))
        {
            rle_blit_2_argb8888(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_MAGIC_MATRIX)
    {
        if ((img_type == RGB888) && (dc_bytes_per_pixel == 4))
        {
            cpu_matrix_blit_rgb888_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 4))
        {
            cpu_matrix_blit_rgb565_2_argb8888(image, dc, rect);
        }
        else if ((img_type == RGB565) && (dc_bytes_per_pixel == 2))
        {
            cpu_matrix_blit_rgb565_2_rgb565(image, dc, rect);
        }
        else if ((img_type == RGBA8888) && (dc_bytes_per_pixel == 4))
        {
            cpu_matrix_blit_rgb8888_2_argb8888(image, dc, rect);
        }
    }

}
/** End of ENGINE_Exported_Functions
  * @}
  */

/** End of ENGINE
  * @}
  */
