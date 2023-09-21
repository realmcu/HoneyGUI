#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include <gui_matrix.h>
#include "acc_engine.h"
#include <rtl_PPEV2.h>
#include <rtl_rtzip.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gdma.h>
#include <dma_channel.h>
#include "trace.h"

#define PPEV2_ACC_MIN_OPA       3
#define PPEV2_MIN_PIXEL         100

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);

void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    if (image->opacity_value <= PPEV2_ACC_MIN_OPA)
    {
        return;
    }
    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));

    switch (dc->bit_depth)
    {
    case 16:
        target.format = PPEV2_RGB565;
        break;
    case 24:
        target.format = PPEV2_RGB888;
        break;
    case 32:
        target.format = PPEV2_RGBA8888;
        break;
    default:
        return;
    }
    target.address = (uint32_t)dc->frame_buf;
    target.width = dc->fb_width;
    target.height = dc->fb_height;
    target.const_color = 0;
    target.win_x_min = 0;
    target.win_x_max = target.width;
    target.win_y_min = 0;
    target.win_y_max = target.height;

    struct gui_rgb_data_head *head = image->data;
    if (head->type != RTZIP_COMPRESS)
    {
        switch (head->type)
        {
        case RGB565:
            source.format = PPEV2_RGB565;
            break;
        case RGB888:
            source.format = PPEV2_RGB888;
            break;
        case RGBA8888:
            source.format = PPEV2_RGBA8888;
            break;
        default:
            return;
        }
        source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
        source.opacity = image->opacity_value;
        source.width = image->img_w;
        source.height = image->img_h;
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
            source.color_key_min = 0;
            source.color_key_max = 0;
        }
        else
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
        }
        source.win_x_min = 0;
        source.win_x_max = target.width;
        source.win_y_min = 0;
        source.win_y_max = target.height;
        memcpy(&source.matrix, image->matrix, sizeof(float) * 9);
        memcpy(&source.inv_matrix, image->inverse, sizeof(float) * 9);
        if (dc->type == DC_RAMLESS)
        {
            ppe_matrix_t temp;
            ppe_get_identity(&temp);
            ppe_translate(0, dc->section.y1 * (-1.0f), &temp);
            ppe_mat_multiply(&temp, &source.matrix);
            memcpy(&source.matrix, &temp, sizeof(float) * 9);
            ppe_matrix_inverse(&temp);
            memcpy(&source.inv_matrix, &temp, sizeof(float) * 9);
        }

        PPEV2_err err = PPEV2_Blend(&target, &source);
        if (err != PPEV2_SUCCESS)
        {
            sw_acc_blit(image, dc, rect);
        }
    }
    else
    {
        if (dc->type == DC_SINGLE)  //may need PSRAM
        {
            RCC_PeriphClockCmd(APBPeriph_RTZIP, APBPeriph_RTZIP_CLOCK, ENABLE);
            const RTZIP_file_header *header = (RTZIP_file_header *)((uint32_t)image->data + sizeof(
                                                                        struct gui_rgb_data_head));
            RTZIP_decode_range range;
            range.start_column = 0;
            range.end_column = header->raw_pic_width - 1;
            range.start_line = 0;
            range.end_line = header->raw_pic_height - 1;
            RTZIP_DMA_config dma_cfg;
            source.address = (uint32_t)gui_malloc((range.end_line - range.start_line + 1) *
                                                  (range.end_column - range.start_column + 1)
                                                  * (header->algorithm_type.pixel_bytes + 2));
            if (source.address == NULL)
            {
                gui_log("no mem remain");
                return;
            }
            source.width = range.end_column - range.start_column + 1;
            source.height = range.end_line - range.start_line + 1;
            uint8_t rtzip_tx_dma_num = 0xa5, rtzip_rx_dma_num = 0xa5;
            if (!GDMA_channel_request(&rtzip_tx_dma_num, NULL, true))
            {
                gui_log("no dma for tx");
                return;
            }
            if (!GDMA_channel_request(&rtzip_rx_dma_num, NULL, true))
            {
                gui_log("no dma for rx");
                return;
            }
            dma_cfg.output_buf = (uint32_t *)source.address;
            dma_cfg.RX_DMA_channel_num = rtzip_rx_dma_num;
            dma_cfg.TX_DMA_channel_num = rtzip_tx_dma_num;
            dma_cfg.RX_DMA_channel = DMA_CH_BASE(rtzip_rx_dma_num);
            dma_cfg.TX_DMA_channel = DMA_CH_BASE(rtzip_tx_dma_num);
            RTZIP_ERROR err = RTZIP_Decode((uint8_t *)header, &range, &dma_cfg);
            if (err)
            {
                gui_free((void *)source.address);
                return;
            }
            source.opacity = image->opacity_value;
            if (image->blend_mode == IMG_FILTER_BLACK)
            {
                source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
                source.color_key_min = 0;
                source.color_key_max = 0;
            }
            else
            {
                source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
            }
            source.win_x_min = 0;
            source.win_x_max = target.width;
            source.win_y_min = 0;
            source.win_y_max = target.height;
            memcpy(&source.matrix, image->matrix, sizeof(float) * 9);
            memcpy(&source.inv_matrix, image->inverse, sizeof(float) * 9);
            PPEV2_Blend(&target, &source);
            gui_free((void *)source.address);
        }
        else
        {
            //TODO: split RTZIP(may be difficult), suppose to have PSRAM
            return;
        }
    }
}
