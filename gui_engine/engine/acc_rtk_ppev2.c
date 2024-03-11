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

#define USE_TESSALLATION 1

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect);

#define PPEV2_ACC_MIN_OPA       3
#define PPEV2_MIN_PIXEL         100
#if USE_TESSALLATION
#define PPEV2_TESS_LENGTH       100
#endif

#if (F_APP_GUI_USE_DSP_SHARE_RAM == 0)
#define TEMP_BUF_SIZE           (50 * 1024)
static uint8_t temp_buf[TEMP_BUF_SIZE];
#else
#include "section.h"
#define TEMP_BUF_SIZE           (40 * 1024)
SHM_DATA_SECTION static uint8_t temp_buf[TEMP_BUF_SIZE];
#endif

static uint8_t memcpy_dma_num = 0xa5, support_dma_num = 0xa5;
static uint32_t temp_buf_offset = 0;
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
    uint32_t dest_address = (uint32_t)temp_buf + temp_buf_offset;
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
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dest_address;

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
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + p_rect->w * i * pixel_size;
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
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + p_rect->w * i * pixel_size;
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
    source->address = (uint32_t)dest_address;
    temp_buf_offset += p_rect->w * p_rect->h * pixel_size;
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

    uint32_t dest_address = ((((uint32_t)temp_buf + temp_buf_offset + 63) >> 6) << 6);

    dma_cfg.output_buf = (uint32_t *)dest_address;
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
        source->address = (uint32_t)dest_address;
        return true;
    }
}

void bare_blit_by_dma(ppe_buffer_t *target, ppe_buffer_t *source, ppe_rect_t *src_rect,
                      ppe_rect_t *dst_trans)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(target->format);
    uint32_t dst_start_address = target->address + (dst_trans->x + dst_trans->y * target->width) *
                                 pixel_size;
    uint32_t src_start_address = source->address + (src_rect->x + src_rect->y * source->width) *
                                 pixel_size;
    uint32_t dma_height = src_rect->h;
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
        return;
    }

    if ((src_rect->w == source->width && src_rect->w == target->width &&
         target->width == source->height)\
        || (src_rect->h == 1))
    {
        use_LLI = false;
    }
    GDMA_LLIDef *GDMA_LLIStruct;
    if (use_LLI)
    {
        GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, src_rect->h * sizeof(GDMA_LLIDef));
        if (GDMA_LLIStruct == NULL)
        {
            GUI_ASSERT("no ram for LLI");
        }
        memset(GDMA_LLIStruct, 0, src_rect->h * sizeof(GDMA_LLIDef));
    }
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_ChannelTypeDef *dma_channel = DMA_CH_BASE(memcpy_dma_num);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = memcpy_dma_num;
    if (pixel_size == 3)
    {
        RX_GDMA_InitStruct.GDMA_BufferSize          = src_rect->w * src_rect->h * 3;
    }
    else
    {
        RX_GDMA_InitStruct.GDMA_BufferSize          = src_rect->w * src_rect->h;
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
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)src_start_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dst_start_address;

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
                GDMA_LLIStruct[i].SAR = src_start_address + source->width * i * pixel_size;
                GDMA_LLIStruct[i].DAR = dst_start_address + target->width * i * pixel_size;
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
                    GDMA_LLIStruct[i].CTL_HIGH = src_rect->w * 3;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = src_rect->w;
                }
            }
            else
            {
                GDMA_LLIStruct[i].SAR = src_start_address + source->width * i * pixel_size;
                GDMA_LLIStruct[i].DAR = dst_start_address + target->width * i * pixel_size;
                GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = dma_channel->CTL_LOW;
                /* configure high 32 bit of CTL register */
                if (pixel_size == 3)
                {
                    GDMA_LLIStruct[i].CTL_HIGH = src_rect->w * 3;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = src_rect->w;
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
}

void bare_blit_by_imdc(ppe_buffer_t *target, ppe_buffer_t *source, ppe_rect_t *src_rect,
                       ppe_rect_t *dst_trans)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(target->format);
    uint32_t dst_start_address = target->address + (dst_trans->x + dst_trans->y * target->width) *
                                 pixel_size;
    uint32_t decompress_start_line;
    uint32_t decompress_end_line;
    uint32_t decompress_start_column;
    uint32_t decompress_end_column;
    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
    IMDC_DMA_config config;
    IMDC_DMA_config *dma_cfg = &config;
    dma_cfg->output_buf = (uint32_t *)dst_start_address;
    dma_cfg->TX_DMA_channel_num = support_dma_num;
    dma_cfg->RX_DMA_channel_num = memcpy_dma_num;
    dma_cfg->TX_DMA_channel = DMA_CH_BASE(support_dma_num);
    dma_cfg->RX_DMA_channel = DMA_CH_BASE(memcpy_dma_num);
    uint32_t compressed_data_start_address = (uint32_t)source->address;
    IMDC_file_header *header = (IMDC_file_header *)compressed_data_start_address;

    decompress_start_line = src_rect->y;
    decompress_end_line = src_rect->y + src_rect->h - 1;
    decompress_start_column = src_rect->x;
    decompress_end_column = src_rect->x + src_rect->w - 1;

    uint32_t start_line_address = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                              decompress_start_line);
    uint32_t compressed_data_size = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                                decompress_end_line + 1) - start_line_address;

    IMDC_InitTypeDef IMDC_struct_init;
    IMDC_struct_init.algorithm_type            = (IMDC_ALGORITHM)header->algorithm_type.algorithm;
    IMDC_struct_init.head_throw_away_byte_num  = THROW_AWAY_0BYTE;
    IMDC_struct_init.pic_pixel_size            = (IMDC_PIXEL_SIZE)header->algorithm_type.pixel_bytes;
    IMDC_struct_init.pic_decompress_height     = (decompress_end_line - decompress_start_line + 1);
    IMDC_struct_init.pic_raw_width             = header->raw_pic_width;
    IMDC_struct_init.tx_column_start           = decompress_start_column;
    IMDC_struct_init.tx_column_end             = decompress_end_column ;
    IMDC_struct_init.compressed_data_size      = compressed_data_size;
    IMDC_struct_init.pic_length2_size          = (IMDC_RLE_RUNLENGTH_SIZE)
                                                 header->algorithm_type.feature_2;
    IMDC_struct_init.pic_length1_size          = (IMDC_RLE_RUNLENGTH_SIZE)
                                                 header->algorithm_type.feature_1;
    IMDC_struct_init.yuv_blur_bit              = (IMDC_YUV_BLUR_BIT)header->algorithm_type.feature_2;
    IMDC_struct_init.yuv_sample_type           = (IMDC_YUV_SAMPLE_TYPE)
                                                 header->algorithm_type.feature_1;
    IMDC_struct_init.rx_fifo_dma_enable        = (uint32_t)ENABLE;
    IMDC_struct_init.tx_fifo_dma_enable        = (uint32_t)ENABLE;
    IMDC_struct_init.rx_fifo_dma_threshold     = IMDC_RX_FIFO_DEPTH / 2;
    IMDC_struct_init.tx_fifo_dma_threshold     = IMDC_TX_FIFO_DEPTH / 2;
    IMDC_struct_init.rx_fifo_int_threshold     = dma_cfg->RX_FIFO_INT_threshold;
    IMDC_struct_init.tx_fifo_int_threshold     = dma_cfg->TX_FIFO_INT_threshold;
#if defined(TARGET_RTL8773E)
    IMDC_struct_init.hw_handshake              = IMDC_HW_HANDSHAKE_DMA;
#endif
    IMDC_Init(&IMDC_struct_init);


    uint32_t DMA_compressed_data_size_word = ((compressed_data_size % 4) ?
                                              (compressed_data_size / 4 + 1) : (compressed_data_size / 4));
    /* Configure DMA */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    GDMA_LLIDef *RX_GDMA_LLIStruct;
    GDMA_LLIDef *TX_GDMA_LLIStruct;
    uint32_t rx_block_num = 0;
    if (compressed_data_size >= 65532)
    {
        rx_block_num = compressed_data_size / 65532;
        if (compressed_data_size % 65532)
        {
            rx_block_num = rx_block_num + 1;
        }
    }
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->RX_DMA_channel_num;
    RX_GDMA_InitStruct.GDMA_BufferSize          = DMA_compressed_data_size_word;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_16;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_8;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_Word;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_Word;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)start_line_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&IMDC->RX_FIFO);
#if defined RTL87x2G
    RX_GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_IMDC_RX;
    RX_GDMA_InitStruct.GDMA_Secure_En = 1;
#elif defined TARGET_RTL8773E
    RX_GDMA_InitStruct.GDMA_DestHandshake       = 43;
#endif
    if (rx_block_num)
    {
        RX_GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(GDMA_LLIDef) * rx_block_num);
        RX_GDMA_InitStruct.GDMA_BufferSize = 65532 / 4;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)RX_GDMA_LLIStruct;
    }
    GDMA_Init(dma_cfg->RX_DMA_channel, &RX_GDMA_InitStruct);
    if (rx_block_num)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            if (i == rx_block_num - 1)
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65532 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (GDMA_DataSize_Byte << 4)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = compressed_data_size - 65532 * i;
                RX_GDMA_LLIStruct[i].CTL_HIGH = block_size;
            }
            else
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65532 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = (uint32_t)&RX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
#if defined(TARGET_RTL8773E)
                RX_GDMA_LLIStruct[i].CTL_LOW = dma_cfg->RX_DMA_channel->CTL_LOW;
#else
                RX_GDMA_LLIStruct[i].CTL_LOW = dma_cfg->RX_DMA_channel->GDMA_CTL_LOWx;
#endif
                /* configure high 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_HIGH = 65532 / 4;
            }
        }
    }

    GDMA_InitTypeDef TX_GDMA_InitStruct;
    uint32_t tx_block_num = 0;
    tx_block_num = src_rect->h;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&TX_GDMA_InitStruct);
    TX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->TX_DMA_channel_num;
    TX_GDMA_InitStruct.GDMA_BufferSize          = src_rect->w / 2;
    TX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    TX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    TX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    TX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_8;                         // 8 msize for source msize
    TX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_8;                         // 8 msize for destiantion msize
    TX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_Word;                   // 32 bit width for destination transaction
    TX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_Word;                   // 32 bit width for source transaction
    TX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&IMDC->TX_FIFO);
    TX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dma_cfg->output_buf;
#if defined RTL87x2G
    TX_GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_IMDC_TX;
    TX_GDMA_InitStruct.GDMA_Secure_En = 1;
#elif defined TARGET_RTL8773E
    TX_GDMA_InitStruct.GDMA_SourceHandshake     = 44;
#endif
    if (tx_block_num)
    {
        TX_GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(GDMA_LLIDef) * tx_block_num);
        TX_GDMA_InitStruct.GDMA_BufferSize = src_rect->w / 2;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        TX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)TX_GDMA_LLIStruct;
    }
    GDMA_Init(dma_cfg->TX_DMA_channel, &TX_GDMA_InitStruct);
    if (tx_block_num)
    {
        for (int i = 0; i < tx_block_num; i++)
        {
            if (i == tx_block_num - 1)
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + target->width * pixel_size * i;
                TX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (TX_GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (TX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (TX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (TX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = src_rect->w / 2;
            }
            else
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + target->width * pixel_size * i;
                TX_GDMA_LLIStruct[i].LLP = (uint32_t)&TX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
#if defined(TARGET_RTL8773E)
                TX_GDMA_LLIStruct[i].CTL_LOW = dma_cfg->TX_DMA_channel->CTL_LOW;
#else
                TX_GDMA_LLIStruct[i].CTL_LOW = dma_cfg->TX_DMA_channel->GDMA_CTL_LOWx;
#endif
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = src_rect->w / 2;
            }
        }
    }

    GDMA_Cmd(dma_cfg->RX_DMA_channel_num, ENABLE);
    GDMA_Cmd(dma_cfg->TX_DMA_channel_num, ENABLE);

    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_FINISH_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_FINISH_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_FINISH_INT, DISABLE);

    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_ERROR_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_ERROR_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_ERROR_INT, DISABLE);

    IMDC_Cmd(ENABLE);
    IMDC_Run(ENABLE);
    while (IMDC->IMDC_CTL0 & BIT0);
#if defined(TARGET_RTL8773E)
    if (GDMA0->ChEnReg & (0x01 << dma_cfg->RX_DMA_channel_num))
#else
    if (GDMA0->GDMA_ChEnReg & (0x01 << dma_cfg->RX_DMA_channel_num))
#endif
    {
        GDMA_SuspendCmd(dma_cfg->TX_DMA_channel, ENABLE);
        GDMA_SuspendCmd(dma_cfg->RX_DMA_channel, ENABLE);
#if defined(TARGET_RTL8773E)
        while (!(dma_cfg->RX_DMA_channel->CFG_LOW & BIT0));
        while (!(dma_cfg->TX_DMA_channel->CFG_LOW & BIT0));
#else
        while (!(dma_cfg->RX_DMA_channel->GDMA_CFG_LOWx & BIT0));
        while (!(dma_cfg->TX_DMA_channel->GDMA_CFG_LOWx & BIT0));
#endif
        GDMA_Cmd(dma_cfg->RX_DMA_channel_num, DISABLE);
        GDMA_Cmd(dma_cfg->TX_DMA_channel_num, DISABLE);
    }
    IMDC_RxFifoClear();
    IMDC_Cmd(DISABLE);
    while (!(IMDC->IMDC_CTL1 & BIT29));
    IMDC_TxFifoClear();
    if (rx_block_num)
    {
        os_mem_free(RX_GDMA_LLIStruct);
    }
    if (tx_block_num)
    {
        os_mem_free(TX_GDMA_LLIStruct);
    }
}

void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect)
{
    if (image->opacity_value <= PPEV2_ACC_MIN_OPA)
    {
        return;
    }
    temp_buf_offset = 0;
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

    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.opacity = image->opacity_value;
    source.width = image->img_w;
    source.height = image->img_h;
    struct gui_rgb_data_head *head = image->data;
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
        if (image->matrix->m[0][0] == 1 && image->matrix->m[1][1] == 1)
        {
            if ((image->blend_mode == IMG_BYPASS_MODE && source.format == target.format))
            {
                uint32_t x_max = (image->target_w + image->img_x - 1) > rect->x2 ? rect-> x2 :
                                 (image->target_w + image->img_x - 1);
                uint32_t y_max = (image->target_h + image->img_y - 1) > rect->y2 ? rect-> y2 :
                                 (image->target_h + image->img_y - 1);
                int32_t x_min = image->img_x < rect->x1 ? rect->x1 : image->img_x;
                int32_t y_min = image->img_y < rect->y1 ? rect->y1 : image->img_y;
                if ((x_max < dc->section.x1) || (y_max < dc->section.y1)
                    || (x_min >= dc->section.x1 + dc->fb_width) || (y_min >= dc->section.y1 + dc->fb_height))
                {
                    return;
                }
                ppe_rect_t dst_rect = {.x = x_min - dc->section.x1, .y = y_min - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
                if (dst_rect.x < 0)
                {
                    dst_rect.x = 0;
                }
                if (dst_rect.y < 0)
                {
                    dst_rect.y = 0;
                }
                if (x_max >= dc->section.x1 + dc->fb_width)
                {
                    dst_rect.w = dc->fb_width - dst_rect.x;
                }
                else
                {
                    dst_rect.w = x_max - dc->section.x1 - dst_rect.x + 1;
                }
                if (y_max >= dc->section.y1 + dc->fb_height)
                {
                    dst_rect.h = dc->fb_height - dst_rect.y;
                }
                else
                {
                    dst_rect.h = y_max - dc->section.y1 - dst_rect.y + 1;
                }
                dst_rect.x += dc->section.x1;
                dst_rect.y += dc->section.y1;
                ppe_rect_t src_rect;
                ppe_matrix_t inv_matrix;
                memcpy(&inv_matrix, image->inverse, sizeof(float) * 9);
                source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
                source.width = image->img_w;
                source.height = image->img_h;
                bool ret = ppe_get_area(&src_rect, &dst_rect, &inv_matrix, &source);
                if (src_rect.w % 2)
                {
                    src_rect.w -= 1;
                    if (src_rect.w == 0)
                    {
                        return;
                    }
                }
                if (!ret)
                {
                    DBG_DIRECT("MAT err! addr %x, section %x", image->data, dc->section_count);
                    return;
                }
                dst_rect.x -= dc->section.x1;
                dst_rect.y -= dc->section.y1;
                if (image->blend_mode == IMG_BYPASS_MODE)
                {
                    if (head->type == IMDC_COMPRESS)
                    {
                        bare_blit_by_imdc(&target, &source, &src_rect, &dst_rect);
                    }
                    else
                    {
                        bare_blit_by_dma(&target, &source, &src_rect, &dst_rect);
                    }
                }
                return;
            }
            else
            {
                mode = PPEV2_SRC_OVER_MODE;
            }
        }
        else if (image->blend_mode == IMG_BYPASS_MODE)
        {
            mode = PPEV2_BYPASS_MODE;
        }
        else
        {
            if (source.format >= PPEV2_BGR888 && source.format <= PPEV2_RGB565 && image->opacity_value == 0xFF)
            {
                mode = PPEV2_BYPASS_MODE;
            }
            else
            {
                mode = PPEV2_SRC_OVER_MODE;
            }
        }
    }
    else if (image->blend_mode == IMG_COVER_MODE)
    {
        mode = PPEV2_BYPASS_MODE;
    }
    else if (image->blend_mode == IMG_RECT)
    {
        mode = PPEV2_CONST_MASK_MODE;
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE || image->blend_mode == IMG_FILTER_BLACK)
    {
        mode = PPEV2_SRC_OVER_MODE;
    }

    int32_t x_max = (image->target_w + rect->x1 - 1) > rect->x2 ? rect-> x2 :
                    (image->target_w + rect->x1 - 1);
    int32_t y_max = (image->target_h + rect->y1 - 1) > rect->y2 ? rect-> y2 :
                    (image->target_h + rect->y1 - 1);
    int32_t x_min = image->img_x < rect->x1 ? rect->x1 : image->img_x;
    int32_t y_min = image->img_y < rect->y1 ? rect->y1 : image->img_y;
#if USE_TESSALLATION
    int32_t tessalation_len = PPEV2_TESS_LENGTH;
    uint32_t block_num = dc->fb_width / tessalation_len;
    if (dc->fb_width % tessalation_len)
    {
        block_num++;
    }
#else
    uint32_t tessalation_len = dc->fb_width; \
    uint32_t block_num = 1;
#endif
    for (int i = 0; i < block_num; i++)
    {
        int32_t section_x1 = tessalation_len * i;
        int32_t section_y1 = dc->section.y1;
        if (tessalation_len * (i + 1) > dc->fb_width)
        {
            tessalation_len = dc->fb_width % tessalation_len;
        }
        if ((x_max < section_x1) || (y_max < section_y1)
            || (x_min >= section_x1 + tessalation_len) || (y_min >= section_y1 + dc->fb_height))
        {
            continue;
        }
        source.win_x_min = 0;
        source.win_x_max = target.width;
        source.win_y_min = 0;
        source.win_y_max = target.height;
        source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
        source.width = image->img_w;
        source.height = image->img_h;
        float x_ref = 0, y_ref = 0;
        uint32_t blend_area = 0;
        ppe_rect_t ppe_rect = {.x = x_min - section_x1, .y = y_min - section_y1, .w = tessalation_len, .h = dc->fb_height};
        if (ppe_rect.x < 0)
        {
            ppe_rect.x = 0;
        }
        if (ppe_rect.y < 0)
        {
            ppe_rect.y = 0;
        }
        if (x_max >= section_x1 + tessalation_len)
        {
            ppe_rect.w = tessalation_len - ppe_rect.x;
        }
        else
        {
            ppe_rect.w = x_max - section_x1 - ppe_rect.x + 1;
        }
        if (y_max >= section_y1 + dc->fb_height)
        {
            ppe_rect.h = dc->fb_height - ppe_rect.y;
        }
        else
        {
            ppe_rect.h = y_max - section_y1 - ppe_rect.y + 1;
        }
        blend_area = ppe_rect.w * ppe_rect.h;
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
            source.color_key_min = 0;
            source.color_key_max = 0x010101;
        }
        else if (image->blend_mode == IMG_BYPASS_MODE)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
        }
        else if (image->blend_mode == IMG_SRC_OVER_MODE)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
        }
        else if (image->blend_mode == IMG_RECT)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
        }
        else if (image->blend_mode == IMG_COVER_MODE)
        {
            source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
        }
        else
        {
            return;
        }
        ppe_matrix_t inverse;
        memcpy(&inverse, image->inverse, sizeof(float) * 9);
        ppe_matrix_t pre_trans;
        ppe_rect.x += section_x1;
        if (mode != PPEV2_CONST_MASK_MODE)
        {
            ppe_rect_t old_rect;
            ppe_rect.y += section_y1;
            bool ret = ppe_get_area(&old_rect, &ppe_rect, &inverse, &source);
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
            ppe_rect.y -= section_y1;
            if (ret)
            {
                ppe_get_identity(&pre_trans);
                ppe_translate(old_rect.x * -1.0f, old_rect.y * -1.0f, &pre_trans);
                ppe_mat_multiply(&pre_trans, &inverse);
                y_ref = section_y1;
                ppe_translate(0, y_ref, &pre_trans);
                memcpy(&inverse, &pre_trans, sizeof(float) * 9);
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
                    x_ref = ppe_rect.x + section_x1;
                    y_ref = ppe_rect.y + section_y1;
                }
                ppe_translate(x_ref, y_ref, &inverse);
            }
        }
        else
        {
            ppe_get_identity(&pre_trans);
            x_ref = section_x1;
            y_ref = section_y1;
            ppe_translate(0, y_ref, &inverse);
            source.const_color = *(uint32_t *)((uint32_t)image->data + 8);
        }
        source.high_quality = image->high_quality;
        if ((image->matrix->m[0][0] == 1 && image->matrix->m[1][1] == 1 && \
             image->matrix->m[2][2] == 1 && image->matrix->m[0][1] == 0 && \
             image->matrix->m[1][0] == 0 && image->matrix->m[2][0] == 0 && \
             image->matrix->m[2][1] == 0) || (blend_area > 250 * 250))
        {
            source.high_quality = false;
        }
        PPEV2_err err = PPEV2_Blit_Inverse(&target, &source, &inverse, &ppe_rect, mode);

        if (err != PPEV2_SUCCESS)
        {
            DBG_DIRECT("PPE err %d", err);
//                sw_acc_blit(image, dc, rect);
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
    DBG_DIRECT("zip rx %d", memcpy_dma_num);
    DBG_DIRECT("zip tx %d", support_dma_num);
}
