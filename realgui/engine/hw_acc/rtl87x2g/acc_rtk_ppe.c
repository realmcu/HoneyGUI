#include <stdio.h>
#include <gui_matrix.h>
#include <rtl_ppe.h>
#include <rtl_idu.h>
#include <drv_lcd.h>
#include "math.h"
#include "draw_img.h"
#include "guidef.h"
#include "hal_idu.h"

#define USE_BARE_COPY_ACC       1
#define USE_FINE_BARE_COPY      1

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

static double acc_ppe_ceil(double _x)
{
#if defined (__GNUC__)
    return (_x + 1.0); //todo, only fix gcc compile issue
#else
    return ceil(_x);
#endif
}

static void hw_ppe_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    ppe_rect_t buf_area = {.x1 = dc->section.x1, .x2 = dc->section.x2, \
                           .y1 = dc->section.y1, .y2 = dc->section.y2
                          };
    ppe_rect_t draw_area =
    {
        .x2 = (image->img_target_w + image->img_target_x - 1),
        .y2 = (image->img_target_h + image->img_target_y - 1),
        .x1 = image->img_target_x,
        .y1 = image->img_target_y
    };
    ppe_rect_t constraint_area;
    if (!ppe_rect_intersect(&constraint_area, &buf_area, &draw_area))
    {
        return;
    }

    struct gui_rgb_data_head *head = image->data;

    ppe_buffer_t source, target;
    memset(&source, 0, sizeof(ppe_buffer_t));
    memset(&target, 0, sizeof(ppe_buffer_t));

    if (dc->bit_depth == 16)
    {
        target.format = PPE_RGB565;
    }
    else if (dc->bit_depth == 32)
    {
        target.format = PPE_ARGB8888;
    }
    else if (dc->bit_depth == 24)
    {
        target.format = PPE_RGB888;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.width  = dc->fb_width;
    target.stride = dc->fb_width;
    target.height = dc->fb_height;

    switch (head->type)
    {
    case RGB565:
        source.format = PPE_RGB565;
        break;
    case RGB888:
        source.format = PPE_RGB888;
        break;
    case ARGB8888:
        source.format = PPE_ARGB8888;
        break;
    case ARGB8565:
        source.format = PPE_ARGB8565;
        break;
    default:
        return;
    }
    source.width = image->img_w;
    source.stride = image->img_w;
    source.height = image->img_h;
    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.memory = (void *)source.address;
    source.global_alpha_en = true;
    source.global_alpha = image->opacity_value;
    PPE_BLEND_MODE mode = PPE_BYPASS_MODE;

    if (image->blend_mode == IMG_FILTER_BLACK)
    {
        source.color_key_en = true;
        source.color_key_value = 0x00000000;
        mode = PPE_SRC_OVER_MODE;
    }
    else if (image->blend_mode == IMG_BYPASS_MODE)
    {
        mode = PPE_BYPASS_MODE;
    }
    else
    {
        if (source.format < PPE_BGR888 || source.format > PPE_RGB565 || \
            (source.global_alpha_en == DISABLE && source.global_alpha != 0xFF))
        {
            mode = PPE_SRC_OVER_MODE;
        }
        else
        {
            mode = PPE_BYPASS_MODE;
        }
    }
    if ((image->matrix.m[0][0] != 1 || image->matrix.m[1][1] != 1) && \
        (image->img_w != image->img_target_w && image->img_h != image->img_target_h))
    {
        float scale_x = image->matrix.m[0][0], scale_y = image->matrix.m[1][1];
        ppe_buffer_t scaled_img;
        ppe_translate_t trans;
        ppe_rect_t range;
        ppe_rect_t scale_rect;
        memset(&scaled_img, 0, sizeof(ppe_buffer_t));
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            scaled_img.color_key_en = true;
            scaled_img.color_key_value = 0x00000000;
            mode = PPE_SRC_OVER_MODE;
        }
        trans.x = constraint_area.x1 - buf_area.x1;
        trans.y = constraint_area.y1 - buf_area.y1;
        scale_rect.top = (constraint_area.y1 - draw_area.y1) / scale_y;
        scale_rect.bottom = ceil((constraint_area.y2 - draw_area.y1) / scale_y) + ceil(
                                1 / scale_y);
        if (scale_rect.bottom >= image->img_h)
        {
            scale_rect.bottom = image->img_h - 1;
        }
        scale_rect.left = (constraint_area.x1 - draw_area.x1) / scale_x;
        scale_rect.right = ceil((constraint_area.x2 - draw_area.x1) / scale_x) + ceil(
                               1 / scale_x);
        if (scale_rect.right >= image->img_w)
        {
            scale_rect.right = image->img_w - 1;
        }
        scaled_img.width = (uint32_t)((scale_rect.right - scale_rect.left + 1) * scale_x);
        scaled_img.height = (uint32_t)((scale_rect.bottom - scale_rect.top + 1) * scale_y);
        scaled_img.stride = scaled_img.width;
        if (scaled_img.height < 1)
        {
            return;
        }
        scaled_img.global_alpha_en = true;
        scaled_img.global_alpha = image->opacity_value;
        if (head->compress)
        {
            source.height = scale_rect.bottom - scale_rect.top + 1;
            source.width = scale_rect.right - scale_rect.left + 1;
            source.stride = source.width;
            const IDU_file_header *header = (IDU_file_header *)((uint32_t)image->data + sizeof(
                                                                    struct gui_rgb_data_head));
            hal_idu_decompress_info info;
            info.start_column = scale_rect.left;
            info.end_column = scale_rect.right;
            info.start_line = scale_rect.top;
            info.end_line = scale_rect.bottom;
            scale_rect.left = 0;
            scale_rect.right = info.end_column - info.start_column;
            scale_rect.top = 0;
            scale_rect.bottom = info.end_line - info.start_line;
            info.raw_data_address = (uint32_t)header;
            source.memory = gui_malloc(source.height * source.width *
                                       ppe_get_format_data_len(source.format));
            source.address = (uint32_t)source.memory;
            bool ret = hal_idu_decompress(&info, (uint8_t *)source.memory);
            if (!ret)
            {
                gui_free(source.memory);
                return;
            }
        }
        scaled_img.format = source.format;
        scaled_img.memory = gui_malloc(scaled_img.width * scaled_img.height * ppe_get_format_data_len(
                                           source.format));
        PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
        if (err == PPE_SUCCESS)
        {
            err = PPE_blend(&scaled_img, &target, &trans, mode);
        }
        if (head->compress)
        {
            gui_free(source.memory);
        }
        gui_free(scaled_img.memory);
    }
    else
    {
        ppe_translate_t trans = {.x = draw_area.x1 - dc->section.x1, .y = draw_area.y1 - dc->section.y1};
        if (head->compress)
        {
            RCC_PeriphClockCmd(APBPeriph_IDU, APBPeriph_IDU_CLOCK, ENABLE);

            const IDU_file_header *header = (IDU_file_header *)((uint32_t)image->data + sizeof(
                                                                    struct gui_rgb_data_head));
            hal_idu_decompress_info info;
            info.start_column = constraint_area.x1 - draw_area.x1;
            info.end_column = constraint_area.x2 - draw_area.x1;
            info.start_line = constraint_area.y1 - draw_area.y1;
            info.end_line = constraint_area.y2 - draw_area.y1;
            info.raw_data_address = (uint32_t)header;
            source.memory = gui_malloc((info.end_line - info.start_line + 1) * (info.end_column -
                                                                                info.start_column + 1)
                                       * ppe_get_format_data_len(source.format));
            source.address = (uint32_t)source.memory;
            source.width = info.end_column - info.start_column + 1;
            source.height = info.end_line - info.start_line + 1;
            source.stride = source.width;
            trans.x = constraint_area.x1 - dc->section.x1;
            trans.y = constraint_area.y1 - dc->section.y1;
            bool ret = hal_idu_decompress(&info, (uint8_t *)source.memory);
            if (!ret)
            {
                gui_free(source.memory);
                return;
            }
        }
        PPE_ERR err = PPE_blend(&source, &target, &trans, PPE_SRC_OVER_MODE);
        if (head->compress)
        {
            gui_free(source.memory);
        }
    }
}

#if USE_BARE_COPY_ACC
static void hw_bare_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    ppe_rect_t buf_area = {.x1 = dc->section.x1, .x2 = dc->section.x2, \
                           .y1 = dc->section.y1, .y2 = dc->section.y2
                          };
    ppe_rect_t draw_area =
    {
        .x2 = (image->img_target_w + image->img_target_x - 1),
        .y2 = (image->img_target_h + image->img_target_y - 1),
        .x1 = image->img_target_x,
        .y1 = image->img_target_y
    };
    ppe_rect_t constraint_area;
    if (!ppe_rect_intersect(&constraint_area, &buf_area, &draw_area))
    {
        return;
    }

    struct gui_rgb_data_head *head = image->data;

    ppe_buffer_t source, target;
    memset(&source, 0, sizeof(ppe_buffer_t));
    memset(&target, 0, sizeof(ppe_buffer_t));

    if (dc->bit_depth == 16)
    {
        target.format = PPE_RGB565;
    }
    else if (dc->bit_depth == 32)
    {
        target.format = PPE_ARGB8888;
    }
    else if (dc->bit_depth == 24)
    {
        target.format = PPE_RGB888;
    }
    uint8_t pixel_size = ppe_get_format_data_len(target.format);
#if USE_FINE_BARE_COPY
    if (((constraint_area.x2 - constraint_area.x1) * pixel_size % 4) != 0 \
        && head->compress == true && image->matrix.m[0][0] == 1 && image->matrix.m[1][1] == 1)
    {
        hw_ppe_blit(image, dc, rect);
        return;
    }
#endif
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    target.stride = dc->fb_width;

    switch (head->type)
    {
    case RGB565:
        source.format = PPE_RGB565;
        break;
    case RGB888:
        source.format = PPE_RGB888;
        break;
    case ARGB8888:
        source.format = PPE_ARGB8888;
        break;
    case ARGB8565:
        source.format = PPE_ARGB8565;
        break;
    default:
        return;
    }
    source.width = image->img_w;
    source.height = image->img_h;
    source.stride = image->img_w;
    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.memory = (void *)source.address;
    source.global_alpha_en = true;
    source.global_alpha = image->opacity_value;

    if ((image->matrix.m[0][0] != 1 || image->matrix.m[1][1] != 1) && \
        (image->img_w != image->img_target_w && image->img_h != image->img_target_h))
    {
        float scale_x = image->matrix.m[0][0], scale_y = image->matrix.m[1][1];
        ppe_buffer_t scaled_img;
        ppe_translate_t trans;
        ppe_rect_t range;
        ppe_rect_t scale_rect;
        memset(&scaled_img, 0, sizeof(ppe_buffer_t));
        trans.x = constraint_area.x1 - buf_area.x1;
        trans.y = constraint_area.y1 - buf_area.y1;
        scale_rect.top = (constraint_area.y1 - draw_area.y1) / scale_y;
        scale_rect.bottom = ceil((constraint_area.y2 - draw_area.y1) / scale_y) + ceil(
                                1 / scale_y);
        if (scale_rect.bottom >= image->img_h)
        {
            scale_rect.bottom = image->img_h - 1;
        }
        scale_rect.left = (constraint_area.x1 - draw_area.x1) / scale_x;
        scale_rect.right = ceil((constraint_area.x2 - draw_area.x1) / scale_x) + ceil(
                               1 / scale_x);
        if (scale_rect.right >= image->img_w)
        {
            scale_rect.right = image->img_w - 1;
        }
        scaled_img.width = (uint32_t)((scale_rect.right - scale_rect.left + 1) * scale_x);
        scaled_img.height = (uint32_t)((scale_rect.bottom - scale_rect.top + 1) * scale_y);
        scaled_img.stride = scaled_img.width;
        if (scaled_img.height < 1)
        {
            return;
        }
        scaled_img.global_alpha_en = true;
        scaled_img.global_alpha = image->opacity_value;
        if (head->compress)
        {
            source.height = scale_rect.bottom - scale_rect.top + 1;
            source.width = scale_rect.right - scale_rect.left + 1;
            source.stride = source.width;
            const IDU_file_header *header = (IDU_file_header *)((uint32_t)image->data + sizeof(
                                                                    struct gui_rgb_data_head));
            hal_idu_decompress_info info;
            info.start_column = scale_rect.left;
            info.end_column = scale_rect.right;
            info.start_line = scale_rect.top;
            info.end_line = scale_rect.bottom;
            scale_rect.left = 0;
            scale_rect.right = info.end_column - info.start_column;
            scale_rect.top = 0;
            scale_rect.bottom = info.end_line - info.start_line;
            info.raw_data_address = (uint32_t)header;
            source.memory = gui_malloc(source.height * source.width *
                                       ppe_get_format_data_len(source.format));
            source.address = (uint32_t)source.memory;
            bool ret = hal_idu_decompress(&info, (uint8_t *)source.memory);
            if (!ret)
            {
                gui_free(source.memory);
                return;
            }
        }
        scaled_img.format = source.format;
        scaled_img.memory = gui_malloc(scaled_img.width * scaled_img.height * ppe_get_format_data_len(
                                           source.format));

        PPE_ERR err = PPE_Scale_Rect_Cover(&source, &target, &trans, scale_x, scale_y, &scale_rect);
        if (head->compress)
        {
            gui_free(source.memory);
        }
        gui_free(scaled_img.memory);
    }
    else
    {
        ppe_translate_t trans = {.x = draw_area.x1 - dc->section.x1, .y = draw_area.y1 - dc->section.y1};
        if (head->compress)
        {
            RCC_PeriphClockCmd(APBPeriph_IDU, APBPeriph_IDU_CLOCK, ENABLE);
            const IDU_file_header *header = (IDU_file_header *)((uint32_t)image->data + sizeof(
                                                                    struct gui_rgb_data_head));
            hal_idu_decompress_info info;
            info.length = (constraint_area.x2 - constraint_area.x1 + 1) * pixel_size;
            if (info.length % 4)
            {
                if (constraint_area.x2 - draw_area.x1 + 1 < image->img_w && constraint_area.x2 + 1 <= buf_area.x2)
                {
                    constraint_area.x2 += 1;
                    info.length += pixel_size;
                }
                else
                {
                    constraint_area.x2 -= 1;
                    info.length -= pixel_size;
                }
            }
            info.start_column = constraint_area.x1 - draw_area.x1;
            info.end_column = constraint_area.x2 - draw_area.x1;
            info.start_line = constraint_area.y1 - draw_area.y1;
            info.end_line = constraint_area.y2 - draw_area.y1;
            info.raw_data_address = (uint32_t)header;
            info.dst_stride = dc->fb_width * pixel_size;
            if (info.length == 0)
            {
                return;
            }
            trans.x = constraint_area.x1 - dc->section.x1;
            trans.y = constraint_area.y1 - dc->section.y1;
            uint8_t *dest_address = (uint8_t *)(target.address + (trans.y * dc->fb_width + trans.x) *
                                                pixel_size);
            bool ret = hal_idu_decompress_rect(&info, dest_address);
            if (!ret)
            {
                sw_acc_blit(image, dc, rect);
                return;
            }
        }
        else
        {
            uint32_t basic_x = constraint_area.x1 - draw_area.x1;
            uint32_t basic_y = constraint_area.y1 - draw_area.y1;
            uint32_t target_x = constraint_area.x1 - dc->section.x1;
            uint32_t target_y = constraint_area.y1 - dc->section.y1;
            hal_idu_dma_info info;
            info.length = (constraint_area.x2 - constraint_area.x1 + 1) * pixel_size;
            info.src_stride = image->img_w * pixel_size;
            info.dst_stride = dc->fb_width * pixel_size;
            info.height = constraint_area.y2 - constraint_area.y1 + 1;
            uint32_t dst_start_address = target.address + (target_x + target_y * dc->fb_width) * pixel_size;
            uint32_t src_start_address = source.address + (basic_x + basic_y * image->img_w) * pixel_size;
            hal_dma_copy(&info, (uint8_t *)src_start_address, (uint8_t *)dst_start_address);
        }
    }
}
#endif


void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    if ((image->matrix.m[0][1] != 0 || image->matrix.m[1][0] != 0 || \
         image->matrix.m[2][0] != 0 || image->matrix.m[2][1] != 0) || \
        image->matrix.m[2][2] != 1)
    {
        sw_acc_blit(image, dc, rect);
        return;
    }
#if USE_BARE_COPY_ACC
    struct gui_rgb_data_head *head = image->data;
    if (image->blend_mode != IMG_FILTER_BLACK && \
        ((dc->bit_depth == 16 && head->type == RGB565) || \
         ((dc->bit_depth == 24 && head->type == RGB888) && \
          (head->compress == 0 || image->matrix.m[0][0] != 1 || image->matrix.m[1][1] != 1)) || \
         (dc->bit_depth == 32 && head->type == ARGB8888 && image->blend_mode == PPE_BYPASS_MODE)) && \
        image->opacity_value == 0xFF)
    {
        hw_bare_blit(image, dc, rect);
    }
    else
#endif
    {
        hw_ppe_blit(image, dc, rect);
    }

}

bool hw_acc_idu_decode(draw_img_t *image, gui_rect_t *rect, uint8_t *output)
{
    if (image == NULL || rect == NULL || output == NULL)
    {
        GUI_ASSERT(image != NULL && rect != NULL && output != NULL);
        return false;
    }
    struct gui_rgb_data_head output_header;
    struct gui_rgb_data_head *head = image->data;
    memset(&output_header, 0, sizeof(struct gui_rgb_data_head));
    const IDU_file_header *header = (IDU_file_header *)((uint32_t)image->data + sizeof(
                                                            struct gui_rgb_data_head));
    output_header.type = head->type;

    hal_idu_decompress_info info;
    info.start_column = rect->x1;
    info.end_column = rect->x2;
    info.start_line = rect->y1;
    info.end_line = rect->y2;
    info.raw_data_address = (uint32_t)header;
    output_header.w = info.end_column - info.start_column + 1;
    output_header.h = info.end_line - info.start_line + 1;
    bool ret = hal_idu_decompress(&info, (uint8_t *)(output + 8));

    if (!ret)
    {
        memcpy(output, &output_header, sizeof(gui_rgb_data_head_t));
        return true;
    }
    else
    {
        return false;
    }
}

void hw_acc_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_IDU, APBPeriph_IDU_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
    uint8_t dma_channl1 = 1, dma_channel2 = 3;
    hal_dma_channel_init(&dma_channl1, &dma_channel2);
}
