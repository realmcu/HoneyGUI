#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>

typedef struct
{
    union
    {
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } channel;
        uint32_t abgr_full;
    };
    /* data */
} ABGR_color_t;

static uint32_t pixel_convert_to_base(GUI_FormatType format, uint32_t input)
{
    uint32_t color;
    switch (format)
    {
    case RGB888:
        {
            color = (input | 0xFF000000);
        }
        break;
    case RGB565:
        {
            color = ((((input & 0x1F) << 3) | ((input & 0x18) >> 2)) |
                     (((input & 0x7E) << 5) | ((input & 0x60) >> 1)) |
                     (((input & 0xF1) << 8) | ((input & 0xE0) << 3)) |
                     0xFF000000);
        }
        break;
    case ARGB8565:
        {
            uint32_t alpha = ((color & 0xFF0000) << 8);
            color = ((((input & 0x1F) << 3) | ((input & 0x18) >> 2)) |
                     (((input & 0x7E) << 5) | ((input & 0x60) >> 1)) |
                     (((input & 0xF1) << 8) | ((input & 0xE0) << 3)) |
                     alpha);
        }
        break;
    case RGBA8888:
        {
            color = input;
        }
        break;
    default:
        break;
    }
    return color;
}

void acc_sw_ppe_sim_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;


    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;

    uint32_t *data = (uint32_t *)image_base;

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t source_bytes_per_pixel = gui_image_get_pixel(image);
    uint8_t opacity_value = image->opacity_value;

    uint32_t *writebuf = (uint32_t *)dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    gui_matrix_t *inverse = &image->inverse;
    int32_t matrix[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] = inverse->m[i][j] * 65536;
        }
    }

    for (uint32_t y = y_start; y <= y_end; y++)
    {
        for (uint32_t x = x_start; x <= x_end; x++)
        {
#if 0
            int64_t bot = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2];
            int64_t xtop = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2];
            int64_t ytop = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2];
            int64_t x_int = (xtop * 1.0f / bot) * 65536;
            int64_t y_int = (ytop * 1.0f / bot) * 65536;
            float x_f = x_int / 65536.0f;
            float y_f = y_int / 65536.0f;
            float x_ratio = (x_int % 65536) / 65536.0f;
            float y_ratio = (x_int % 65536) / 65536.0f;
            int32_t old_x = (int)x_f;
            int32_t old_y = (int)y_f;
#else
            float X = inverse->m[0][0] * x + inverse->m[0][1] * y + inverse->m[0][2];
            float Y = inverse->m[1][0] * x + inverse->m[1][1] * y + inverse->m[1][2];
            float Z = inverse->m[2][0] * x + inverse->m[2][1] * y + inverse->m[2][2];
            float x_f = X / Z;
            float y_f = Y / Z;
            int32_t old_x = (int)x_f;
            int32_t old_y = (int)y_f;
            float x_ratio = x_f - old_x;
            float y_ratio = y_f - old_y;
            if (x_ratio < 0)
            {
                x_ratio = x_ratio * -1;
            }
            if (y_ratio < 0)
            {
                y_ratio = y_ratio * -1;
            }
#endif
            if ((x_f > source_w - 0.5) || (x_f <= -0.5) || (y_f <= -0.5) || (y_f > source_h - 0.5))
            {
                continue;
            }
            if (rect != NULL)
            {
                if ((x_f >= rect->x2) || (x_f < rect->x1) || (y_f < rect->y1) || (y_f >= rect->y2))
                {
                    continue;
                }
            }

            ABGR_color_t pix1, pix2, pix3, pix4;
            pix1.abgr_full = 0xFFFFFFFF;
            pix2.abgr_full = 0xFFFFFFFF;
            pix3.abgr_full = 0xFFFFFFFF;
            pix4.abgr_full = 0xFFFFFFFF;
            if (x_f < 0)
            {
                pix1.abgr_full = 0;
                pix3.abgr_full = 0;
            }
            if (y_f < 0)
            {
                pix1.abgr_full = 0;
                pix2.abgr_full = 0;
            }
            if (x_f > source_w - 1)
            {
                pix2.abgr_full = 0;
                pix4.abgr_full = 0;
            }
            if (y_f > source_h - 1)
            {
                pix3.abgr_full = 0;
                pix4.abgr_full = 0;
            }

            if (pix1.abgr_full != 0)
            {
                pix1.abgr_full = data[old_y * source_w + old_x];
            }
            if (pix2.abgr_full != 0)
            {
                pix2.abgr_full = data[old_y * source_w + old_x + 1];
            }
            if (pix3.abgr_full != 0)
            {
                pix3.abgr_full = data[(old_y + 1) * source_w + old_x];
            }
            if (pix4.abgr_full != 0)
            {
                pix4.abgr_full = data[(old_y + 1) * source_w + old_x + 1];
            }

            ABGR_color_t result;
            result.channel.a = (pix1.channel.a * (1 - x_ratio) + pix2.channel.a * x_ratio) * (1 - y_ratio) + \
                               (pix3.channel.a * (1 - x_ratio) + pix4.channel.a * x_ratio) * y_ratio;
            result.channel.b = (pix1.channel.b * (1 - x_ratio) + pix2.channel.b * x_ratio) * (1 - y_ratio) + \
                               (pix3.channel.b * (1 - x_ratio) + pix4.channel.b * x_ratio) * y_ratio;
            result.channel.g = (pix1.channel.g * (1 - x_ratio) + pix2.channel.g * x_ratio) * (1 - y_ratio) + \
                               (pix3.channel.g * (1 - x_ratio) + pix4.channel.g * x_ratio) * y_ratio;
            result.channel.r = (pix1.channel.r * (1 - x_ratio) + pix2.channel.r * x_ratio) * (1 - y_ratio) + \
                               (pix3.channel.r * (1 - x_ratio) + pix4.channel.r * x_ratio) * y_ratio;
            int write_off = (y - dc->section.y1) * dc->fb_width + x;
            ABGR_color_t bg = {.abgr_full = writebuf[write_off]};
            ABGR_color_t blend = {.channel.a = result.channel.a * result.channel.a / 255.0f + (0xFF - result.channel.a) * bg.channel.a / 255.0f,
                                  .channel.b = result.channel.a * result.channel.b / 255.0f + (0xFF - result.channel.a) * bg.channel.b / 255.0f,
                                  .channel.g = result.channel.a * result.channel.g / 255.0f + (0xFF - result.channel.a) * bg.channel.g / 255.0f,
                                  .channel.r = result.channel.a * result.channel.r / 255.0f + (0xFF - result.channel.a) * bg.channel.r / 255.0f
                                 };
            writebuf[write_off] = blend.abgr_full;
        }
    }

}
