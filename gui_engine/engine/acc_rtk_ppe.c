#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include <rtl_ppe.h>
#include <rtl_rtzip.h>
#include <drv_lcd.h>
#include "math.h"
#include "trace.h"


#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);



static double acc_ppe_ceil(double _x)
{
#if defined (__GNUC__)
    return (_x + 1.0); //todo, only fix gcc compile issue
#endif
    return ceil(_x);
}


void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    if ((rect->x1 >= dc->screen_width - 1) || (rect->y1 >= dc->screen_height - 1) ||
        (rect->x1 + image->img_w <= 0) || (rect->y1 + image->img_h <= 0))
    {
        return;
    }
    ppe_buffer_t source, target;
    memset(&source, 0, sizeof(ppe_buffer_t));
    memset(&target, 0, sizeof(ppe_buffer_t));
    struct gui_rgb_data_head *head = image->data;

    if (dc->bit_depth == 16)
    {
        target.format = PPE_BGR565;
    }
    else if (dc->bit_depth == 32)
    {
        target.format = PPE_ABGR8888;
    }
    else if (dc->bit_depth == 24)
    {
        target.format = PPE_BGR888;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;

    switch (head->type)
    {
    case RGB565:
        source.format = PPE_BGR565;
        break;
    case RGB888:
        source.format = PPE_BGR888;
        break;
    case RGBA8888:
        source.format = PPE_ABGR8888;
        break;
    case RTZIP_COMPRESS:
        {
            const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                        struct gui_rgb_data_head));
            if (header->algorithm_type.pixel_bytes == 0)
            {
                source.format = PPE_BGR565;
            }
            else if (header->algorithm_type.pixel_bytes == 2)
            {
                source.format = PPE_ABGR8888;
            }
            else if (header->algorithm_type.pixel_bytes == 1)
            {
                source.format = PPE_BGR888;
            }
            else
            {
                return;
            }
            break;
        }
    default:
        return;
    }
    source.width = image->img_w;
    source.height = image->img_h;
    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.memory = (void *)source.address;
    source.global_alpha_en = true;
    source.global_alpha = image->opacity_value << 24;
    if (image->blend_mode == IMG_MAGIC_MATRIX)
    {
        if ((image->matrix->m[0][1] == 0) && (image->matrix->m[1][0] == 0))
        {
            if ((image->matrix->m[0][0] != 1) || (image->matrix->m[1][1] != 1) && 1)
            {
                float scale_x = image->matrix->m[0][0], scale_y = image->matrix->m[1][1];
                if ((image->img_w == (int)(image->img_w * scale_x)) &&
                    (image->img_h == (int)(image->img_h * scale_y)))
                {
                    PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                    if (head->type == RTZIP_COMPRESS)
                    {
                        RCC_PeriphClockCmd(APBPeriph_RTZIP, APBPeriph_RTZIP_CLOCK, ENABLE);
                        uint32_t start_line = 0, end_line = 0;
                        if ((dc->section.y1 <= rect->y1) && (dc->section.y2 > rect->y1))
                        {
                            start_line = 0;
                            if ((rect->y1 + image->img_h) <= dc->section.y2)
                            {
                                end_line = image->img_h - 1;
                            }
                            else
                            {
                                end_line = dc->section.y2 - rect->y1 - 1;
                            }
                        }
                        else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
                        {
                            trans.y = 0;
                            start_line = dc->section.y1 - rect->y1;
                            end_line = dc->section.y2 - rect->y1 - 1;
                        }
                        else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                                 && (dc->section.y1 < (rect->y1 + image->img_h)))
                        {
                            trans.y = 0;
                            start_line = dc->section.y1 - rect->y1;
                            end_line = image->img_h - 1;
                        }
                        source.height = end_line - start_line + 1;
                        const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                                    struct gui_rgb_data_head));
                        RTZIP_decode_range range;
                        range.start_column = rect->x1 < 0 ? -rect->x1 : 0;
                        range.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                                           - range.start_column : header->raw_pic_width - 1;
                        range.start_line = start_line;
                        range.end_line = end_line;
                        RTZIP_DMA_config dma_cfg;
                        source.memory = gui_malloc((end_line - start_line + 1) * (range.end_column - range.start_column + 1)
                                                   * (header->algorithm_type.pixel_bytes + 2));
                        source.address = (uint32_t)source.memory;
                        source.width = range.end_column - range.start_column + 1;
                        dma_cfg.output_buf = (uint32_t *)source.memory;
                        dma_cfg.RX_DMA_channel_num = 3;
                        dma_cfg.TX_DMA_channel_num = 1;
                        dma_cfg.RX_DMA_channel = GDMA_Channel3;
                        dma_cfg.TX_DMA_channel = GDMA_Channel1;
                        trans.x = rect->x1 < 0 ? 0 : rect->x1 - dc->section.x1;
                        RTZIP_ERROR err = RTZIP_Decode((uint8_t *)header, &range, &dma_cfg);
                        if (err)
                        {
                            gui_free(source.memory);
                            return;
                        }
                    }
                    PPE_blend(&source, &target, &trans);
                    return;
                }
                ppe_buffer_t scaled_img;
                PPE_translate_t trans;
                PPE_rect_t range;
                PPE_rect_t scale_rect;
                memset(&scaled_img, 0, sizeof(ppe_buffer_t));
                scaled_img.width = image->img_w * scale_x;
                scaled_img.height = image->img_h * scale_y > (dc->section.y2 - dc->section.y1) ?
                                    (dc->section.y2 - dc->section.y1) : image->img_h * scale_y;
                scaled_img.height += acc_ppe_ceil(scale_y);
                uint32_t modified_height = image->img_h * scale_y;
                switch (source.format)
                {
                case PPE_BGR565:
                    scaled_img.format = PPE_BGR565;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 2);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 +
                                                                           (int)acc_ppe_ceil(
                                                                               scale_y) * 2 + 1) * 2);
                    }
                    break;
                case PPE_BGR888:
                    scaled_img.format = PPE_BGR888;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 3);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 +
                                                                           (int)acc_ppe_ceil(
                                                                               scale_y) + 1) * 3);
                    }
                    break;
                case PPE_BGRA8888:
                    scaled_img.format = PPE_BGRA8888;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 4);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 +
                                                                           (int)acc_ppe_ceil(
                                                                               scale_y) + 1) * 4);
                    }
                    break;
                default:
                    return;
                }
                trans.x = (int)image->matrix->m[0][2];
                trans.y = 0;
                range.left = 0;
                range.right = scaled_img.width - 1;
                range.top = 0;
                range.bottom = scaled_img.height - 1;
                scaled_img.global_alpha_en = true;
                scaled_img.global_alpha = image->opacity_value << 24;
                if ((dc->section.y1 <= rect->y1) && (dc->section.y2 > rect->y1))
                {
                    scale_rect.top = 0;
                    if ((rect->y1 + modified_height) <= dc->section.y2)
                    {
                        scale_rect.bottom = source.height - 1;
                    }
                    else
                    {
                        scale_rect.bottom = (dc->section.y2 - rect->y1) / scale_y;
                    }
                    trans.y = (int)image->matrix->m[1][2] - dc->section.y1;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                }
                else if ((dc->section.y2 < (rect->y1 + modified_height)) && (dc->section.y1 > rect->y1))
                {
                    scale_rect.top = (dc->section.y1 - rect->y1) / scale_y;
                    scale_rect.bottom = (dc->section.y2 - rect->y1) / scale_y;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                }
                else if ((dc->section.y2 >= (rect->y1 + modified_height)) && (dc->section.y1 > rect->y1)
                         && (dc->section.y1 < (rect->y1 + modified_height)))
                {
                    scale_rect.top = (dc->section.y1 - rect->y1) / scale_y;
                    scale_rect.bottom = source.height - 1;
                    scaled_img.height = rect->y1 + modified_height - dc->section.y1;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                }
                else
                {
                    if (scaled_img.memory != NULL)
                    {
                        gui_free(scaled_img.memory);
                    }
                    return;
                }
                if (head->type == RTZIP_COMPRESS)
                {
                    source.height = scale_rect.bottom - scale_rect.top + 1;
                    const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                                struct gui_rgb_data_head));
                    RTZIP_decode_range range;
                    range.start_column = scale_rect.left;
                    range.end_column = scale_rect.right;
                    range.start_line = scale_rect.top;
                    range.end_line = scale_rect.bottom;
                    scale_rect.left = 0;
                    scale_rect.right = range.end_column - range.start_column;
                    scale_rect.top = 0;
                    scale_rect.bottom = range.end_line - range.start_line;
                    RTZIP_DMA_config dma_cfg;
                    source.memory = gui_malloc(source.height * header->raw_pic_width *
                                               (header->algorithm_type.pixel_bytes + 2));
                    source.address = (uint32_t)source.memory;
                    dma_cfg.output_buf = (uint32_t *)source.memory;
                    dma_cfg.RX_DMA_channel_num = 3;
                    dma_cfg.TX_DMA_channel_num = 1;
                    dma_cfg.RX_DMA_channel = GDMA_Channel3;
                    dma_cfg.TX_DMA_channel = GDMA_Channel1;
                    RTZIP_ERROR err = RTZIP_Decode((uint8_t *)header, &range, &dma_cfg);
                    if (err)
                    {
                        gui_free(source.memory);
                        return;
                    }
                }
                PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
                if (err == PPE_SUCCESS)
                {
                    PPE_blend(&scaled_img, &target, &trans);
                }
                if (dc->type == DC_SINGLE)
                {
                    //tlsf_free(tlsf, scaled_img.memory);
                }
                else if (dc->type == DC_RAMLESS)
                {
                    if (head->type == RTZIP_COMPRESS)
                    {
                        gui_free(source.memory);
                    }
                    gui_free(scaled_img.memory);
                }
            }
            else
            {
                PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                if (head->type == RTZIP_COMPRESS)
                {
                    RCC_PeriphClockCmd(APBPeriph_RTZIP, APBPeriph_RTZIP_CLOCK, ENABLE);
                    uint32_t start_line = 0, end_line = 0;
                    if ((dc->section.y1 <= rect->y1) && (dc->section.y2 > rect->y1))
                    {
                        start_line = 0;
                        if ((rect->y1 + image->img_h) <= dc->section.y2)
                        {
                            end_line = image->img_h - 1;
                        }
                        else
                        {
                            end_line = dc->section.y2 - rect->y1 - 1;
                        }
                    }
                    else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
                    {
                        trans.y = 0;
                        start_line = dc->section.y1 - rect->y1;
                        end_line = dc->section.y2 - rect->y1 - 1;
                    }
                    else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                             && (dc->section.y1 < (rect->y1 + image->img_h)))
                    {
                        trans.y = 0;
                        start_line = dc->section.y1 - rect->y1;
                        end_line = image->img_h - 1;
                    }
                    source.height = end_line - start_line + 1;
                    const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                                struct gui_rgb_data_head));
                    RTZIP_decode_range range;
                    range.start_column = rect->x1 < 0 ? -rect->x1 : 0;
                    range.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                                       - range.start_column : header->raw_pic_width - 1;
                    range.start_line = start_line;
                    range.end_line = end_line;
                    RTZIP_DMA_config dma_cfg;
                    source.memory = gui_malloc((end_line - start_line + 1) * (range.end_column - range.start_column + 1)
                                               * (header->algorithm_type.pixel_bytes + 2));
                    source.address = (uint32_t)source.memory;
                    source.width = range.end_column - range.start_column + 1;
                    dma_cfg.output_buf = (uint32_t *)source.memory;
                    dma_cfg.RX_DMA_channel_num = 3;
                    dma_cfg.TX_DMA_channel_num = 1;
                    dma_cfg.RX_DMA_channel = GDMA_Channel3;
                    dma_cfg.TX_DMA_channel = GDMA_Channel1;
                    trans.x = rect->x1 < 0 ? 0 : rect->x1 - dc->section.x1;
                    RTZIP_ERROR err = RTZIP_Decode((uint8_t *)header, &range, &dma_cfg);

                    if (err)
                    {
                        gui_free(source.memory);
                        return;
                    }
                }
                PPE_blend(&source, &target, &trans);
                if (head->type == RTZIP_COMPRESS)
                {
                    gui_free(source.memory);
                }
            }
        }
        else
        {
            sw_acc_blit(image, dc, rect);
        }
    }
    else
    {
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            source.color_key_en = true;
            source.color_key_value = 0x00000000;
        }

        PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
        if (head->type == RTZIP_COMPRESS)
        {
            RCC_PeriphClockCmd(APBPeriph_RTZIP, APBPeriph_RTZIP_CLOCK, ENABLE);
            uint32_t start_line = 0, end_line = 0;
            if ((dc->section.y1 <= rect->y1) && (dc->section.y2 > rect->y1))
            {
                start_line = 0;
                if ((rect->y1 + image->img_h) <= dc->section.y2)
                {
                    end_line = image->img_h - 1;
                }
                else
                {
                    end_line = dc->section.y2 - rect->y1 - 1;
                }
            }
            else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
            {
                trans.y = 0;
                start_line = dc->section.y1 - rect->y1;
                end_line = dc->section.y2 - rect->y1 - 1;
            }
            else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                     && (dc->section.y1 < (rect->y1 + image->img_h)))
            {
                trans.y = 0;
                start_line = dc->section.y1 - rect->y1;
                end_line = image->img_h - 1;
            }
            source.height = end_line - start_line + 1;
            const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                        struct gui_rgb_data_head));
            RTZIP_decode_range range;
            range.start_column = rect->x1 < 0 ? -rect->x1 : 0;
            range.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                               - range.start_column : header->raw_pic_width - 1;
            range.start_line = start_line;
            range.end_line = end_line;
            RTZIP_DMA_config dma_cfg;
            source.memory = gui_malloc((end_line - start_line + 1) * (range.end_column - range.start_column + 1)
                                       * (header->algorithm_type.pixel_bytes + 2));
            source.address = (uint32_t)source.memory;
            source.width = range.end_column - range.start_column + 1;
            dma_cfg.output_buf = (uint32_t *)source.memory;
            dma_cfg.RX_DMA_channel_num = 3;
            dma_cfg.TX_DMA_channel_num = 1;
            dma_cfg.RX_DMA_channel = GDMA_Channel3;
            dma_cfg.TX_DMA_channel = GDMA_Channel1;
            trans.x = rect->x1 < 0 ? 0 : rect->x1 - dc->section.x1;
            RTZIP_ERROR err = RTZIP_Decode((uint8_t *)header, &range, &dma_cfg);
            if (err)
            {
                gui_free(source.memory);
                return;
            }
        }
        PPE_blend(&source, &target, &trans);
        if (head->type == RTZIP_COMPRESS)
        {
            gui_free(source.memory);
        }
    }
}
