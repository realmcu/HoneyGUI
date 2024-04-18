#include <stdio.h>
#include <gui_matrix.h>
#include <rtl_ppe.h>
#include <rtl_imdc.h>
#include <drv_lcd.h>
#include "math.h"
#include "trace.h"
#include "draw_img.h"
#include "guidef.h"
#include "hal_imdc.h"

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);



static double acc_ppe_ceil(double _x)
{
#if defined (__GNUC__)
    return (_x + 1.0); //todo, only fix gcc compile issue
#endif
    return ceil(_x);
}


void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    if ((rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y2) ||
        (rect->x2 < dc->section.x1) || (rect->y2 < dc->section.y1))
    {
        return;
    }
    ppe_buffer_t source, target;
    memset(&source, 0, sizeof(ppe_buffer_t));
    memset(&target, 0, sizeof(ppe_buffer_t));
    struct gui_rgb_data_head *head = image->data;

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
    target.height = dc->fb_height;

    switch (head->type)
    {
    case RGB565:
        source.format = PPE_RGB565;
        break;
    case RGB888:
        source.format = PPE_RGB888;
        break;
    case RGBA8888:
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
    if (1)
    {
        if ((image->matrix->m[0][1] == 0) && (image->matrix->m[1][0] == 0))
        {
            if ((image->matrix->m[0][0] != 1) || (image->matrix->m[1][1] != 1))
            {
                float scale_x = image->matrix->m[0][0], scale_y = image->matrix->m[1][1];
                if ((image->img_w == image->target_w) &&
                    (image->img_h == image->target_h))
                {
                    ppe_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                    if (head->compress)
                    {
                        RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
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
                                end_line = dc->section.y2 - rect->y1;
                            }
                        }
                        else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
                        {
                            trans.y = 0;
                            start_line = dc->section.y1 - rect->y1;
                            end_line = dc->section.y2 - rect->y1;
                        }
                        else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                                 && (dc->section.y1 < (rect->y1 + image->img_h)))
                        {
                            trans.y = 0;
                            start_line = dc->section.y1 - rect->y1;
                            end_line = image->img_h - 1;
                        }
                        source.height = end_line - start_line + 1;
                        const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                                  struct gui_rgb_data_head));
                        hal_imdc_decompress_info info;
                        info.start_column = rect->x1 < 0 ? -rect->x1 : 0;
                        info.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                                          - info.start_column : header->raw_pic_width - 1;
                        info.start_line = start_line;
                        info.end_line = end_line;
                        info.raw_data_address = (uint32_t)header;
                        source.memory = gui_malloc((end_line - start_line + 1) * (info.end_column - info.start_column + 1)
                                                   * (header->algorithm_type.pixel_bytes + 2));
                        source.address = (uint32_t)source.memory;
                        source.width = info.end_column - info.start_column + 1;
                        trans.x = rect->x1 < 0 ? 0 : rect->x1 - dc->section.x1;
                        bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
                        if (!ret)
                        {
                            gui_free(source.memory);
                            return;
                        }
                    }
                    PPE_blend(&source, &target, &trans, mode);
                    return;
                }
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
                trans.x = rect->x1 - dc->section.x1;
                trans.y = 0;
                if (rect->y1 < dc->section.y1)
                {
                    scale_rect.top = (dc->section.y1 - rect->y1) / scale_y;
                }
                else
                {
                    scale_rect.top = 0;
                    trans.y = rect->y1 - dc->section.y1;
                }
                if (rect->y2 > dc->section.y1 + dc->fb_height)
                {
                    scale_rect.bottom = ceil((dc->section.y1 + dc->fb_height - rect->y1 - 1) / scale_y) + ceil(
                                            1 / scale_y);
                }
                else
                {
                    scale_rect.bottom = ceil((rect->y2 - rect->y1 + 1) / scale_y);
                }
                if (scale_rect.bottom >= image->img_h)
                {
                    scale_rect.bottom = image->img_h - 1;
                }
                scale_rect.left = 0;
                scale_rect.right = image->img_w - 1;
                scaled_img.width = (uint32_t)image->img_w * scale_x;
                scaled_img.height = (uint32_t)((scale_rect.bottom - scale_rect.top + 1) * scale_y);
                if (scaled_img.height < 1)
                {
                    return;
                }
                scaled_img.global_alpha_en = true;
                scaled_img.global_alpha = image->opacity_value;
                if (head->compress)
                {
                    source.height = scale_rect.bottom - scale_rect.top + 1;
                    const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                              struct gui_rgb_data_head));
                    hal_imdc_decompress_info info;
                    info.start_column = scale_rect.left;
                    info.end_column = scale_rect.right;
                    info.start_line = scale_rect.top;
                    info.end_line = scale_rect.bottom;
                    scale_rect.left = 0;
                    scale_rect.right = info.end_column - info.start_column;
                    scale_rect.top = 0;
                    scale_rect.bottom = info.end_line - info.start_line;
                    info.raw_data_address = (uint32_t)header;
                    source.memory = gui_malloc(source.height * header->raw_pic_width *
                                               (header->algorithm_type.pixel_bytes + 2));
                    source.address = (uint32_t)source.memory;
                    bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
                    if (!ret)
                    {
                        gui_free(source.memory);
                        return;
                    }
                }
                scaled_img.format = source.format;
                switch (source.format)
                {
                case PPE_RGB565:
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 2);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * scaled_img.height * 2);
                    }
                    break;
                case PPE_RGB888:
                case PPE_ARGB8565:
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 3);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * scaled_img.height * 3);
                    }
                    break;
                case PPE_ARGB8888:
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)acc_ppe_ceil(scale_y)) * 4);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * scaled_img.height * 4);
                    }
                    break;
                default:
                    return;
                }
                PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
                if (err == PPE_SUCCESS)
                {
                    err = PPE_blend(&scaled_img, &target, &trans, mode);
                }
                if (dc->type == DC_SINGLE)
                {
                    //tlsf_free(tlsf, scaled_img.memory);
                }
                else if (dc->type == DC_RAMLESS)
                {
                    if (head->compress)
                    {
                        gui_free(source.memory);
                    }
                    gui_free(scaled_img.memory);
                }
            }
            else
            {
                ppe_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                if (head->compress)
                {
                    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
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
                            end_line = dc->section.y2 - rect->y1;
                        }
                    }
                    else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
                    {
                        trans.y = 0;
                        start_line = dc->section.y1 - rect->y1;
                        end_line = dc->section.y2 - rect->y1;
                    }
                    else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                             && (dc->section.y1 < (rect->y1 + image->img_h)))
                    {
                        trans.y = 0;
                        start_line = dc->section.y1 - rect->y1;
                        end_line = image->img_h - 1;
                    }
                    source.height = end_line - start_line + 1;
                    const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                              struct gui_rgb_data_head));
                    hal_imdc_decompress_info info;
                    info.start_column = rect->x1 < 0 ? -rect->x1 : 0;
                    info.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                                      - info.start_column : header->raw_pic_width - 1;
                    info.start_line = start_line;
                    info.end_line = end_line;
                    info.raw_data_address = (uint32_t)header;
                    source.memory = gui_malloc((end_line - start_line + 1) * (info.end_column - info.start_column + 1)
                                               * (header->algorithm_type.pixel_bytes + 2));
                    source.address = (uint32_t)source.memory;
                    source.width = info.end_column - info.start_column + 1;
                    trans.x = rect->x1 < 0 ? 0 : rect->x1 - dc->section.x1;
                    bool ret = hal_imdc_decompress(&info, (uint8_t *)source.memory);
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

        ppe_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
        if (head->compress)
        {
            RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
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
                    end_line = dc->section.y2 - rect->y1;
                }
            }
            else if ((dc->section.y2 < (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1))
            {
                trans.y = 0;
                start_line = dc->section.y1 - rect->y1;
                end_line = dc->section.y2 - rect->y1;
            }
            else if ((dc->section.y2 >= (rect->y1 + image->img_h)) && (dc->section.y1 > rect->y1)
                     && (dc->section.y1 < (rect->y1 + image->img_h)))
            {
                trans.y = 0;
                start_line = dc->section.y1 - rect->y1;
                end_line = image->img_h - 1;
            }
            source.height = end_line - start_line + 1;
            const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                      struct gui_rgb_data_head));
            IMDC_decode_range range;
            range.start_column = rect->x1 < 0 ? -rect->x1 : 0;
            range.end_column = rect->x1 + header->raw_pic_width > dc->screen_width ? dc->screen_width - rect->x1
                               - range.start_column : header->raw_pic_width - 1;
            range.start_line = start_line;
            range.end_line = end_line;
            IMDC_DMA_config dma_cfg;
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
            IMDC_ERROR err = IMDC_Decode((uint8_t *)header, &range, &dma_cfg);
            if (err)
            {
                gui_free(source.memory);
                return;
            }
        }
        PPE_blend(&source, &target, &trans, mode);
        if (head->compress)
        {
            gui_free(source.memory);
        }
    }
}

bool hw_acc_imdc_decode(draw_img_t *image, gui_rect_t *rect, uint8_t *output)
{
    if (image == NULL || rect == NULL || output == NULL)
    {
        GUI_ASSERT(image != NULL && rect != NULL && output != NULL);
        return false;
    }
    struct gui_rgb_data_head output_header;
    struct gui_rgb_data_head *head = image->data;
    memset(&output_header, 0, sizeof(struct gui_rgb_data_head));
    const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                              struct gui_rgb_data_head));
    output_header.type = head->type;

    hal_imdc_decompress_info info;
    info.start_column = rect->x1;
    info.end_column = rect->x2;
    info.start_line = rect->y1;
    info.end_line = rect->y2;
    info.raw_data_address = (uint32_t)header;
    output_header.w = info.end_column - info.start_column + 1;
    output_header.h = info.end_line - info.start_line + 1;
    bool ret = hal_imdc_decompress(&info, (uint8_t *)(output + 8));

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
    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
    uint8_t dma_channl1 = 1, dma_channel2 = 3;
    hal_dma_channel_init(&dma_channl1, &dma_channel2);
}
