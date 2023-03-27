#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include "acc_engine.h"
#include "gui_canvas.h"


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

static void normal_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
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

            int x = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            int y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
//            uint8_t opacity_value = pixel[3];
            canvas_color_t color = {.color.channel.alpha = pixel[3],
                                    .color.channel.blue = pixel[0],
                                    .color.channel.green = pixel[1],
                                    .color.channel.red = pixel[2],
                                   };
            switch (image->opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    blend_rgba_to_rgb_screen(writebuf + (write_off + j) * dc_bytes_per_pixel, color.color.rgba);
                }
                break;
            default:
                {
                    color.color.channel.alpha = color.color.channel.alpha * image->opacity_value / UINT8_MAX;
                    blend_rgba_to_rgb_screen(writebuf + (write_off + j) * dc_bytes_per_pixel, color.color.rgba);
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
                        blend_565withalpha_to_rgb_screen(writebuf + (write_off + j) * dc_bytes_per_pixel, pixel,
                                                         image->opacity_value);
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
            uint32_t *rgba = (uint32_t *)read_off;
            blend_rgba_to_screen(writebuf + write_off + j, *((uint32_t *)rgba + j));
        }
    }

}
void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    struct gui_rgb_data_head *head = image->data;

    char img_type = head->type;

    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        if ((img_type == RGB888) && (dc_bytes_per_pixel == 4))
        {
            //no_blending_blit_rgb888_2_argb8888(image, dc, rect);
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




//#define RGBA(R,G,B,A) ((uint32_t)((((uint32_t)R)<<24)+(((uint32_t)G)<<16)+(((uint32_t)B)<<8)+((uint32_t)A)))






void (sw_draw_circle)(canvas_circle_t *c, struct gui_dispdev *dc)
{

    gui_circleshape_t circle;
    float center_y = (float)c->cy;
    float center_x = (float)c->cx;
    float radius = (float)c->r;
    circle.center.x = center_x;
    circle.center.y = center_y;
    circle.radius = radius;
    uint32_t *framebuffer = (void *)(dc->frame_buf);
    int x = _UI_MAX((int16_t)(center_x - radius), 0);
    int y = _UI_MAX((int16_t)(center_y - radius), 0);
    int x2 = _UI_MIN(radius * 2 + (int16_t)(center_x - radius), dc->fb_width);
    int y2 = _UI_MIN(radius * 2 + (int16_t)(center_y - radius), dc->fb_height);
    if (get_alpha_from_rgba(c->fill.color_data.rgba))
    {
        for (int i = y; i < y2; i++)
        {
            for (int j = x; j < x2; j++)
            {
                gui_point_t point = {.x = (float)j, .y = (float)i};
                if (judge_point_in_circle(&(circle), &point))
                {
                    //blend_rgba_to_rgba_screen((uint32_t *)framebuffer+i*dc->fb_width+j, c->fill_rgba);
                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), c->fill.color_data.rgba);
                }
            }
        }
    }
    if (get_alpha_from_rgba(c->stroke.fill.color_data.rgba) && c->stroke.stroke_width)
    {
        gui_circleshape_t circle_outer;
        float center_y = (float)c->cy;
        float center_x = (float)c->cx;
        float radius = (float)(c->r + c->stroke.stroke_width);
        circle_outer.center.x = center_x;
        circle_outer.center.y = center_y;
        circle_outer.radius = radius;
        int x = _UI_MAX((int16_t)(center_x - radius), 0);
        int y = _UI_MAX((int16_t)(center_y - radius), 0);
        int x2 = _UI_MIN(radius * 2 + (int16_t)(center_x - radius), dc->fb_width);
        int y2 = _UI_MIN(radius * 2 + (int16_t)(center_y - radius), dc->fb_height);
        for (int i = y; i < y2; i++)
        {
            for (int j = x; j < x2; j++)
            {
                gui_point_t point = {.x = (float)j, .y = (float)i};
                if ((!judge_point_in_circle(&(circle), &point)) && judge_point_in_circle(&(circle_outer), &point))
                {
                    //blend_rgba_to_rgba_screen((uint32_t *)framebuffer+i*dc->fb_width+j, c->fill_rgba);
                    blend_rgba_to_screen((void *)(framebuffer + i * dc->fb_width + j), c->stroke.fill.color_data.rgba);
                }
            }
        }
    }




}

void (sw_draw_rectangle)(canvas_rectangle_t *r, struct gui_dispdev *dc)
{
    uint32_t *framebuffer = (void *)(dc->frame_buf);
    int x = _UI_MAX(r->x, 0);
    int y = _UI_MAX(r->y, 0);
    int x2 = _UI_MIN(((int)(r->x + r->width)), ((int)dc->fb_width));
    int y2 = _UI_MIN(((int)(r->y + r->height)), ((int)dc->fb_height));
    for (int i = y; i < y2; i++)
    {
        for (int j = x; j < x2; j++)
        {
            blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), r->fill.color_data.rgba);
        }
    }
}
void (sw_draw_arc)(canvas_arc_t *c, struct gui_dispdev *dc)
{
    float start_degree = c->start_angle - ((int)c->start_angle) / 360 * 360;
    float end_degree = c->end_angle - ((int)c->end_angle) / 360;
    if (start_degree != end_degree)
    {
        gui_circleshape_t circle;
        float center_y = (float)c->cy;
        float center_x = (float)c->cx;
        float radius = (float)c->r;
        circle.center.x = center_x;
        circle.center.y = center_y;
        circle.radius = radius;
        uint32_t *framebuffer = (void *)(dc->frame_buf);
        int x = _UI_MAX((int16_t)(center_x - radius), 0);
        int y = _UI_MAX((int16_t)(center_y - radius), 0);
        int x2 = _UI_MIN(radius * 2 + (int16_t)(center_x - radius), dc->fb_width);
        int y2 = _UI_MIN(radius * 2 + (int16_t)(center_y - radius), dc->fb_height);
        gui_line_t cut_line, cut_line2;
        gui_point_t center = {.x = center_x, .y = center_y};
        get_line_from_point_and_degree(&center, start_degree, &cut_line);
        get_line_from_point_and_degree(&center, end_degree, &cut_line2);
        bool start_judge_show = false;
        bool end_judge_show = false;
        if (get_alpha_from_rgba(c->fill.color_data.rgba))
        {
            for (int i = y; i < y2; i++)
            {
                for (int j = x; j < x2; j++)
                {
                    gui_point_t point = {.x = (float)j, .y = (float)i};
                    if (judge_point_in_circle(&(circle), &point))
                    {
                        bool start_up = judge_point_above_line(&cut_line, &point);
                        bool end_up = judge_point_above_line(&cut_line2, &point);
                        if (start_degree < 180)
                        {
                            if (j < center_x)
                            {
                                start_judge_show = true;
                            }
                            else
                            {
                                start_judge_show = !start_up;
                            }
                        }
                        else
                        {
                            if (j < center_x)
                            {
                                start_judge_show = start_up;
                            }
                            else
                            {
                                start_judge_show = false;
                            }

                        }
                        if (start_degree == 0)
                        {
                            start_judge_show = true;
                        }
                        if (start_degree == 360)
                        {
                            start_judge_show = false;
                        }


                        if (end_degree < 180)
                        {
                            if (j >= center_x)
                            {
                                end_judge_show = end_up;
                            }
                            else
                            {
                                end_judge_show = false;
                            }

                        }
                        else
                        {
                            if (j >= center_x)
                            {
                                end_judge_show = true;
                            }
                            else
                            {
                                end_judge_show = !end_up;
                            }

                        }
                        if (end_degree == 360)
                        {
                            end_judge_show = true;
                        }
                        if (end_degree == 0)
                        {
                            end_judge_show = false;
                        }

                        if (end_degree > start_degree)
                        {
                            if (!(end_judge_show && start_judge_show))
                            {
                                if (c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), c->fill.color_data.rgba);
                                }
                            }
                            else if (!c->counterclockwise)
                            {
                                blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), c->fill.color_data.rgba);
                            }

                        }
                        else if (end_degree < start_degree)
                        {
                            if (!(end_judge_show || start_judge_show))
                            {
                                if (c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), c->fill.color_data.rgba);
                                }
                            }
                            else
                            {
                                if (!c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j), c->fill.color_data.rgba);
                                }
                            }

                        }
                        //blend_rgba_to_rgba_screen((uint32_t *)framebuffer+i*dc->fb_width+j, c->fill_rgba);

                    }
                }
            }
        }
        if (get_alpha_from_rgba(c->stroke.fill.color_data.rgba) && c->stroke.stroke_width)
        {
            gui_circleshape_t circle_outer;
            float center_y = (float)c->cy;
            float center_x = (float)c->cx;
            float radius = (float)(c->r + c->stroke.stroke_width);
            circle_outer.center.x = center_x;
            circle_outer.center.y = center_y;
            circle_outer.radius = radius;
            int x = _UI_MAX((int16_t)(center_x - radius), 0);
            int y = _UI_MAX((int16_t)(center_y - radius), 0);
            int x2 = _UI_MIN(radius * 2 + (int16_t)(center_x - radius), dc->fb_width);
            int y2 = _UI_MIN(radius * 2 + (int16_t)(center_y - radius), dc->fb_height);
            for (int i = y; i < y2; i++)
            {
                for (int j = x; j < x2; j++)
                {
                    gui_point_t point = {.x = (float)j, .y = (float)i};
                    if ((!judge_point_in_circle(&(circle), &point)) && judge_point_in_circle(&(circle_outer), &point))
                    {
                        bool start_up = judge_point_above_line(&cut_line, &point);
                        bool end_up = judge_point_above_line(&cut_line2, &point);
                        if (start_degree < 180)
                        {
                            if (j < center_x)
                            {
                                start_judge_show = true;
                            }
                            else
                            {
                                start_judge_show = !start_up;
                            }
                        }
                        else
                        {
                            if (j < center_x)
                            {
                                start_judge_show = start_up;
                            }
                            else
                            {
                                start_judge_show = false;
                            }

                        }
                        if (start_degree == 0)
                        {
                            start_judge_show = true;
                        }
                        if (start_degree == 360)
                        {
                            start_judge_show = false;
                        }


                        if (end_degree < 180)
                        {
                            if (j >= center_x)
                            {
                                end_judge_show = end_up;
                            }
                            else
                            {
                                end_judge_show = false;
                            }

                        }
                        else
                        {
                            if (j >= center_x)
                            {
                                end_judge_show = true;
                            }
                            else
                            {
                                end_judge_show = !end_up;
                            }

                        }
                        if (end_degree == 360)
                        {
                            end_judge_show = true;
                        }
                        if (end_degree == 0)
                        {
                            end_judge_show = false;
                        }

                        if (end_degree > start_degree)
                        {
                            if (!(end_judge_show && start_judge_show))
                            {
                                if (c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j),
                                                             c->stroke.fill.color_data.rgba);
                                }
                            }
                            else if (!c->counterclockwise)
                            {
                                blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j),
                                                         c->stroke.fill.color_data.rgba);
                            }

                        }
                        else if (end_degree < start_degree)
                        {
                            if (!(end_judge_show || start_judge_show))
                            {
                                if (c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j),
                                                             c->stroke.fill.color_data.rgba);
                                }
                            }
                            else
                            {
                                if (!c->counterclockwise)
                                {
                                    blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j),
                                                             c->stroke.fill.color_data.rgba);
                                }
                            }

                        }
                    }
                }
            }
            if (c->stroke.stroke_linecap == ROUND)
            {
                gui_point_t cstart; bool start_left = true;
                gui_point_t cend; bool end_left = true;
                if (start_degree > 0 && start_degree <= 180)
                {
                    start_left = false;
                }
                if (end_degree > 0 && end_degree <= 180)
                {
                    end_left = false;
                }
                get_piont_line_cross_circle(center_x, center_y, radius - c->stroke.stroke_width / 2, &cut_line,
                                            &cstart, start_left);
                get_piont_line_cross_circle(center_x, center_y, radius - c->stroke.stroke_width / 2, &cut_line2,
                                            &cend, end_left);
                canvas_circle_t circle = {0};
                circle.cx = (cstart.x);
                circle.cy = (cstart.y + 0.5f);
                //circle.fill.color_data.rgba = 0xffff00ff;
                circle.r = c->stroke.stroke_width / 2;
                circle.fill.color_data.rgba = c->stroke.fill.color_data.rgba;
                sw_draw_circle(&circle, dc);
                circle.cx = (cend.x);
                circle.cy = (cend.y + 0.5f);
                sw_draw_circle(&circle, dc);
            }

        }
    }

}
#include "math.h"
static void get_first_rect_point(gui_rect_t *rect, gui_point_t *point1, gui_point_t *point2,
                                 float width)
{
    //DBG_DIRECT("$$$$line89$$$$ point1.x = %f,point1.y =%f,point2.x = %f,point2.y =%f ",point1->x,point1->y,point2->x,point2->y);
    float hypotenuse = sqrtf((point1->y - point2->y) * (point1->y - point2->y) +
                             (point1->x - point2->x) * (point1->x - point2->x));
    //DBG_DIRECT("$$$$line92$$$$ hypotenuse = %f",hypotenuse);
    float y = width / 2 * (point1->x - point2->x) / (hypotenuse);
    float x = (point1->y - point2->y) / (point1->x - point2->x) * y;
    //DBG_DIRECT("$$$$line95$$$$ y = %f,x = %f,",y,x);
    rect->p1.x = point2->x - x;
    rect->p1.y = point2->y + y;
    rect->p2.x = point2->x + x;
    rect->p2.y = point2->y - y;
    rect->p3.x = point1->x + x;
    rect->p3.y = point1->y - y;
    rect->p4.x = point1->x - x;
    rect->p4.y = point1->y + y;
    // DBG_DIRECT("$$$$line104$$$$ rect->p1.x = %f,rect->p1.y =%f,rect->p2.x= %f,rect->p2.y =%f ",rect->p1.x,rect->p1.y,rect->p2.x,rect->p2.y);
}
static void get_rect(gui_rect_t *rect, gui_point_t *point1, gui_point_t *point2, float width)
{
    get_first_rect_point(rect, point1, point2, width);
    get_line_from_point(&rect->p1, &rect->p2, &rect->l1);
    get_line_from_point(&rect->p2, &rect->p3, &rect->l2);
    get_line_from_point(&rect->p3, &rect->p4, &rect->l3);
    get_line_from_point(&rect->p4, &rect->p1, &rect->l4);
    /* DBG_DIRECT("$$$$line114$$$$ rect->l1.a = %f,rect->l1.b = %f,",rect->l1.a,rect->l1.b);
     DBG_DIRECT("$$$$line114$$$$ rect->l1.a = %f,rect->l1.b = %f,",rect->l1.a,rect->l1.b);
     DBG_DIRECT("$$$$line114$$$$ rect->l1.a = %f,rect->l1.b = %f,",rect->l1.a,rect->l1.b);
     DBG_DIRECT("$$$$line114$$$$ rect->l1.a = %f,rect->l1.b = %f,",rect->l1.a,rect->l1.b);*/
}
static void get_scale(gui_rect_t *rect, gui_scale_t *scale)
{
    float maxx = rect->p1.x;
    // DBG_DIRECT("$$$$line118$$$$ maxx = %f,",maxx);
    if (maxx < rect->p1.x)
    {
        maxx = rect->p1.x;
    }
    if (maxx < rect->p2.x)
    {
        maxx = rect->p2.x;
    }
    if (maxx < rect->p3.x)
    {
        maxx = rect->p3.x;
    }
    if (maxx < rect->p4.x)
    {
        maxx = rect->p4.x;
    }

    float minx = rect->p1.x;
    if (minx > rect->p1.x)
    {
        minx = rect->p1.x;
    }
    if (minx > rect->p2.x)
    {
        minx = rect->p2.x;
    }
    if (minx > rect->p3.x)
    {
        minx = rect->p3.x;
    }
    if (minx > rect->p4.x)
    {
        minx = rect->p4.x;
    }
    float maxy = rect->p1.y;
    if (maxy < rect->p1.y)
    {
        maxy = rect->p1.y;
    }
    if (maxy < rect->p2.y)
    {
        maxy = rect->p2.y;
    }
    if (maxy < rect->p3.y)
    {
        maxy = rect->p3.y;
    }
    if (maxy < rect->p4.y)
    {
        maxy = rect->p4.y;
    }
    float miny = rect->p1.y;
    if (miny > rect->p1.y)
    {
        miny = rect->p1.y;
    }
    if (miny > rect->p2.y)
    {
        miny = rect->p2.y;
    }
    if (miny > rect->p3.y)
    {
        miny = rect->p3.y;
    }
    if (miny > rect->p4.y)
    {
        miny = rect->p4.y;
    }
    //DBG_DIRECT("$$$$line118$$$$ maxx = %f,",maxx);
    scale->maxx = (int)ceilf(maxx);
    //DBG_DIRECT("$$$$line118$$$$ maxx = %d,",scale->maxx);
    scale->maxy = (int)ceilf(maxy);
    scale->minx = (int)floor(minx);
    scale->miny = (int)floor(miny);
    if (scale->maxx < 0)
    {
        scale->maxx = 0;
    }
    if (scale->maxy < 0)
    {
        scale->maxy = 0;
    }
    if (scale->minx < 0)
    {
        scale->minx = 0;
    }
    if (scale->miny < 0)
    {
        scale->miny = 0;
    }
    //DBG_DIRECT("$$$$line205$$$$ scale->maxx = %d,scale->maxy =%d,scale->minx= %d,scale->miny =%d ",scale->maxx,scale->maxy,scale->minx,scale->miny);

}
static bool judge_point_in_rect(gui_rect_t *rect, gui_point_t *point)
{
    if (rect->l1.vertical || rect->l3.vertical)
    {
        //l1 right
        //DBG_DIRECT("$$$$judge point->y=%f,rect->l1.b = %f,rect->l1.a= %f",point->y ,rect->l1.b,rect->l1.a);
        float x = rect->l1.x;
        //DBG_DIRECT("$$$$judge ,x=%f,point->x = %f",x,point->x);
        if ((int)floorf(x) < (int)ceilf(point->x))
        {
            return false;
        }
        //l2 up
        float y = rect->l2.a * point->x + rect->l2.b;
        if ((int)floorf(y) < (int)ceilf(point->y))
        {
            return false;
        }
        //l3 left
        x = rect->l3.x;
        if ((int)ceilf(x) > (int)floorf(point->x))
        {
            return false;
        }
        //l4 down
        y = rect->l4.a * point->x + rect->l4.b;
        if ((int)ceilf(y) > (int)floorf(point->y))
        {
            return false;
        }
        return true;
    }
    else
    {
        //l1 right
        //DBG_DIRECT("$$$$judge point->y=%f,rect->l1.b = %f,rect->l1.a= %f",point->y ,rect->l1.b,rect->l1.a);
        float x = (point->y - rect->l1.b) / rect->l1.a;
        //DBG_DIRECT("$$$$judge ,x=%f,point->x = %f",x,point->x);
        if ((int)floorf(x) < (int)ceilf(point->x))
        {
            return false;
        }
        //l2 up
        float y = rect->l2.a * point->x + rect->l2.b;
        if ((int)floorf(y) < (int)ceilf(point->y))
        {
            return false;
        }
        //l3 left
        x = (point->y - rect->l3.b) / rect->l3.a;
        if ((int)ceilf(x) > (int)floorf(point->x))
        {
            return false;
        }
        //l4 down
        y = rect->l4.a * point->x + rect->l4.b;
        if ((int)ceilf(y) > (int)floorf(point->y))
        {
            return false;
        }
        return true;

    }


}
void (sw_draw_line)(canvas_line_t *l, struct gui_dispdev *dc)
{
    gui_rect_t rect = {0};
    gui_point_t point1 = {0};
    gui_point_t point2 = {0};
    point1.x = l->x1;
    point1.y = l->y1;
    point2.x = l->x2;
    point2.y = l->y2;
    get_rect(&rect, &point1, &point2, l->stroke.stroke_width);
    gui_scale_t scale = {0};
    get_scale(&rect, &scale);
    gui_point_t point = {0};
    uint32_t *framebuffer = (void *)(dc->frame_buf);
    for (size_t i = scale.miny; i < scale.maxy; i++)
    {
        for (size_t j = scale.minx; j < scale.maxx; j++)
        {
            point.x = j;
            point.y = i;
            if (judge_point_in_rect(&rect, &point))
            {
                //DBG_DIRECT("$$$$judge_point_in_rect true");
                //change bit
                blend_rgba_to_rgb_screen((void *)(framebuffer + i * dc->fb_width + j),
                                         l->stroke.fill.color_data.rgba);
            }

        }
    }
}
void (sw_draw_polyline)(canvas_polyline_t *p, struct gui_dispdev *dc)
{

}
#include <stdio.h>
#include <string.h>
#include <float.h>

//#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
void (sw_draw_svg)(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
                   float scale)
{
    {
        NSVGimage *image = svg;

        NSVGrasterizer *rast = NULL;
        unsigned char *img = NULL;
        int w, h;
        //const char* filename = "Third_Party/nanosvg/example/blurry-gradient-haikei.svg";
        // const char *filename = svg;
        //printf("parsing %s\n", filename);

        //image = nsvgParseFromFile(filename, "px", 96.0f);
        if (image == NULL)
        {
            gui_log("Could not open SVG image.\n");
            goto error;
        }
        w = (int)(image->width * scale);
        h = (int)(image->height * scale);

        rast = nsvgCreateRasterizer();
        if (rast == NULL)
        {
            gui_log("Could not init rasterizer.\n");
            goto error;
        }

        img = gui_malloc(w * h * 4);
        if (img == NULL)
        {
            gui_log("Could not alloc image buffer.\n");
            goto error;
        }

        gui_log("rasterizing image %d x %d\n", w, h);
        nsvgRasterize(rast, image, 0, 0, scale, img, w, h, w * 4);

        //stbi_write_png("svg.png", w, h, 4, img, w*4);
        {

            int image_x = x;
            int image_y = y;

            int image_w = w ;//for more data,20220104,howie
            int image_h = h ;//for more data,20220104,howie
            // int source_w = w;
            // int source_h = h;

            int x_start = _UI_MAX(image_x, 0);
            int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
            int y_start = _UI_MAX(dc->section.y1, image_y);
            int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

            if ((x_start >= x_end) || (y_start >= y_end))
            {
                return;
            }
            uint32_t image_off = (uint32_t)img;
            uint8_t *writebuf = dc->frame_buf;


            uint8_t source_bytes_per_pixel = 4;
            uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
            int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
            for (uint32_t i = y_start; i < y_end; i++)
            {
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                               source_bytes_per_pixel * x_start;
                for (uint32_t j = x_start; j < x_end; j++)
                {

                    // int x = j;
                    // int y = i;


                    uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                    // uint8_t opacity_value = pixel[3];

                    canvas_color_t color = {.color.channel.alpha = pixel[3],
                                            .color.channel.blue = pixel[2],
                                            .color.channel.green = pixel[1],
                                            .color.channel.red = pixel[0],
                                           };

                    blend_rgba_to_rgb_screen(writebuf + (write_off + j) * dc_bytes_per_pixel, color.color.rgba);

                }

            }
        }


error:
        nsvgDeleteRasterizer(rast);
        //nsvgDelete(image);

        gui_free(img);
    }
}
void (sw_draw_path)(void *path, struct gui_dispdev *dc)
{

}

