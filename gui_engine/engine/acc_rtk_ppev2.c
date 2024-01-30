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
#include "os_mem.h"
#include "rtl876x_pinmux.h"
#include "math.h"
#include "fmc_api_ext.h"
#include "os_sync.h"
#include "section.h"

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect);

#define PPEV2_ACC_MIN_OPA       3
#define PPEV2_MIN_PIXEL         100

#if (F_APP_GUI_USE_DSP_SHARE_RAM == 0)
#define TEMP_BUF_SIZE           (50 * 1024)
#else
#define TEMP_BUF_SIZE           (40 * 1024)
#endif

#if (F_APP_GUI_USE_DSP_SHARE_RAM == 0)
static uint8_t temp_buf[TEMP_BUF_SIZE];
#else
SHM_DATA_SECTION static uint8_t temp_buf[TEMP_BUF_SIZE];
#endif
static uint8_t memcpy_dma_num = 0xa5, support_dma_num = 0xa5;
static bool memcpy_by_dma(ppe_rect_t *p_rect, ppe_buffer_t *source)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(source->format);
    if (p_rect->w * p_rect->h * pixel_size > TEMP_BUF_SIZE)
    {
        return false;
    }
    bool use_LLI = true;
    uint8_t m_size = 0, data_size = 0;
    if (pixel_size == 4)
    {
        data_size = GDMA_DataSize_Word;
        m_size = GDMA_Msize_16;
    }
    else if (pixel_size == 2)
    {
        data_size = GDMA_DataSize_HalfWord;
        m_size = GDMA_Msize_32;
    }
    else if (pixel_size == 3)
    {
        data_size = GDMA_DataSize_Byte;
        m_size = GDMA_Msize_64;
    }
    else
    {
        return false;
    }
    uint32_t dma_height = p_rect->h;

    if (p_rect->w == source->width || dma_height == 1)
    {
        use_LLI = false;
    }
    GDMA_LLIDef *GDMA_LLIStruct;
    if (use_LLI)
    {
        GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, p_rect->h * sizeof(GDMA_LLIDef));
        if (GDMA_LLIStruct == NULL)
        {
            GUI_ASSERT("no ram for LLI");
        }
        memset(GDMA_LLIStruct, 0, p_rect->h * sizeof(GDMA_LLIDef));
    }
    uint32_t start_address = source->address + (p_rect->x + p_rect->y * source->width) * pixel_size;

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_ChannelTypeDef *dma_channel = DMA_CH_BASE(memcpy_dma_num);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = memcpy_dma_num;
    if (pixel_size == 3)
    {
        RX_GDMA_InitStruct.GDMA_BufferSize          = p_rect->w * dma_height * 3;
    }
    else
    {
        RX_GDMA_InitStruct.GDMA_BufferSize          = p_rect->w * dma_height;
    }
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        m_size;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_32;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_HalfWord;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        data_size;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)start_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)temp_buf;

    if (use_LLI)
    {
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)GDMA_LLIStruct;
    }

    GDMA_Init(dma_channel, &RX_GDMA_InitStruct);
    if (use_LLI)
    {
        for (int i = 0; i < dma_height; i++)
        {
            if (i == dma_height - 1)
            {
                GDMA_LLIStruct[i].SAR = start_address + source->width * i * pixel_size;
                GDMA_LLIStruct[i].DAR = (uint32_t)temp_buf + p_rect->w * i * pixel_size;
                GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                             | (data_size << 4)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                             | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                             | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                             | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                if (pixel_size == 3)
                {
                    GDMA_LLIStruct[i].CTL_HIGH = p_rect->w * 3;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = p_rect->w;
                }
            }
            else
            {
                GDMA_LLIStruct[i].SAR = start_address + source->width * i * pixel_size;
                GDMA_LLIStruct[i].DAR = (uint32_t)temp_buf + p_rect->w * i * pixel_size;
                GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = dma_channel->CTL_LOW;
                /* configure high 32 bit of CTL register */
                if (pixel_size == 3)
                {
                    GDMA_LLIStruct[i].CTL_HIGH = p_rect->w * 3;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = p_rect->w;
                }
            }
        }
    }

    GDMA_INTConfig(memcpy_dma_num, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(memcpy_dma_num, ENABLE);
    while (GDMA_GetTransferINTStatus(memcpy_dma_num) != SET);
    GDMA_ClearINTPendingBit(memcpy_dma_num, GDMA_INT_Transfer);

    if (use_LLI)
    {
        os_mem_free(GDMA_LLIStruct);
    }
    return true;
}

static bool memcpy_by_imdc(ppe_rect_t *p_rect, ppe_buffer_t *source)
{
    IMDC_file_header *header = (IMDC_file_header *)source->address;
    IMDC_decode_range range;
    range.start_column = p_rect->x;
    range.end_column = p_rect->x + p_rect->w - 1;
    range.start_line = p_rect->y;
    range.end_line = p_rect->y + p_rect->h - 1;
    IMDC_DMA_config dma_cfg;

    dma_cfg.output_buf = (uint32_t *)temp_buf;
    dma_cfg.RX_DMA_channel_num = support_dma_num;
    dma_cfg.TX_DMA_channel_num = memcpy_dma_num;
    dma_cfg.RX_DMA_channel = DMA_CH_BASE(support_dma_num);
    dma_cfg.TX_DMA_channel = DMA_CH_BASE(memcpy_dma_num);
    IMDC_ERROR err = IMDC_Decode((uint8_t *)header, &range, &dma_cfg);
    if (err != IMDC_SUCCESS)
    {
        return false;
    }
    else
    {
        return true;
    }
}


void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect)
{
    if (image->opacity_value <= PPEV2_ACC_MIN_OPA)
    {
        return;
    }

    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    PPEV2_BLEND_MODE mode = PPEV2_BYPASS_MODE;

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
    target.use_bg = true;
    struct gui_rgb_data_head *head = image->data;
    if (1)
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
        case ARGB8565:
            source.format = PPEV2_ARGB8565;
            break;
        case IMDC_COMPRESS:
            {
                const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image->data + sizeof(
                                                                          struct gui_rgb_data_head));
                if (header->algorithm_type.pixel_bytes == 0)
                {
                    source.format = PPEV2_RGB565;
                }
                else if (header->algorithm_type.pixel_bytes == 2)
                {
                    source.format = PPEV2_ARGB8888;
                }
                else if (header->algorithm_type.pixel_bytes == 1)
                {
                    source.format = PPEV2_RGB888;
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
        if (image->matrix->m[2][2] == 1 && image->matrix->m[0][1] == 0 && \
            image->matrix->m[1][0] == 0 && image->matrix->m[2][0] == 0 && \
            image->matrix->m[2][1] == 0)
        {
            if (image->blend_mode == IMG_BYPASS_MODE)
            {
                if (image->opacity_value < 0xFF)
                {
                    mode = PPEV2_TRANSPARENT_MODE;
                }
                else
                {
                    mode = PPEV2_BYPASS_MODE;
                }
            }
            else if (image->blend_mode == IMG_FILTER_BLACK)
            {
                mode = PPEV2_SRC_OVER_MODE;
            }
            else
            {
                if (source.format >= PPEV2_BGR888 && source.format <= PPEV2_RGB565)
                {
                    mode = PPEV2_BYPASS_MODE;
                }
                else
                {
                    mode = PPEV2_SRC_OVER_MODE;
                }
            }
        }
        else
        {
            mode = PPEV2_SRC_OVER_MODE;
        }
        uint32_t x_max = (image->target_w + rect->x1 - 1) > rect->x2 ? rect-> x2 :
                         (image->target_w + rect->x1 - 1);
        uint32_t y_max = (image->target_h + rect->y1 - 1) > rect->y2 ? rect-> y2 :
                         (image->target_h + rect->y1 - 1);

        if ((x_max < dc->section.x1) || (y_max < dc->section.y1)
            || (rect->x1 >= dc->section.x1 + dc->fb_width) || (rect->y1 >= dc->section.y1 + dc->fb_height))
        {
            return;
        }
        source.win_x_min = 0;
        source.win_x_max = target.width;
        source.win_y_min = 0;
        source.win_y_max = target.height;
        float x_ref = 0, y_ref = 0;
        uint32_t blend_area = 0;
        ppe_rect_t ppe_rect = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
        if (ppe_rect.x < 0)
        {
            ppe_rect.x = 0;
        }
        if (ppe_rect.y < 0)
        {
            ppe_rect.y = 0;
        }
        if (x_max >= dc->section.x1 + dc->fb_width)
        {
            ppe_rect.w = dc->fb_width - ppe_rect.x;
        }
        else
        {
            ppe_rect.w = x_max - dc->section.x1 - ppe_rect.x + 1;
        }
        if (y_max >= dc->section.y1 + dc->fb_height)
        {
            ppe_rect.h = dc->fb_height - ppe_rect.y;
        }
        else
        {
            ppe_rect.h = y_max - dc->section.y1 - ppe_rect.y + 1;
        }
        blend_area = ppe_rect.w * ppe_rect.h;
        source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
        source.opacity = image->opacity_value;
        source.width = image->img_w;
        source.height = image->img_h;
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
            source.color_key_min = 0;
            source.color_key_max = 0x010101;
        }
        else if (image->blend_mode == IMG_BYPASS_MODE)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
            target.use_bg = false;
        }
        else if (image->blend_mode == IMG_SRC_OVER_MODE)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
            target.use_bg = true;
        }
        else
        {
            return;
        }

        memcpy(&source.matrix, image->matrix, sizeof(float) * 9);
        memcpy(&source.inv_matrix, image->inverse, sizeof(float) * 9);
        ppe_matrix_t pre_trans;
        ppe_rect_t old_rect;

        ppe_rect.x += dc->section.x1;
        ppe_rect.y += dc->section.y1;
        bool ret = ppe_get_area(&old_rect, &ppe_rect, &source);
        if (!ret)
        {
            DBG_DIRECT("MAT err! addr %x, section %x", image->data, dc->section_count);
            return;
        }
        if (head->type == IMDC_COMPRESS)
        {
            ret = memcpy_by_imdc(&old_rect, &source);
        }
        else
        {
            ret = memcpy_by_dma(&old_rect, &source);
        }
        ppe_rect.x -= dc->section.x1;
        ppe_rect.y -= dc->section.y1;
        if (ret)
        {
            ppe_get_identity(&pre_trans);
            ppe_translate(old_rect.x * -1.0f, old_rect.y * -1.0f, &pre_trans);
            ppe_mat_multiply(&pre_trans, &source.inv_matrix);
            x_ref = ppe_rect.x + dc->section.x1;
            y_ref = ppe_rect.y + dc->section.y1;
            ppe_translate(x_ref, y_ref, &pre_trans);
            memcpy(&source.inv_matrix, &pre_trans, sizeof(float) * 9);
            source.address = (uint32_t)temp_buf;
            source.width = old_rect.w;
            source.height = old_rect.h;
        }
        else
        {
            if (head->type == IMDC_COMPRESS)
            {
                return;
            }
            ppe_get_identity(&pre_trans);
            if (dc->type == DC_RAMLESS)
            {
                x_ref = ppe_rect.x + dc->section.x1;
                y_ref = ppe_rect.y + dc->section.y1;
            }
            ppe_translate(x_ref, y_ref, &source.inv_matrix);
        }
        source.high_quality = image->high_quality;
        if ((image->matrix->m[0][0] == 1 && image->matrix->m[1][1] == 1 && \
             image->matrix->m[2][2] == 1 && image->matrix->m[0][1] == 0 && \
             image->matrix->m[1][0] == 0 && image->matrix->m[2][0] == 0 && \
             image->matrix->m[2][1] == 0) || (blend_area > 250 * 250))
        {
            source.high_quality = false;
        }
        PPEV2_err err = PPEV2_Blend(&target, &source, &ppe_rect, mode);


        if (err != PPEV2_SUCCESS)
        {
//                DBG_DIRECT("PPE err %d", err);
//                sw_acc_blit(image, dc, rect);
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
    if (!GDMA_channel_request(&memcpy_dma_num, NULL, true))
    {
        GUI_ASSERT("no dma for tx");
        return;
    }
    if (!GDMA_channel_request(&support_dma_num, NULL, false))
    {
        GUI_ASSERT("no dma for support");
        return;
    }
#if PPE_ACC_CACHE_SOURCE
    ppe_fb_port1 = ppe_cache_buf;
    ppe_fb_port2 = ppe_fb_port1 + PPEV2_CACHE_BUF_SIZE / 2;
#endif
}
