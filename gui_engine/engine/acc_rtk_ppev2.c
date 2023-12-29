#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include <gui_matrix.h>
#include "acc_engine.h"
#include <rtl_PPEV2.h>
#include <rtl_imdc.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gdma.h>
#include <dma_channel.h>
#include "trace.h"
#include "rtl876x_gpio.h"

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

#define PPE_ACC_CACHE_SOURCE        0

#define PPEV2_ACC_MIN_OPA       3
#define PPEV2_MIN_PIXEL         100

#if PPE_ACC_CACHE_SOURCE
#define PPEV2_CACHE_BUF_SIZE       (1 * 1024)
#define PPEV2_PFB_SIZE             (PPEV2_CACHE_BUF_SIZE/2)
static uint8_t ppe_cache_buf[PPEV2_CACHE_BUF_SIZE];
static uint8_t *ppe_fb_port1, *ppe_fb_port2, *pfb_buffer;
static uint8_t memcpy_dma_num = 0xa5;
static void memcpy_by_dma(void *dst, void *src, uint32_t size)
{

    GUI_ASSERT((uint32_t)dst % 4 == 0);
    GUI_ASSERT((uint32_t)src % 2 == 0);
    GUI_ASSERT(size % 2 == 0);
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum = memcpy_dma_num;
    GDMA_InitStruct.GDMA_DIR = GDMA_DIR_MemoryToMemory;
    GDMA_InitStruct.GDMA_BufferSize = size / 2;
    GDMA_InitStruct.GDMA_SourceInc = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_SourceAddr = (uint32_t)src;
    GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)dst;
    GDMA_Init(DMA_CH_BASE(memcpy_dma_num), &GDMA_InitStruct);
    GDMA_INTConfig(memcpy_dma_num, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(memcpy_dma_num, ENABLE);

}

static void wait_memcpy_dma(void)
{
    while (GDMA_GetTransferINTStatus(memcpy_dma_num) != SET);
    GDMA_ClearINTPendingBit(memcpy_dma_num, GDMA_INT_Transfer);
}

static void ppe_get_new_area(ppe_rect_t *rect, ppe_buffer_t *image)
{
    struct gui_pox pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul((gui_matrix_t *)&image->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];


    pox.p[0] = image->width - 1;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul((gui_matrix_t *)&image->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }


    pox.p[0] = 0.0f;
    pox.p[1] = image->height - 1;
    pox.p[2] = 1.0f;
    pox_mul((gui_matrix_t *)&image->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = image->width - 1;
    pox.p[1] = image->height - 1;
    pox.p[2] = 1.0f;
    pox_mul((gui_matrix_t *)&image->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    if (x_min < 0)
    {
        rect->x = 0;
    }
    else
    {
        rect->x = (int16_t)x_min;
    }
    if (y_min < 0)
    {
        rect->y = 0;
    }
    else
    {
        rect->y = (int16_t)y_min;
    }
    rect->w = (int16_t)x_max - rect->x + 1;
    rect->h = (int16_t)y_max - rect->y + 1;
}
#endif


void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
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
        target.format = PPEV2_ARGB8888;
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
    if (head->type != IMDC_COMPRESS)    //Not compressed
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
            source.format = PPEV2_ARGB8888;
            break;
        default:
            return;
        }
        if (image->blend_mode == IMG_SRC_OVER_MODE)
        {
            if ((image->target_w + image->img_x < dc->section.x1) ||
                (image->target_h + image->img_y < dc->section.y1)
                || (image->img_x >= dc->section.x1 + dc->fb_width) ||
                (image->img_y >= dc->section.y1 + dc->fb_height))
            {
                return;
            }
#if PPE_ACC_CACHE_SOURCE
            uint32_t image_size = image->img_w * image->img_h * PPEV2_Get_Pixel_Size(source.format);
            if (image_size <= PPEV2_CACHE_BUF_SIZE)
            {
#endif
                ppe_rect_t ppe_rect = {.x = 0, .y = 0, .w = dc->fb_width, .h = dc->fb_height};
                if (image->img_x - dc->section.x1 < 0)
                {
                    ppe_rect.x = 0;
                }
                else if (image->img_x > dc->section.x1)
                {
                    ppe_rect.x = image->img_x - dc->section.x1;
                }
                if (image->img_y < 0)
                {
                    ppe_rect.y = 0;
                }
                else if (image->img_y > dc->section.y1)
                {
                    ppe_rect.y = image->img_y - dc->section.y1;
                }
                if (image->img_x + image->target_w >= dc->fb_width)
                {
                    ppe_rect.w = dc->fb_width - ppe_rect.x;
                }
                if (image->img_y + image->target_h >= dc->fb_height)
                {
                    ppe_rect.h = dc->fb_height - ppe_rect.y;
                }
                source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
#if PPE_ACC_CACHE_SOURCE
                memcpy_by_dma(ppe_cache_buf, (void *)source.address, image_size);
                source.address = (uint32_t)ppe_cache_buf;
#endif
                source.opacity = image->opacity_value;
                source.width = image->img_w;
                source.height = image->img_h;
                if (image->blend_mode == IMG_FILTER_BLACK)
                {
                    source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
                    source.color_key_min = 0;
                    source.color_key_max = 0x010101;
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
                ppe_matrix_t pre_trans;
                ppe_get_identity(&pre_trans);
                ppe_translate(ppe_rect.x * (-1.0f), ppe_rect.y * (-1.0f), &pre_trans);
                ppe_mat_multiply(&pre_trans, &source.matrix);
                if (dc->type == DC_RAMLESS)
                {
                    ppe_matrix_t temp;
                    ppe_get_identity(&temp);
                    ppe_translate(0, dc->section.y1 * (-1.0f), &temp);
                    ppe_mat_multiply(&temp, &pre_trans);
                    memcpy(&pre_trans, &temp, sizeof(float) * 9);
                }
                memcpy(&source.matrix, &pre_trans, sizeof(float) * 9);
                ppe_matrix_inverse(&pre_trans);
                memcpy(&source.inv_matrix, &pre_trans, sizeof(float) * 9);
                PPEV2_err err = PPEV2_Blend(&target, &source, &ppe_rect);
                if (err != PPEV2_SUCCESS)
                {
                    sw_acc_blit(image, dc, rect);
                }
#if PPE_ACC_CACHE_SOURCE
            }
            else
            {
                uint16_t blend_count = image_size / PPEV2_PFB_SIZE;
                if (image_size % PPEV2_PFB_SIZE)
                {
                    blend_count += 1;
                }
                uint16_t new_image_height = image->img_h / blend_count ;
                uint16_t rest_image_height = image->img_h - (blend_count - 1) * new_image_height;
                if (rest_image_height > new_image_height)
                {
                    blend_count += rest_image_height / new_image_height;
                    rest_image_height = rest_image_height % new_image_height;
                }
                uint32_t image_data_address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
                if (image->img_h % new_image_height)
                {
                    blend_count = image->img_h / new_image_height;
                }
                else
                {
                    blend_count = image->img_h / new_image_height + 1;
                }
                for (int i = 0; i < blend_count; i++)
                {
                    ppe_rect_t ppe_rect;
                    source.width = image->img_w;
                    if (i < blend_count - 1)
                    {
                        source.height = new_image_height;
                    }
                    else
                    {
                        source.height = rest_image_height;
                    }
                    source.address = image_data_address + i * image->img_w * new_image_height * PPEV2_Get_Pixel_Size(
                                         source.format);
                    if (i == 0)
                    {
                        pfb_buffer = ppe_fb_port1;
                        memcpy_by_dma(pfb_buffer, (void *)source.address,
                                      source.width * source.height * PPEV2_Get_Pixel_Size(source.format));
                        wait_memcpy_dma();
                    }
                    else
                    {
                        wait_memcpy_dma();
                    }
                    source.address = (uint32_t)pfb_buffer;
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
                    ppe_translate(0, new_image_height * i, &source.matrix);
                    ppe_get_new_area(&ppe_rect, &source);
                    if (ppe_rect.x + ppe_rect.w >= dc->fb_width)
                    {
                        ppe_rect.w = dc->fb_width - ppe_rect.x;
                    }
                    if (ppe_rect.y + ppe_rect.h >= dc->fb_height)
                    {
                        ppe_rect.h = dc->fb_height - ppe_rect.y;
                    }
                    ppe_matrix_t pre_trans;
                    ppe_get_identity(&pre_trans);
                    ppe_translate(ppe_rect.x * (-1.0f), ppe_rect.y * (-1.0f), &pre_trans);
                    ppe_mat_multiply(&pre_trans, &source.matrix);
                    if (dc->type == DC_RAMLESS)
                    {
                        ppe_matrix_t temp;
                        ppe_get_identity(&temp);
                        ppe_translate(0, dc->section.y1 * (-1.0f), &temp);
                        ppe_mat_multiply(&temp, &pre_trans);
                        memcpy(&pre_trans, &temp, sizeof(float) * 9);
                    }
                    memcpy(&source.matrix, &pre_trans, sizeof(float) * 9);
                    ppe_matrix_inverse(&pre_trans);
                    memcpy(&source.inv_matrix, &pre_trans, sizeof(float) * 9);
                    if ((i + 1) % 2 == 0)
                    {
                        pfb_buffer = ppe_fb_port1;
                    }
                    else
                    {
                        pfb_buffer = ppe_fb_port2;
                    }
                    if (i < blend_count - 1)
                    {
                        uint32_t next_addr = image_data_address + (i + 1) * image->img_w * new_image_height *
                                             PPEV2_Get_Pixel_Size(source.format);
                        memcpy_by_dma(pfb_buffer, (void *)next_addr,
                                      source.width * new_image_height * PPEV2_Get_Pixel_Size(source.format));
                    }
                    else
                    {
                        uint32_t next_addr = image_data_address + (i + 1) * image->img_w * new_image_height *
                                             PPEV2_Get_Pixel_Size(source.format);
                        memcpy_by_dma(pfb_buffer, (void *)next_addr,
                                      source.width * rest_image_height * PPEV2_Get_Pixel_Size(source.format));
                    }
                    PPEV2_err err = PPEV2_Blend(&target, &source, &ppe_rect);
                    if (err != PPEV2_SUCCESS)
                    {
                        sw_acc_blit(image, dc, rect);
                    }
                }
            }
#endif
        }
        else
        {
            if ((rect->x2 < dc->section.x1) || (rect->y2 < dc->section.y1)
                || (rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y1 + dc->fb_height))
            {
                return;
            }
            float x_ref = 0, y_ref = 0;
            ppe_rect_t ppe_rect = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
            if (ppe_rect.x < 0)
            {
                x_ref = ppe_rect.x;
                ppe_rect.x = 0;
            }
            if (ppe_rect.y < 0)
            {
                y_ref = ppe_rect.y;
                ppe_rect.y = 0;
            }
            if (rect->x2 >= dc->fb_width)
            {
                ppe_rect.w = dc->fb_width - ppe_rect.x;
            }
            else
            {
                ppe_rect.w = rect->x2 - ppe_rect.x;
            }
            if (rect->y2 >= dc->fb_height)
            {
                ppe_rect.h = dc->fb_height - ppe_rect.y;
            }
            else
            {
                ppe_rect.h = rect->y2 - ppe_rect.y;
            }
            source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
#if PPE_ACC_CACHE_SOURCE
            memcpy_by_dma(ppe_cache_buf, (void *)source.address, image_size);
            source.address = (uint32_t)ppe_cache_buf;
#endif
            source.opacity = image->opacity_value;
            source.width = image->img_w;
            source.height = image->img_h;
            if (image->blend_mode == IMG_FILTER_BLACK)
            {
                source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
                source.color_key_min = 0;
                source.color_key_max = 0x010101;
            }
            else
            {
                source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
            }
            source.win_x_min = 0;
            source.win_x_max = target.width;
            source.win_y_min = 0;
            source.win_y_max = target.height;

            ppe_matrix_t pre_trans;
            ppe_get_identity(&pre_trans);
            if ((x_ref < 0) || (y_ref < 0))
            {
                ppe_translate(x_ref * (-1.0f), y_ref * (-1.0f), &pre_trans);
            }
            memcpy(&source.inv_matrix, &pre_trans, sizeof(float) * 9);
            PPEV2_err err = PPEV2_Blend(&target, &source, &ppe_rect);
            if (err != PPEV2_SUCCESS)
            {
                sw_acc_blit(image, dc, rect);
            }
        }
    }
    else
    {
        if (dc->type == DC_SINGLE)  //may need PSRAM
        {
            if (image->matrix->m[0][0] != 1 || image->matrix->m[1][1] != 1 || \
                image->matrix->m[2][2] != 1 || image->matrix->m[0][1] != 0 || \
                image->matrix->m[1][0] != 0 || image->matrix->m[2][0] != 0 || \
                image->matrix->m[2][1] != 0)
            {
                GUI_ASSERT(0);
                return;
            }
            if ((rect->x2 < dc->section.x1) || (rect->y2 < dc->section.y1)
                || (rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y1 + dc->fb_height))
            {
                return;
            }
            uint32_t start_line = 0, end_line = 0;
            uint32_t start_column = 0, end_column = 0;
            ppe_rect_t ppe_rect = {.x = rect->x1, .y = rect->y1, .w = image->img_w, .h = image->img_h};
            if (rect->x1 < dc->section.x1)
            {
                ppe_rect.x = 0;
                start_column = dc->section.x1 - rect->x1;
            }
            if (rect->y1 < dc->section.y1)
            {
                ppe_rect.y = 0;
                start_line = dc->section.y1 - rect->y1;
            }
            if (rect->y1 + image->img_h > dc->section.y1 + dc->fb_height)
            {
                end_line = dc->section.y2 - rect->y1 - 1;
            }
            if (rect->x1 + image->img_w > dc->section.x1 + dc->fb_width)
            {
                end_column = dc->section.x2 - rect->x1 - 1;
            }
            ppe_rect.h = end_line - start_line + 1;
            ppe_rect.w = end_column - start_column + 1;
            RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
            const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                      struct gui_rgb_data_head));
            IMDC_decode_range range;
            range.start_column = start_column;
            range.end_column = end_column;
            range.start_line = start_line;
            range.end_line = end_line;
            IMDC_DMA_config dma_cfg;
            source.width = range.end_column - range.start_column + 1;
            source.height = range.end_line - range.start_line + 1;
            uint8_t imdc_rx_dma_num = 0xa5;
            if (!GDMA_channel_request(&imdc_rx_dma_num, NULL, true))
            {
                DBG_DIRECT("no dma for rx");
                return;
            }
            dma_cfg.RX_DMA_channel_num = imdc_rx_dma_num;
            dma_cfg.RX_DMA_channel = DMA_CH_BASE(imdc_rx_dma_num);
            IMDC_ERROR err = IMDC_Decode_Direct((uint8_t *)header, &range, &dma_cfg);
            if (err)
            {
                if (imdc_rx_dma_num != 0xa5)
                {
                    GDMA_channel_release(imdc_rx_dma_num);
                }
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
            ppe_get_identity(&source.matrix);
            ppe_get_identity(&source.inv_matrix);
            PPEV2_Blend_Handshake(&target, &source, &ppe_rect);
            if (imdc_rx_dma_num != 0xa5)
            {
                GDMA_channel_release(imdc_rx_dma_num);
            }
        }
        else
        {
            //TODO: suppose to be overed by DC_SINGLE branch, will verify later
            return;
        }
    }
}

void hw_acc_init(void)
{
#if PPE_ACC_CACHE_SOURCE
    if (!GDMA_channel_request(&memcpy_dma_num, NULL, true))
    {
        GUI_ASSERT("no dma for tx");
        return;
    }
    ppe_fb_port1 = ppe_cache_buf;
    ppe_fb_port2 = ppe_fb_port1 + PPEV2_CACHE_BUF_SIZE / 2;
#endif
}
